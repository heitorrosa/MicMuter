#define INITGUID
#include "functions.h"

bool g_beepEnabled = 0, g_waitingForKey = 0, g_running = 1;
int g_hotkey = VK_RCONTROL, g_muted = 0;
HICON g_iconOn = 0, g_iconOff = 0;

void playBeep(bool m) {
    int f[] = {m ? 1200 : 400, 800, m ? 400 : 1200};
    int d[] = {80, 80, m ? 100 : 120};
    for(int i = 0; i < 3; i++) { Beep(f[i], d[i]); Sleep(30); }
}

const char* keyToString(int v) {
    static char b[32];
    if((v >= 'A' && v <= 'Z') || (v >= '0' && v <= '9')) { sprintf(b, "%c", v); return b; }
    
    struct { int k; const char* s; } t[] = {
        {VK_LCONTROL,"LCtrl"},{VK_RCONTROL,"RCtrl"},{VK_LSHIFT,"LShift"},{VK_RSHIFT,"RShift"},
        {VK_LMENU,"LAlt"},{VK_RMENU,"RAlt"},{VK_SPACE,"Space"},{VK_TAB,"Tab"},
        {VK_RETURN,"Enter"},{VK_ESCAPE,"Esc"},{VK_INSERT,"Ins"},{VK_DELETE,"Del"},
        {VK_HOME,"Home"},{VK_END,"End"},{VK_PRIOR,"PgUp"},{VK_NEXT,"PgDn"},
        {VK_UP,"Up"},{VK_DOWN,"Down"},{VK_LEFT,"Left"},{VK_RIGHT,"Right"}
    };
    
    for(int i = 0; i < 20; i++) if(v == t[i].k) return t[i].s;
    if(v >= VK_F1 && v <= VK_F15) { sprintf(b, "F%d", v - VK_F1 + 1); return b; }
    sprintf(b, "Key%d", v); return b;
}

int stringToKey(const char* s) {
    if(strlen(s) == 1) return (s[0] >= 'A' && s[0] <= 'Z') || (s[0] >= '0' && s[0] <= '9') ? s[0] : VK_RCONTROL;
    
    struct { const char* s; int k; } t[] = {
        {"LCtrl",VK_LCONTROL},{"RCtrl",VK_RCONTROL},{"LShift",VK_LSHIFT},{"RShift",VK_RSHIFT},
        {"LAlt",VK_LMENU},{"RAlt",VK_RMENU},{"Space",VK_SPACE},{"Enter",VK_RETURN},{"Esc",VK_ESCAPE}
    };
    
    for(int i = 0; i < 9; i++) if(!strcmp(s, t[i].s)) return t[i].k;
    return VK_RCONTROL;
}

int getKeyInput(void) {
    int last = -1;
    while(g_waitingForKey) {
        for(int i = 0; i < 256; i++) {
            if((GetAsyncKeyState(i) & 0x8000) && i != last && i > 32 && i < 255) {
                last = i; Sleep(200); return i;
            }
        }
        Sleep(10);
    }
    return VK_RCONTROL;
}

void loadSettings(void) {
    FILE* f = fopen("config.ini", "r");
    if(!f) return;
    char l[256], k[32];
    while(fgets(l, 256, f)) {
        if(sscanf(l, "hotkey=%31s", k) == 1) g_hotkey = stringToKey(k);
        else sscanf(l, "beep=%d", (int*)&g_beepEnabled);
    }
    fclose(f);
}

void saveSettings(void) {
    FILE* f = fopen("config.ini", "w");
    if(f) { fprintf(f, "hotkey=%s\nbeep=%d\n", keyToString(g_hotkey), g_beepEnabled); fclose(f); }
}

void loadIcons(void) {
    g_iconOn = LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(101));
    g_iconOff = LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(102));
    if(!g_iconOn) g_iconOn = (HICON)LoadImage(0, IDI_INFORMATION, IMAGE_ICON, 32, 32, LR_SHARED);
    if(!g_iconOff) g_iconOff = (HICON)LoadImage(0, IDI_HAND, IMAGE_ICON, 32, 32, LR_SHARED);
}

void updateIcon(bool m) {
    NOTIFYICONDATA n = {sizeof(n), g_mainWindow, ID_TRAY, NIF_ICON, 0, m ? g_iconOff : g_iconOn};
    Shell_NotifyIcon(NIM_MODIFY, &n);
}

void muteAudio(bool m) {
    IMMDeviceEnumerator *e = 0; IMMDevice *d = 0; IAudioEndpointVolume *v = 0;
    CoInitializeEx(0, COINIT_MULTITHREADED);
    if(SUCCEEDED(CoCreateInstance(&CLSID_MMDeviceEnumerator, 0, CLSCTX_ALL, &IID_IMMDeviceEnumerator, (void**)&e))) {
        if(SUCCEEDED(e->lpVtbl->GetDefaultAudioEndpoint(e, eCapture, eCommunications, &d))) {
            if(SUCCEEDED(d->lpVtbl->Activate(d, &IID_IAudioEndpointVolume, CLSCTX_ALL, 0, (void**)&v))) {
                v->lpVtbl->SetMute(v, m, 0); v->lpVtbl->Release(v);
            }
            d->lpVtbl->Release(d);
        }
        e->lpVtbl->Release(e);
    }
    CoUninitialize();
}