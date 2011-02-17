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

#include "networkreplydownloadview.h"
#include <QResizeEvent>

NetworkReplyDownloadView::NetworkReplyDownloadView(QWidget *parent) :
    QTableView(parent)
{
}

void NetworkReplyDownloadView::resizeEvent(QResizeEvent *event) {
    QTableView::resizeEvent(event);
    int row1 = event->size().width() - 30;
    setColumnWidth(0, row1);
    setColumnWidth(1, 30);
}
