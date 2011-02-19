/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gestionconnexion.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMessageBox>
#include "propertiesdialog.h"
#ifdef USE_ICU
#include "icu_charset.h"
#endif

GestionConnexion::GestionConnexion(MainWindow * parent) :
    QObject(parent), m_connected(false),
    m_connecting(false), m_deconnecting(false),
    m_quitAfterDisconnect(false),
    m_delaiContact(900000), // 15 minutes
    m_askForDisconnect(0),
    m_downloadByQueryOnlyIfEmpty(false),
    m_downloadByQueryAfterHash(false)
{
    m_keepContactTimer.setSingleShot(true);
    QObject::connect(& (parent->client()->networkManager()),
            SIGNAL(finished ( QNetworkReply * )),
            this, SLOT(launchKeepContactTimer()));
    QObject::connect(&m_keepContactTimer, SIGNAL(timeout()), this, SLOT(keepContact()));
}

void GestionConnexion::connect() {
    if (m_connecting) return;
    m_connecting = true;
    win()->followNetworkReply(win()->client()->connect(
            m_userName,
            m_passwd,
            m_userAgent,
            "fr"), trUtf8("Logging in"));
}

void GestionConnexion::connected() {
    m_connecting = false;
    m_connected = true;
    win()->updateConnexionState();
    win()->showStatusMessage(trUtf8("Logged."));
}

void GestionConnexion::disconnect() {
    m_askForDisconnect++;
    if (m_deconnecting) return;
    m_deconnecting = true;
    m_userName.clear();
    win()->followNetworkReply(win()->client()->disconnect(), trUtf8("Disconnecting"), false);
    win()->subtitlesList()->clearSubtitlesData();
}

void GestionConnexion::disconnected() {
    m_askForDisconnect = 0;
    m_deconnecting = false;
    m_connected = false;
    win()->updateConnexionState();
    win()->showStatusMessage(trUtf8("Disconnected."));
    if (m_quitAfterDisconnect) win()->close();
}

void GestionConnexion::launchKeepContactTimer() {
    //le compte à rebours recommence à chaque requete
    m_keepContactTimer.stop();
    m_keepContactTimer.start(m_delaiContact);
}

void GestionConnexion::keepContact() {
    win()->followNetworkReply(win()->client()->keepContact(), trUtf8("Verifying server connection"), false);
}



/**
  * @brief Slot de réception d'une erreur de transmission / abandon.
  */
void GestionConnexion::rpcFault(int errn, const QString & err, QNetworkReply * reply) {
    OpenSubtitlesRPCClient::REQUEST_TYPE rtype = win()->m_client->replyType(reply);
    win()->ui->statusBar->clearMessage();
    switch (rtype) {
    case OpenSubtitlesRPCClient::CONNEXION:
    case OpenSubtitlesRPCClient::DECONNEXION:
    case OpenSubtitlesRPCClient::KEEP_CONTACT:
        m_connecting = false;
        m_connected = false;
        break;
    case OpenSubtitlesRPCClient::LISTE_SOUSTITRES:
        m_downloadByQueryAfterHash = false;
        break;
    default: break;
    }
    if (errn == QNetworkReply::OperationCanceledError) {
        // abandon utilisateur
        win()->ui->statusBar->showMessage(trUtf8("User interruption."));
        if (rtype == OpenSubtitlesRPCClient::COMMENTS_SOUSTITRES) {
            win()->ui->subtitleDetail->downloadAborted();
        }
    } else {
        if (rtype == OpenSubtitlesRPCClient::KEEP_CONTACT) connect();
        else QMessageBox::critical(win(), trUtf8("RPC reception error"), trUtf8("Error while communicating with opensubtitles.org:\n") + err);
    }
}

/**
  * @brief slot de réception de liste des sous-titres
  * @param data sous-titres recu
  */
