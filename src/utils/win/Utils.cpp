#include "Windows.h"
#include "WinUser.h"

bool wow_is_foreground_window() {
    HWND foreground = GetForegroundWindow();
    if (!foreground) {
        return false;
    }
    char window_title[256];
    GetWindowText(foreground, (LPWSTR) window_title, 256);

    QString title = QString::fromWCharArray((const wchar_t *) window_title);

    return title == QString("World of Warcraft");

}
