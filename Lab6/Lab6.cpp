// Lab6.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Lab6.h"
#include "matrix_protocol.h"
#include <string>

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

using MatrixParams = MatrixProtocol::Parameters;

namespace
{
    constexpr UINT_PTR Object2StartupTimer = 1;
    constexpr UINT_PTR Object3WorkTimer = 2;

    bool waitingForObject3 = false;
    bool object3RequestSent = false;
    ULONGLONG object3StartTime = 0;
    HWND object3Window = nullptr;

    constexpr ULONGLONG StartupTimeoutMs = 10000;

    bool waitingForObject2 = false;
    ULONGLONG object2StartupTime = 0;
    MatrixParams pendingParams{};
}

void StartObject3OrSend(HWND hWnd);

INT_PTR CALLBACK MatrixParamsProc(
    HWND hDlg,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        MatrixParams* params =
            reinterpret_cast<MatrixParams*>(lParam);

        SetWindowLongPtrW(
            hDlg,
            DWLP_USER,
            reinterpret_cast<LONG_PTR>(params)
        );

        SetDlgItemInt(hDlg, IDC_MATRIX_SIZE, params->size, TRUE);
        SetDlgItemInt(hDlg, IDC_MATRIX_MIN, params->minValue, TRUE);
        SetDlgItemInt(hDlg, IDC_MATRIX_MAX, params->maxValue, TRUE);

        return TRUE;
    }

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
        {
            BOOL sizeValid = FALSE;
            BOOL minValid = FALSE;
            BOOL maxValid = FALSE;

            const int size = static_cast<int>(
                GetDlgItemInt(
                    hDlg, IDC_MATRIX_SIZE, &sizeValid, TRUE)
                );

            const int minValue = static_cast<int>(
                GetDlgItemInt(
                    hDlg, IDC_MATRIX_MIN, &minValid, TRUE)
                );

            const int maxValue = static_cast<int>(
                GetDlgItemInt(
                    hDlg, IDC_MATRIX_MAX, &maxValid, TRUE)
                );

            if (!sizeValid || !minValid || !maxValid)
            {
                MessageBoxW(
                    hDlg,
                    L"Enter an integer in each field.",
                    L"Invalid input",
                    MB_OK | MB_ICONWARNING
                );
                return TRUE;
            }

            // Application limits for matrix size and element values.
            if (size < 1 || size > 10)
            {
                MessageBoxW(
                    hDlg,
                    L"Matrix size must be between 1 and 10.",
                    L"Invalid input",
                    MB_OK | MB_ICONWARNING
                );
                return TRUE;
            }

            if (minValue < -1000 || minValue > 1000 ||
                maxValue < -1000 || maxValue > 1000)
            {
                MessageBoxW(
                    hDlg,
                    L"Min and Max must be between -1000 and 1000.",
                    L"Invalid input",
                    MB_OK | MB_ICONWARNING
                );
                return TRUE;
            }

            if (minValue > maxValue)
            {
                MessageBoxW(
                    hDlg,
                    L"Min must not be greater than Max.",
                    L"Invalid input",
                    MB_OK | MB_ICONWARNING
                );
                return TRUE;
            }

            MatrixParams* params =
                reinterpret_cast<MatrixParams*>(
                    GetWindowLongPtrW(hDlg, DWLP_USER)
                    );

            if (params == nullptr)
            {
                EndDialog(hDlg, IDCANCEL);
                return TRUE;
            }

            params->size = size;
            params->minValue = minValue;
            params->maxValue = maxValue;

            EndDialog(hDlg, IDOK);
            return TRUE;
        }

        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        break;

    case WM_CLOSE:
        EndDialog(hDlg, IDCANCEL);
        return TRUE;
    }

    return FALSE;
}

bool SendParametersToObject2(
    HWND sender,
    const MatrixParams& params)
{
    HWND receiver = FindWindowW(
        MatrixProtocol::Object2Class,
        nullptr
    );

    if (receiver == nullptr)
    {
        MessageBoxW(
            sender,
            L"Open Object2 first, then try again.",
            L"Object2 is not running",
            MB_OK | MB_ICONWARNING
        );

        return false;
    }

    MatrixParams payload = params;

    COPYDATASTRUCT data{};
    data.dwData = MatrixProtocol::GenerateMatrixCommand;
    data.cbData = static_cast<DWORD>(sizeof(payload));
    data.lpData = &payload;

    const LRESULT accepted = SendMessageW(
        receiver,
        WM_COPYDATA,
        reinterpret_cast<WPARAM>(sender),
        reinterpret_cast<LPARAM>(&data)
    );

    if (accepted == FALSE)
    {
        MessageBoxW(
            sender,
            L"Object2 could not complete the request. Check its status and try again.",
            L"Communication error",
            MB_OK | MB_ICONERROR
        );

        return false;
    }

    StartObject3OrSend(sender);
    return true;
}

