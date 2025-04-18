/* 7zFile.h -- File IO
2009-11-24 : Igor Pavlov : Public domain */

#ifndef __7Z_ASSETS_H
#define __7Z_ASSETS_H

#include "7zTypes.h"

/* ---------- Memory Asset ---------- */

typedef struct {
    const void *asset;
    UInt64 length;
    UInt64 pos;
} CSzAssetFile;

WRes InAssetFile_Open(CSzAssetFile *p, const void *buffer, UInt64 buf_size);

WRes AssetFile_Close(CSzAssetFile *p);

typedef struct {
    ISeekInStream vt;
    CSzAssetFile assetFile;
} CAssetFileInStream;

void AssetFileInStream_CreateVTable(CAssetFileInStream *p);

#endif
