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

#include "OpenSubtitlesHelper.h"
#include "zlib.h"

int ungzip( QByteArray & compressData, QByteArray & uncompressed )
{
    //decompress GZIP data

    //strip header and trailer
    compressData.remove(0, 10);
    compressData.chop(12);

#define buffersize 16384
    quint8 buffer[buffersize];

    z_stream cmpr_stream;
    cmpr_stream.next_in = (unsigned char *)compressData.data();
    cmpr_stream.avail_in = compressData.count();
    cmpr_stream.total_in = 0;

    cmpr_stream.next_out = buffer;
    cmpr_stream.avail_out = buffersize;
    cmpr_stream.total_out = 0;

    cmpr_stream.zalloc = Z_NULL;
    cmpr_stream.zalloc = Z_NULL;

    int iRes = inflateInit2(&cmpr_stream, -15 );
    if( iRes != Z_OK) {
        return iRes;
    }

    int status;
    do {
        cmpr_stream.next_out = buffer;
        cmpr_stream.avail_out = buffersize;

        status = inflate( &cmpr_stream, Z_NO_FLUSH );


        if(status == Z_OK || status == Z_STREAM_END) {
            uncompressed.append(QByteArray::fromRawData((char *)buffer, buffersize - cmpr_stream.avail_out));
        } else {
            return status;
        }

        if(status == Z_STREAM_END) {
            inflateEnd(&cmpr_stream);
            break;
        }

    } while(cmpr_stream.avail_out == 0);

    return Z_OK;
}

#define MAX(x,y) (((x) > (y)) ? (x) : (y))

QString compute_hash(QFile *openedFile) {
    qulonglong hash, fsize, tmp, i;
    fsize = openedFile->size();
    openedFile->seek(0);

    hash = fsize;
    for(tmp = 0, i = 0; i < 65536/sizeof(tmp) && openedFile->read((char*)&tmp, sizeof(quint64)); i++, hash += tmp);
    openedFile->seek(MAX(0, (qint64)fsize - 65536));
    for(tmp = 0, i = 0; i < 65536/sizeof(tmp) && openedFile->read((char*)&tmp, sizeof(quint64)); i++, hash += tmp);

    return QString::number(hash, 16);
}