void GestionConnexion::subtitlesListLoaded(const QList<QVariant> &subs) {
    win()->fillSubtitlesList(subs);
    if (m_autoDownloadComments) win()->ui->subtitleDetail->downloadSubtitlesComments(subs);
    if (m_downloadByQueryAfterHash) {
        if (!m_downloadByQueryOnlyIfEmpty || subs.isEmpty()) {
            QString fname = QFileInfo(m_lastQueryText).baseName();
            QList<QVariant> q = win()->client()->createSearchSubtitlesByFileName(fname, searchLangs()).toList();
            win()->searchSubtitles(q);
        }
        m_downloadByQueryAfterHash =  false;
    }
}

/**
  * @brief Slot de réception des sous-titres téléchargés.
  * @param data les sous-titres deja decompresses
  */
void GestionConnexion::subtitlesDownloaded(const QHash<QString, QByteArray> &hash) {

    QStringList errors;
    QDir downloadPath(m_downloadBaseDir);

    PropertiesDialog::DOWNLOAD_MODE downloadMode;
    switch(m_downloadMode) {
    case PropertiesDialog::ASK:
        downloadMode = QMessageBox::question(win(), trUtf8("Download subtitles"),
                                         trUtf8("Download subtitles near movie file ?\n"
                                                "Otherwise, it will be downloaded in %1.")
                                         .arg(downloadPath.absolutePath()),
                                         QMessageBox::Yes | QMessageBox::No,
                                         QMessageBox::Yes) == QMessageBox::Yes ?
        PropertiesDialog::NEAR_MOVIEFILE : PropertiesDialog::BASE_DIR;
        break;
    case PropertiesDialog::NEAR_MOVIEFILE: downloadMode = m_lastQueryMode == SearchDialog::BY_QUERY ?
                                                          PropertiesDialog::BASE_DIR : PropertiesDialog::NEAR_MOVIEFILE; break;
    default: downloadMode = PropertiesDialog::BASE_DIR; break;
    }

    QHash<QString, QByteArray>::const_iterator i;
    for (i = hash.constBegin(); i != hash.constEnd(); ++i) {
        const QString & subId = i.key();
        QByteArray subtileRealDataUncompressed = i.value();

        QMap<QString, QVariant> dataSub = win()->ui->subtitlesTableView->subtitlesData("IDSubtitleFile", subId);

        QString fname;
        if (downloadMode == PropertiesDialog::NEAR_MOVIEFILE) {
            QFileInfo qinfo(lastQueryText());
            fname = qinfo.absolutePath() + "/"
                    + qinfo.completeBaseName() + "."
                    + QFileInfo(dataSub.value("SubFileName").toString()).suffix();
        } else {
            fname = downloadPath.absoluteFilePath(dataSub.value("SubFileName").toString());
        }

#ifdef USE_ICU
        if (! m_reencode.isEmpty()) {
            // reencodage
            QByteArray out;
            UErrorCode err = charsetConvert(out, m_reencode, subtileRealDataUncompressed);
            if (err != U_ZERO_ERROR) {
                errors << trUtf8("[Conversion error] Unable to convert subtitle %1 to %2").arg(downloadPath.absoluteFilePath(fname)).arg(m_reencode);
            }
            else {
                subtileRealDataUncompressed = out;
            }
        }
#endif

        QFile f(fname);
        if (f.open(QIODevice::WriteOnly)) {
            f.write(subtileRealDataUncompressed);
            f.close();
        } else
            errors << trUtf8("[Save error] Unable to save the subtitle file %1").arg(downloadPath.absoluteFilePath(fname));
    }
    m_downloadByQueryAfterHash = false;
    if(!errors.isEmpty())
        QMessageBox::critical(win(), trUtf8("Downloaderror"), errors.join("\n"));
}

bool GestionConnexion::tryIsLogged() {
    bool res = isLogged();
    if (! res) QMessageBox::information(win(), trUtf8("Authorization error"), trUtf8("You have to be logged to OpenSubtitles server with an account(login / pass).\n"
                                                                                     "Please register on http://www.opensubtitles.org then fill your credentials in the properties dialog."));
    return res;
}

void GestionConnexion::commentSended() {
    QMessageBox::information(win(), trUtf8("Comment sended"), trUtf8("The comment has been send and will be available in a few for other users."));
}
