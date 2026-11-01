// Lab5.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Lab5.h"

#include <windowsx.h>
#include <fstream>

#include "my_editor.h"
#include "point_shape.h"
#include "line_shape.h"
#include "rect_shape.h"
#include "ellipse_shape.h"
#include "line_oo_shape.h"
#include "cube_shape.h"
#include "toolbar.h"
#include "my_table.h"
#include "drawing_file.h"
#include <commdlg.h>

#pragma comment(lib, "Comdlg32.lib")

#define MAX_LOADSTRING 100

Toolbar toolbar;
MyTable objectTable;

static void OpenDrawing(HWND hWnd)
{
    wchar_t path[MAX_PATH]{};

    OPENFILENAMEW dialog{};
    dialog.lStructSize = sizeof(dialog);
    dialog.hwndOwner = hWnd;
    dialog.lpstrFilter =
        L"Drawing files (*.txt)\0*.txt\0All files (*.*)\0*.*\0";
    dialog.lpstrFile = path;
    dialog.nMaxFile = MAX_PATH;
    dialog.lpstrTitle = L"Open drawing";
    dialog.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

    if (!GetOpenFileNameW(&dialog))
        return;

    std::vector<std::unique_ptr<Shape>> loaded;
    std::wstring error;

    if (!ReadDrawingFile(path, loaded, error))
    {
        MessageBoxW(hWnd, error.c_str(), L"Open drawing", MB_OK | MB_ICONERROR);
        return;
    }

    // Read all rows before transferring ownership to the editor.
    objectTable.Clear();

    for (const auto& shape : loaded)
    {
        long x1, y1, x2, y2;
        shape->GetCoordinates(x1, y1, x2, y2);
        objectTable.Add(shape->GetName(), x1, y1, x2, y2);
    }

    MyEditor::GetInstance().ReplaceShapes(hWnd, loaded);
}

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB5, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB5));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (objectTable.ProcessDialogMessage(msg))
            continue;
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB5));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB5);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    MyEditor& editor = MyEditor::GetInstance();

    switch (message)
    {
    case WM_CREATE:
        if (!toolbar.Create(hWnd, hInst))
        {
            MessageBoxW(
                hWnd,
                L"Failed to create the toolbar.",
                L"Lab5",
                MB_OK | MB_ICONERROR
            );

            return -1;
        }

        objectTable.SetSelectionCallback([hWnd](int index)
            {
                MyEditor::GetInstance().HighlightShape(index);
                InvalidateRect(hWnd, nullptr, FALSE);
            });

        objectTable.SetDeleteCallback([hWnd](int index)
            {
                if (!MyEditor::GetInstance().RemoveShape(index))
                    return false;

                InvalidateRect(hWnd, nullptr, FALSE);
                return true;
            });

        editor.Start(new PointShape());

        CheckMenuRadioItem(
            GetMenu(hWnd),
            IDM_POINT,
            IDM_CUBE,
            IDM_POINT,
            MF_BYCOMMAND
        );

        return 0;

    case WM_SIZE:
        toolbar.Resize();
        InvalidateRect(hWnd, nullptr, FALSE);
        return 0;

    case WM_NOTIFY:
        return toolbar.OnNotify(lParam);

    case WM_LBUTTONDOWN:
        if (GET_Y_LPARAM(lParam) >= toolbar.GetHeight())
        {
            editor.OnLBdown(
                hWnd,
                GET_X_LPARAM(lParam),
                GET_Y_LPARAM(lParam)
            );
        }
        return 0;

    case WM_MOUSEMOVE:
        editor.OnMouseMove(
            hWnd,
            GET_X_LPARAM(lParam),
            GET_Y_LPARAM(lParam)
        );
        return 0;

    case WM_LBUTTONUP:
    {
        const Shape* completed = editor.OnLBup(
            hWnd,
            GET_X_LPARAM(lParam),
            GET_Y_LPARAM(lParam)
        );

        if (completed == nullptr)
            return 0;

        long x1, y1, x2, y2;
        completed->GetCoordinates(x1, y1, x2, y2);
        const char* name = completed->GetName();

        objectTable.Add(name, x1, y1, x2, y2);

        std::ofstream file("shapes.txt", std::ios::app);
        file << name << '\t' << x1 << '\t' << y1
            << '\t' << x2 << '\t' << y2 << '\n';
        file.close();

        if (!file)
        {
            MessageBoxW(hWnd, L"Could not save the shape to shapes.txt.",
                L"Lab5", MB_OK | MB_ICONERROR);
        }

        return 0;
    }

    case WM_CAPTURECHANGED:
        editor.CancelDrawing(hWnd);
        return 0;

    case WM_CANCELMODE:
        editor.CancelDrawing(hWnd);
        return 0;

    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
        {
            editor.CancelDrawing(hWnd);
            return 0;
        }
        return DefWindowProc(hWnd, message, wParam, lParam);

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        if (wmId >= IDM_POINT && wmId <= IDM_CUBE)
        {
            toolbar.SetActiveTool(wmId);

            CheckMenuRadioItem(
                GetMenu(hWnd),
                IDM_POINT,
                IDM_CUBE,
                wmId,
                MF_BYCOMMAND
            );

            SetFocus(hWnd);
        }
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_POINT:
            editor.CancelDrawing(hWnd);
            editor.Start(new PointShape());
            break;

        case IDM_LINE:
            editor.CancelDrawing(hWnd);
            editor.Start(new LineShape());
            break;

        case IDM_RECTANGLE:
            editor.CancelDrawing(hWnd);
            editor.Start(new RectShape());
            break;

        case IDM_ELLIPSE:
            editor.CancelDrawing(hWnd);
            editor.Start(new EllipseShape());
            break;
        case IDM_LINEOO:
            editor.CancelDrawing(hWnd);
            editor.Start(new LineOOShape());
            break;
        case IDM_CUBE:
            editor.CancelDrawing(hWnd);
            editor.Start(new CubeShape());
            break;
        case IDM_OBJECT_TABLE:
            if (!objectTable.Toggle(hWnd, hInst))
            {
                MessageBoxW(hWnd, L"Could not open the object table.",
                    L"Lab5", MB_OK | MB_ICONERROR);
            }
            break;
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_OPEN_DRAWING:
            OpenDrawing(hWnd);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_ERASEBKGND:
        // The background is painted together with the shapes.
        return 1;
    case WM_PAINT:
    {
        PAINTSTRUCT ps{};
        HDC hdc = BeginPaint(hWnd, &ps);

        RECT clientRect{};
        GetClientRect(hWnd, &clientRect);

        const int width = clientRect.right - clientRect.left;
        const int height = clientRect.bottom - clientRect.top;

        if (width > 0 && height > 0)
        {
            // Create an off-screen drawing surface.
            HDC bufferDC = CreateCompatibleDC(hdc);
            HBITMAP bufferBitmap =
                CreateCompatibleBitmap(hdc, width, height);

            if (bufferDC != nullptr && bufferBitmap != nullptr)
            {
                HGDIOBJ oldBitmap =
                    SelectObject(bufferDC, bufferBitmap);

                // Prepare the entire frame in memory.
                FillRect(
                    bufferDC,
                    &clientRect,
                    static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH))
                );

                editor.OnPaint(bufferDC);

                // Copy the completed frame to the window.
                BitBlt(
                    hdc,
                    0, 0,
                    width, height,
                    bufferDC,
                    0, 0,
                    SRCCOPY
                );

                SelectObject(bufferDC, oldBitmap);
            }
            else
            {
                // Fall back to direct drawing if allocation fails.
                FillRect(
                    hdc,
                    &clientRect,
                    static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH))
                );

                editor.OnPaint(hdc);
            }

            if (bufferBitmap != nullptr)
            {
                DeleteObject(bufferBitmap);
            }

            if (bufferDC != nullptr)
            {
                DeleteDC(bufferDC);
            }
        }

        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        objectTable.Close();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
