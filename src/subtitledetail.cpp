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

#include "subtitledetail.h"
#include "mainwindow.h"
#include "columnssubtitleslistmodel.h"
#include <QMenu>
#include <QDateTime>

SubtitleDetail::SubtitleDetail(QWidget *parent) :
    QPlainTextEdit(parent),
    m_optimizeDownloads(false)
{
    QStringList patt;
    foreach (const QString & c, ColumnsSubtitlesListModel::ALL_COLUMNS) {
        patt << QRegExp::escape(QString("$%1").arg(c));
    }
    m_regReplaceSubFields.setPattern(QString("(%1)").arg(patt.join("|")));

    m_actionSetLinedWrap = new QAction(trUtf8("Enable line wrapping"), this);
    m_actionSetLinedWrap->setCheckable(true);
    m_actionSetLinedWrap->setChecked(false);
    connect(m_actionSetLinedWrap, SIGNAL(toggled(bool)), this, SLOT(setLineWrap(bool)));
}

void SubtitleDetail::contextMenuEvent(QContextMenuEvent *e) {
    QMenu * menu = createStandardContextMenu();
    menu->addAction(m_actionSetLinedWrap);
    menu->exec(e->globalPos());
    delete menu;
}

void SubtitleDetail::replaceSubtitlesField(QString &str, const QMap<QString, QVariant> &subtitle) {
    int pos = 0;
    while ( (pos = m_regReplaceSubFields.indexIn(str, pos)) != -1 ) {
        QString key = m_regReplaceSubFields.cap(1).mid(1);
        QString value = subtitle.value(key).toString();
        if (value.isNull()) continue;
        str.replace(pos, key.length() + 1, value);
        pos += value.length();
    }
}

void SubtitleDetail::downloadSubtitlesComments(const QList<QVariant> &subtitlesList) {
    QStringList lst;
    foreach (const QVariant & sub, subtitlesList) {
        QMap<QString, QVariant> subInfo = sub.toMap();
        QString key = subInfo.value("IDSubtitle").toString();
        if (m_comments.contains(key)) {
            // on a deja vu le sub. s'il a été annulé, on le reprends
            COMMENT_STATE state = m_comments.value(key).first;
            if (state != DOWNLOAD_ABORTED) continue;
        } else {
            // jamais vu. il possede des ssitres, dapres les infos actuelles ?
            // si non et si optimize vaut vrai, on télécharge pas
            if (m_optimizeDownloads && subInfo.value("SubComments").toString().toInt() == 0) {
                QPair<COMMENT_STATE, QVariant> comment;
                comment.first = NO_COMMENTS;
                m_comments.insert(key, comment);
                continue;
            }
        }
        // ajout aux téléchargements
        QPair<COMMENT_STATE, QVariant> comment;
        comment.first = DOWNLOADING;
        m_comments.insert(key, comment);
        lst << key;
    }
    if (!lst.isEmpty()) {
        MainWindow * win = MainWindow::instance();
        win->followNetworkReply(win->client()->comments(lst), trUtf8("Downloading subtitles comments"));
    }
}

void SubtitleDetail::downloadAborted() {
    QMap<QString, QPair<COMMENT_STATE, QVariant> >::iterator c = m_comments.begin();
    while (c != m_comments.end()) {
        if (c.value().first == DOWNLOADING) {
            c.value().first = DOWNLOAD_ABORTED;
        }
        c++;
    }
}

void SubtitleDetail::commentsLoaded(const QMap<QString, QVariant> &comments) {
    MainWindow::instance()->showStatusMessage(trUtf8("Comments loaded."));
    QMap<QString, QPair<COMMENT_STATE, QVariant> >::iterator c = m_comments.begin();
    while (c != m_comments.end()) {
         QMap<QString, QVariant>::const_iterator i = comments.find(c.key());
         if (i != comments.end()) {
             c.value().first = DOWNLOADED;
             c.value().second = i.value();
         } else if (c.value().first == DOWNLOADING) {
             c.value().first = NO_COMMENTS;
         }
        c++;
    }
    writeComments(m_currentSubtitle);
}

void SubtitleDetail::subtitlesListSelectionChanged() {
    SubtitlesView * view = MainWindow::instance()->subtitlesList();
    QList<QMap<QString, QVariant> > subs;
    // seulement 1 détail affiché
    if (view->selectionModel()->hasSelection()) {
        subs << view->subtitlesData(view->currentIndex());
    }
    writeComments(subs);
}

void SubtitleDetail::clearSubtitles() {
    clear();
    m_comments.clear();
    m_currentSubtitle.clear();
}

void SubtitleDetail::writeComments(const QList<QMap<QString, QVariant> > &subtitles) {
    m_currentSubtitle = subtitles;
    clear();
    QTextCursor cursor = textCursor();
    QMap<QString, QVariant> subtitle;
    foreach (subtitle, subtitles) {
        QString key = subtitle.value("IDSubtitle").toString();
        QMap<QString, QPair<COMMENT_STATE, QVariant> >::iterator c = m_comments.find(key);
        if (c == m_comments.end()) continue;
        QList<QVariant> comments = c.value().second.toList();

        QString line = m_detailContent;
        replaceSubtitlesField(line, subtitle);
        cursor.insertHtml(line);
        cursor.insertBlock();
        cursor.insertBlock();
        COMMENT_STATE state = c.value().first;
        switch (state) {
        case NO_COMMENTS:
            cursor.insertHtml(QString("<b>%1</b>").arg(trUtf8("No comments.")));
            break;
        case DOWNLOADING:
            cursor.insertHtml(QString("<b style=\"color:red;\">%1</b>").arg(trUtf8("Comments downloading...")));
            break;
        case DOWNLOAD_ABORTED:
            cursor.insertHtml(QString("<b style=\"color:red;\">%1</b>").arg(trUtf8("Comments downloading aborted.")));
            break;
        case DOWNLOADED:
            foreach (const QVariant & comment, comments) {
                writeComment(cursor, comment.toMap());
            }
        }
        cursor.insertBlock();
    }
}

/**
  * @brief écrit un comment de sous titre
  */
void SubtitleDetail::writeComment(QTextCursor &c, const QMap<QString, QVariant> &comment) {
    QString cstr = trUtf8("On <b>%1</b>, <b>%2</b> wrote:<br>%3<br><br>").arg(
            QDateTime::fromString(comment.value("Created").toString(), "yyyy-MM-dd hh:mm:ss").toString(Qt::SystemLocaleShortDate))
            .arg(comment.value("UserNickName").toString())
            .arg(comment.value("Comment").toString());
    c.insertHtml(cstr);
}

void SubtitleDetail::setLineWrap(bool ww) {
    if (ww) {
        setLineWrapMode(QPlainTextEdit::WidgetWidth);
    } else {
        setLineWrapMode(QPlainTextEdit::NoWrap);
    }
}
