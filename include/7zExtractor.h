//
// Created by huzongyao on 17-11-24.
//


#ifndef ANDROIDUN7ZIP_7ZEXTRACTOR_H
#define ANDROIDUN7ZIP_7ZEXTRACTOR_H

#define DEFAULT_IN_BUF_SIZE 0x100000

#ifdef __cplusplus
extern "C" {
#endif

int extract7zFile(const char *srcFile, const char *destDir, unsigned long inBufSize);

#ifdef __cplusplus
}
#endif

#endif //ANDROIDUN7ZIP_7ZEXTRACTER_H
