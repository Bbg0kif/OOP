// Object2.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Object2.h"
#include <random>
#include <sstream>
#include <string>
#include <vector>
#include "../matrix_protocol.h"
#include <cstring>

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
    std::vector<int> matrix;
    int matrixSize = 0;
    std::wstring matrixStatus = L"Waiting for parameters.";

    void GenerateMatrix(int size, int minValue, int maxValue)
    {
        static std::mt19937 generator{ std::random_device{}() };

        std::uniform_int_distribution<int> distribution(
            minValue, maxValue
        );

        std::vector<int> generated(
            static_cast<size_t>(size) * size
        );

        for (int& value : generated)
        {
            value = distribution(generator);
        }

        matrix.swap(generated);
        matrixSize = size;

        matrixStatus =
            L"Matrix " + std::to_wstring(size) +
            L" x " + std::to_wstring(size) +
            L", range [" + std::to_wstring(minValue) +
            L", " + std::to_wstring(maxValue) + L"]";
    }

    void PaintMatrix(HWND hWnd, HDC hdc)
    {
        RECT client{};
        GetClientRect(hWnd, &client);

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(0, 0, 0));

        RECT titleRect{
            16, 12,
            client.right - 16, 52
        };

        DrawTextW(
            hdc,
            matrixStatus.c_str(),
            -1,
            &titleRect,
            DT_LEFT | DT_WORDBREAK | DT_NOPREFIX
        );

        if (matrixSize == 0)
        {
            return;
        }

        constexpr int left = 16;
        constexpr int top = 64;
        constexpr int cellWidth = 64;
        constexpr int cellHeight = 28;

        HGDIOBJ oldFont = SelectObject(
            hdc,
            GetStockObject(ANSI_FIXED_FONT)
        );

        for (int row = 0; row < matrixSize; ++row)
        {
            for (int column = 0; column < matrixSize; ++column)
            {
                const size_t index =
                    static_cast<size_t>(row) * matrixSize + column;

                const std::wstring text =
                    std::to_wstring(matrix[index]);

                RECT cell{
                    left + column * cellWidth,
                    top + row * cellHeight,
                    left + (column + 1) * cellWidth,
                    top + (row + 1) * cellHeight
                };

                DrawTextW(
                    hdc,
                    text.c_str(),
                    -1,
                    &cell,
                    DT_RIGHT | DT_SINGLELINE | DT_VCENTER
                );
            }
        }

        SelectObject(hdc, oldFont);
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
        MatrixProtocol::Object2Class,
        MAX_LOADSTRING
    );
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OBJECT2));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OBJECT2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_OBJECT2);
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
    case WM_COPYDATA:
    {
        const COPYDATASTRUCT* data =
            reinterpret_cast<const COPYDATASTRUCT*>(lParam);

        if (data == nullptr ||
            data->dwData != MatrixProtocol::GenerateMatrixCommand ||
            data->cbData != sizeof(MatrixProtocol::Parameters) ||
            data->lpData == nullptr)
        {
            return FALSE;
        }

        MatrixProtocol::Parameters params{};

        std::memcpy(
            &params,
            data->lpData,
            sizeof(params)
        );

        if (!MatrixProtocol::IsValid(params))
        {
            return FALSE;
        }

        GenerateMatrix(
            params.size,
            params.minValue,
            params.maxValue
        );

        InvalidateRect(hWnd, nullptr, TRUE);

        return TRUE;
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

        PaintMatrix(hWnd, hdc);

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
