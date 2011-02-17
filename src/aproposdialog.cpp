#include "aproposdialog.h"
#include "ui_aproposdialog.h"

AProposDialog::AProposDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AProposDialog)
{
    ui->setupUi(this);
}

AProposDialog::~AProposDialog()
{
    delete ui;
}

void AProposDialog::changeEvent(QEvent *e)
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
