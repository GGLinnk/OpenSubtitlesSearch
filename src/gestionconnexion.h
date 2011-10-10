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

#ifndef GESTIONCONNEXION_H
#define GESTIONCONNEXION_H

#include <QObject>
#include <QTimer>
#include <QHash>

class QNetworkReply;
class MainWindow;

class GestionConnexion : public QObject
{
Q_OBJECT
public:
    explicit GestionConnexion(MainWindow * parent);

    inline bool isConnected() { return m_connected; }
    inline void setQuitAfterDisconnect(bool quit) { m_quitAfterDisconnect = quit; }
    inline QString userName() { return m_userName; }
    inline bool isLogged() { return m_connected && ! m_userName.isEmpty(); }
    inline int askForDisconnect() { return m_askForDisconnect; }
    inline MainWindow * win() { return (MainWindow *) parent(); }
    inline void setCredentials(const QString & name, const QString & passwd) { m_userName = name; m_passwd = passwd; }
    inline void setAppUserAgent(const QString & ua) { m_userAgent = ua; }

    inline void setDownloadByQueryAfterHash(bool onlyIfEmpty) {
        m_downloadByQueryAfterHash = true;
        m_downloadByQueryOnlyIfEmpty = onlyIfEmpty;
    }

    inline void setLastQuery(int mode, const QString & txt) { m_lastQueryMode = mode; m_lastQueryText = txt; }
    inline QString lastQueryText() { return m_lastQueryText; }
    inline int lastQueryMode() { return m_lastQueryMode; }

    inline void setDownloadMode(int mode) {
        m_downloadMode = mode;
    }

    bool tryIsLogged();

    inline void setSearchLangs(const QString & langs) { m_searchLangs = langs; }
    inline QString searchLangs() { return m_searchLangs; }

    inline void setDownloadBaseDir(const QString & bd) { m_downloadBaseDir = bd; }
    inline QString downloadBaseDir() { return m_downloadBaseDir; }

    inline void setReencode(const QString & enc) { m_reencode = enc; }
    inline QString reencode() { return m_reencode; }

    inline void setAutoDownloadComments(bool on) { m_autoDownloadComments = on; }
    inline bool autoDownloadComments() { return m_autoDownloadComments; }

public slots:
    void connect();
    void disconnect();

    void rpcFault(int, const QString &, QNetworkReply * reply);
    void subtitlesListLoaded(const QList<QVariant> & liste);
    void subtitlesDownloaded(const QHash<QString, QByteArray> & liste);
    void commentSended();

signals:
    void subtitleSavedOnDisk(const QString & subId, const QString & fname, const QString & movieName);

private slots:
    void connected();
    void disconnected();

    void launchKeepContactTimer();
    void keepContact();

private:
    bool m_connected;
    bool m_connecting, m_deconnecting;
    bool m_quitAfterDisconnect;
    QTimer m_keepContactTimer;
    int m_delaiContact;
    QString m_userName;
    QString m_passwd;
    QString m_userAgent;
    int m_askForDisconnect;

    QString m_lastQueryText;
    int     m_lastQueryMode;
    int     m_downloadMode;
    bool    m_downloadByQueryOnlyIfEmpty;
    bool    m_downloadByQueryAfterHash;

    QString m_searchLangs;
    QString m_downloadBaseDir;
    bool m_autoDownloadComments;
#ifdef USE_ICU
    QString m_reencode;
#endif
};

#endif // GESTIONCONNEXION_H
