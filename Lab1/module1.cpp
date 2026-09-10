#include "framework.h"
#include "module1.h"
#include "module1.rh"

static int currentValue = 1;

static INT_PTR CALLBACK DialogProc_MOD1(
    HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

static INT_PTR OnInitDialog_MOD1(HWND hDlg);
static void OnScroll_MOD1(HWND hDlg, WPARAM wParam);
static void UpdateValue_MOD1(HWND hDlg);

int Func_MOD1(HWND hWnd)
{
    HINSTANCE instance = reinterpret_cast<HINSTANCE>(
        GetWindowLongPtrW(hWnd, GWLP_HINSTANCE));

    return static_cast<int>(DialogBoxW(
        instance,
        MAKEINTRESOURCEW(IDD_DIALOG1_MOD1),
        hWnd,
        DialogProc_MOD1
    ));
}

static INT_PTR OnInitDialog_MOD1(HWND hDlg)
{
    currentValue = 1;

    HWND scrollBar = GetDlgItem(hDlg, IDC_SCROLLBAR_MOD1);

    SetScrollRange(scrollBar, SB_CTL, 1, 100, FALSE);

    UpdateValue_MOD1(hDlg);
    return TRUE;
}

static void UpdateValue_MOD1(HWND hDlg)
{
    HWND scrollBar = GetDlgItem(hDlg, IDC_SCROLLBAR_MOD1);

    SetScrollPos(scrollBar, SB_CTL, currentValue, TRUE);

    SetDlgItemInt(
        hDlg,
        IDC_VALUE_MOD1,
        static_cast<UINT>(currentValue),
        FALSE
    );
}

static void OnScroll_MOD1(HWND hDlg, WPARAM wParam)
{
    switch (LOWORD(wParam))
    {
    case SB_LINELEFT:
        --currentValue;
        break;

    case SB_LINERIGHT:
        ++currentValue;
        break;

    case SB_PAGELEFT:
        currentValue -= 10;
        break;

    case SB_PAGERIGHT:
        currentValue += 10;
        break;

    case SB_THUMBTRACK:
    case SB_THUMBPOSITION:
        currentValue = static_cast<int>(HIWORD(wParam));
        break;

    case SB_LEFT:
        currentValue = 1;
        break;

    case SB_RIGHT:
        currentValue = 100;
        break;

    default:
        return;
    }

    if (currentValue < 1)
        currentValue = 1;

    if (currentValue > 100)
        currentValue = 100;

    UpdateValue_MOD1(hDlg);
}

static INT_PTR CALLBACK DialogProc_MOD1(
    HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        return OnInitDialog_MOD1(hDlg);

    case WM_HSCROLL:
        if (reinterpret_cast<HWND>(lParam)
            == GetDlgItem(hDlg, IDC_SCROLLBAR_MOD1))
        {
            OnScroll_MOD1(hDlg, wParam);
            return TRUE;
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            EndDialog(hDlg, currentValue);
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