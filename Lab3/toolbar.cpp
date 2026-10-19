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
        TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | CCS_TOP,
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
        buttons[i].fsStyle =
            BTNS_CHECKGROUP | BTNS_AUTOSIZE | BTNS_SHOWTEXT;
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

    // Select the point tool when the toolbar is created.
    SetActiveTool(IDM_POINT);

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

void Toolbar::SetActiveTool(UINT commandId)
{
    if (hToolbar == nullptr)
    {
        return;
    }

    const UINT commands[] = {
        IDM_POINT,
        IDM_LINE,
        IDM_RECTANGLE,
        IDM_ELLIPSE
    };

    // Synchronize all buttons with the selected tool.
    for (UINT id : commands)
    {
        SendMessageW(
            hToolbar,
            TB_CHECKBUTTON,
            id,
            MAKELPARAM(id == commandId, 0)
        );
    }
}

LRESULT Toolbar::OnNotify(LPARAM lParam)
{
    if (hToolbar == nullptr || lParam == 0)
    {
        return 0;
    }

    const NMHDR* header =
        reinterpret_cast<const NMHDR*>(lParam);

    HWND hTooltip = reinterpret_cast<HWND>(
        SendMessageW(hToolbar, TB_GETTOOLTIPS, 0, 0)
        );

    // Process notifications from this toolbar's tooltip only.
    if (header->hwndFrom != hTooltip)
    {
        return 0;
    }

    const wchar_t* wideText = L"";
    const char* narrowText = "";

    switch (header->idFrom)
    {
    case IDM_POINT:
        wideText = L"Draw a point";
        narrowText = "Draw a point";
        break;

    case IDM_LINE:
        wideText = L"Draw a line";
        narrowText = "Draw a line";
        break;

    case IDM_RECTANGLE:
        wideText = L"Draw a rectangle";
        narrowText = "Draw a rectangle";
        break;

    case IDM_ELLIPSE:
        wideText = L"Draw an ellipse";
        narrowText = "Draw an ellipse";
        break;

    default:
        return 0;
    }

    // Support both Unicode and ANSI tooltip notifications.
    if (header->code == TTN_GETDISPINFOW)
    {
        NMTTDISPINFOW* info =
            reinterpret_cast<NMTTDISPINFOW*>(lParam);

        lstrcpynW(info->szText, wideText, 80);
        info->lpszText = info->szText;
        info->hinst = nullptr;
    }
    else if (header->code == TTN_GETDISPINFOA)
    {
        NMTTDISPINFOA* info =
            reinterpret_cast<NMTTDISPINFOA*>(lParam);

        lstrcpynA(info->szText, narrowText, 80);
        info->lpszText = info->szText;
        info->hinst = nullptr;
    }

    return 0;
}