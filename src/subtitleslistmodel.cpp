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

#include "subtitleslistmodel.h"

SubtitlesListModel::SubtitlesListModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    m_colonnes << "SubFileName" << "SubLanguageID";
}

void SubtitlesListModel::setColumns(const QStringList &columns) {
    beginResetModel();
    m_colonnes = columns;
    endResetModel();
}

void SubtitlesListModel::clearSubtitlesData() {
    beginResetModel();
    m_openSubtitlesData.clear();
    endResetModel();
}

void SubtitlesListModel::addSubtitlesData(const QList<QVariant> &data) {
    QList<QVariant> rdata;
    foreach (const QVariant & d, data) {
        QString id = d.toMap().value("SubtitleID").toString();
        bool exist = false;
        foreach (const QVariant & d2, m_openSubtitlesData) {
            if (id == d2.toMap().value("SubtitleID").toString()) {
                exist = true;
                break;
            }
        }
        if (!exist) rdata << d;
    }

    int pos = m_openSubtitlesData.count();
    int datac = pos + rdata.count() -1;
    if (datac < pos) return;
    beginInsertRows(QModelIndex(), pos, datac);
    m_openSubtitlesData.append(rdata);
    endInsertRows();
}

int SubtitlesListModel::rowCount ( const QModelIndex & ) const {
    return m_openSubtitlesData.count();
}

int SubtitlesListModel::columnCount ( const QModelIndex & ) const {
    return m_colonnes.count();
}

QVariant SubtitlesListModel::headerData ( int section, Qt::Orientation orientation, int role ) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        return m_colonnes.at(section);
    }
    return QVariant();
}

QVariant SubtitlesListModel::data ( const QModelIndex & index, int role ) const {
    if (role == Qt::DisplayRole) {
        return m_openSubtitlesData.at(index.row()).toMap().value(m_colonnes.at(index.column())).toString();
    }
    return QVariant();
}

QMap<QString, QVariant> SubtitlesListModel::subtitlesData(int row) {
    return m_openSubtitlesData.at(row).toMap();
}

QMap<QString, QVariant> SubtitlesListModel::subtitleData(const QString & key, const QString & value) {
    foreach (QVariant subData, m_openSubtitlesData) {
        QMap<QString, QVariant> s = subData.toMap();
        if (s.value(key) == value) return s;
    }
    return QMap<QString, QVariant>();
}
