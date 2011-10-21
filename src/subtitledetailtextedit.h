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

#ifndef SUBLTITLEDETAILTEXTEDIT_H
#define SUBLTITLEDETAILTEXTEDIT_H

#include <QTextEdit>
#include <QCompleter>
#include <QSyntaxHighlighter>

class SubtitleDetailTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit SubtitleDetailTextEdit(QWidget *parent = 0);
    QString textUnderCursor() const;
signals:

protected slots:
    void insertCompletion(const QString& completion);
protected:
    void keyPressEvent(QKeyEvent *e);
    void focusInEvent(QFocusEvent *e);
    QCompleter * m_completer;
    QSyntaxHighlighter * m_highlighter;
};

class SubtitleDetailTextEdithighlighter : public QSyntaxHighlighter {
public:
    SubtitleDetailTextEdithighlighter(QTextEdit * parent);
    void highlightBlock ( const QString & text );
protected:
    QRegExp m_reg;
};

#endif // SUBLTITLEDETAILTEXTEDIT_H
