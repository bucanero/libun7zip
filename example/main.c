#include <un7zip.h>
#include <string.h>
#include <stdio.h>


int main(void)
{
    // Test 7-Zip archive contents
    Test7zFile("/dev_hdd0/tmp/archive.7z", DEFAULT_IN_BUF_SIZE);

    // Extract 7-Zip archive contents to /dev_hdd0/tmp/
    Extract7zFile("/dev_hdd0/tmp/archive.7z", "/dev_hdd0/tmp", DEFAULT_IN_BUF_SIZE);
    
    return 0;
}
