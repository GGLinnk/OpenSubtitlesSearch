#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include "ui_searchByHash.h"
#include "ui_searchByQuery.h"
#include "ui_searchbyMixed.h"

class OpenSubtitlesRPCClient;
class Settings;

namespace Ui {
    class SearchDialog;
}

class SearchWidget;

class SearchDialog : public QDialog {
    Q_OBJECT
public:
    enum MODE {
        NONE = -1,
        BY_HASH = 0,
        BY_QUERY = 1,
        BY_MIXED = 2
    };

    SearchDialog(QWidget *parent = 0);
    ~SearchDialog();

    QString selectedLangs();
    void setSelectedLangs(const QString &);

    MODE mode();

    void setText(const QString & txt);

    SearchWidget * searchWidget();

public slots:
    void setMode(MODE mode);
    void modeChanged(int);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::SearchDialog *ui;
};

class SearchWidget : public QWidget {
    Q_OBJECT
public:
    SearchWidget(QWidget * p) : QWidget(p) {}
    virtual bool isValid() = 0;
    virtual void setFocus() = 0;
    virtual QList<QVariant> createSearchArgs(OpenSubtitlesRPCClient * client, const QString & langs) = 0;
    virtual QString queryText() { return QString(); }
    virtual void setText(const QString &) { }
signals:
    void stateChanged(bool);
};

class SearchByQuery : public SearchWidget, public Ui::SearchByQuery {
    Q_OBJECT
public:
    SearchByQuery(QWidget* parent);
    bool isValid() { return ! queryLineEdit->text().isEmpty(); }
    void setFocus() { queryLineEdit->setFocus(Qt::OtherFocusReason); }
    QList<QVariant> createSearchArgs(OpenSubtitlesRPCClient * client, const QString & langs);
    QString queryText() { return queryLineEdit->text(); }
    void setText(const QString & txt) { queryLineEdit->setText(txt); }
protected slots:
    void verifyInput(const QString & text);

};

class SearchByHash : public SearchWidget, public Ui::SearchByHash {
    Q_OBJECT
public:
    SearchByHash(QWidget *);
    bool isValid() { return videoWidget->isValid(); }
    void setFocus() { videoWidget->setFocus(); }
    QList<QVariant> createSearchArgs(OpenSubtitlesRPCClient * client, const QString & langs);
    QString queryText() { return videoWidget->text(); }
    void setText(const QString & txt) { videoWidget->setText(txt); }
};

class SearchByMixed : public SearchWidget, public Ui::SearchByMixed {
    Q_OBJECT
public:
    SearchByMixed(QWidget *);
    bool isValid() { return videoWidget->isValid(); }
    void setFocus() { videoWidget->setFocus(); }
    QList<QVariant> createSearchArgs(OpenSubtitlesRPCClient * client, const QString & langs);
    QString queryText() { return videoWidget->text(); }
    void setText(const QString & txt) { videoWidget->setText(txt); }
    bool downloadOnlyIfEmpty() { return cbOnlyIfHashEmpty->isChecked(); };

};

#endif // SEARCHDIALOG_H
