#include "framework.h"
#include "toolbar.h"
#include "Resource.h"

#include <commctrl.h>

#pragma comment(lib, "Comctl32.lib")

namespace
{
    // Append icons for the line with circles and the wireframe cube.
    bool AddExtraImages(HIMAGELIST images)
    {
        HDC screenDC = GetDC(nullptr);

        if (screenDC == nullptr)
        {
            return false;
        }

        HDC memoryDC = CreateCompatibleDC(screenDC);
        HBITMAP bitmap = CreateCompatibleBitmap(screenDC, 48, 24);

        ReleaseDC(nullptr, screenDC);

        HBRUSH background = CreateSolidBrush(RGB(192, 192, 192));
        HPEN pen = CreatePen(PS_SOLID, 1, RGB(32, 32, 32));

        if (memoryDC == nullptr || bitmap == nullptr ||
            background == nullptr || pen == nullptr)
        {
            if (pen != nullptr) DeleteObject(pen);
            if (background != nullptr) DeleteObject(background);
            if (bitmap != nullptr) DeleteObject(bitmap);
            if (memoryDC != nullptr) DeleteDC(memoryDC);

            return false;
        }

        HGDIOBJ oldBitmap = SelectObject(memoryDC, bitmap);

        RECT bounds{ 0, 0, 48, 24 };
        FillRect(memoryDC, &bounds, background);

        HGDIOBJ oldPen = SelectObject(memoryDC, pen);
        HGDIOBJ oldBrush =
            SelectObject(memoryDC, GetStockObject(NULL_BRUSH));

        // First icon: a line connecting two circles.
        MoveToEx(memoryDC, 7, 7, nullptr);
        LineTo(memoryDC, 16, 16);

        Ellipse(memoryDC, 1, 1, 9, 9);
        Ellipse(memoryDC, 15, 15, 23, 23);

        // Second icon: two square faces and four connecting edges.
        Rectangle(memoryDC, 26, 8, 40, 22);
        Rectangle(memoryDC, 32, 2, 46, 16);

        MoveToEx(memoryDC, 26, 8, nullptr);
        LineTo(memoryDC, 32, 2);

        MoveToEx(memoryDC, 39, 8, nullptr);
        LineTo(memoryDC, 45, 2);

        MoveToEx(memoryDC, 39, 21, nullptr);
        LineTo(memoryDC, 45, 15);

        MoveToEx(memoryDC, 26, 21, nullptr);
        LineTo(memoryDC, 32, 15);

        SelectObject(memoryDC, oldBrush);
        SelectObject(memoryDC, oldPen);
        SelectObject(memoryDC, oldBitmap);

        const int firstIndex = ImageList_AddMasked(
            images,
            bitmap,
            RGB(192, 192, 192)
        );

        DeleteObject(pen);
        DeleteObject(background);
        DeleteObject(bitmap);
        DeleteDC(memoryDC);

        return firstIndex == 4 &&
            ImageList_GetImageCount(images) == 6;
    }
}

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
        TBSTYLE_TOOLTIPS | CCS_TOP,
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

    // Reserve space for six 24 x 24 images.
    hImages = ImageList_Create(
        24,
        24,
        ILC_COLOR24 | ILC_MASK,
        6,
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

    if (firstImage != 0 ||
        ImageList_GetImageCount(hImages) != 4 ||
        !AddExtraImages(hImages))
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
        L"Point\0Line\0Rectangle\0Ellipse\0Line with circles\0Cube\0";

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
        IDM_ELLIPSE,
        IDM_LINEOO,
        IDM_CUBE
    };

    TBBUTTON buttons[6]{};

    for (int i = 0; i < 6; ++i)
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
        6,
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
        IDM_ELLIPSE,
        IDM_LINEOO,
        IDM_CUBE
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

    case IDM_LINEOO:
        wideText = L"Draw a line with circles at both endpoints";
        narrowText = "Draw a line with circles at both endpoints";
        break;

    case IDM_CUBE:
        wideText = L"Draw a wireframe cube";
        narrowText = "Draw a wireframe cube";
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

int Toolbar::GetHeight() const
{
    if (hToolbar == nullptr)
    {
        return 0;
    }

    RECT bounds{};
    GetWindowRect(hToolbar, &bounds);

    return bounds.bottom - bounds.top;
}