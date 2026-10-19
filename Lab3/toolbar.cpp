#include "framework.h"
#include "toolbar.h"
#include "Resource.h"

#include <commctrl.h>

#pragma comment(lib, "Comctl32.lib")

Toolbar::Toolbar()
    : hToolbar(nullptr),
    hImages(nullptr)
{
}

Toolbar::~Toolbar()
{
    // The global Toolbar object outlives the application windows.
    if (hImages != nullptr)
    {
        ImageList_Destroy(hImages);
    }
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

    // Set the dimensions of each image in the bitmap strip.
    if (!SendMessageW(
        hToolbar,
        TB_SETBITMAPSIZE,
        0,
        MAKELPARAM(24, 24)
    ))
    {
        DestroyWindow(hToolbar);
        hToolbar = nullptr;
        return false;
    }

    // Load the toolbar bitmap in the display-compatible format.
        HBITMAP hBitmap = reinterpret_cast<HBITMAP>(
            LoadImageW(
                hInstance,
                MAKEINTRESOURCEW(IDB_TOOLBAR),
                IMAGE_BITMAP,
                0,
                0,
                LR_DEFAULTCOLOR
            )
        );

    if (hBitmap == nullptr)
    {
        DestroyWindow(hToolbar);
        hToolbar = nullptr;
        return false;
    }

    // Create a list containing four 24 x 24 images.
    hImages = ImageList_Create(
        24,
        24,
        ILC_COLOR24 | ILC_MASK,
        4,
        0
    );

    if (hImages == nullptr)
    {
        DeleteObject(hBitmap);
        DestroyWindow(hToolbar);
        hToolbar = nullptr;
        return false;
    }

    // Treat only the gray background as transparent.
    int firstImage = ImageList_AddMasked(
        hImages,
        hBitmap,
        RGB(192, 192, 192)
    );

    // The image list stores its own copy of the bitmap.
    DeleteObject(hBitmap);

    if (firstImage == -1)
    {
        ImageList_Destroy(hImages);
        hImages = nullptr;

        DestroyWindow(hToolbar);
        hToolbar = nullptr;
        return false;
    }

    // Assign the image list to the toolbar.
    SendMessageW(
        hToolbar,
        TB_SETIMAGELIST,
        0,
        reinterpret_cast<LPARAM>(hImages)
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
        buttons[i].iBitmap = static_cast<int>(firstImage) + i;
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