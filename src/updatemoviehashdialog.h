#ifndef UPDATEMOVIEHASHDIALOG_H
#define UPDATEMOVIEHASHDIALOG_H

#include <QDialog>

namespace Ui {
    class UpdateMovieHashDialog;
}

class QNetworkReply;

class UpdateMovieHashDialog : public QDialog {
    Q_OBJECT
public:
    UpdateMovieHashDialog(QWidget *parent = 0);
    ~UpdateMovieHashDialog();

    bool setSubtitles(const QList< QMap<QString, QVariant> > & subs, QString * err = 0);

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *);

protected slots:
    void sendUpdateHash();
    void updateHashReceived(const QMap<QString, QVariant> & liste);
    void replyFinished();
    void validate();

private:
    Ui::UpdateMovieHashDialog *ui;
    QNetworkReply * m_currentReply;
};

#endif // UPDATEMOVIEHASHDIALOG_H
