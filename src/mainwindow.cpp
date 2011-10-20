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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "propertiesdialog.h"
#include <QCloseEvent>
#include <QTimer>
#include <QHeaderView>
#include <QNetworkReply>
#include <QDir>
#include <QLineEdit>
#include <QMessageBox>
#include "networkreplydownloaddelegate.h"
#include "networkreplydownloadtablemodel.h"
#include "aproposdialog.h"
#include "updatemoviehashdialog.h"
#include "addcommentdialog.h"
#include "settings.h"
#include <QNetworkProxy>

MainWindow * MainWindow::_instance = NULL;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_client(new OpenSubtitlesRPCClient(this)),
    m_gestionConnexion(this),
    m_networkReplyModel(new NetworkReplyDownloadTableModel(this))
{
    ui->setupUi(this);
    _instance = this;

    QMenu * viewMenu = createPopupMenu();
    viewMenu->setTitle(trUtf8("Views"));
    viewMenu->setParent(this);
    ui->menuBar->insertMenu(ui->menuAide->menuAction(), viewMenu);

    setAcceptDrops(true);
    // init gestion connexion
    connect(m_client, SIGNAL(connected(QNetworkReply*)), &m_gestionConnexion, SLOT(connected()));
    connect(m_client, SIGNAL(disconnected(QNetworkReply*)), &m_gestionConnexion, SLOT(disconnected()));
    connect(ui->actionConnexion, SIGNAL(triggered()), &m_gestionConnexion, SLOT(connect()));
    connect(ui->actionD_connexion, SIGNAL(triggered()), &m_gestionConnexion, SLOT(disconnect()));
    updateConnexionState();
    Settings settings;
    m_gestionConnexion.setAppUserAgent(settings.value(Settings::USERAGENT).toString());
    m_gestionConnexion.setCredentials(
            settings.value(Settings::USER).toString(),
            settings.value(Settings::PASSWD).toString()
            );
    m_gestionConnexion.setAutoDownloadComments(settings.value(Settings::SUBDETAIL_AUTOCOMMENT).toBool());

    ui->networkReplytableView->setModel(m_networkReplyModel);
    ui->networkReplytableView->setItemDelegate(new NetworkReplyDownloadDelegate(this));

    connect(m_client, SIGNAL(error(int,QString,QNetworkReply*)), &m_gestionConnexion, SLOT(rpcFault(int,QString, QNetworkReply *)));
    connect(m_client, SIGNAL(subtitlesListLoaded(QList<QVariant>,QNetworkReply*)), &m_gestionConnexion, SLOT(subtitlesListLoaded(QList<QVariant>)));
    connect(m_client, SIGNAL(subtitlesDownloaded(QHash<QString,QByteArray>,QNetworkReply*)), &m_gestionConnexion, SLOT(subtitlesDownloaded(QHash<QString,QByteArray>)));
    connect(m_client, SIGNAL(commentSended(QNetworkReply*)), &m_gestionConnexion, SLOT(commentSended()));
    connect(m_client, SIGNAL(commentsLoaded(QMap<QString,QVariant>,QNetworkReply*)), ui->subtitleDetail, SLOT(commentsLoaded(QMap<QString,QVariant>)));

    connect(&m_gestionConnexion, SIGNAL(subtitleSavedOnDisk(QString,QString,QString)), m_networkReplyModel, SLOT(subtitleSavedOnDisk(QString,QString,QString)));

    ui->subtitlesTableView->setContextMenu(ui->menuSous_titres);

    connect(ui->subtitlesTableView, SIGNAL(activated(QModelIndex)), this, SLOT(rawDownloadCurrentSubtitle()));


    restoreState(settings.value(Settings::MWIN_STATE).toByteArray());
    resize(settings.value(Settings::MWIN_SIZE).toSize());
    m_searchDialogSize = settings.value(Settings::SEARCH_DIALOG_SIZE).toSize();
    reloadProperties(&settings);
    resizeSubtitlesList();

    connect(ui->subtitlesTableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), ui->subtitleDetail, SLOT(subtitlesListSelectionChanged()));

    if (settings.value(Settings::AUTOCONNECT).toBool()) m_gestionConnexion.connect();
}

