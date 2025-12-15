#include "functions.h"

HWND g_mainWindow = NULL;
bool g_muted = false;

DWORD WINAPI HotkeyThread(LPVOID p) {
    bool wasPressed = false;
    
    while(g_running) {
        bool isPressed = (GetAsyncKeyState(g_hotkey) & 0x8000) != 0;
        
        if(isPressed && !wasPressed && !g_waitingForKey) {
            g_muted = !g_muted;
            muteAudio(g_muted);
            updateIcon(g_muted);
            if(g_beepEnabled) playBeep(g_muted);
        }
        
        wasPressed = isPressed;
        Sleep(10);
    }
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    static NOTIFYICONDATA nid = {sizeof(nid)};
    
    if(msg == WM_CREATE) {
        nid.hWnd = hwnd;
        nid.uID = ID_TRAY;
        nid.uFlags = NIF_ICON | NIF_MESSAGE;
        nid.uCallbackMessage = WM_USER + 1;
        nid.hIcon = g_iconOn;
        nid.uVersion = NOTIFYICON_VERSION_4;  // Enable newer notification features
        Shell_NotifyIcon(NIM_ADD, &nid);
        Shell_NotifyIcon(NIM_SETVERSION, &nid);  // Set to latest version for better scaling
        CreateThread(NULL, 0, HotkeyThread, NULL, 0, NULL);
    }
    else if(msg == WM_USER + 1 && lp == WM_RBUTTONUP) {
        POINT pt;
        GetCursorPos(&pt);
        HMENU menu = CreatePopupMenu();
        char txt[64];
        sprintf(txt, "Hotkey: %s", g_waitingForKey ? "..." : keyToString(g_hotkey));
        AppendMenu(menu, MF_STRING | (g_waitingForKey ? MF_GRAYED : 0), ID_KEYBIND, txt);
        AppendMenu(menu, MF_STRING | (g_beepEnabled ? MF_CHECKED : 0), ID_BEEP, "Beep Sound");
        AppendMenu(menu, MF_SEPARATOR, 0, NULL);
        AppendMenu(menu, MF_STRING, ID_EXIT, "Exit");
        SetForegroundWindow(hwnd);
        TrackPopupMenu(menu, TPM_RIGHTALIGN | TPM_BOTTOMALIGN, pt.x, pt.y, 0, hwnd, NULL);
        DestroyMenu(menu);
    }
    else if(msg == WM_COMMAND) {
        switch(LOWORD(wp)) {
            case ID_KEYBIND:
                g_waitingForKey = true;
                g_hotkey = getKeyInput();
                g_waitingForKey = false;
                saveSettings();
                break;
            case ID_BEEP:
                g_beepEnabled = !g_beepEnabled;
                saveSettings();
                break;
            case ID_EXIT:
                PostQuitMessage(0);
                break;
        }
    }
    else if(msg == WM_DESTROY) {
        g_running = false;
        Shell_NotifyIcon(NIM_DELETE, &nid);
        PostQuitMessage(0);
    }
    
    return DefWindowProc(hwnd, msg, wp, lp);
}

int main() {
    loadSettings();
    loadIcons();
    
    WNDCLASS wc = {.lpfnWndProc = WindowProc, .lpszClassName = "MicMuter"};
    RegisterClass(&wc);
    g_mainWindow = CreateWindowEx(0, "MicMuter", "", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, NULL, NULL);
    
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)) {
        DispatchMessage(&msg);
    }
    
    return 0;
}