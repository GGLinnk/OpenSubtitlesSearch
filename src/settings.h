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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

class Settings : public QSettings
{
Q_OBJECT
public:
    explicit Settings(QObject *parent = 0);
    QVariant value(const char * key);
    QVariant defaut(const char * key);

    static const char * MWIN_SIZE;
    static const char * MWIN_STATE;
    static const char * SUBS_COLS;
    static const char * SUBS_DOWNLOADDIR;
    static const char * SUBS_SEARCHLANGS;
    static const char * SUBS_DOWNLOADMODE;
#ifdef USE_ICU
    static const char * SUBS_REENCODE;
#endif
    static const char * USER;
    static const char * PASSWD;
    static const char * USERAGENT;
    static const char * AUTOCONNECT;
    static const char * SHOWDOWNLOADSONCEDONE;
    static const char * SUBDETAIL_NORMAL;
    static const char * SUBDETAIL_AUTOCOMMENT;
signals:

public slots:

};

#endif // SETTINGS_H