void MainWindow::reloadProperties(Settings *settings) {
    m_networkReplyModel->setDeleteNetworkReplyAfter(settings->value(Settings::SHOWDOWNLOADSONCEDONE).toDouble() * 1000);
    ui->subtitlesTableView->setColumns(settings->value(Settings::SUBS_COLS).toStringList());
    m_gestionConnexion.setDownloadMode(settings->value(Settings::SUBS_DOWNLOADMODE).toInt());
    m_gestionConnexion.setSearchLangs(settings->value(Settings::SUBS_SEARCHLANGS).toString());
    m_gestionConnexion.setDownloadBaseDir(settings->value(Settings::SUBS_DOWNLOADDIR).toString());
    m_gestionConnexion.setAutoDownloadComments(settings->value(Settings::SUBDETAIL_AUTOCOMMENT).toBool());
    ui->subtitleDetail->setDetailContent(settings->value(Settings::SUBDETAIL_NORMAL).toString());
#ifdef USE_ICU
    m_gestionConnexion.setReencode(settings->value(Settings::SUBS_REENCODE).toString());
#endif
    // (re)define proxy
    QNetworkProxy proxy;
    if (settings->value(Settings::PROXY_USED).toBool()) {
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(settings->value(Settings::PROXY_HOST).toString());
        proxy.setPort(settings->value(Settings::PROXY_PORT).toInt());
        proxy.setUser(settings->value(Settings::PROXY_USER).toString());
        proxy.setPassword(settings->value(Settings::PROXY_PASSWD).toString());
    }
    QNetworkProxy::setApplicationProxy(proxy);
    ui->subtitleDetail->enableLineWrap(settings->value(Settings::SUBDETAIL_WRAPLINE).toBool());
    ui->subtitlesTableView->setMovieLink(settings->value(Settings::MOVIELINK).toString());
}

MainWindow::~MainWindow()
{
    Settings settings;
    settings.setValue(Settings::MWIN_STATE, saveState());
    settings.setValue(Settings::MWIN_SIZE, size());
    settings.setValue(Settings::SUBS_SEARCHLANGS, m_gestionConnexion.searchLangs());
    settings.setValue(Settings::SUBS_DOWNLOADDIR, m_gestionConnexion.downloadBaseDir());
    settings.setValue(Settings::SUBS_REENCODE, m_gestionConnexion.reencode());
    settings.setValue(Settings::SUBDETAIL_AUTOCOMMENT, m_gestionConnexion.autoDownloadComments());
    settings.setValue(Settings::SUBDETAIL_NORMAL, ui->subtitleDetail->detailContent());
#ifdef USE_ICU
    settings.setValue(Settings::SUBS_REENCODE, m_gestionConnexion.reencode());
#endif
    settings.setValue(Settings::SEARCH_DIALOG_SIZE, m_searchDialogSize);
    settings.setValue(Settings::SUBDETAIL_WRAPLINE, ui->subtitleDetail->isLineWrapEnabled());
    delete ui;
}

void MainWindow::showStatusMessage(const QString &message, bool for_ever) {
    ui->statusBar->showMessage(message, for_ever ? 0 : 2000);
}

void MainWindow::followNetworkReply(QNetworkReply *reply, const QString &message, bool canCancel) {
    m_networkReplyModel->addNetWorkReply(reply, message + " (%1)", canCancel);
    ui->statusBar->showMessage(message);
}

void MainWindow::searchSubtitles(QList<QVariant> &args) {
    followNetworkReply(client()->searchSubtitles(args),
                            trUtf8("Looking for subtitles"));
}

SubtitlesView * MainWindow::subtitlesList() {
    return ui->subtitlesTableView;
}

void MainWindow::fillSubtitlesList(const QList<QVariant> &subs) {
    ui->subtitlesTableView->addSubtitlesData(subs);
    ui->statusBar->showMessage(trUtf8("Sutitles loaded."), 2000);
    ui->subtitleDetail->clearSubtitles();
    ui->actionD_tail->setEnabled(!subs.isEmpty());
    QTimer::singleShot(200, this, SLOT(resizeSubtitlesList()));
}

void MainWindow::updateConnexionState() {
    QString title = "OpenSubtitlesSearch";
    bool connected = m_gestionConnexion.isConnected();
    if (connected) {
        title += trUtf8(" - connected");
        if (m_gestionConnexion.isLogged()) title += QString(" (%1)").arg(m_gestionConnexion.userName());
    }
    setWindowTitle(title);
    ui->actionSearch->setEnabled(connected);
    ui->actionD_connexion->setEnabled(connected);
    ui->actionConnexion->setEnabled(! connected);
}


/**
  * @brief Requète de téléchargement de sous-titres. Télécharge la selection courante.
  */
void MainWindow::rawDownloadCurrentSubtitle() {
    QStringList subIds = ui->subtitlesTableView->selectedSubtitlesValue("IDSubtitleFile");
    if (subIds.isEmpty()) {
        ui->statusBar->showMessage(trUtf8("Nothing to download."));
        return;
    }
    QNetworkReply * reply = m_client->downloadSubtitles(subIds);
    followNetworkReply(reply, trUtf8("Downloading subtitles"));
}



void MainWindow::showDetail() {
    ui->subtitleDetail->downloadSubtitlesComments(ui->subtitlesTableView->subtitlesData());
    if (ui->dockWidgetDetail->isHidden()) ui->dockWidgetDetail->show();
    ui->dockWidgetDetail->raise();
}

