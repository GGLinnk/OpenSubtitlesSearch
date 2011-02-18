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

#include "addcommentdialog.h"
#include "ui_addcommentdialog.h"
#include <QPushButton>
#include <QMessageBox>

AddCommentDialog::AddCommentDialog(QMap<QString, QVariant> & subtitle, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddCommentDialog),
    m_subtitle(subtitle)
{
    ui->setupUi(this);
    QPushButton* btnOk = ui->buttonBox->button(QDialogButtonBox::Ok);
    btnOk->setEnabled(false);
    connect(ui->txtComment, SIGNAL(textChanged()), this, SLOT(testOkButton()));
    ui->label_2->setText(m_subtitle.value("SubFileName").toString());
}

AddCommentDialog::~AddCommentDialog()
{
    delete ui;
}

bool AddCommentDialog::isBadSubtitle() {
    return ui->cbBadSubtitle->isChecked();
}

QString AddCommentDialog::commentSubtitle() {
    return ui->txtComment->toPlainText();
}

QString AddCommentDialog::idSubtitle() {
    return m_subtitle.value("IDSubtitle").toString();
}

void AddCommentDialog::testOkButton() {
    QPushButton* btnOk = ui->buttonBox->button(QDialogButtonBox::Ok);
    btnOk->setEnabled(!ui->txtComment->toPlainText().isEmpty());
}

void AddCommentDialog::accept() {
    QString message = trUtf8("Are you sure you want to submit this comment for the subtitle %1 ?")
                      .arg(m_subtitle.value("SubFileName").toString());
    if (isBadSubtitle()) message += trUtf8("\n It will be marked as bad.");
    if (QMessageBox::question(this, trUtf8("Send subtitle comment"),
                                    message, QMessageBox::Yes | QMessageBox::Cancel,
                                    QMessageBox::Cancel) == QMessageBox::Yes) {
        QDialog::accept();
    }
}

void AddCommentDialog::changeEvent(QEvent *e)
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
