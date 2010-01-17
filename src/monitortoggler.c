/*
	License: FreeBSD License, see COPYING.
    Author: Jari Pennanen (2010) <jari.pennanen@gmail.com>
*/
#include <stdio.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <math.h>

int ChangeSettingsExResult(long int result) {
	switch (result) {
		case DISP_CHANGE_SUCCESSFUL:
			puts("  The settings change was successful.");
			return 1;
			break;
		case DISP_CHANGE_BADFLAGS:
			fputs("  Error: An invalid set of values was used in the dwFlags parameter.", stderr);
			break;
		case DISP_CHANGE_BADMODE:
			fputs("  Error: The graphics mode is not supported.", stderr);
			break;
		case DISP_CHANGE_BADPARAM:
			fputs("  Error: An invalid parameter was used.", stderr);
			break;
		case DISP_CHANGE_FAILED:
			fputs("  Error: The display driver failed the specified graphics mode.", stderr);
			break;
		case DISP_CHANGE_NOTUPDATED:
			fputs(  "Error: ChangeDisplaySettingsEx was unable to write settings to the registry.", stderr);
			break;
		case DISP_CHANGE_RESTART:
			fputs("  Error: The user must restart the computer for the graphics mode to work.", stderr);
			break;
	}

	return 0;
}

int main(int argc, char *argv[]){
	// TODO: This is one horribly long main(), not good! But works for now.

    HDC hDC = NULL;
    DISPLAY_DEVICE displayDeviceToggled;
	DEVMODE defaultMode;
	DEVMODE currentMode;
    long int monitor_number;
	long int forceState = -1;
	long int apply = 1;
    int i = 0;
    
	// Initialize DISPLAY_DEVICE
    FillMemory(&displayDeviceToggled, sizeof(DISPLAY_DEVICE), 0);
    displayDeviceToggled.cb = sizeof(DISPLAY_DEVICE);

	// Initialize DEVMODEs
	ZeroMemory(&defaultMode, sizeof(DEVMODE));
	defaultMode.dmSize = sizeof(DEVMODE);

	ZeroMemory(&currentMode, sizeof(DEVMODE));
	currentMode.dmSize = sizeof(DEVMODE);


	if (argc <= 1) {
        puts("Monitor toggler 0.3"
		"\r\n"
		"\r\nUsage: monitortoggler.exe <monitornumber> [<forceState>] [<apply>]"
		"\r\n"
		"\r\n  Capable of attaching and deattaching monitors from command line."
		"\r\n"
		"\r\n  Note: Resolution / display settings should be configured from Windows dialog."
		"\r\n        At least in Windows 7 the changes are saved to registry, and next time "
		"\r\n        you attach the same screen it retrieves same settings. Thus it seems"
		"\r\n        to be useless to define them while attaching."
		"\r\n"
		"\r\n  monitornumber:"
		"\r\n			 Monitor number, this may, and usually differs from monitor numbers"
		"\r\n			 in the screen resolution dialog. So you have to try several numbers"
		"\r\n			 until you find the right one :)"
		"\r\n"
		"\r\n  forceState (optional):"
		"\r\n			-1 = Toggles. (Tries to get the current mode, and toggles.) (default)"
		"\r\n			 1 = Attach"
		"\r\n			 0 = Detach"
		"\r\n"
		"\r\n  apply (optional):"
		"\r\n			 1 = Apply changes immediately (default)"
		"\r\n			 0 = Do not apply changes"
		"\r\n			     You have to call second time to apply changes"
		"\r\n"
		"\r\n  Author:     Jari Pennanen (2010) <jari.pennanen@gmail.com>"
		"\r\n  License:    FreeBSD License, see COPYING"
		"\r\n  Repository: http://github.com/Ciantic/monitortoggler"
		"\r\n");
		return 0;
	}

	// Parse monitor number
    monitor_number = strtol(argv[1], NULL, 0);

	// Parse forceState parameter, if given
	if (argc >= 3) {
		forceState = strtol(argv[2], NULL, 0);

		if (forceState > 1 || forceState < -1) {
			fputs("  Error: <force> can only be -1, 0, or 1.", stderr);
			return 0;
		}
	}

	// Parse apply parameter, if given
	if (argc == 4) {
		apply = strtol(argv[3], NULL, 0);

		if (apply > 1 || apply < 0) {
			fputs("  Error: <apply> can only be 0, or 1.", stderr);
			return 0;
		}

	}

	// Empty line for fun
	puts("");

	// Get Display Device
    if (!EnumDisplayDevices(NULL, monitor_number-1, (DISPLAY_DEVICE*) &displayDeviceToggled, 0)) {
        fprintf(stderr, "  Error: Monitor number %d is not valid according to 'EnumDisplayDevices'.\r\n", monitor_number);
        return 0;
    }

	// If user wants to toggle, we must get the state is it on or off.
	if (forceState == -1) {
		printf("Retrieving state of %d using 'EnumDisplaySettingsEx'...\r\n", monitor_number);

		// Query the state
		if (!EnumDisplaySettingsEx((LPSTR) displayDeviceToggled.DeviceName, ENUM_CURRENT_SETTINGS, &currentMode, NULL)) {
			// It is turned off most likely at the moment.
			puts("  Monitor is currently detached.");

			// Attach
			forceState = 1;
		} else {
			puts("  Monitor is currently attached.");

			// Detach
			forceState = 0;
		}
		puts("  ^ If above statement is lie, you have to use <forceState>.");
		puts("Ok.\n");
	}

	printf("Trying to change settings of '%s' using ChangeDisplaySettingsEx...\r\n", displayDeviceToggled.DeviceName);

	// It's unwise to deal with Primary monitors
	if (displayDeviceToggled.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) {
		puts("Sorry, this program does not allow to detach / reattach primary monitor.\r\n");
		return 0;
	}

	// DEVMODE Settings
	defaultMode.dmFields = DM_POSITION;

	// Detach or attach?
	if (forceState == 0) {
		puts("  Detaching monitor...");
		// Setting these, means "detaching" monitor Detaching code: http://support.microsoft.com/kb/306399
		defaultMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_POSITION;
		defaultMode.dmPelsWidth = 0;
		defaultMode.dmPelsHeight = 0;
	} else {
		// Attaching code: http://support.microsoft.com/kb/308216
		puts("  Attaching monitor...");
	}

	// Change the settings
	if (!ChangeSettingsExResult(ChangeDisplaySettingsEx((LPSTR)displayDeviceToggled.DeviceName, &defaultMode, NULL, CDS_NORESET|CDS_UPDATEREGISTRY, NULL)))
		return 0;

	puts("Ok.\n");

	if (apply == 1) {
		puts("Trying to apply settings changes...");

		// Second call applies the changes
		if (!ChangeSettingsExResult(ChangeDisplaySettingsEx (NULL, NULL, NULL, 0, NULL)))
			return 0;

		puts("Ok.\n");
	}

    return 0;
}
