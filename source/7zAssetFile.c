/* 7zFile.c -- File IO
2009-11-24 : Igor Pavlov : Public domain */
#include <string.h>
#include "7zAssetFile.h"


WRes InAssetFile_Open(CSzAssetFile *p, const void *buffer, UInt64 buf_size) {
    p->asset = buffer;
    p->length = buf_size;
    p->pos = 0;
    return 0;
}

WRes AssetFile_Close(CSzAssetFile *p) {
    if (p->asset != NULL) {
        p->length = 0;
        p->pos = 0;
        p->asset = NULL;
    }
    return 0;
}

WRes AssetFile_Read(CSzAssetFile *p, void *data, size_t *size) {
    size_t originalSize = *size;
    if (originalSize == 0) {
        return 0;
    }
    if (!p) return -1;

    if (p->pos + originalSize > p->length) {
        originalSize = p->length - p->pos;
    }
    memcpy(data, p->asset + p->pos, originalSize);
    if (*size != originalSize) {
        *size = originalSize;
        return -1;
    }
    return 0;
}

WRes AssetFile_Seek(CSzAssetFile *p, Int64 *pos, ESzSeek origin) {
    if (!p) return -1;

    switch (origin) {
        case SZ_SEEK_SET:
            p->pos = *pos;
            break;
        case SZ_SEEK_CUR:
            p->pos += *pos;
            break;
        case SZ_SEEK_END:
            p->pos = p->length + *pos;
            break;
        default:
            return -1;
    }
    if (p->pos < 0) {
        p->pos = 0;
        return -1;
    }
    if (p->pos > p->length) {
        p->pos = p->length;
    }

    return 0;
}

static SRes AssetFileInStream_Read(const ISeekInStream *pp, void *buf, size_t *size) {
    CAssetFileInStream *p = (CAssetFileInStream *) pp;
    return (AssetFile_Read(&p->assetFile, buf, size) == 0) ? SZ_OK : SZ_ERROR_READ;
}

static SRes AssetFileInStream_Seek(const ISeekInStream *pp, Int64 *pos, ESzSeek origin) {
    CAssetFileInStream *p = (CAssetFileInStream *) pp;
    return AssetFile_Seek(&p->assetFile, pos, origin);
}

void AssetFileInStream_CreateVTable(CAssetFileInStream *p) {
    p->vt.Read = AssetFileInStream_Read;
    p->vt.Seek = AssetFileInStream_Seek;
}
