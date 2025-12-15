#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
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

extern bool g_beepEnabled, g_waitingForKey;
extern int g_hotkey;
extern volatile bool g_running;
extern HWND g_mainWindow;
extern HICON g_iconOn, g_iconOff;

void muteAudio(bool muted);
void playBeep(bool muted);
const char* keyToString(int vkey);
int stringToKey(const char* str);
int getKeyInput(void);
void loadSettings(void);
void saveSettings(void);
void loadIcons(void);
void updateIcon(bool muted);

#endif