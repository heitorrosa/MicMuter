#include "functions.h"

HWND g_mainWindow = 0;

DWORD WINAPI HotkeyThread(LPVOID p) {
    bool was = 0;
    while(g_running) {
        bool is = (GetAsyncKeyState(g_hotkey) & 0x8000) != 0;
        if(is && !was && !g_waitingForKey) {
            g_muted = !g_muted;
            muteAudio(g_muted);
            updateIcon(g_muted);
            if(g_beepEnabled) playBeep(g_muted);
        }
        was = is;
        Sleep(10);
    }
    return 0;
}

LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    static NOTIFYICONDATA n = {0};
    
    if(m == WM_CREATE) {
        n.cbSize = sizeof(n); n.hWnd = h; n.uID = ID_TRAY;
        n.uFlags = NIF_ICON | NIF_MESSAGE; n.uCallbackMessage = WM_APP;
        n.hIcon = g_iconOn; n.uVersion = NOTIFYICON_VERSION_4;
        Shell_NotifyIcon(NIM_ADD, &n); Shell_NotifyIcon(NIM_SETVERSION, &n);
        CreateThread(0, 0, HotkeyThread, 0, 0, 0);
    }
    else if(m == WM_APP) {
        if(LOWORD(l) == WM_RBUTTONUP) {
            POINT p; GetCursorPos(&p);
            HMENU mn = CreatePopupMenu();
            char t[64]; sprintf(t, "Hotkey: %s", g_waitingForKey ? "..." : keyToString(g_hotkey));
            AppendMenu(mn, MF_STRING | (g_waitingForKey ? MF_GRAYED : 0), ID_KEYBIND, t);
            AppendMenu(mn, MF_STRING | (g_beepEnabled ? MF_CHECKED : 0), ID_BEEP, "Beep Sound");
            AppendMenu(mn, MF_SEPARATOR, 0, 0); 
            AppendMenu(mn, MF_STRING, ID_EXIT, "Exit");
            SetForegroundWindow(h);
            TrackPopupMenu(mn, TPM_RIGHTALIGN | TPM_BOTTOMALIGN, p.x, p.y, 0, h, 0);
            DestroyMenu(mn);
        }
    }
    else if(m == WM_COMMAND) {
        int id = LOWORD(w);
        if(id == ID_KEYBIND) { 
            g_waitingForKey = 1; 
            g_hotkey = getKeyInput(); 
            g_waitingForKey = 0; 
            saveSettings(); 
        }
        else if(id == ID_BEEP) { 
            g_beepEnabled = !g_beepEnabled; 
            saveSettings(); 
        }
        else if(id == ID_EXIT) PostQuitMessage(0);
    }
    else if(m == WM_DESTROY) { 
        g_running = 0; 
        Shell_NotifyIcon(NIM_DELETE, &n); 
        PostQuitMessage(0); 
    }
    
    return DefWindowProc(h, m, w, l);
}

int wmain() {
    FreeConsole();  // Hide console window if any
    loadSettings(); 
    loadIcons();
    WNDCLASS wc = {0, WndProc, 0, 0, 0, 0, 0, 0, 0, "MicMuter"};
    RegisterClass(&wc);
    g_mainWindow = CreateWindowEx(0, "MicMuter", "", 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, 0);
    MSG msg;
    while(GetMessage(&msg, 0, 0, 0)) DispatchMessage(&msg);
    return 0;
}