void StartObject2OrSend(
    HWND hWnd,
    const MatrixParams& params)
{
    if (waitingForObject2)
    {
        return;
    }

    // Reuse an already running application.
    if (FindWindowW(MatrixProtocol::Object2Class, nullptr) != nullptr)
    {
        SendParametersToObject2(hWnd, params);
        return;
    }

    // Locate Object2.exe beside Lab6.exe.
    wchar_t modulePath[32768]{};

    const DWORD length = GetModuleFileNameW(
        nullptr,
        modulePath,
        static_cast<DWORD>(_countof(modulePath))
    );

    if (length == 0 || length >= _countof(modulePath))
    {
        MessageBoxW(
            hWnd,
            L"Cannot determine the application folder.",
            L"Startup error",
            MB_OK | MB_ICONERROR
        );
        return;
    }

    const std::wstring fullPath(modulePath, length);
    const size_t separator = fullPath.find_last_of(L"\\/");

    if (separator == std::wstring::npos)
    {
        MessageBoxW(
            hWnd,
            L"Invalid application path.",
            L"Startup error",
            MB_OK | MB_ICONERROR
        );
        return;
    }

    const std::wstring directory =
        fullPath.substr(0, separator + 1);

    const std::wstring executable =
        directory + L"Object2.exe";

    // Set up asynchronous waiting before starting the process.
    if (SetTimer(hWnd, Object2StartupTimer, 100, nullptr) == 0)
    {
        MessageBoxW(
            hWnd,
            L"Cannot create the startup timer.",
            L"Startup error",
            MB_OK | MB_ICONERROR
        );
        return;
    }

    pendingParams = params;
    object2StartupTime = GetTickCount64();
    waitingForObject2 = true;

    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);

    PROCESS_INFORMATION process{};

    const BOOL started = CreateProcessW(
        executable.c_str(),
        nullptr,
        nullptr,
        nullptr,
        FALSE,
        0,
        nullptr,
        directory.c_str(),
        &startup,
        &process
    );

    if (!started)
    {
        const DWORD error = GetLastError();

        KillTimer(hWnd, Object2StartupTimer);
        waitingForObject2 = false;

        const std::wstring message =
            L"Cannot start Object2.exe.\n"
            L"Build all projects and check that Object2.exe "
            L"is beside Lab6.exe.\n\nWindows error: " +
            std::to_wstring(error);

        MessageBoxW(
            hWnd,
            message.c_str(),
            L"Startup error",
            MB_OK | MB_ICONERROR
        );
        return;
    }

    CloseHandle(process.hThread);
    CloseHandle(process.hProcess);
}

void CheckObject2Startup(HWND hWnd)
{
    if (!waitingForObject2)
    {
        return;
    }

    HWND receiver = FindWindowW(
        MatrixProtocol::Object2Class,
        nullptr
    );

    if (receiver != nullptr)
    {
        KillTimer(hWnd, Object2StartupTimer);
        waitingForObject2 = false;

        SendParametersToObject2(hWnd, pendingParams);
        return;
    }

    if (GetTickCount64() - object2StartupTime >= StartupTimeoutMs)
    {
        KillTimer(hWnd, Object2StartupTimer);
        waitingForObject2 = false;

        MessageBoxW(
            hWnd,
            L"Object2 did not create its window within 10 seconds.",
            L"Startup timeout",
            MB_OK | MB_ICONERROR
        );
    }
}

void StopObject3Waiting(HWND hWnd)
{
    KillTimer(hWnd, Object3WorkTimer);

    waitingForObject3 = false;
    object3RequestSent = false;
    object3Window = nullptr;
}

