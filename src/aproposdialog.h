#ifndef APROPOSDIALOG_H
#define APROPOSDIALOG_H

#include <QDialog>

namespace Ui {
    class AProposDialog;
}

class AProposDialog : public QDialog {
    Q_OBJECT
public:
    AProposDialog(QWidget *parent = 0);
    ~AProposDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::AProposDialog *ui;
};

#endif // APROPOSDIALOG_H
