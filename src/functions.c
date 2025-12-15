#define INITGUID
#include "functions.h"

// Define global variables
bool g_beepEnabled = false, g_waitingForKey = false;
int g_hotkey = VK_RCONTROL;
volatile bool g_running = true;
HICON g_iconOn = NULL, g_iconOff = NULL;

void playBeep(bool muted) {
    int freq1 = muted ? 1200 : 400;
    int freq2 = muted ? 800 : 800;
    int freq3 = muted ? 400 : 1200;
    
    Beep(freq1, 80); Sleep(30);
    Beep(freq2, 80); Sleep(30);
    Beep(freq3, muted ? 100 : 120);
}

const char* keyToString(int vkey) {
    static char buf[32];
    
    if(vkey >= 'A' && vkey <= 'Z') { sprintf(buf, "%c", vkey); return buf; }
    if(vkey >= '0' && vkey <= '9') { sprintf(buf, "%c", vkey); return buf; }
    
    switch(vkey) {
        case VK_LCONTROL: return "LCtrl";
        case VK_RCONTROL: return "RCtrl";
        case VK_LSHIFT: return "LShift";
        case VK_RSHIFT: return "RShift";
        case VK_LMENU: return "LAlt";
        case VK_RMENU: return "RAlt";
        case VK_SPACE: return "Space";
        case VK_TAB: return "Tab";
        case VK_RETURN: return "Enter";
        case VK_ESCAPE: return "Esc";
        case VK_F1 ... VK_F15: sprintf(buf, "F%d", vkey - VK_F1 + 1); return buf;
        case VK_INSERT: return "Ins";
        case VK_DELETE: return "Del";
        case VK_HOME: return "Home";
        case VK_END: return "End";
        case VK_PRIOR: return "PgUp";
        case VK_NEXT: return "PgDn";
        case VK_UP: return "Up";
        case VK_DOWN: return "Down";
        case VK_LEFT: return "Left";
        case VK_RIGHT: return "Right";
        default: sprintf(buf, "Key%d", vkey); return buf;
    }
}

int stringToKey(const char* str) {
    if(strlen(str) == 1) {
        if(str[0] >= 'A' && str[0] <= 'Z') return str[0];
        if(str[0] >= '0' && str[0] <= '9') return str[0];
    }
    
    if(strcmp(str, "LCtrl") == 0) return VK_LCONTROL;
    if(strcmp(str, "RCtrl") == 0) return VK_RCONTROL;
    if(strcmp(str, "LShift") == 0) return VK_LSHIFT;
    if(strcmp(str, "RShift") == 0) return VK_RSHIFT;
    if(strcmp(str, "LAlt") == 0) return VK_LMENU;
    if(strcmp(str, "RAlt") == 0) return VK_RMENU;
    if(strcmp(str, "Space") == 0) return VK_SPACE;
    if(strcmp(str, "Enter") == 0) return VK_RETURN;
    if(strcmp(str, "Esc") == 0) return VK_ESCAPE;
    
    return VK_RCONTROL;
}

int getKeyInput(void) {
    int lastKey = -1;
    while(g_waitingForKey) {
        for(int i = 0; i < 256; i++) {
            if(GetAsyncKeyState(i) & 0x8000) {
                if(i != lastKey && i > 32 && i < 255) {
                    lastKey = i;
                    Sleep(200);
                    return i;
                }
            }
        }
        Sleep(10);
    }
    return VK_RCONTROL;
}

void loadSettings(void) {
    FILE* f = fopen("config.ini", "r");
    if(!f) return;
    
    char line[256];
    while(fgets(line, sizeof(line), f)) {
        if(sscanf(line, "hotkey=%255s", line) == 1) g_hotkey = stringToKey(line);
        else if(sscanf(line, "beep=%d", (int*)&g_beepEnabled) == 1) continue;
    }
    fclose(f);
}

void saveSettings(void) {
    FILE* f = fopen("config.ini", "w");
    if(f) {
        fprintf(f, "hotkey=%s\nbeep=%d\n", keyToString(g_hotkey), g_beepEnabled);
        fclose(f);
    }
}

void loadIcons(void) {
    HINSTANCE hInst = GetModuleHandle(NULL);
    
    // Load 32x32 icons from resources
    g_iconOn = LoadIcon(hInst, MAKEINTRESOURCE(101));
    g_iconOff = LoadIcon(hInst, MAKEINTRESOURCE(102));
    
    // If resource icons not found, create larger system icons
    if(!g_iconOn) {
        g_iconOn = LoadImage(NULL, IDI_INFORMATION, IMAGE_ICON, 32, 32, LR_SHARED);
    }
    if(!g_iconOff) {
        g_iconOff = LoadImage(NULL, IDI_HAND, IMAGE_ICON, 32, 32, LR_SHARED);
    }
}

void updateIcon(bool muted) {
    NOTIFYICONDATA nid = {sizeof(nid)};
    nid.hWnd = g_mainWindow;
    nid.uID = ID_TRAY;
    nid.uFlags = NIF_ICON;
    nid.hIcon = muted ? g_iconOff : g_iconOn;
    Shell_NotifyIcon(NIM_MODIFY, &nid);
}

void muteAudio(bool muted) {
    IMMDeviceEnumerator *pEnum = NULL;
    IMMDevice *pDev = NULL;
    IAudioEndpointVolume *pVol = NULL;

    CoInitializeEx(NULL, COINIT_MULTITHREADED);
    
    if(SUCCEEDED(CoCreateInstance(&CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, &IID_IMMDeviceEnumerator, (void**)&pEnum))) {
        if(SUCCEEDED(pEnum->lpVtbl->GetDefaultAudioEndpoint(pEnum, eCapture, eCommunications, &pDev))) {
            if(SUCCEEDED(pDev->lpVtbl->Activate(pDev, &IID_IAudioEndpointVolume, CLSCTX_ALL, NULL, (void**)&pVol))) {
                pVol->lpVtbl->SetMute(pVol, muted, NULL);
                pVol->lpVtbl->Release(pVol);
            }
            pDev->lpVtbl->Release(pDev);
        }
        pEnum->lpVtbl->Release(pEnum);
    }
    
    CoUninitialize();
}