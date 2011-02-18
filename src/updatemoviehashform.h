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

#ifndef UPDATEMOVIEHASHFORM_H
#define UPDATEMOVIEHASHFORM_H

#include <QWidget>
#include <QVariant>

class QValidator;

namespace Ui {
    class UpdateMovieHashForm;
}

class UpdateMovieHashForm : public QWidget {
    Q_OBJECT
public:
    UpdateMovieHashForm(QObject * obj, const char * sig, QWidget *parent = 0);
    ~UpdateMovieHashForm();

    void setSubtitle(const QMap<QString, QVariant> & subtitle);
    QVariant createUpdateHashArgs();

    void setLineNumberValidator(QValidator * v);

    bool isValid();
protected:
    void changeEvent(QEvent *e);

private:
    Ui::UpdateMovieHashForm *ui;
    QMap<QString, QVariant> m_subtitle;
};

#endif // UPDATEMOVIEHASHFORM_H
