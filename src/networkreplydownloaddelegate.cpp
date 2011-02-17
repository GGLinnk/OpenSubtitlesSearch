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

#include "networkreplydownloaddelegate.h"
#include "networkreplydownloadtablemodel.h"
#include <QStyleOptionProgressBarV2>
#include <QApplication>

NetworkReplyDownloadDelegate::NetworkReplyDownloadDelegate(QObject *parent) :
        QStyledItemDelegate(parent), m_inconDelete(":/remove")
{
}

/**
  * @brief Dessine les barres de progression et les boutons d'annulations.
  */
void NetworkReplyDownloadDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    const NetworkReplyElem & elem = ((NetworkReplyDownloadTableModel *)index.model())->networkReplyElem(index.row());
    if (index.column() == 0) {
        QString attente = trUtf8("Waiting server ...");
        QStyleOptionProgressBarV2 style;
        style.rect = option.rect;
        style.textVisible = true;
        if (elem.total == -1) {
            style.progress = (int) elem.progress;
            if (elem.state == DOWNLOAD) {
                // show 50 %
                style.maximum = style.progress * 2;
            }
            else style.maximum = (int) elem.total;

            if (elem.progress == 0) style.text = elem.text.arg(attente);
            else style.text = elem.text.arg(trUtf8("Downloading - %1").arg(elem.progress));
        } else {
            style.progress = (int) (((double)elem.progress / elem.total) * 50);
            if (elem.state == DOWNLOAD) style.progress += 50;

            style.maximum = 100;
            style.text = style.progress == 50 ? elem.text.arg(attente) : elem.text.arg(QString("%1%").arg(style.progress));
        }
        QApplication::style()->drawControl(QStyle::CE_ProgressBar, &style, painter);
    } else if (index.column() == 1) {
        QStyleOptionButton style;
        style.icon = m_inconDelete;
        style.rect = option.rect;
        style.state = option.state;
        if (!elem.canCancel) style.state ^= QStyle::State_Enabled;
        style.iconSize = QSize(30, 30);
        QApplication::style()->drawControl(QStyle::CE_PushButton, &style, painter);
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}
