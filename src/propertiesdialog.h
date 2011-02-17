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

#ifndef PROPERTIESDIALOG_H
#define PROPERTIESDIALOG_H

#include <QDialog>

class ColumnsSubtitlesListModel;
class Settings;

#ifdef USE_ICU
#include <QCheckBox>
#include <QComboBox>
#endif

namespace Ui {
    class PropertiesDialog;
}

class PropertiesDialog : public QDialog {
    Q_OBJECT
public:
    enum DOWNLOAD_MODE {
        BASE_DIR = 0,
        NEAR_MOVIEFILE = 1,
        ASK = 2
    };
    PropertiesDialog(QWidget *parent = 0);
    ~PropertiesDialog();

    void load(Settings * settings);
    void save(Settings * setings);

protected:
    void setDownloadMode(DOWNLOAD_MODE mode);
    DOWNLOAD_MODE downloadMode();
    void changeEvent(QEvent *e);
#ifdef USE_ICU
    QCheckBox * m_checkChangeEncoding;
    QComboBox * m_comboChangeEncoding;
#endif

protected slots:
    void moveSubtitleColumnDown();
    void moveSubtitleColumnUp();

private:
    Ui::PropertiesDialog *ui;
    ColumnsSubtitlesListModel * m_columnsSubtitlesModel;
};

#endif // PROPERTIESDIALOG_H