void StartObject3OrSend(HWND hWnd)
{
    if (waitingForObject3)
    {
        return;
    }

    if (SetTimer(hWnd, Object3WorkTimer, 100, nullptr) == 0)
    {
        MessageBoxW(
            hWnd,
            L"Cannot create the Object3 timer.",
            L"Lab6",
            MB_OK | MB_ICONERROR
        );
        return;
    }

    waitingForObject3 = true;
    object3RequestSent = false;
    object3StartTime = GetTickCount64();
    object3Window = nullptr;

    // The timer will send a request to an existing window.
    if (FindWindowW(MatrixProtocol::Object3Class, nullptr) != nullptr)
    {
        return;
    }

    wchar_t modulePath[32768]{};

    const DWORD length = GetModuleFileNameW(
        nullptr,
        modulePath,
        static_cast<DWORD>(_countof(modulePath))
    );

    if (length == 0 || length >= _countof(modulePath))
    {
        StopObject3Waiting(hWnd);

        MessageBoxW(
            hWnd,
            L"Cannot determine the application folder.",
            L"Startup error",
            MB_OK | MB_ICONERROR
        );
        return;
    }

    const std::wstring fullPath(modulePath, length);
    const size_t separator = fullPath.find_last_of(L"\\/");

    if (separator == std::wstring::npos)
    {
        StopObject3Waiting(hWnd);

        MessageBoxW(
            hWnd,
            L"Invalid application path.",
            L"Startup error",
            MB_OK | MB_ICONERROR
        );
        return;
    }

    const std::wstring directory =
        fullPath.substr(0, separator + 1);

    const std::wstring executable =
        directory + L"Object3.exe";

    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);

    PROCESS_INFORMATION process{};

    if (!CreateProcessW(
        executable.c_str(),
        nullptr,
        nullptr,
        nullptr,
        FALSE,
        0,
        nullptr,
        directory.c_str(),
        &startup,
        &process))
    {
        const DWORD error = GetLastError();
        StopObject3Waiting(hWnd);

        const std::wstring message =
            L"Cannot start Object3.exe.\r\nWindows error: " +
            std::to_wstring(error);

        MessageBoxW(
            hWnd,
            message.c_str(),
            L"Startup error",
            MB_OK | MB_ICONERROR
        );
        return;
    }

    CloseHandle(process.hThread);
    CloseHandle(process.hProcess);
}

void CheckObject3Work(HWND hWnd)
{
    if (!waitingForObject3)
    {
        return;
    }

    if (GetTickCount64() - object3StartTime >= StartupTimeoutMs)
    {
        StopObject3Waiting(hWnd);

        MessageBoxW(
            hWnd,
            L"Object3 did not finish within 10 seconds.",
            L"Timeout",
            MB_OK | MB_ICONERROR
        );
        return;
    }

    if (object3RequestSent)
    {
        return;
    }

    object3Window = FindWindowW(
        MatrixProtocol::Object3Class,
        nullptr
    );

    if (object3Window == nullptr)
    {
        return;
    }

    object3RequestSent = true;

    if (!PostMessageW(
        object3Window,
        MatrixProtocol::ReadMatrixMessage,
        reinterpret_cast<WPARAM>(hWnd),
        0))
    {
        StopObject3Waiting(hWnd);

        MessageBoxW(
            hWnd,
            L"Cannot send the request to Object3.",
            L"Communication error",
            MB_OK | MB_ICONERROR
        );
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
    LoadStringW(hInstance, IDC_LAB6, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB6));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB6));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAB6);
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
    case WM_TIMER:
        if (wParam == Object2StartupTimer)
        {
            CheckObject2Startup(hWnd);
            return 0;
        }

        if (wParam == Object3WorkTimer)
        {
            CheckObject3Work(hWnd);
            return 0;
        }

        return DefWindowProcW(hWnd, message, wParam, lParam);
    case MatrixProtocol::MatrixResultMessage:
    {
        const HWND sender = reinterpret_cast<HWND>(lParam);

        if (!waitingForObject3 ||
            !object3RequestSent ||
            sender != object3Window)
        {
            return 0;
        }

        StopObject3Waiting(hWnd);

        if (wParam == 0)
        {
            MessageBoxW(
                hWnd,
                L"Object3 could not process the matrix. Check its window.",
                L"Calculation error",
                MB_OK | MB_ICONERROR
            );
        }

        return 0;
    }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_RUN:
            {
                if (waitingForObject2 || waitingForObject3)
                {
                    MessageBoxW(
                        hWnd,
                        L"A calculation is in progress. Please wait.",
                        L"Lab6",
                        MB_OK | MB_ICONINFORMATION
                    );
                    break;
                }

                MatrixParams params{};

                const INT_PTR result = DialogBoxParamW(
                    hInst,
                    MAKEINTRESOURCEW(IDD_MATRIX_PARAMS),
                    hWnd,
                    MatrixParamsProc,
                    reinterpret_cast<LPARAM>(&params)
                );

                if (result == IDOK)
                {
                    StartObject2OrSend(hWnd, params);
                }

                else if (result == -1)
                {
                    MessageBoxW(
                        hWnd,
                        L"Failed to create the parameters dialog.",
                        L"Lab6",
                        MB_OK | MB_ICONERROR
                    );
                }

                break;
            }
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
            PAINTSTRUCT ps;
            BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code here...
            EndPaint(hWnd, &ps);
        }
        break;
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
