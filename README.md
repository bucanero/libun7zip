# un7zip library

This library provides 7-Zip (.7z) archive handling and extraction to the PlayStation 3 using the [PSL1GHT SDK](https://github.com/ps3dev/PSL1GHT/).

## Source Version

 - 7-Zip [19.00](https://sourceforge.net/projects/sevenzip/files/7-Zip/19.00/) (2019-02-21)
 - based on [AndroidUn7zip](https://github.com/hzy3774/AndroidUn7zip)

### Exports

 - Extract7zFile
 - Extract7zFileEx
 ```c
int Extract7zFile(const char *srcFile, const char *destDir);
int Extract7zFileEx(const char *srcFile, const char *destDir, callback7z_t callback, unsigned long inBufSize);
 ```
 - Test7zFile
 - Test7zFileEx
 ```c
int Test7zFile(const char *srcFile); 
int Test7zFileEx(const char *srcFile, callback7z_t callback, unsigned long inBufSize);
 ```
 - List7zFile
 ```c
int List7zFile(const char *srcFile, callback7z_t callback);
 ```

## Build/Install

Build the library with: 
```
make
```

Install the library to your PSL1GHT setup with:
```
make install
```

## Documentation

 - [7-Zip SDK](https://www.7-zip.org/sdk.html)

## Sample app

You can find a sample PSL1GHT app using the library [here](./example).

## License

`libun7zip` is released under the [Apache-2.0 License](./LICENSE).
