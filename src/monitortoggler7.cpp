/*
    License: FreeBSD License, see COPYING.
    Author: Jari Pennanen (2010) <jari.pennanen@gmail.com>
    
    I might have found a most difficult way of retrieving "\\.\DISPLAYX" string.
    
    http://msdn.microsoft.com/en-us/library/dd567877.aspx
*/
#include <stdio.h>
#include <stdlib.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <math.h>

#define PATH_ARRAY_ELEMENTS_NUM 10;

int QueryDisplayConfigResult(int result) {
    switch (result) {
        case ERROR_SUCCESS:
            return 1;
        case ERROR_INVALID_PARAMETER:
            fputs("Error: The combination of parameters and flags that are specified is invalid.", stderr);
            break;
        case ERROR_NOT_SUPPORTED:
            fputs("Error: The system is not running a graphics driver that was written according to the Windows Vista Display Driver Model. The function is only supported on a system with a Windows Vista Display Driver Model driver running.", stderr);
            break;
        case ERROR_ACCESS_DENIED:
            fputs("Error: The caller does not have access to the console session. This error occurs if the calling process does not have access to the current desktop or is running on a remote session.", stderr);
            break;
        case ERROR_GEN_FAILURE:
            fputs("Error: An unspecified error occurred.", stderr);
            break;
        case ERROR_INSUFFICIENT_BUFFER:
            fputs("Error: The supplied path and mode buffer are too small.", stderr);
            break;
    }
    
    return 0;
}

int DisplayConfigGetDeviceInfoResult(int result) {
    switch (result) {
        case ERROR_SUCCESS:
             puts(" The function succeeded.");
             return 1;
        case ERROR_INVALID_PARAMETER:
             fputs(" The combination of parameters and flags specified are invalid.", stderr);
             break;
        case ERROR_NOT_SUPPORTED:
             fputs(" The system is not running a graphics driver that was written according to the Windows Vista Display Driver Model. The function is only supported on a system with a Windows Vista Display Driver Model driver running.", stderr);
             break;
        case ERROR_ACCESS_DENIED:
             fputs(" The caller does not have access to the console session. This error occurs if the calling process does not have access to the current desktop or is running on a remote session.", stderr);
             break;
        case ERROR_INSUFFICIENT_BUFFER:
             fputs(" The size of the packet that the caller passes is not big enough for the information that the caller requestes.", stderr);
             break;
        case ERROR_GEN_FAILURE:
             fputs(" An unspecified error occurred.", stderr);
             break;
    }
    return 0;
}

void getDisplayConfigSourceName(LUID adapterId, UINT32 sourceId) {
    DISPLAYCONFIG_SOURCE_DEVICE_NAME deviceName;
    DISPLAYCONFIG_DEVICE_INFO_HEADER header;
    header.size = sizeof(DISPLAYCONFIG_SOURCE_DEVICE_NAME);
    header.adapterId = adapterId;
    header.id = sourceId;
    header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
    deviceName.header = header;
    if (!DisplayConfigGetDeviceInfoResult(DisplayConfigGetDeviceInfo( (DISPLAYCONFIG_DEVICE_INFO_HEADER*) &deviceName )))
        return;
    wprintf(deviceName.viewGdiDeviceName);
}

int main(int argc, char *argv[]){
	UINT32 num_of_paths = 0;
    UINT32 num_of_infos = 0;
    GetDisplayConfigBufferSizes(QDC_ALL_PATHS, &num_of_paths, &num_of_infos);
    
    printf("num of paths: %d, num of infos: %d\r\n", num_of_paths, num_of_infos);
	
    DISPLAYCONFIG_PATH_INFO* dpaths = (DISPLAYCONFIG_PATH_INFO*)malloc(sizeof(DISPLAYCONFIG_PATH_INFO)*num_of_paths);
    memset(dpaths, 0, sizeof(DISPLAYCONFIG_PATH_INFO)*num_of_paths);
    DISPLAYCONFIG_MODE_INFO* dinfos = (DISPLAYCONFIG_MODE_INFO*)malloc(sizeof(DISPLAYCONFIG_MODE_INFO)*num_of_infos);
    memset(dinfos, 0, sizeof(DISPLAYCONFIG_MODE_INFO)*num_of_infos);
    
    if (!QueryDisplayConfigResult(QueryDisplayConfig(QDC_ALL_PATHS, &num_of_paths, dpaths, &num_of_infos, dinfos, NULL)))
        return 0;
    
    for (int i = 0; i < num_of_paths; i++) {
        printf("Path %d\r\n", i);
        getDisplayConfigSourceName(dpaths[i].sourceInfo.adapterId, dpaths[i].sourceInfo.id);
        printf("  Source id: %d", dpaths[i].sourceInfo.id);
    }
    
    
    for (int i = 0; i < num_of_infos; i++) {
        printf("Info %d\r\n", i);
        printf("  Target adapter: %d\r\n", dinfos[i].adapterId);
    }
}
