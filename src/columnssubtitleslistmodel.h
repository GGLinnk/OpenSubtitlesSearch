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

#ifndef COLUMNSSUBTITLESLISTMODEL_H
#define COLUMNSSUBTITLESLISTMODEL_H

#include <QAbstractTableModel>
#include <QPair>
#include <QList>
#include <QStringList>

/**
  * @brief Modele du tableau de selection de colonnes à afficher pour la liste de sous-titres.
  * Stocke une liste de QPair<QString bool> représentant les colonnes et un état (sélectionné ou non).
  */
class ColumnsSubtitlesListModel : public QAbstractTableModel
{
Q_OBJECT
public:
    static QStringList ALL_COLUMNS;
    explicit ColumnsSubtitlesListModel(QObject *parent = 0);

    int rowCount ( const QModelIndex & = QModelIndex() ) const;
    int columnCount ( const QModelIndex & = QModelIndex() ) const;
    Qt::ItemFlags flags ( const QModelIndex & index ) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    QStringList checkedColumns();
    void setCheckedColumns(const QStringList & cols);

    bool moveRowDown(int rowSrc);
    bool moveRowUp(int rowSrc);
signals:

public slots:

protected:
    inline void addColumn(const QString & name, bool checked) {
        QPair<QString, bool> p;
        p.first = name;
        p.second = checked;
        m_colonnes << p;
    }

private:
    QList<QPair<QString, bool> > m_colonnes;
};

#endif // COLUMNSSUBTITLESLISTMODEL_H
