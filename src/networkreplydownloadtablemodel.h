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

#ifndef NETWORKREPLYDOWNLOADTABLEMODEL_H
#define NETWORKREPLYDOWNLOADTABLEMODEL_H

#include <QAbstractTableModel>
#include <QList>

class QNetworkReply;

enum NetworkReplyElemState {
    UPLOAD,
    DOWNLOAD,
    FINISHED
};

/**
  * @brief Contenu d'une ligne de progression de téléchargement
  */
struct NetworkReplyElem {
    QNetworkReply * reply;
    QString text;
    qint64 progress;
    qint64 total;
    bool canCancel;
    NetworkReplyElemState state;
};

/**
  * @brief Modele pour représenter l'avancement des téléchargements (requetes) et pouvoir les abandonner.
  * Peut détruire automatiquement les requêtes terminées. (Défaut)
  */
class NetworkReplyDownloadTableModel : public QAbstractTableModel
{
Q_OBJECT
public:
    explicit NetworkReplyDownloadTableModel(QObject *parent = 0);

    int rowCount(const QModelIndex & = QModelIndex()) const { return m_elems.count(); }
    int columnCount(const QModelIndex & = QModelIndex()) const { return 2; }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;

    void addNetWorkReply(QNetworkReply * reply, const QString & text, bool canCancel = true);

    inline const NetworkReplyElem & networkReplyElem(int iRow) const { return m_elems.at(iRow); }

    // supprimer apres un temps donné <0:non|0:direct|>0 temps ms
    inline void setDeleteNetworkReplyAfter(int df) { m_deleteAfter = df; }
    inline int deleteNetworkReplyAfter() { return m_deleteAfter; }
signals:

public slots:
    void replyFinished();
    void deleteNextReply();
    void up(qint64 bytesSent, qint64 bytesTotal);
    void down(qint64 bytesSent, qint64 bytesTotal);
    void subtitleSavedOnDisk(const QString & subId, const QString & fname, const QString & movieName);

private:
    NetworkReplyElem * findElem(QNetworkReply *, int & pos);
    QList<NetworkReplyElem> m_elems;
    int m_deleteAfter;
    QList<QNetworkReply *> m_replysToDelete;
};

#endif // NETWORKREPLYDOWNLOADTABLEMODEL_H
