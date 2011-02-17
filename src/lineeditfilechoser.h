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

#ifndef LINEEDITFILECHOSER_H
#define LINEEDITFILECHOSER_H

#include <QLineEdit>

/**
  * @brief LineEdit spécialisée pour récupérer un fichier.
  * Autocomplétion, glisser/déposé et ouverture de dialogue de sélection lors du doubleclick.
  */
class LineEditFileChoser : public QLineEdit
{
Q_OBJECT
public:
    explicit LineEditFileChoser(QWidget *parent = 0);

protected:
    void dropEvent ( QDropEvent * e );
    void mouseDoubleClickEvent ( QMouseEvent * event );

signals:
    void doubleClicked();

};

#include <QPushButton>

class WidgetFileChooser : public QWidget {
    Q_OBJECT
public:
    enum MODE {
        FILE,
        DIRECTORY
    };

    explicit WidgetFileChooser(QWidget *parent = 0);
    inline void setText(const QString & text) { m_lineEdit->setText(text); }
    inline QString text() { return m_lineEdit->text(); }
    inline bool isValid() { return m_valid; }
    inline void setMode(MODE m) { m_mode = m; }
    inline MODE mode() { return m_mode; }
    void setFocus();

signals:
    void validStateChanged(bool newState);

protected slots:
    void openDialog();
    void checkText(const QString & text);

protected:
    void setValid(bool state);

private:
    LineEditFileChoser * m_lineEdit;
    QPushButton * m_btnOpen;
    bool m_valid;
    MODE m_mode;
};

#include <QDirModel>

/**
  * @brief DirModel spécialisé pour renvoyer la totalité du path ( et pas juste le dernier composant du path )
  */
class LineEditDirModel : public QDirModel {
public:
    LineEditDirModel(QObject * parent = 0) : QDirModel(parent) { }

    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const {
        if (role == Qt::DisplayRole && index.column() == 0) {
            return filePath(index);
        }
        return QDirModel::data(index, role);
    }
};

#endif // LINEEDITFILECHOSER_H
