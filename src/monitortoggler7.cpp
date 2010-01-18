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

/* 
    result = QueryDisplayConfig(...)
*/
int Result_QDC(int result) {
    switch (result) {
        case ERROR_SUCCESS:
            return 1;
        case ERROR_INVALID_PARAMETER:
            fputs("  Error: The combination of parameters and flags that are specified is invalid.", stderr);
            break;
        case ERROR_NOT_SUPPORTED:
            fputs("  Error: The system is not running a graphics driver that was written according to the Windows Vista Display Driver Model. The function is only supported on a system with a Windows Vista Display Driver Model driver running.", stderr);
            break;
        case ERROR_ACCESS_DENIED:
            fputs("  Error: The caller does not have access to the console session. This error occurs if the calling process does not have access to the current desktop or is running on a remote session.", stderr);
            break;
        case ERROR_GEN_FAILURE:
            fputs("  Error: An unspecified error occurred.", stderr);
            break;
        case ERROR_INSUFFICIENT_BUFFER:
            fputs("  Error: The supplied path and mode buffer are too small.", stderr);
            break;
    }
    
    return 0;
}

/* 
    result = DisplayConfigGetDeviceInfo(...)
    result = GetDisplayConfigBufferSizes(...)
    result = SetDisplayConfig(...)
*/
int Result_DCGDI(int result) {
    switch (result) {
        case ERROR_SUCCESS:
             return 1;
        case ERROR_INVALID_PARAMETER:
             fputs("  Error: The combination of parameters and flags specified are invalid.", stderr);
             break;
        case ERROR_NOT_SUPPORTED:
             fputs("  Error: The system is not running a graphics driver that was written according to the Windows Vista Display Driver Model. The function is only supported on a system with a Windows Vista Display Driver Model driver running.", stderr);
             break;
        case ERROR_ACCESS_DENIED:
             fputs("  Error: The caller does not have access to the console session. This error occurs if the calling process does not have access to the current desktop or is running on a remote session.", stderr);
             break;
        case ERROR_INSUFFICIENT_BUFFER:
             fputs("  Error: The size of the packet that the caller passes is not big enough for the information that the caller requestes.", stderr);
             break;
        case ERROR_GEN_FAILURE:
             fputs("  Error: An unspecified error occurred.", stderr);
             break;
    }
    return 0;
}

/*
    Gets GDI Device name from Source (e.g. \\.\DISPLAY4).
*/
void getGDIDeviceNameFromSource(LUID adapterId, UINT32 sourceId) {
    DISPLAYCONFIG_SOURCE_DEVICE_NAME deviceName;
    DISPLAYCONFIG_DEVICE_INFO_HEADER header;
    header.size = sizeof(DISPLAYCONFIG_SOURCE_DEVICE_NAME);
    header.adapterId = adapterId;
    header.id = sourceId;
    header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
    deviceName.header = header;
    if (!Result_DCGDI(DisplayConfigGetDeviceInfo( (DISPLAYCONFIG_DEVICE_INFO_HEADER*) &deviceName )))
        return;
    printf("  GDI Device name: ");
    wprintf(deviceName.viewGdiDeviceName);
    puts("");
}

/*
    Gets Device Path from Target
    e.g. \\?\DISPLAY#SAM0304#5&9a89472&0&UID33554704#{e6f07b5f-ee97-4a90-b076-33f57bf4eaa7}
*/
void getMonitorDevicePathFromTarget(LUID adapterId, UINT32 targetId) {
    DISPLAYCONFIG_TARGET_DEVICE_NAME deviceName;
    DISPLAYCONFIG_DEVICE_INFO_HEADER header;
    header.size = sizeof(DISPLAYCONFIG_TARGET_DEVICE_NAME);
    header.adapterId = adapterId;
    header.id = targetId;
    header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
    deviceName.header = header;
    if (!Result_DCGDI(DisplayConfigGetDeviceInfo( (DISPLAYCONFIG_DEVICE_INFO_HEADER*) &deviceName )))
        return;
    printf("  monitor device path: ");
    wprintf(deviceName.monitorDevicePath);
    puts("");
}


/*
    Gets Friendly name from Target (e.g. "SyncMaster")
*/
void getFriendlyNameFromTarget(LUID adapterId, UINT32 targetId) {
    DISPLAYCONFIG_TARGET_DEVICE_NAME deviceName;
    DISPLAYCONFIG_DEVICE_INFO_HEADER header;
    header.size = sizeof(DISPLAYCONFIG_TARGET_DEVICE_NAME);
    header.adapterId = adapterId;
    header.id = targetId;
    header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
    deviceName.header = header;
    if (!Result_DCGDI(DisplayConfigGetDeviceInfo( (DISPLAYCONFIG_DEVICE_INFO_HEADER*) &deviceName )))
        return;
    printf("  monitor friendly name: ");
    wprintf(deviceName.monitorFriendlyDeviceName);
    puts("");
}

