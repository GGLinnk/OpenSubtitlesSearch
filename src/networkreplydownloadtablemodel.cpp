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

#include "networkreplydownloadtablemodel.h"
#include <QNetworkReply>
#include <QTimer>

NetworkReplyDownloadTableModel::NetworkReplyDownloadTableModel(QObject *parent) :
    QAbstractTableModel(parent), m_deleteAfter(1500)
{
}

Qt::ItemFlags NetworkReplyDownloadTableModel::flags ( const QModelIndex & index) const {
    Qt::ItemFlags f = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    return index.column() == 1 ? f | Qt::ItemIsUserCheckable : f;
}

QVariant NetworkReplyDownloadTableModel::data(const QModelIndex & index, int role) const {
    if (role == Qt::CheckStateRole && index.column() == 1) {
        return false;
    }
    return QVariant();
}

/**
  * @brief le role Qt::CheckStateRole permet d'annuler une requête si cela est permis
  */
bool NetworkReplyDownloadTableModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (role == Qt::CheckStateRole && index.column() == 1) {
        const NetworkReplyElem & e = networkReplyElem(index.row());
        if (e.canCancel) {
            QNetworkReply * reply = e.reply;
            // abort: delete value
            reply->abort();
            m_replysToDelete << reply;
            deleteNextReply();
            return true;
        }
    }
    return false;
}

/**
  * @brief Ajoute une requête à la liste. Prévient les vues.
  * @param reply la requete
  * @param text le texte à afficher (doit contenir %1 pour montrer l'avancement)
  * @param canCancel Peut être stoppée par l'utilisateur ou non
  */
void NetworkReplyDownloadTableModel::addNetWorkReply(QNetworkReply *reply, const QString &text, bool canCancel) {
    connect(reply, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(up(qint64,qint64)));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(down(qint64,qint64)));
    connect(reply, SIGNAL(finished()), this, SLOT(replyFinished()));
    int pos = rowCount();
    beginInsertRows(QModelIndex(),pos, pos);
    struct NetworkReplyElem elem;
    elem.reply = reply;
    elem.text = text;
    elem.progress = 0;
    elem.total = -1;
    elem.canCancel = canCancel;
    elem.state = UPLOAD;
    m_elems << elem;
    endInsertRows();
}

void NetworkReplyDownloadTableModel::up(qint64 bytesSent, qint64 bytesTotal) {
    QNetworkReply * reply = (QNetworkReply *) sender();
    int iPos;
    NetworkReplyElem * elem = findElem(reply, iPos);
    if (!elem) return;
    elem->state = UPLOAD;
    elem->progress = bytesSent;
    elem->total = bytesTotal;
    emit dataChanged(index(iPos, 0), index(iPos, columnCount() - 1));
}

void NetworkReplyDownloadTableModel::down(qint64 bytesSent, qint64 bytesTotal) {
    QNetworkReply * reply = (QNetworkReply *) sender();
    int iPos;
    NetworkReplyElem * elem = findElem(reply, iPos);
    if (!elem) return;
    elem->state = DOWNLOAD;
    elem->progress = bytesSent;
    elem->total = bytesTotal;
    emit dataChanged(index(iPos, 0), index(iPos, columnCount() - 1));
}

NetworkReplyElem * NetworkReplyDownloadTableModel::findElem(QNetworkReply * reply, int &pos) {
    NetworkReplyElem * e = NULL, * elem;
    int iMax = m_elems.count();
    for (int i =0; i < iMax; i++) {
        elem = (NetworkReplyElem *) &m_elems.at(i);
        if (elem->reply == reply) {
            e = elem;
            pos = i;
            break;
        }
    }
    return e;
}

void NetworkReplyDownloadTableModel::replyFinished() {
    QNetworkReply * reply = (QNetworkReply *) sender();
    int iPos;
    NetworkReplyElem * elem = findElem(reply, iPos);
    if (!elem) return;
    elem->canCancel = false;
    // suppression ?
    if (m_deleteAfter < 0) return;
    m_replysToDelete << reply;
    QTimer::singleShot(m_deleteAfter, this, SLOT(deleteNextReply()));
}

/**
  * @brief slot permettant de supprimer la requête suivante en attente de suppression.
  */
void NetworkReplyDownloadTableModel::deleteNextReply() {
    int iPos = -1;
    // pochain reply a supprimer
    QNetworkReply * reply = m_replysToDelete.takeFirst();
    // on le cherche dans les donnes
    int iMax = m_elems.count();
    for (int i =0; i < iMax; i++) {
        const NetworkReplyElem & elem = m_elems.at(i);
        if (elem.reply == reply) {
            iPos = i;
            break;
        }
    }

    // on supprime la ligne
    if (iPos > -1) {
        beginRemoveRows(QModelIndex(), iPos, iPos);
        m_elems.removeAt(iPos);
        endRemoveRows();
    }
}
