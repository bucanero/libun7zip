//
// un7zip library by Bucanero
// https://github.com/bucanero/libun7zip
//
// Based on https://github.com/hzy3774/AndroidUn7zip
// by huzongyao on 17-11-24.
//

#include <sys/stat.h>
#include "7zTypes.h"
#include "7z.h"
#include "7zFile.h"
#include "7zAlloc.h"
#include "7zCrc.h"
#include "7zFunctions.h"
#include "7zAssetFile.h"
#include "7zExtractor.h"

#define OPTION_DETAIL 0x01
#define OPTION_TEST 0x02
#define OPTION_OUTPUT 0x04
#define OPTION_EXTRACT (OPTION_TEST|OPTION_OUTPUT)

static const ISzAlloc g_Alloc = {SzAlloc, SzFree};

static SRes
extractStream(ISeekInStream *seekStream, const char *destDir,
              const int options, callback7z_t callback, size_t inBufSize) {

    ISzAlloc allocImp = g_Alloc;
    ISzAlloc allocTempImp = g_Alloc;
    CLookToRead2 lookStream;
    CSzArEx db;
    SRes res;
    UInt16 *temp = NULL;
    size_t tempSize = 0;

    LookToRead2_CreateVTable(&lookStream, False);
    lookStream.buf = NULL;
    res = SZ_OK;
    lookStream.buf = (Byte *) ISzAlloc_Alloc(&allocImp, inBufSize);
    if (!lookStream.buf)
        res = SZ_ERROR_MEM;
    else {
        lookStream.bufSize = inBufSize;
        lookStream.realStream = seekStream;
        LookToRead2_Init(&lookStream);
    }

    CrcGenerateTable();
    SzArEx_Init(&db);
    if (res == SZ_OK) {
        res = SzArEx_Open(&db, &lookStream.vt, &allocImp, &allocTempImp);
    }
    if (res == SZ_OK) {
        UInt32 i;
        /*
        if you need cache, use these 3 variables.
        if you use external function, you can make these variable as static.
        */
        UInt32 blockIndex = 0xFFFFFFFF; /* it can have any value before first call (if outBuffer = 0) */
        Byte *outBuffer = NULL; /* it must be 0 before first call for each new archive. */
        size_t outBufferSize = 0;  /* it can have any value before first call (if outBuffer = 0) */
        CBuf fileNameBuf;
        Buf_Init(&fileNameBuf);

        for (i = 0; i < db.NumFiles; i++) {
            size_t offset = 0;
            size_t outSizeProcessed = 0;
            size_t len;
            BoolInt isDir = SzArEx_IsDir(&db, i);
            len = SzArEx_GetFileNameUtf16(&db, i, NULL);
            if (len > tempSize) {
                SzFree(NULL, temp);
                tempSize = len;
                temp = (UInt16 *) SzAlloc(NULL, tempSize * sizeof(temp[0]));
                if (!temp) {
                    res = SZ_ERROR_MEM;
                    break;
                }
            }
            SzArEx_GetFileNameUtf16(&db, i, temp);
            res = Utf16_To_Char(&fileNameBuf, temp);
            if (res != SZ_OK) {
                break;
            }
            UInt64 fileSize = SzArEx_GetFileSize(&db, i);

            if (callback)
                callback ((char*) fileNameBuf.data, fileSize, i+1, db.NumFiles);

            if (options & OPTION_DETAIL) {
                char attr[8], file_size[32], file_time[32];
                GetAttribString(SzBitWithVals_Check(&db.Attribs, i)
                                ? db.Attribs.Vals[i] : 0, isDir, attr);
                UInt64ToStr(fileSize, file_size, 10);
                if (SzBitWithVals_Check(&db.MTime, i))
                    ConvertFileTimeToString(&db.MTime.Vals[i], file_time);
                else {
                    size_t j;
                    for (j = 0; j < 19; j++)
                        file_time[j] = ' ';
                    file_time[j] = '\0';
                }
            }
            if (options & OPTION_TEST) {
                if (!isDir) {
                    res = SzArEx_Extract(&db, &lookStream.vt, i, &blockIndex, &outBuffer,
                                         &outBufferSize, &offset, &outSizeProcessed,
                                         &allocImp, &allocTempImp);
                    if (res != SZ_OK)
                        break;
                }
                if (options & OPTION_OUTPUT) {
                    CSzFile outFile;
                    size_t processedSize;
                    UInt16 *name = temp;
                    const UInt16 *destPath = (const UInt16 *) name;
                    for (size_t j = 0; name[j] != 0; j++) {
                        if (name[j] == CHAR_PATH_SEPARATOR) {
                            name[j] = 0;
                            MyCreateDir(name, destDir);
                            name[j] = CHAR_PATH_SEPARATOR;
                        }
                    }
                    if (isDir) {
                        MyCreateDir(name, destDir);
                        continue;
                    } else if (OutFile_OpenUtf16(&outFile, destPath, destDir)) {
                        PrintError("can not open output file");
                        res = SZ_ERROR_FAIL;
                        break;
                    }
                    processedSize = outSizeProcessed;
                    if (File_Write(&outFile, outBuffer + offset, &processedSize) != 0 ||
                        processedSize != outSizeProcessed) {
                        PrintError("can not write output file");
                        res = SZ_ERROR_WRITE;
                        break;
                    }
                    if (File_Close(&outFile)) {
                        PrintError("can not close output file");
                        res = SZ_ERROR_FAIL;
                        break;
                    }
                }
            }
        }
        Buf_Free(&fileNameBuf, &g_Alloc);
        ISzAlloc_Free(&allocImp, outBuffer);
    }
    SzFree(NULL, temp);
    SzArEx_Free(&db, &allocImp);
    ISzAlloc_Free(&allocImp, lookStream.buf);
    if (res != SZ_OK) {
        PrintError("Stream Extract Error");
    }
    return res;
}

