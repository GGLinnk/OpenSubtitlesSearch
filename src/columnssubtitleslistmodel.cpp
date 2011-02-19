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

#include "columnssubtitleslistmodel.h"

QStringList ColumnsSubtitlesListModel::ALL_COLUMNS
        = QStringList()
                << "MatchedBy"
                << "IDSubMovieFile"
                << "MovieHash"
                << "MovieByteSize"
                << "MovieTimeMS"
                << "IDSubtitleFile"
                << "SubFileName"
                << "SubActualCD"
                << "SubSize"
                << "SubHash"
                << "IDSubtitle"
                << "UserID"
                << "SubLanguageID"
                << "SubFormat"
                << "SubSumCD"
                << "SubAuthorComment"
                << "SubAddDate"
                << "SubBad"
                << "SubRating"
                << "SubDownloadsCnt"
                << "MovieReleaseName"
                << "IDMovie"
                << "IDMovieImdb"
                << "MovieName"
                << "MovieNameEng"
                << "MovieYear"
                << "MovieImdbRating"
                << "UserNickName"
                << "ISO639"
                << "LanguageName"
                << "SubComments"
                << "SubDownloadLink"
                << "ZipDownloadLink"
                << "SubtitlesLink";

ColumnsSubtitlesListModel::ColumnsSubtitlesListModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    QStringList cols(ALL_COLUMNS);
    cols.sort();
    foreach (QString c, cols) {
        addColumn(c, false);
    }
}

int ColumnsSubtitlesListModel::rowCount ( const QModelIndex & ) const {
    return m_colonnes.count();
}

int ColumnsSubtitlesListModel::columnCount ( const QModelIndex & ) const {
    return 1;
}

Qt::ItemFlags ColumnsSubtitlesListModel::flags ( const QModelIndex & ) const {
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
}

QVariant ColumnsSubtitlesListModel::data ( const QModelIndex & index, int role ) const {
    if (role == Qt::DisplayRole) {
        return m_colonnes.at(index.row()).first;
    }
    else if (role == Qt::CheckStateRole) {
        return m_colonnes.at(index.row()).second ? Qt::Checked : Qt::Unchecked;
    }
    return QVariant();
}

/**
  * @brief Permet la sélection / déselection
  */
bool ColumnsSubtitlesListModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (role == Qt::CheckStateRole) {
        int pos = index.row();
        QPair<QString, bool> pair = m_colonnes.takeAt(pos);
        pair.second = value.toBool();
        m_colonnes.insert(pos, pair);
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

/**
  * @brief Renvoie la liste odonnée des colonnes sélectionnées.
  */
QStringList ColumnsSubtitlesListModel::checkedColumns() {
    QStringList res;
    QPair<QString, bool> pair;
    foreach (pair, m_colonnes) {
        if (pair.second) res << pair.first;
    }
    return res;
}

/**
  * @brief Définit la liste ordonnée des colonnes à sélectionner.
  */
void ColumnsSubtitlesListModel::setCheckedColumns(const QStringList &cols) {
    beginResetModel();
    m_colonnes.clear();
    QStringList checked, unchecked;
    foreach (QString c, cols) {
        checked << c;
    }
    QStringList allCols(ALL_COLUMNS);
    allCols.sort();
    foreach (QString c, allCols) {
        if (!checked.contains(c)) {
            unchecked << c;
        }
    }
    foreach (QString c, checked) addColumn(c, true);
    foreach (QString c, unchecked) addColumn(c, false);
    endResetModel();
}

/**
  * @brief Déplacer une ligne d'un cran vers le bas.
  */
bool ColumnsSubtitlesListModel::moveRowDown(int rowSrc) {
    QModelIndex parent = QModelIndex();
    if (beginMoveRows(parent, rowSrc, rowSrc, parent, rowSrc + 2)) {
        QPair<QString, bool> b = m_colonnes.takeAt(rowSrc);
        m_colonnes.insert(rowSrc + 1, b);
        endMoveRows();
        return true;
    }
    return false;
}

/**
  * @brief Déplacer une ligne d'un cran vers le haut.
  */
bool ColumnsSubtitlesListModel::moveRowUp(int rowSrc) {
    QModelIndex parent = QModelIndex();
    if (beginMoveRows(parent, rowSrc, rowSrc, parent, rowSrc - 1)) {
        QPair<QString, bool> b = m_colonnes.takeAt(rowSrc);
        m_colonnes.insert(rowSrc - 1, b);
        endMoveRows();
        return true;
    }
    return false;
}
