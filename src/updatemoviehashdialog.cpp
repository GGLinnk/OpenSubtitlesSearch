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

#include "updatemoviehashdialog.h"
#include "ui_updatemoviehashdialog.h"
#include "updatemoviehashform.h"
#include "mainwindow.h"
#include <QRegExpValidator>
#include <QPushButton>
#include <QMessageBox>
#include <QCloseEvent>
#include <QNetworkReply>

UpdateMovieHashDialog::UpdateMovieHashDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UpdateMovieHashDialog),
    m_currentReply(NULL)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

UpdateMovieHashDialog::~UpdateMovieHashDialog()
{
    delete ui;
}

bool UpdateMovieHashDialog::setSubtitles(const QList<QMap<QString, QVariant> > &subs, QString *err) {
    Q_ASSERT(!subs.empty());
    ui->tabWidget->clear();
    QMap<QString, QVariant> sub;
    QList<QMap<QString, QVariant> > ordered;
    // order subtitle selection
    foreach (sub, subs) {
        int pos = sub.value("SubActualCD").toInt() -1;
        if (pos >= ordered.count()) ordered << sub;
        else ordered.insert(pos, sub);
    }

    // verification of the selection
    QString mName = ordered.first().value("MovieName").toString();
    QString mNameEng = ordered.first().value("MovieNameEng").toString();
    int i = 1;
    foreach (sub, ordered) {
        if ((sub.value("MovieName").toString() != mName) || (sub.value("MovieNameEng").toString() != mNameEng)) {
            // movie names don't match
            if (err) *err = trUtf8("Selection must not contains different movie's name.");
            return false;
        }
        if (sub.value("SubActualCD").toInt() != i) {
            // seems a cd is missing for the movie
            if (err) *err = trUtf8("Incomplete selection. CD %1 is missing.").arg(i);
            return false;
        }
        if (sub.value("SubSumCD").toInt() != ordered.count()) {
            // seems selection is not for the same moovie
            if (err) *err = trUtf8("The number of cds is inconsistent.");
            return false;
        }
        i++;
    }

    QValidator * v = new QRegExpValidator(QRegExp("\\d*"), this);
    foreach (sub, ordered) {
        UpdateMovieHashForm * form = new UpdateMovieHashForm(this, SLOT(validate()), this);
        form->setSubtitle(sub);
        form->setLineNumberValidator(v);
        ui->tabWidget->addTab(form, QIcon(), QString("%1/%2").arg(
                sub.value("SubActualCD").toString()).arg(sub.value("SubSumCD").toString()));
    }
    ui->tabWidget->setCurrentIndex(0);
    return true;
}

void UpdateMovieHashDialog::sendUpdateHash() {
    if (m_currentReply) return;
    if (QMessageBox::question(this, windowTitle(),
                          trUtf8("Associate movies with subtitles ?"),
                          QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No) return;
    QList<QVariant> cds;
    for (int i = 0; i < ui->tabWidget->count(); i++) {
        cds << ((UpdateMovieHashForm *) ui->tabWidget->widget(0))->createUpdateHashArgs();
    }
    MainWindow * win = MainWindow::instance();
    m_currentReply = win->client()->tryUploadSubtitles(cds);
    connect(m_currentReply, SIGNAL(finished()), this, SLOT(replyFinished()));
    win->followNetworkReply(m_currentReply, trUtf8("Asking association movies hash / subtitles"));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

void UpdateMovieHashDialog::updateHashReceived(const QMap<QString, QVariant> &infos) {
    bool newFname = infos.value("MoviefilenameWasAlreadyInDb").toString() == "0";
    bool newHash = infos.value("HashWasAlreadyInDb").toString() == "0";
    QString message = newHash ? trUtf8("Association movies hash / subtitles OK. It will be visible to any user within 24h.") :
                      trUtf8("Association movies hash / subtitles was existing. No update.");
    message += '\n';
    message += newFname ? trUtf8("Movie name has been updated.") : QString();
    QMessageBox::information(this, windowTitle(), message);
    disconnect(m_currentReply, SIGNAL(finished()), this, SLOT(replyFinished()));
    m_currentReply = NULL;
    close();
}

void UpdateMovieHashDialog::replyFinished() {
    if (m_currentReply->error() != QNetworkReply::NoError) {
        QMessageBox::critical(this, windowTitle(),
                              trUtf8("Transaction error."));
    }
    m_currentReply = NULL;
}

void UpdateMovieHashDialog::validate() {
    QPushButton * btnok = ui->buttonBox->button(QDialogButtonBox::Ok);
    bool allValid = true;
    for (int i = 0; i < ui->tabWidget->count(); i++) {
        UpdateMovieHashForm * form = (UpdateMovieHashForm *) ui->tabWidget->widget(i);
        if (! form->isValid()) {
            allValid = false;
            break;
        }
    }
    if (btnok->isEnabled() != allValid) {
        btnok->setEnabled(allValid);
    }
}

void UpdateMovieHashDialog::closeEvent(QCloseEvent * e) {
    if (m_currentReply) {
        int rep = QMessageBox::question(this, windowTitle(),
                              trUtf8("Work in progress...\nStop it ?"),
                              QMessageBox::Yes | QMessageBox::No |QMessageBox::Cancel, QMessageBox::Cancel);
        switch (rep) {
        case QMessageBox::Cancel: e->ignore(); break;
        case QMessageBox::Yes: m_currentReply->close(); m_currentReply = NULL; e->accept(); break;
        case QMessageBox::No: e->accept(); break;
        }
    }
    e->accept();
}

void UpdateMovieHashDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