/**
  * @brief Force la déconnexion du serveur avant la fermeture.
  */
void MainWindow::closeEvent(QCloseEvent *e) {
    if (! m_gestionConnexion.isConnected()) e->accept();
    else {
        m_gestionConnexion.setQuitAfterDisconnect(true);
        m_gestionConnexion.disconnect();
        if (m_gestionConnexion.askForDisconnect() >= 3) {
            // demande de forcage de deconnexion
            if (QMessageBox::question(this, trUtf8("Quit"),
                                  trUtf8("The application is not diconnected from server.\nDiconnect anyway ?"),
                                  QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) {
                e->accept();
                return;
            }
        }
        e->ignore();
    }
}



/**
  * @brief Lance le dialogue de propriétés.
  */
void MainWindow::editProperties() {
    PropertiesDialog dialog(this);
    Settings settings;
    dialog.load(&settings);
    if (dialog.exec() == QDialog::Accepted) {
        dialog.save(&settings);
        reloadProperties(&settings);
        resizeSubtitlesList();
    }
}

void MainWindow::showAPropos() {
    AProposDialog d(this);
    d.exec();
}

void MainWindow::showUpdateMovieHash() {
    if (!m_gestionConnexion.tryIsLogged()) return;
    UpdateMovieHashDialog dialog(this);
    QList<QMap<QString, QVariant> > subs;
    QModelIndexList indexes = ui->subtitlesTableView->selectionModel()->selectedRows(0);
    foreach (const QModelIndex & index, indexes) {
        subs << ui->subtitlesTableView->subtitlesData(index);
    }
    QString err;
    if (! dialog.setSubtitles(subs, &err)) {
        QMessageBox::critical(this, trUtf8("Unable to associate the selected subtitles"), err);
    } else {
        connect(client(), SIGNAL(tryUpSubtitlesAlreadyExists(QMap<QString,QVariant>,QNetworkReply*)), &dialog, SLOT(updateHashReceived(QMap<QString,QVariant>)));
        dialog.exec();
    }
}

/**
  * @brief Redimensionne la liste des sous-titres après réception.
  */
void MainWindow::resizeSubtitlesList() {
    ui->subtitlesTableView->resizeColumnsToContents();
    ui->subtitlesTableView->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls() || event->mimeData()->hasText()) {
        event->acceptProposedAction();
    }
}

/**
  * @brief lancer automatiquement les dialogues de recherche de sus titres
  */
void MainWindow::dropEvent(QDropEvent *event) {
    if (!m_gestionConnexion.isConnected()) {
        QMessageBox::critical(this, trUtf8("Error"), trUtf8("The application is not connected to opensubtitles server."));
    } else {
        if (event->mimeData()->hasUrls()) {
            QUrl url = event->mimeData()->urls().first();
            showSearchDialog(SearchDialog::BY_HASH, url.toLocalFile());
        } else if (event->mimeData()->hasText()) {
            showSearchDialog(SearchDialog::BY_QUERY, event->mimeData()->text());
        }
        event->acceptProposedAction();
    }
}

void MainWindow::showSearchDialog(SearchDialog::MODE m, const QString & text) {
    SearchDialog d(this);
    d.resize(m_searchDialogSize);
    d.setSelectedLangs(m_gestionConnexion.searchLangs());
    if (m != SearchDialog::NONE) d.setMode(m);
    d.setText(text);
    if (d.exec() == QDialog::Accepted) {
        SearchWidget * w = d.searchWidget();
        m_gestionConnexion.setLastQuery((int) d.mode(), w->queryText());
        if (d.mode() == SearchDialog::BY_MIXED) {
            m_gestionConnexion.setDownloadByQueryAfterHash(((SearchByMixed *)w)->downloadOnlyIfEmpty());
        }
        ui->subtitlesTableView->clearSubtitlesData();
        QString langs = d.selectedLangs();
        QList<QVariant> args = w->createSearchArgs(client(), langs);
        followNetworkReply(client()->searchSubtitles(args), trUtf8("Looking for subtitles"));
        m_gestionConnexion.setSearchLangs(langs);
    }
    m_searchDialogSize = d.size();
}

void MainWindow::showAddComment() {
    if (! m_gestionConnexion.tryIsLogged()) return;
    QMap<QString, QVariant> subData = ui->subtitlesTableView->subtitlesData(ui->subtitlesTableView->currentIndex());
    AddCommentDialog dlg(subData, this);
    if (dlg.exec() == QDialog::Accepted) {
        QNetworkReply * reply = client()->sendComment(dlg.idSubtitle(), dlg.commentSubtitle(), dlg.isBadSubtitle());
        followNetworkReply(reply, trUtf8("Sending comment for subtitle ") + dlg.idSubtitle());
    }
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
