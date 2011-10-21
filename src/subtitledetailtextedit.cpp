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

#include "subtitledetailtextedit.h"
#include "columnssubtitleslistmodel.h"
#include <QKeyEvent>
#include<QAbstractItemView>
#include <QScrollBar>

SubtitleDetailTextEdit::SubtitleDetailTextEdit(QWidget *parent) :
    QTextEdit(parent)
{
    QStringList lst;
    foreach (const QString & st, ColumnsSubtitlesListModel::ALL_COLUMNS) {
        lst << QString("$") + st;
    }

    m_completer = new QCompleter(lst, this);
    m_completer->setWidget(this);
    m_completer->setCompletionMode(QCompleter::PopupCompletion);
    connect(m_completer, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));
    m_highlighter = new SubtitleDetailTextEdithighlighter(this);
}

void SubtitleDetailTextEdit::focusInEvent(QFocusEvent *e) {
    m_completer->setWidget(this);
    QTextEdit::focusInEvent(e);
}

QString SubtitleDetailTextEdit::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

void SubtitleDetailTextEdit::insertCompletion(const QString& completion)
{
    if (m_completer->widget() != this) return;
    QTextCursor tc = textCursor();
    int extra = completion.length() - m_completer->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    setTextCursor(tc);
}

void SubtitleDetailTextEdit::keyPressEvent(QKeyEvent *e) {
     if (m_completer->popup()->isVisible()) {
         // The following keys are forwarded by the completer to the widget
        switch (e->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
             e->ignore();
             return; // let the completer do default behavior
        default:
            break;
        }
     }

     bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_Space); // CTRL+E
     if (!isShortcut) // do not process the shortcut when we have a completer
         QTextEdit::keyPressEvent(e);

     const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
     if ((ctrlOrShift && e->text().isEmpty()))
         return;

     static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
     bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
     QString completionPrefix = textUnderCursor();

     if (!isShortcut && (hasModifier || e->text().isEmpty() || (!completionPrefix.startsWith('$'))
                       || eow.contains(e->text().right(1)))) {
         m_completer->popup()->hide();
         return;
     }

     if (completionPrefix != m_completer->completionPrefix()) {
         m_completer->setCompletionPrefix(completionPrefix);
         m_completer->popup()->setCurrentIndex(m_completer->completionModel()->index(0, 0));
     }
     QRect cr = cursorRect();
     cr.setWidth(m_completer->popup()->sizeHintForColumn(0)
                 + m_completer->popup()->verticalScrollBar()->sizeHint().width());
     m_completer->complete(cr); // popup it up!
 }

SubtitleDetailTextEdithighlighter::SubtitleDetailTextEdithighlighter(QTextEdit *parent) : QSyntaxHighlighter(parent) {
    QStringList patterns;
    foreach (const QString & st, ColumnsSubtitlesListModel::ALL_COLUMNS) patterns << QRegExp::escape(QString("$") + st);
    m_reg.setPattern(QString("(%1)").arg(patterns.join("|")));
}

void SubtitleDetailTextEdithighlighter::highlightBlock(const QString &text) {
    QTextCharFormat myClassFormat;
    myClassFormat.setFontWeight(QFont::Bold);
    myClassFormat.setForeground(Qt::darkMagenta);

    int index = text.indexOf(m_reg);
    while (index >= 0) {
        int length = m_reg.matchedLength();
        setFormat(index, length, myClassFormat);
        index = text.indexOf(m_reg, index + length);
    }
}
