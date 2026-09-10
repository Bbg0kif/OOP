#include "framework.h"
#include "module3.h"
#include "module3.rh"

static INT_PTR CALLBACK DialogProc_MOD3(
    HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

int Func_MOD3(HWND hWnd)
{
    HINSTANCE instance = reinterpret_cast<HINSTANCE>(
        GetWindowLongPtrW(hWnd, GWLP_HINSTANCE));

    return static_cast<int>(DialogBoxW(
        instance,
        MAKEINTRESOURCEW(IDD_DIALOG_MOD3),
        hWnd,
        DialogProc_MOD3
    ));
}

static INT_PTR CALLBACK DialogProc_MOD3(
    HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BACK_MOD3:
            EndDialog(hDlg, 2);
            return TRUE;

        case IDOK:
            EndDialog(hDlg, 1);
            return TRUE;

        case IDCANCEL:
            EndDialog(hDlg, 0);
            return TRUE;
        }
        break;

    case WM_CLOSE:
        EndDialog(hDlg, 0);
        return TRUE;
    }

    return FALSE;
}