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

#ifndef SUBTITLESLISTMODEL_H
#define SUBTITLESLISTMODEL_H

#include <QAbstractTableModel>
#include <QStringList>

class SubtitlesListModel : public QAbstractTableModel
{
Q_OBJECT
public:
    explicit SubtitlesListModel(QObject *parent = 0);

    void clearSubtitlesData();
    void addSubtitlesData(const QList<QVariant> & data);

    inline const QList<QVariant> & subtitlesData() { return m_openSubtitlesData; }
    QMap<QString, QVariant> subtitlesData(int row);

    QMap<QString, QVariant> subtitleData(const QString & key, const QString & value);

    void setColumns(const QStringList & columns);

    int rowCount ( const QModelIndex & = QModelIndex() ) const;
    int columnCount ( const QModelIndex & = QModelIndex() ) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;


signals:

public slots:

protected:

private:
    QList<QVariant> m_openSubtitlesData;
    QStringList m_colonnes;
};

#endif // SUBTITLESLISTMODEL_H
