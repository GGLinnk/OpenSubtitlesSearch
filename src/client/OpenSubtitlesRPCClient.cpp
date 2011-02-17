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

#include "OpenSubtitlesRPCClient.h"
#include "OpenSubtitlesHelper.h"
#include <QFileInfo>
#include <QDebug>

void OpenSubtitlesRPCClient::emitResponse(QNetworkReply* reply, REQUEST_TYPE rtype, const QMap<QString, QVariant> &ans) {
    switch (rtype) {
        case CONNEXION:
            m_sessionId = ans.value("token").toString();
            emit connected(reply);
            break;
        case DECONNEXION:
            m_sessionId = "";
            emit disconnected(reply);
            break;
        case LISTE_SOUSTITRES:
            {
                QList<QVariant> liste = ans.value("data").toList();
                emit subtitlesListLoaded(liste, reply);
            }
            break;
        case DOWNLOAD_SOUS_TITRES:
            {
                QList<QVariant> subs = ans.value("data").toList();
                QHash<QString, QByteArray> result;
                QString subId;
                int iResUnGzip = 0;
                foreach (QVariant sub, subs) {
                    QMap<QString, QVariant> subReceivedData = sub.toMap();
                    subId = subReceivedData.value("idsubtitlefile").toString();
                    QByteArray subtileRawData = QByteArray::fromBase64(subReceivedData.value("data").toString().toLatin1());
                    QByteArray subtileRealDataUncompressed;
                    iResUnGzip = ungzip(subtileRawData, subtileRealDataUncompressed);
                    if (iResUnGzip != 0) {
                        break;
                    }
                    result.insert(subId, subtileRealDataUncompressed);
                }
                if (iResUnGzip != 0) {
                    emit error(1300, trUtf8("Error %1 while ungzipping %2.").arg(iResUnGzip).arg(subId), reply);
                } else {
                    emit subtitlesDownloaded(result, reply);
                }
            }
            break;
        case COMMENTS_SOUSTITRES:
            {

                QMap<QString, QVariant> data = ans.value("data").toMap();
                QMap<QString, QVariant> result;
                QMapIterator<QString, QVariant> it(data);
                while (it.hasNext()) {
                    it.next();
                    result.insert(it.key().mid(1), it.value());
                }
                emit commentsLoaded(result, reply);
            }
            break;
        case TRY_UPLOAD_SUBTITLES:
            if (ans.value("alreadyindb").toString().toInt() == 1) {
                emit tryUpSubtitlesAlreadyExists(ans.value("data").toMap(), reply);
            } else {
                emit tryUpSubtitlesNotExists(ans.value("data").toList(), reply);
            }
            break;
        case SEND_COMMENT:
            emit commentSended(reply);
            break;
        default: break;
    }
}

QNetworkReply *  OpenSubtitlesRPCClient::connect(const QString & user, const QString & passwd, const QString & userAgent, const QString & language) {
    QList<QVariant> args;
    args << user << passwd << language << userAgent;
    return call("LogIn", args, CONNEXION);
}

QNetworkReply *  OpenSubtitlesRPCClient::disconnect() {
    QList<QVariant> args;
    args << m_sessionId;
    return call("LogOut", args, DECONNEXION);
}

QVariant OpenSubtitlesRPCClient::createSearchSubtitlesByFileName(const QString &fname, const QString &langs) {
    QMap<QString, QVariant> searchMap;
    searchMap.insert("sublanguageid", langs);
    searchMap.insert("query", fname);
    return QVariant(searchMap);
}

QVariant OpenSubtitlesRPCClient::createSearchSubtitlesByHash(QFile * file, const QString &langs) {
    QMap<QString, QVariant> searchMap;
    searchMap.insert("sublanguageid", langs);
    searchMap.insert("moviebytesize", QString::number(file->size()));
    searchMap.insert("moviehash", compute_hash(file));
    return QVariant(searchMap);
}

QNetworkReply * OpenSubtitlesRPCClient::searchSubtitles(QList<QVariant> &searchList) {
    QList<QVariant> args;
    args << m_sessionId << QVariant(searchList);
    return call("SearchSubtitles", args, LISTE_SOUSTITRES);
}

QNetworkReply * OpenSubtitlesRPCClient::downloadSubtitles(const QStringList &subIds) {
    QList<QVariant> rsubIds;
    foreach (QString id, subIds) rsubIds << QVariant(id);
    QList<QVariant> args;
    args << m_sessionId;
    args << QVariant(rsubIds);
    return call("DownloadSubtitles", args, DOWNLOAD_SOUS_TITRES);
}

QNetworkReply * OpenSubtitlesRPCClient::keepContact() {
    QList<QVariant> args;
    args << m_sessionId;
    return call("NoOperation", args, KEEP_CONTACT);
}

QNetworkReply * OpenSubtitlesRPCClient::comments(const QStringList &subIds) {
    QList<QVariant> rsubIds;
    foreach (QString id, subIds) rsubIds << QVariant(id);
    QList<QVariant> args;
    args << m_sessionId;
    args << QVariant(rsubIds);
    return call("GetComments", args, COMMENTS_SOUSTITRES);
}

QVariant OpenSubtitlesRPCClient::createTryUploadSubtitlesCd(const QString &subfname, const QString &subHash, QFile *movie, const QString &movieTimeMS, const QString &movieFrames, const QString &movieFps) {
    QMap<QString, QVariant> map;
    map.insert("subhash", subHash);
    map.insert("subfilename", subfname);
    map.insert("moviefilename", QFileInfo(*movie).fileName());
    map.insert("moviebytesize", QString::number(movie->size()));
    map.insert("moviehash", compute_hash(movie));
    if (!movieFrames.isEmpty()) map.insert("movieframes", movieFrames);
    if (!movieFps.isEmpty()) map.insert("moviefps", movieFps);
    if (!movieTimeMS.isEmpty()) map.insert("movietimems", movieTimeMS);
    return QVariant(map);
}

QNetworkReply * OpenSubtitlesRPCClient::tryUploadSubtitles(const QList<QVariant> &cds) {
    QList<QVariant> args;
    QMap<QString, QVariant> cdsMaps;
    for (int i = 0; i < cds.count(); i++) {
        cdsMaps.insert(QString("cd%1").arg(i + 1), cds.at(i));
        qDebug() << cds.at(i);
    }
    args << m_sessionId << QVariant(cdsMaps);
    return call("TryUploadSubtitles", args, TRY_UPLOAD_SUBTITLES);
}

QNetworkReply * OpenSubtitlesRPCClient::sendComment(const QString &idSubtitle, const QString &comment, bool badComment){
    QMap<QString, QVariant> map;
    map.insert("idsubtitle", idSubtitle);
    map.insert("comment", comment);
    if (badComment) map.insert("badsubtitle", 1);
    QList<QVariant> args;
    args << m_sessionId;
    args << QVariant(map);
    return call("AddComment", args, SEND_COMMENT);
}
