#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <windows.h>
#include <shellapi.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

#define ID_TRAY 1000
#define ID_KEYBIND 1001
#define ID_BEEP 1002
#define ID_EXIT 1003

extern bool g_beepEnabled, g_waitingForKey, g_running;
extern int g_hotkey, g_muted;
extern HWND g_mainWindow;
extern HICON g_iconOn, g_iconOff;

void muteAudio(bool m), playBeep(bool m), loadSettings(void), saveSettings(void), ensureConfig(void);
void loadIcons(void), updateIcon(bool m), createTrayIcon(HWND h);
const char* keyToString(int v);
int stringToKey(const char* s), getKeyInput(void);

#endif