#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <zlib.h>
#include "global.h"


class Utility
{
public:
    Utility();
    static void decompressAndSaveToFile(const QString &compressedFile, const QString &decompressedFile) {
        QFile compressedFileStream(compressedFile);
        if (!compressedFileStream.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "Error opening file" << compressedFile;
            return;
        }

        // Read the compressed file into a QByteArray
        QByteArray compressedData = compressedFileStream.readAll();
        compressedFileStream.close();

        // Decompress the data using zlib
        z_stream zlibStream;
        zlibStream.zalloc = Z_NULL;
        zlibStream.zfree = Z_NULL;
        zlibStream.opaque = Z_NULL;

        if (inflateInit(&zlibStream) != Z_OK) {
            qDebug() << "Error initializing zlib";
            return;
        }

        zlibStream.avail_in = compressedData.size();
        zlibStream.next_in = (Bytef*)compressedData.data();

        QFile decompressedFileStream(decompressedFile);
        if (!decompressedFileStream.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Error opening file for writing" << decompressedFile;
            inflateEnd(&zlibStream);
            return;
        }

        char decompressedBuffer[BUFFER_SIZE];

        do {
            zlibStream.avail_out = BUFFER_SIZE;
            zlibStream.next_out = (Bytef*)decompressedBuffer;

            if (inflate(&zlibStream, Z_NO_FLUSH) == Z_STREAM_ERROR) {
                qDebug() << "Error decompressing data";
                inflateEnd(&zlibStream);
                return;
            }

            decompressedFileStream.write(decompressedBuffer, BUFFER_SIZE - zlibStream.avail_out);

        } while (zlibStream.avail_out == 0);

        inflateEnd(&zlibStream);
        decompressedFileStream.close();
    }
};

#endif // UTILITY_H
