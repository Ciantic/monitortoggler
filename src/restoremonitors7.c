/*
    License: FreeBSD License, see COPYING.
    Author: Jari Pennanen (2010) <jari.pennanen@gmail.com>
    
    I might have found a most difficult way of retrieving "\\.\DISPLAYX" string.
    
    http://msdn.microsoft.com/en-us/library/dd567877.aspx
    
    TODO: Naming conventions in this file are odd.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

int saveSettingsToFile(char* filename, UINT32 num_of_paths, UINT32 num_of_modes, DISPLAYCONFIG_PATH_INFO **displayPaths, DISPLAYCONFIG_MODE_INFO **displayModes) {    
    FILE* settingsFile;
    
    settingsFile = fopen(filename, "wb");
    if (settingsFile == NULL) {
        fputs("  Error: Could not open file...", stderr);
        return 0;
    }
    
    fputs("SetDisplayConfig", settingsFile);
    fwrite(&num_of_paths, sizeof(UINT32), 1, settingsFile);
    fwrite(&num_of_modes, sizeof(UINT32), 1, settingsFile);
    fwrite((*displayPaths), sizeof(DISPLAYCONFIG_PATH_INFO)*num_of_paths, 1, settingsFile);
    fwrite((*displayModes), sizeof(DISPLAYCONFIG_MODE_INFO)*num_of_modes, 1, settingsFile);
    fclose(settingsFile);
    return 1;
}

int openSettingsFromFile(char* filename, UINT32 *num_of_paths, UINT32 *num_of_modes, DISPLAYCONFIG_PATH_INFO **displayPaths, DISPLAYCONFIG_MODE_INFO **displayModes) {
    FILE* settingsFile;
    char header [17];
    settingsFile = fopen(filename, "rb");
    if (settingsFile == NULL) {
        fputs("  Error: Could not open file...", stderr);
        return 0;
    }
    
    // Header "SetDisplayConfig"
    fgets(header, 17, settingsFile);
    if (strcmp(header, "SetDisplayConfig") != 0) {
        fputs("  Error: Header of the file is incorrect...", stderr);
        return 0;
    }
    
    // Array sizes should be first
    fread(num_of_paths, sizeof(UINT32), 1, settingsFile);
    fread(num_of_modes, sizeof(UINT32), 1, settingsFile);
    
    // Allocate arrays
    *displayPaths = (DISPLAYCONFIG_PATH_INFO*)calloc((int)num_of_paths, sizeof(DISPLAYCONFIG_PATH_INFO));
    *displayModes = (DISPLAYCONFIG_MODE_INFO*)calloc((int)num_of_modes, sizeof(DISPLAYCONFIG_MODE_INFO));
    
    // Read arrays
    fread((*displayPaths), sizeof(DISPLAYCONFIG_PATH_INFO) * (*num_of_paths), 1, settingsFile);
    fread((*displayModes), sizeof(DISPLAYCONFIG_MODE_INFO) * (*num_of_modes), 1, settingsFile);
    
    // It should be now at the end of file:
    fgetc(settingsFile);
    if (!feof(settingsFile)) {
        fputs("  Error: EOF is not in right place...", stderr);
        return 0;
    }
    
    // Close the file
    fclose(settingsFile);
    return 1;
}

void printDisplayPaths(UINT32 num_of_paths, DISPLAYCONFIG_PATH_INFO* displayPaths) {
    int i;
    puts("");
    puts("Display paths:");
    puts("--------------");
    for (i = 0; i < num_of_paths; i++) {
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
}

void printDisplayModeInfos(UINT32 num_of_modes, DISPLAYCONFIG_MODE_INFO* displayModes) {
    int i;
    // but same monitor is twice in the array, once as SOURCE and once as TARGET.
    puts("Attached monitor infos:");
    puts("-----------------------");
    for (i = 0; i < num_of_modes; i++) {
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
}

int getCurrentSettings(UINT32* num_of_paths, UINT32* num_of_modes, DISPLAYCONFIG_PATH_INFO** displayPaths, DISPLAYCONFIG_MODE_INFO** displayModes) {
    // Get number of paths, and number of modes in query
    if (!Result_DCGDI(GetDisplayConfigBufferSizes(QDC_ALL_PATHS, num_of_paths, num_of_modes)))
        return 0;
    
    // Allocate paths and modes dynamically
    *displayPaths = (DISPLAYCONFIG_PATH_INFO*)calloc((int)num_of_paths, sizeof(DISPLAYCONFIG_PATH_INFO));
    *displayModes = (DISPLAYCONFIG_MODE_INFO*)calloc((int)num_of_modes, sizeof(DISPLAYCONFIG_MODE_INFO));
    
    // Query for the information (fill in the arrays above)
    if (!Result_QDC(QueryDisplayConfig(QDC_ALL_PATHS, num_of_paths, (*displayPaths), num_of_modes, (*displayModes), NULL)))
        return 0;
    
    return 1;
}

enum ActionType { OPEN, SAVE, EQUAL };

int main(int argc, char *argv[]){
    UINT32 num_of_paths = 0;
    UINT32 num_of_modes = 0;
    DISPLAYCONFIG_PATH_INFO* displayPaths = NULL; 
    DISPLAYCONFIG_MODE_INFO* displayModes = NULL;
    
    UINT32 num_of_paths2 = 0;
    UINT32 num_of_modes2 = 0;
    DISPLAYCONFIG_PATH_INFO* displayPaths2 = NULL; 
    DISPLAYCONFIG_MODE_INFO* displayModes2 = NULL;

    enum ActionType action;
    char* filename;
    int debug = 0;
    BOOL show_usage = TRUE;
    
    // -save argument given
    if (argc == 3 && strcmp(argv[1], "-save") == 0) {
        action = SAVE;
        filename = argv[2];
        show_usage = FALSE;
        
    // -equal argument given
    } else if (argc == 3 && strcmp(argv[1], "-equal") == 0) {
        action = EQUAL;
        filename = argv[2];
        show_usage = FALSE;
        
    // No arguments given, assumed open
    } else if (argc == 2) {
        action = OPEN;
        filename = argv[1];
        show_usage = FALSE;
    }

    // Show usage if invalid options or none
    if (show_usage) {
        puts(
            "Restore Monitors 0.2\n"
            "\n"
            "Usage: restoremonitors7.exe [<-save>] <filename>\n"
            "\n"
            "  Capable of restoring monitors to saved state under Windows 7.\n"
            "  Uses Windows 7 CCD API to save, and restore the settings from\n"
            "  file.\n"
            "\n"
            "  By giving only filename the program tries to open and restore\n"
            "  the saved settings in the file.\n"
            "\n"
            "   -save\n"
            "       Used to save settings to file.\n"
            "\n"
            "   -equal\n"
            "       Prints '1' if current settings equals the one in the file,\n"
            "       otherwise '0' or '  ERROR:...'.\n"
            "\n"
            "\n  Author:     Jari Pennanen (2010) <jari.pennanen@gmail.com>"
            "\n  License:    FreeBSD License, see COPYING"
            "\n  Repository: http://github.com/Ciantic/monitortoggler"
        );
        
        return 0;
    }
    
    switch (action) {
        case EQUAL:
            // Settings in file:
            if (!openSettingsFromFile(filename, &num_of_paths, &num_of_modes, &displayPaths, &displayModes))
                return 0;
            
            // Current settings in computer
            getCurrentSettings(&num_of_paths2, &num_of_modes2, &displayPaths2, &displayModes2);
            
            // Compare settings of file and computer
            if (num_of_paths == num_of_paths2 && num_of_modes == num_of_modes2) {
                if (memcmp(displayPaths, displayPaths2, num_of_paths*sizeof(DISPLAYCONFIG_PATH_INFO)) != 0) {
                    puts("0");
                    return 0;
                }
                if (memcmp(displayModes, displayModes2, num_of_modes*sizeof(DISPLAYCONFIG_MODE_INFO)) != 0) {
                    puts("0");
                    return 0;
                }
                puts("1");
                return 0;
            }
            puts("0");
            return 0;

        
        case OPEN:
            printf("Opening settings from '%s' file...\n", filename);
            if (!openSettingsFromFile(filename, &num_of_paths, &num_of_modes, &displayPaths, &displayModes))
                return 0;
            
            if (debug) {
                printf("num of paths %d\n", num_of_paths);
                printf("num of modes %d\n", num_of_modes);
                printDisplayPaths(num_of_paths, displayPaths);
                printDisplayModeInfos(num_of_modes, displayModes);
            }
            
            puts("Validating settings...");
            if (!Result_DCGDI(SetDisplayConfig(num_of_paths, displayPaths, num_of_modes, displayModes, SDC_VALIDATE | SDC_USE_SUPPLIED_DISPLAY_CONFIG)))
                return 0;
            puts("Restoring settings...");
            Result_DCGDI(SetDisplayConfig(num_of_paths, displayPaths, num_of_modes, displayModes, SDC_APPLY | SDC_USE_SUPPLIED_DISPLAY_CONFIG));
            break;
            
            
        case SAVE:
            puts("Querying current settings...");
            getCurrentSettings(&num_of_paths, &num_of_modes, &displayPaths, &displayModes);
            
            if (debug) {
                printf("num of paths %d\n", num_of_paths);
                printf("num of modes %d\n", num_of_modes);
                printDisplayPaths(num_of_paths, displayPaths);
                printDisplayModeInfos(num_of_modes, displayModes);
            }
            
            printf("Saving settings to '%s'...\n", filename);
            if (!saveSettingsToFile(filename, num_of_paths, num_of_modes, &displayPaths, &displayModes))
                return 0;
            break;
    }
    free(displayPaths);
    free(displayModes);
    puts("Done.");
    return 0;
}
