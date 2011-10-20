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

#ifndef COMMENTTEXT_H
#define COMMENTTEXT_H

#include <QPlainTextEdit>
#include <QVariant>
#include <QMap>
#include <QRegExp>
#include <QAction>

class SubtitleDetail : public QPlainTextEdit
{
Q_OBJECT
public:
    enum COMMENT_STATE {
        NO_COMMENTS,
        DOWNLOADING,
        DOWNLOADED,
        DOWNLOAD_ABORTED
    };

    explicit SubtitleDetail(QWidget *parent = 0);
    void downloadSubtitlesComments(const QList<QVariant> & subtitlesList);
    void writeComments(const QList<QMap<QString, QVariant> > & subtitle);
    void clearSubtitles();
    void downloadAborted();

    inline void setDetailContent(const QString & detail) { m_detailContent = detail; }
    inline QString detailContent() { return m_detailContent; }
    inline void enableLineWrap(bool lw) { m_actionSetLinedWrap->setChecked(lw); }
    inline bool isLineWrapEnabled() { return m_actionSetLinedWrap->isChecked(); }

protected:
    void contextMenuEvent(QContextMenuEvent *e);

private:
    QMap<QString, QPair<COMMENT_STATE, QVariant> > m_comments;
    void writeComment(QTextCursor & cursor, const QMap<QString, QVariant> & comment);
    QList<QMap<QString, QVariant> > m_currentSubtitle;
    bool m_optimizeDownloads;
    QRegExp m_regReplaceSubFields;
    void replaceSubtitlesField(QString & str, const QMap<QString, QVariant> &subtitle);
    QAction * m_actionSetLinedWrap;
    QString m_detailContent;
signals:

public slots:
    void commentsLoaded(const QMap<QString, QVariant> & comments);
    void subtitlesListSelectionChanged();
    void setLineWrap(bool ww);
};

#endif // COMMENTTEXT_H
