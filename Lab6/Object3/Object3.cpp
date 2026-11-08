// Object3.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Object3.h"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include "../matrix_protocol.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

namespace
{
    std::wstring resultText = L"Waiting for a matrix.";

    bool ReadClipboardText(HWND hWnd, std::wstring& text)
    {
        text.clear();

        if (!IsClipboardFormatAvailable(CF_UNICODETEXT))
        {
            return false;
        }

        if (!OpenClipboard(hWnd))
        {
            return false;
        }

        HANDLE memory = GetClipboardData(CF_UNICODETEXT);

        if (memory == nullptr)
        {
            CloseClipboard();
            return false;
        }

        const SIZE_T bytes = GlobalSize(memory);

        // Our matrix text is small. Reject unexpectedly large data.
        if (bytes < sizeof(wchar_t) || bytes > 65536)
        {
            CloseClipboard();
            return false;
        }

        const wchar_t* source =
            static_cast<const wchar_t*>(GlobalLock(memory));

        if (source == nullptr)
        {
            CloseClipboard();
            return false;
        }

        const size_t capacity = bytes / sizeof(wchar_t);
        size_t length = 0;

        while (length < capacity && source[length] != L'\0')
        {
            ++length;
        }

        const bool valid = length < capacity;

        if (valid)
        {
            text.assign(source, length);
        }

        GlobalUnlock(memory);
        CloseClipboard();

        return valid;
    }

    bool ParseMatrix(
        const std::wstring& text,
        int& size,
        std::vector<double>& values)
    {
        std::wistringstream input(text);

        if (!(input >> size) || size < 1 || size > 10)
        {
            return false;
        }

        values.clear();
        values.reserve(static_cast<size_t>(size) * size);

        for (int i = 0; i < size * size; ++i)
        {
            int value = 0;

            if (!(input >> value) || value < -1000 || value > 1000)
            {
                return false;
            }

            values.push_back(static_cast<double>(value));
        }

        // Only whitespace may follow the last matrix element.
        input >> std::ws;
        return input.eof();
    }

    double CalculateDeterminant(
        std::vector<double> values,
        int size)
    {
        double determinant = 1.0;

        for (int column = 0; column < size; ++column)
        {
            // Choose the largest available pivot by absolute value.
            int pivotRow = column;

            for (int row = column + 1; row < size; ++row)
            {
                if (std::abs(values[row * size + column]) >
                    std::abs(values[pivotRow * size + column]))
                {
                    pivotRow = row;
                }
            }

            const double pivotCandidate =
                values[pivotRow * size + column];

            if (pivotCandidate == 0.0)
            {
                return 0.0;
            }

            if (pivotRow != column)
            {
                for (int j = 0; j < size; ++j)
                {
                    std::swap(
                        values[column * size + j],
                        values[pivotRow * size + j]
                    );
                }

                determinant = -determinant;
            }

            const double pivot =
                values[column * size + column];

            determinant *= pivot;

            for (int row = column + 1; row < size; ++row)
            {
                const double factor =
                    values[row * size + column] / pivot;

                values[row * size + column] = 0.0;

                for (int j = column + 1; j < size; ++j)
                {
                    values[row * size + j] -=
                        factor * values[column * size + j];
                }
            }
        }

        return determinant;
    }

    bool ProcessClipboardMatrix(HWND hWnd)
    {
        std::wstring text;

        if (!ReadClipboardText(hWnd, text))
        {
            resultText =
                L"Cannot read Unicode text from the clipboard.";

            InvalidateRect(hWnd, nullptr, TRUE);
            return false;
        }

        int size = 0;
        std::vector<double> values;

        if (!ParseMatrix(text, size, values))
        {
            resultText =
                L"Invalid matrix format.\r\n"
                L"Expected: n followed by n*n integers.\r\n"
                L"n: 1..10; elements: -1000..1000.";

            InvalidateRect(hWnd, nullptr, TRUE);
            return false;
        }

        const double determinant =
            CalculateDeterminant(values, size);

        std::wostringstream output;

        output << L"Matrix size: "
            << size << L" x " << size
            << L"\r\n\r\nDeterminant = "
            << std::setprecision(15)
            << determinant
            << L"\r\n\r\nMethod: Gaussian elimination "
            << L"with partial pivoting.";

        resultText = output.str();

        InvalidateRect(hWnd, nullptr, TRUE);
        return true;
    }

}

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
    lstrcpynW(
        szWindowClass,
        MatrixProtocol::Object3Class,
        MAX_LOADSTRING
    );
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OBJECT3));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
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

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OBJECT3));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_OBJECT3);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
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
    switch (message)
    {
    case MatrixProtocol::ReadMatrixMessage:
    {
        const HWND manager = reinterpret_cast<HWND>(wParam);

        if (!IsWindow(manager))
        {
            return 0;
        }

        const bool success = ProcessClipboardMatrix(hWnd);

        PostMessageW(
            manager,
            MatrixProtocol::MatrixResultMessage,
            success ? 1 : 0,
            reinterpret_cast<LPARAM>(hWnd)
        );

        return 0;
    }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps{};
        HDC hdc = BeginPaint(hWnd, &ps);

        RECT area{};
        GetClientRect(hWnd, &area);
        InflateRect(&area, -16, -16);

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(0, 0, 0));

        DrawTextW(
            hdc,
            resultText.c_str(),
            -1,
            &area,
            DT_LEFT | DT_WORDBREAK | DT_NOPREFIX
        );

        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_DESTROY:
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
