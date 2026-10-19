#include "framework.h"
#include "toolbar.h"
#include "Resource.h"

#include <commctrl.h>

#pragma comment(lib, "Comctl32.lib")

Toolbar::Toolbar()
    : hToolbar(nullptr)
{
}

bool Toolbar::Create(HWND hParent, HINSTANCE hInstance)
{
    // Initialize the common controls used by the toolbar.
    INITCOMMONCONTROLSEX controls{};
    controls.dwSize = sizeof(controls);
    controls.dwICC = ICC_BAR_CLASSES;

    if (!InitCommonControlsEx(&controls))
    {
        return false;
    }

    // Create a toolbar at the top of the parent window.
    hToolbar = CreateWindowExW(
        0,
        TOOLBARCLASSNAMEW,
        nullptr,
        WS_CHILD | WS_VISIBLE |
        TBSTYLE_FLAT | CCS_TOP,
        0, 0, 0, 0,
        hParent,
        nullptr,
        hInstance,
        nullptr
    );

    if (hToolbar == nullptr)
    {
        return false;
    }

    SendMessageW(
        hToolbar,
        TB_BUTTONSTRUCTSIZE,
        sizeof(TBBUTTON),
        0
    );

    // Store the button labels in the toolbar's string pool.
    const wchar_t labels[] =
        L"Point\0Line\0Rectangle\0Ellipse\0";

    LRESULT firstString = SendMessageW(
        hToolbar,
        TB_ADDSTRINGW,
        0,
        reinterpret_cast<LPARAM>(labels)
    );

    if (firstString == -1)
    {
        DestroyWindow(hToolbar);
        hToolbar = nullptr;
        return false;
    }

    const int commands[] = {
        IDM_POINT,
        IDM_LINE,
        IDM_RECTANGLE,
        IDM_ELLIPSE
    };

    TBBUTTON buttons[4]{};

    for (int i = 0; i < 4; ++i)
    {
        buttons[i].iBitmap = I_IMAGENONE;
        buttons[i].idCommand = commands[i];
        buttons[i].fsState = TBSTATE_ENABLED;
        buttons[i].fsStyle = BTNS_BUTTON | BTNS_AUTOSIZE;
        buttons[i].iString = firstString + i;
    }

    // Use the same command IDs as the Objects menu.
    if (!SendMessageW(
        hToolbar,
        TB_ADDBUTTONSW,
        4,
        reinterpret_cast<LPARAM>(buttons)
    ))
    {
        DestroyWindow(hToolbar);
        hToolbar = nullptr;
        return false;
    }

    Resize();
    return true;
}

void Toolbar::Resize()
{
    if (hToolbar != nullptr)
    {
        SendMessageW(hToolbar, TB_AUTOSIZE, 0, 0);
    }
}