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

#include "lineeditfilechoser.h"
#include <QCompleter>
#include <QDropEvent>
#include <QMouseEvent>
#include <QUrl>
#include <QFileDialog>
#include <QApplication>
#include <QHBoxLayout>

LineEditFileChoser::LineEditFileChoser(QWidget *parent) :
    QLineEdit(parent)
{
    QDirModel * fmodel = new LineEditDirModel(this);
    QCompleter * completer = new QCompleter(fmodel, this);
    setCompleter(completer);
}

void LineEditFileChoser::dropEvent(QDropEvent *e) {
    if (e->mimeData()->hasUrls()) {
        QUrl url = e->mimeData()->urls().first();
        setText(url.toLocalFile());
        e->acceptProposedAction();
    } else {
        QLineEdit::dropEvent(e);
    }
}

void LineEditFileChoser::mouseDoubleClickEvent(QMouseEvent *) {
    emit doubleClicked();
}


WidgetFileChooser::WidgetFileChooser(QWidget *parent) :
        QWidget(parent),
        m_lineEdit(new LineEditFileChoser(this)),
        m_btnOpen(new QPushButton("...", this)),
        m_mode(FILE)
{
    QHBoxLayout * l = new QHBoxLayout(this);
    this->setLayout(l);
    l->addWidget(m_lineEdit);
    l->addWidget(m_btnOpen);
    connect(m_btnOpen, SIGNAL(pressed()), this, SLOT(openDialog()));
    connect(m_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(checkText(QString)));
    connect(m_lineEdit, SIGNAL(doubleClicked()), this, SLOT(openDialog()));
    setValid(false);
    l->setMargin(0);
}

void WidgetFileChooser::setFocus() {
    m_lineEdit->setFocus(Qt::OtherFocusReason);
}

void WidgetFileChooser::openDialog() {
    QString file;
    switch (m_mode) {
    case FILE:
        file = QFileDialog::getOpenFileName(QApplication::activeWindow());
        break;
    case DIRECTORY:
        file = QFileDialog::getExistingDirectory(QApplication::activeWindow());
        break;
    }

    if (! file.isEmpty()) m_lineEdit->setText(file);
}

void WidgetFileChooser::setValid(bool state) {
    m_valid = state;
    m_lineEdit->setStyleSheet(state ? "" : "QLineEdit { color: red; }");
    emit validStateChanged(state);
}

void WidgetFileChooser::checkText(const QString &text) {
    bool state;
    switch (m_mode) {
    case FILE:
        state = QFileInfo(text).isFile(); break;
    case DIRECTORY:
        state = QFileInfo(text).isDir(); break;
    }
    if (m_valid == state) return;
    setValid(state);
}