int main(int argc, char *argv[]){
    UINT32 num_of_paths = 0;
    UINT32 num_of_modes = 0;
    
    // Get number of paths, and number of modes in query
    if (!Result_DCGDI(GetDisplayConfigBufferSizes(QDC_ALL_PATHS, &num_of_paths, &num_of_modes)))
        return 0;
    
    printf("num of paths: %d, num of infos: %d\r\n\r\n", num_of_paths, num_of_modes);
	
    // Allocate paths and modes dynamically
    DISPLAYCONFIG_PATH_INFO* displayPaths = (DISPLAYCONFIG_PATH_INFO*)malloc(sizeof(DISPLAYCONFIG_PATH_INFO)*num_of_paths);
    memset(displayPaths, 0, sizeof(DISPLAYCONFIG_PATH_INFO)*num_of_paths);
    DISPLAYCONFIG_MODE_INFO* displayModes = (DISPLAYCONFIG_MODE_INFO*)malloc(sizeof(DISPLAYCONFIG_MODE_INFO)*num_of_modes);
    memset(displayModes, 0, sizeof(DISPLAYCONFIG_MODE_INFO)*num_of_modes);
    
    // Query for the information (fill in the arrays above)
    if (!Result_QDC(QueryDisplayConfig(QDC_ALL_PATHS, &num_of_paths, displayPaths, &num_of_modes, displayModes, NULL)))
        return 0;
    
    // Loop through all paths
    puts("");
    puts("Display paths:");
    puts("--------------");
    for (int i = 0; i < num_of_paths; i++) {
        printf("Path %d:\r\n", i);
        
        if (displayPaths[i].flags & DISPLAYCONFIG_PATH_ACTIVE)
            puts("  ACTIVE");
        else
            puts("  NOT ACTIVE");
        
        if (displayPaths[i].targetInfo.targetAvailable)
            puts("  AVAILABLE");
        else
            puts("  NOT AVAILABLE");
        printf("  Source AID: %d\r\n", displayPaths[i].sourceInfo.adapterId);
        printf("  Source  ID: %d\r\n", displayPaths[i].sourceInfo.id);
        printf("  Target AID: %d\r\n", displayPaths[i].targetInfo.adapterId);
        printf("  Target  ID: %d\r\n", displayPaths[i].targetInfo.id);
        
        getGDIDeviceNameFromSource(displayPaths[i].sourceInfo.adapterId, displayPaths[i].sourceInfo.id);
    }
    
    puts("");
    
    // Loop through all attached monitors (mode = monitor, I suspect)
    // but same monitor is twice in the array, once as SOURCE and once as TARGET.
    puts("Attached monitor infos:");
    puts("-----------------------");
    for (int i = 0; i < num_of_modes; i++) {
        printf("Info %d:\r\n", i);
        
        switch (displayModes[i].infoType) {
            
            // This case is for all sources
            case DISPLAYCONFIG_MODE_INFO_TYPE_SOURCE:
                getGDIDeviceNameFromSource(displayModes[i].adapterId, displayModes[i].id);
                printf("  Source AID: %d\r\n", displayModes[i].adapterId);
                printf("  Source  ID: %d\r\n", displayModes[i].id);
                break;
            
            // This case is for all targets
            case DISPLAYCONFIG_MODE_INFO_TYPE_TARGET:
                getMonitorDevicePathFromTarget(displayModes[i].adapterId, displayModes[i].id);
                getFriendlyNameFromTarget(displayModes[i].adapterId, displayModes[i].id);
                printf("  Target AID: %d\r\n", displayModes[i].adapterId);
                printf("  Target  ID: %d\r\n", displayModes[i].id);
                break;
            
            default:
                fputs("  ERROR: infoType is invalid.", stderr);
                break;
        }
    }
    /*
        Discovered from SetDisplayConfig so far:
        (First you must know what WIN + P shortcut does in Windows 7, try that.)
            Computer only:
                SDC_APPLY|SDC_TOPOLOGY_INTERNAL
            Projector only:
                SDC_APPLY|SDC_TOPOLOGY_EXTERNAL
            Extended
                SDC_APPLY|SDC_TOPOLOGY_EXTEND
            Clone
                SDC_APPLY|SDC_TOPOLOGY_CLONE
    */
    if (!Result_DCGDI(SetDisplayConfig(0, NULL, 0, NULL, SDC_VALIDATE|SDC_TOPOLOGY_INTERNAL)))
        return 0;
    Result_DCGDI(SetDisplayConfig(0, NULL, 0, NULL, SDC_APPLY|SDC_TOPOLOGY_INTERNAL));
    puts("Ahoy there!");
}
