
#ifdef AAA_SYSTEM_KEYMAP_H
#error "SYSTEM_KEYMAP_H included more than once."
#endif
#define AAA_SYSTEM_KEYMAP_H 1


#ifndef AAA_SYSTEMKEYBOARD_H
#	include "system/win32/SystemKeyboard.h"
#endif
#ifndef AAA_SYSTEMMOUSE_H
#	include "system/shared/SystemMouse.h"
#endif
#ifndef	AAA_AAA_OS_H
#	include "aaa_os.h"
#endif

namespace keymap
{
    keyboard::SYMBOL mapVirtualKeyCode(WPARAM vCode);

    // NOTE: translateKeyEvent must be called from the Window Procedure
    keyboard::SYMBOL translateKeyEvent(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, bool &filter);

    keyboard::MODIFIER getKeyModState();

    keyboard::MODIFIER mapKeyMod(WPARAM wparam);
    mouse::BUTTON_MASK mapButtons(WPARAM wparam);

    mouse::BUTTON mapButton(UINT message, WPARAM wparam);
    bool mapButtonState(UINT message);
}

