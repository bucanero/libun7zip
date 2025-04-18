//
// Created by huzongyao on 17-11-24.
//


#ifndef ANDROIDUN7ZIP_7ZEXTRACTOR_H
#define ANDROIDUN7ZIP_7ZEXTRACTOR_H

#define DEFAULT_IN_BUF_SIZE 0x100000

#ifdef __cplusplus
extern "C" {
#endif

/*
	void callback(const char* fileName, unsigned long fileSize, unsigned fileNum, unsigned numFiles);
*/
typedef void (*callback7z_t)(const char*, unsigned long, unsigned, unsigned);

int List7zFile(const char *srcFile, callback7z_t callback);
int Test7zFileEx(const char *srcFile, callback7z_t callback, unsigned long inBufSize);
int Extract7zFileEx(const char *srcFile, const char *destDir, callback7z_t callback, unsigned long inBufSize);

#define Test7zFile(File)                Test7zFileEx(File, NULL, DEFAULT_IN_BUF_SIZE)
#define Extract7zFile(File, Dir)        Extract7zFileEx(File, Dir, NULL, DEFAULT_IN_BUF_SIZE)

#ifdef __cplusplus
}
#endif

#endif //ANDROIDUN7ZIP_7ZEXTRACTER_H
