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

#include "updatemoviehashform.h"
#include "ui_updatemoviehashform.h"
#include "mainwindow.h"

UpdateMovieHashForm::UpdateMovieHashForm(QObject * obj, const char * sig, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UpdateMovieHashForm)
{
    ui->setupUi(this);
    connect(ui->widgetMovie, SIGNAL(validStateChanged(bool)), obj, sig);
}

UpdateMovieHashForm::~UpdateMovieHashForm()
{
    delete ui;
}

void UpdateMovieHashForm::setLineNumberValidator(QValidator *v) {
    ui->frames->setValidator(v);
    ui->ms->setValidator(v);
}

void UpdateMovieHashForm::setSubtitle(const QMap<QString, QVariant> &subtitle) {
    m_subtitle = subtitle;
    ui->label_sstitre->setText(subtitle.value("SubFileName").toString());
}

QVariant UpdateMovieHashForm::createUpdateHashArgs() {
    OpenSubtitlesRPCClient * client = MainWindow::instance()->client();
    QFile f(ui->widgetMovie->text());
    if (!f.open(QIODevice::ReadOnly)) return QVariant();
    double fps = ui->fps->value();
    QVariant res = client->createTryUploadSubtitlesCd(m_subtitle.value("SubFileName").toString(),
                                       m_subtitle.value("SubHash").toString(),
                                       &f,
                                       ui->ms->text(),
                                       ui->frames->text(),
                                       fps == 0.0 ? "" : QString::number(fps));
    f.close();
    return res;
}

bool UpdateMovieHashForm::isValid() {
    return ui->widgetMovie->isValid();
}

void UpdateMovieHashForm::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
