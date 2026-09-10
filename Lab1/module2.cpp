#include "framework.h"
#include "module2.h"
#include "module2.rh"

static INT_PTR CALLBACK DialogProc_MOD2(
    HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

int Func_MOD2(HWND hWnd)
{
    HINSTANCE instance = reinterpret_cast<HINSTANCE>(
        GetWindowLongPtrW(hWnd, GWLP_HINSTANCE));

    return static_cast<int>(DialogBoxW(
        instance,
        MAKEINTRESOURCEW(IDD_DIALOG_MOD2),
        hWnd,
        DialogProc_MOD2
    ));
}

static INT_PTR CALLBACK DialogProc_MOD2(
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