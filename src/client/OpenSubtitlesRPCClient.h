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

#ifndef OPENSUBTITLES_RPC_CLIENT_H
#define OPENSUBTITLES_RPC_CLIENT_H

#include <QObject>
#include <QDomElement>
#include <QVariant>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QHash>
#include <QList>
#include <QStringList>
#include <QRegExp>
#include <QFile>

class QNetworkReply;

class OpenSubtitlesRPCClient : public QObject {
	Q_OBJECT

public:
    enum REQUEST_TYPE {
        ERROR, // not a real request type
        CONNEXION,
        DECONNEXION,
        LISTE_SOUSTITRES,
        DOWNLOAD_SOUS_TITRES,
        KEEP_CONTACT,
        COMMENTS_SOUSTITRES,
        TRY_UPLOAD_SUBTITLES,
        SEND_COMMENT
    };
    
    OpenSubtitlesRPCClient(QObject* parent = 0);
    inline void setUrl(QUrl url) { m_request.setUrl(url); }
    inline void setUserAgent(QString userAgent) { m_request.setRawHeader("User-Agent", userAgent.toAscii()); }
    inline const QNetworkAccessManager & networkManager() { return m_manager; }
    
    REQUEST_TYPE replyType(QNetworkReply * reply);
    
    QNetworkReply *  connect(const QString & user, const QString & passwd, const QString & userAgent, const QString & language);
    QNetworkReply *  disconnect();
    QVariant createSearchSubtitlesByFileName(const QString & fname, const QString & langs = "");
    QVariant createSearchSubtitlesByHash(QFile * file, const QString & langs = "");
    QNetworkReply * searchSubtitles(QList<QVariant> & searchMap);
    inline QNetworkReply * searchSubtitlesByFileName(const QString & fname, const QString & langs = "") {
        return searchSubtitles(QList<QVariant>() << createSearchSubtitlesByFileName(fname, langs));
    }
    inline QNetworkReply * searchSubtitlesByHash(QFile * f, const QString & langs = "") {
        return searchSubtitles(QList<QVariant>() << createSearchSubtitlesByHash(f, langs));
    }
    QNetworkReply * downloadSubtitles(const QStringList & subIds);
    inline QNetworkReply * downloadSubtitle(const QString & subId) { return downloadSubtitles(QStringList() << subId); }
    QNetworkReply * keepContact();
    QNetworkReply * comments(const QStringList & subIds);
    QVariant createTryUploadSubtitlesCd(const QString & subfname, const QString & subHash, QFile * movie,
                                        const QString & movieTimeMS, const QString & movieFrames,
                                        const QString & movieFps);
    QNetworkReply * tryUploadSubtitles(const QList<QVariant> & cds);
    QNetworkReply * sendComment(const QString & idSubtitle, const QString & comment, bool badComment);
    
protected slots:
    void replyFinished(QNetworkReply*);
    
signals:
    void error(int errn, const QString & message, QNetworkReply*);
    void connected(QNetworkReply*);
    void disconnected(QNetworkReply*);
    void subtitlesListLoaded(const QList<QVariant> & liste, QNetworkReply*);
    void subtitlesDownloaded(const QHash<QString, QByteArray> & liste, QNetworkReply*);
    void commentsLoaded(const QMap<QString, QVariant> & comments, QNetworkReply*);
    void tryUpSubtitlesAlreadyExists(const QMap<QString, QVariant> & liste, QNetworkReply *);
    void tryUpSubtitlesNotExists(const QList<QVariant> & lst, QNetworkReply *);
    void commentSended(QNetworkReply*);
    
protected:
    static QDomElement toXml(const QVariant & arg);
    static QVariant fromXml(const QDomElement &elem);
    static QRegExp REGEXP_OS_STATUS;
    QNetworkReply* call(const QString & method, const QList<QVariant> & args, REQUEST_TYPE rtype);
    void emitResponse(QNetworkReply* reply, REQUEST_TYPE rtype, const QMap<QString, QVariant> & ans);
    QNetworkAccessManager m_manager;
    QNetworkRequest m_request;
    QHash<QNetworkReply*, REQUEST_TYPE> m_callmap;
    QString m_sessionId;
};

#endif // OPENSUBTITLES_RPC_CLIENT_H
