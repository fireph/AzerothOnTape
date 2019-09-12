#include <ApplicationServices/ApplicationServices.h>

bool cwow_is_foreground_window() {
    auto windowList = CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly, kCGNullWindowID);
    CFIndex numWindows = CFArrayGetCount(windowList);

    for (int i = 0; i < (int) numWindows; i++) {
        char windowName[128] = {0};

        auto dict = static_cast<CFDictionaryRef>(CFArrayGetValueAtIndex(windowList, i));
        auto cfwindowname = static_cast<CFStringRef>(CFDictionaryGetValue(dict, kCGWindowName));
        CFStringGetCString(cfwindowname, windowName, sizeof(windowName), kCFStringEncodingUTF8);
        windowName[sizeof(windowName) - 1] = '\n';

        uint32_t layer = 0;
        CFNumberGetValue(static_cast<CFNumberRef>(CFDictionaryGetValue(dict, kCGWindowLayer)),
                         kCFNumberSInt32Type,
                         &layer);

        if (layer == 0) {
            CFRelease(windowList);
            bool ret = strcmp(windowName, "world of warcraft") == 0 || strcmp(windowName, "World of Warcraft") == 0;
            return ret;
        }
    }
    CFRelease(windowList);
    return false;

}
