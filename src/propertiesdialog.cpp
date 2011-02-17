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

#include "propertiesdialog.h"
#include "ui_propertiesdialog.h"
#include <QModelIndexList>
#include "settings.h"
#include "columnssubtitleslistmodel.h"

#ifdef USE_ICU
#include "icu_charset.h"
#include <QStringList>
#include <QStringListModel>
#endif

PropertiesDialog::PropertiesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PropertiesDialog)
{
    ui->setupUi(this);
    ui->widgetDir->setMode(WidgetFileChooser::DIRECTORY);
    m_columnsSubtitlesModel = new ColumnsSubtitlesListModel(this);
    ui->columnsSubtitesView->setModel(m_columnsSubtitlesModel);
    ui->btnSubtitlesColumnUp->setDefaultAction(ui->actionD_placer_vers_le_haut);
    ui->btnSubtitlesColumnDown->setDefaultAction(ui->actionD_placer_vers_le_bas);
    QMenu * menu = new QMenu(this);
    menu->addAction(ui->actionD_placer_vers_le_haut);
    menu->addAction(ui->actionD_placer_vers_le_bas);
    ui->columnsSubtitesView->setContextMenu(menu);

#ifdef USE_ICU
    m_checkChangeEncoding = new QCheckBox(trUtf8("Change encoding of downloaded subtitles"), this);
    QFormLayout * l = (QFormLayout *) ui->frame_telechargement->layout();
    l->addRow(m_checkChangeEncoding);
    m_comboChangeEncoding = new QComboBox(this);
    m_comboChangeEncoding->setEnabled(false);
    l->addRow(trUtf8("Encoding :"), m_comboChangeEncoding);
    connect(m_checkChangeEncoding, SIGNAL(toggled(bool)), m_comboChangeEncoding,SLOT(setEnabled(bool)));
    QStringList codings = availablesCharset();
    codings.sort();
    m_comboChangeEncoding->setModel(new QStringListModel(codings, this));
#endif
}

PropertiesDialog::~PropertiesDialog()
{
    delete ui;
}

void PropertiesDialog::load(Settings *settings) {
    m_columnsSubtitlesModel->setCheckedColumns(settings->value(Settings::SUBS_COLS).toStringList());
    ui->widgetDir->setText(settings->value(Settings::SUBS_DOWNLOADDIR).toString());
    setDownloadMode((DOWNLOAD_MODE) settings->value(Settings::SUBS_DOWNLOADMODE).toInt());
    ui->cb_autoConnexion->setChecked(settings->value(Settings::AUTOCONNECT).toBool());
    ui->le_user->setText(settings->value(Settings::USER).toString());
    ui->le_passwd->setText(settings->value(Settings::PASSWD).toString());
    ui->time_showDoneReply->setValue(settings->value(Settings::SHOWDOWNLOADSONCEDONE).toDouble());
    ui->detailContent->setPlainText(settings->value(Settings::SUBDETAIL_NORMAL).toString());
    ui->AutoDownloadComment->setChecked(settings->value(Settings::SUBDETAIL_AUTOCOMMENT).toBool());
#ifdef USE_ICU
    QString value = settings->value(Settings::SUBS_REENCODE).toString();
    m_checkChangeEncoding->setChecked(! value.isEmpty());
    m_comboChangeEncoding->setCurrentIndex(m_comboChangeEncoding->findText(value));
#endif
}

void PropertiesDialog::save(Settings *settings) {
    settings->setValue(Settings::SUBS_DOWNLOADDIR, ui->widgetDir->text());
    settings->setValue(Settings::SUBS_DOWNLOADMODE, (int) downloadMode());
    settings->setValue(Settings::SUBS_COLS, m_columnsSubtitlesModel->checkedColumns());
    settings->setValue(Settings::AUTOCONNECT, ui->cb_autoConnexion->isChecked());
    settings->setValue(Settings::USER, ui->le_user->text());
    settings->setValue(Settings::PASSWD, ui->le_passwd->text());
    settings->setValue(Settings::SHOWDOWNLOADSONCEDONE, ui->time_showDoneReply->value());
    settings->setValue(Settings::SUBDETAIL_NORMAL, ui->detailContent->toPlainText());
    settings->setValue(Settings::SUBDETAIL_AUTOCOMMENT, ui->AutoDownloadComment->isChecked());
#ifdef USE_ICU
    settings->setValue(Settings::SUBS_REENCODE, m_comboChangeEncoding->isEnabled() ? m_comboChangeEncoding->currentText() : "");
#endif
}

void PropertiesDialog::setDownloadMode(DOWNLOAD_MODE mode) {
    switch (mode) {
    case ASK: ui->download_ask->setChecked(true); break;
    case NEAR_MOVIEFILE: ui->download_nearMovie->setChecked(true); break;
    default: ui->download_dir->setChecked(true); break;
    }
}

PropertiesDialog::DOWNLOAD_MODE PropertiesDialog::downloadMode() {
    if (ui->download_ask->isChecked()) return ASK;
    if (ui->download_nearMovie->isChecked()) return NEAR_MOVIEFILE;
    return BASE_DIR;
}

void PropertiesDialog::moveSubtitleColumnDown() {
    QModelIndexList list = ui->columnsSubtitesView->selectionModel()->selectedRows(0);
    if (!list.isEmpty()) {
        m_columnsSubtitlesModel->moveRowDown(list.first().row());
    }
}

void PropertiesDialog::moveSubtitleColumnUp() {
    QModelIndexList list = ui->columnsSubtitesView->selectionModel()->selectedRows(0);
    if (!list.isEmpty()) {
        m_columnsSubtitlesModel->moveRowUp(list.first().row());
    }
}

void PropertiesDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
