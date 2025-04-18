//
// un7zip library by Bucanero
// https://github.com/bucanero/libun7zip
//
// Based on https://github.com/hzy3774/AndroidUn7zip
// by huzongyao on 17-11-24.
//

#ifndef ANDROIDUN7ZIP_7ZEXTRACTOR_H
#define ANDROIDUN7ZIP_7ZEXTRACTOR_H

#define UN7Z_IN_BUF_SIZE                0x80000

#define List7zFile                      un7z_ListArchive
#define Test7zFileEx                    un7z_TestArchive
#define Extract7zFileEx                 un7z_ExtractArchive
#define Test7zFile(File)                un7z_TestArchive(File, NULL, UN7Z_IN_BUF_SIZE)
#define Extract7zFile(File, Dir)        un7z_ExtractArchive(File, Dir, NULL, UN7Z_IN_BUF_SIZE)

#ifdef __cplusplus
extern "C" {
#endif

/*
	void callback(const char* fileName, unsigned long fileSize, unsigned int fileNum, unsigned int totalFiles);
*/
typedef void (*callback7z_t)(const char*, unsigned long, unsigned int, unsigned int);

// from storage 7z file
int un7z_ListArchive(const char *srcFile, callback7z_t callback);
int un7z_TestArchive(const char *srcFile, callback7z_t callback, unsigned long inBufSize);
int un7z_ExtractArchive(const char *srcFile, const char *destDir, callback7z_t callback, unsigned long inBufSize);

// from memory 7z buffer
int un7z_ListAsset(const void *buffer, unsigned long size, callback7z_t callback);
int un7z_TestAsset(const void *buffer, unsigned long size, callback7z_t callback, unsigned long inBufSize);
int un7z_ExtractAsset(const void *buffer, unsigned long size, const char *destDir, callback7z_t callback, unsigned long inBufSize);

#ifdef __cplusplus
}
#endif

#endif //ANDROIDUN7ZIP_7ZEXTRACTER_H
