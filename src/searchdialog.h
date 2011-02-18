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
