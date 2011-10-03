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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "client/OpenSubtitlesRPCClient.h"
#include "gestionconnexion.h"
#include "subtitlesview.h"
#include "searchdialog.h"

class Settings;
class NetworkReplyDownloadTableModel;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    static MainWindow * instance() { return _instance; }

    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    inline OpenSubtitlesRPCClient * client() { return m_client; }

    void showStatusMessage(const QString & message, bool for_ever = false);

    void followNetworkReply(QNetworkReply * reply, const QString & message, bool canCancel = true);

    void updateConnexionState();

    void searchSubtitles(QList<QVariant> & args);

    SubtitlesView * subtitlesList();

    void fillSubtitlesList(const QList<QVariant> & subs);

public slots:

    void editProperties();
    void showSearchDialog(SearchDialog::MODE m = SearchDialog::NONE, const QString & text = QString());
    void rawDownloadCurrentSubtitle();
    void showDetail();
    void showAPropos();
    void showUpdateMovieHash();
    void showAddComment();

    void resizeSubtitlesList();

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *e);
    void dragEnterEvent(QDragEnterEvent *);
    void dropEvent(QDropEvent *);
    void reloadProperties(Settings * settings);

private:
    static MainWindow * _instance;
    Ui::MainWindow *ui;
    OpenSubtitlesRPCClient * m_client;
    GestionConnexion m_gestionConnexion;
    NetworkReplyDownloadTableModel * m_networkReplyModel;
    QSize m_searchDialogSize;

    friend class GestionConnexion;
};

#endif // MAINWINDOW_H
