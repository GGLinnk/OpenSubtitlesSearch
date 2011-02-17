#ifndef ADDCOMMENTDIALOG_H
#define ADDCOMMENTDIALOG_H

#include <QDialog>

namespace Ui {
    class AddCommentDialog;
}

class AddCommentDialog : public QDialog {
    Q_OBJECT
public:
    AddCommentDialog(QMap<QString, QVariant> & subtitle, QWidget *parent = 0);
    ~AddCommentDialog();
    QString commentSubtitle();
    bool isBadSubtitle();
    QString idSubtitle();

protected:
    void changeEvent(QEvent *e);
    void accept();

protected slots:
    void testOkButton();

private:
    Ui::AddCommentDialog *ui;
    QMap<QString, QVariant> & m_subtitle;
};

#endif // ADDCOMMENTDIALOG_H
