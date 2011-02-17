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

#ifndef CHARSET_H
#define CHARSET_H

#include <unicode/utypes.h>
#include <QByteArray>
#include <QString>
#include <QStringList>

QStringList availablesCharset();

UErrorCode charsetName(QString & charset, const QByteArray & data);

UErrorCode charsetConvert(QByteArray & to, const QString & charsetTo, const QByteArray & from, const QString & charsetFrom);

inline UErrorCode charsetConvert(QByteArray & to, const QString & charsetTo, const QByteArray & from) {
    QString cName;
    UErrorCode err = charsetName(cName, from);
    if (err == U_ZERO_ERROR) {
        err = charsetConvert(to, charsetTo, from, cName);
    }
    return err;
}

#endif // CHARSET_H
