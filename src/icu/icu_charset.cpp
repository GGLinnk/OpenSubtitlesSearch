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

#include "icu_charset.h"
#include <unicode/ucnv.h>
#include <unicode/ucsdet.h>
#include <QDebug>

QStringList availablesCharset() {
    QStringList lst;
    int32_t nbAvail = ucnv_countAvailable(), i;
    for (i = 0; i < nbAvail; i++)
        lst << ucnv_getAvailableName(i);
    return lst;
}

UErrorCode charsetName(QString & charset, const QByteArray &data) {
    UErrorCode status = U_ZERO_ERROR;
    UCharsetDetector * detector = ucsdet_open(&status);
    if (status != U_ZERO_ERROR) return status;

    ucsdet_setText(detector, data.data(),data.size(),&status);
    if (status != U_ZERO_ERROR) {
        ucsdet_close(detector);
        return status;
    }

    const UCharsetMatch *ucm = ucsdet_detect(detector, &status);
    if (status != U_ZERO_ERROR) {
        ucsdet_close(detector);
        return status;
    }

    charset = ucsdet_getName(ucm, &status);
    ucsdet_close(detector);
    return status;
}

UErrorCode charsetConvert(QByteArray &to, const QString &charsetTo, const QByteArray &from, const QString &charsetFrom) {
    UErrorCode status = U_ZERO_ERROR;
    to.resize(from.size() * 4);
    int32_t realToSize = ucnv_convert(charsetTo.toAscii().data(),
                         charsetFrom.toAscii().data(),
                         to.data(),
                         to.size(),
                         from.data(),
                         from.size(),
                         &status);
    if (status == U_ZERO_ERROR) {
        to.resize(realToSize);
    }
    return status;
}
