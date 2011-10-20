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

#include "settings.h"
#include <QSize>
#include <QDir>

const char * Settings::MWIN_SIZE = "mainwindow/size";
const char * Settings::MWIN_STATE = "mainwindow/state";
const char * Settings::SUBS_COLS = "subtitles/colonnes";
const char * Settings::SUBS_DOWNLOADDIR = "subtitles/downloadDir";
const char * Settings::SUBS_DOWNLOADMODE = "subtitles/downloadMode";
const char * Settings::SUBS_SEARCHLANGS = "subtitles/searchLangs";
#ifdef USE_ICU
const char * Settings::SUBS_REENCODE = "subtitles/reencode";
#endif
const char * Settings::USER = "user";
const char * Settings::PASSWD = "passwd";
const char * Settings::USERAGENT = "userAgent";
const char * Settings::AUTOCONNECT = "autoconnect";
const char * Settings::SHOWDOWNLOADSONCEDONE = "showDownloadsOnceDone";
const char * Settings::SUBDETAIL_NORMAL = "subDetail/normal";
const char * Settings::SUBDETAIL_AUTOCOMMENT = "subDetail/autoComment";
const char * Settings::SEARCH_DIALOG_SIZE = "searchDialog/size";
const char * Settings::PROXY_USED = "proxy/used";
const char * Settings::PROXY_HOST = "proxy/host";
const char * Settings::PROXY_USER = "proxy/user";
const char * Settings::PROXY_PASSWD = "proxy/passwd";
const char * Settings::PROXY_PORT = "proxy/port";

Settings::Settings(QObject *parent) :
    QSettings("Parcouss Apps", "OpenSubtitlesSearch", parent)
{
}

QVariant Settings::value(const char *key) {
    return QSettings::value(key, defaut(key));
}

QVariant Settings::defaut(const char * key) {
    if (key == MWIN_SIZE) return QSize(800, 600);
    if (key == SUBS_COLS) return QStringList() << "MovieName" << "LanguageName" << "SubFileName";
    if (key == USERAGENT) return "OpenSubtitlesSearch 1.0";
    if (key == SUBS_DOWNLOADDIR) return QDir::homePath();
    if (key == SUBS_DOWNLOADMODE) return 0;
    if (key == AUTOCONNECT) return true;
    if (key == SHOWDOWNLOADSONCEDONE) return 2.0;
    if (key == SUBDETAIL_NORMAL) return trUtf8(
            "Subtitle: <b>$SubFileName</b> ($IDSubtitle)<br>"
            "Lang: <b>$LanguageName</b><br>"
            "Format : <b>$SubFormat</b> (cd $SubActualCD/$SubSumCD)<br>"
            "Note : <b>$SubRating</b> (bad: <b>$SubBad</b> times)"
            );
    if (key == SUBDETAIL_AUTOCOMMENT) return true;
#ifdef USE_ICU
    if (key == SUBS_REENCODE) return "";
#endif
    if (key == SEARCH_DIALOG_SIZE) return QSize(640, 480);
	if (key == PROXY_USED) return false;
	if (key == PROXY_PORT) return 80;
    return QVariant();
}