static int _process7zFile(const char *srcFile, const char *destDir, int opts, callback7z_t callback, size_t inBufSize)
{
    CFileInStream archiveStream;
    if (InFile_Open(&archiveStream.file, srcFile)) {
        PrintError("Input File Open Error");
        return SZ_ERROR_ARCHIVE;
    }
    FileInStream_CreateVTable(&archiveStream);
    SRes res = extractStream(&archiveStream.vt, destDir, opts, callback, inBufSize);
    File_Close(&archiveStream.file);

    return res;
}

static int _process7zAsset(const void *buf, size_t buf_size, const char *destDir, int opts, callback7z_t callback, size_t inBufSize)
{
    CAssetFileInStream archiveStream;
    if (InAssetFile_Open(&archiveStream.assetFile, buf, buf_size)) {
        PrintError("Memory Asset Open Error");
        return SZ_ERROR_ARCHIVE;
    }
    AssetFileInStream_CreateVTable(&archiveStream);
    SRes res = extractStream(&archiveStream.vt, destDir, opts, callback, inBufSize);
    AssetFile_Close(&archiveStream.assetFile);

    return res;
}

/**
 * extract all from 7z
 */
int un7z_ExtractArchive(const char *srcFile, const char *destDir, callback7z_t callback, unsigned long inBufSize)
{
    mkdir(destDir, 0777);
    return _process7zFile(srcFile, destDir, OPTION_EXTRACT, callback, inBufSize);
}

int un7z_TestArchive(const char *srcFile, callback7z_t callback, unsigned long inBufSize)
{
    return _process7zFile(srcFile, NULL, OPTION_TEST, callback, inBufSize);
}

int un7z_ListArchive(const char *srcFile, callback7z_t callback)
{
    return _process7zFile(srcFile, NULL, 0, callback, UN7Z_IN_BUF_SIZE);
}

/**
 * extract all from memory 7z buffer
 */
int un7z_ExtractAsset(const void *buffer, unsigned long size, const char *destDir, callback7z_t callback, unsigned long inBufSize)
{
    mkdir(destDir, 0777);
    return _process7zAsset(buffer, size, destDir, OPTION_EXTRACT, callback, inBufSize);
}

int un7z_TestAsset(const void *buffer, unsigned long size, callback7z_t callback, unsigned long inBufSize)
{
    return _process7zAsset(buffer, size, NULL, OPTION_TEST, callback, inBufSize);
}

int un7z_ListAsset(const void *buffer, unsigned long size, callback7z_t callback)
{
    return _process7zAsset(buffer, size, NULL, 0, callback, UN7Z_IN_BUF_SIZE);
}
