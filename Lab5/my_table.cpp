#include "my_table.h"
#include "my_table_ids.h"

#include <commctrl.h>

#pragma comment(lib, "Comctl32.lib")

bool MyTable::Toggle(HWND owner, HINSTANCE instance)
{
    if (dialog != nullptr)
    {
        Close();
        return true;
    }

    INITCOMMONCONTROLSEX controls{};
    controls.dwSize = sizeof(controls);
    controls.dwICC = ICC_LISTVIEW_CLASSES;

    if (!InitCommonControlsEx(&controls))
        return false;

    HWND created = CreateDialogParamW(
        instance,
        MAKEINTRESOURCEW(IDD_OBJECT_TABLE),
        owner,
        DialogProc,
        reinterpret_cast<LPARAM>(this)
    );

    if (created == nullptr)
        return false;

    ShowWindow(created, SW_SHOW);
    return true;
}

void MyTable::Close()
{
    if (dialog != nullptr)
        DestroyWindow(dialog);
}

bool MyTable::ProcessDialogMessage(MSG& message) const
{
    return dialog != nullptr && IsDialogMessageW(dialog, &message);
}

INT_PTR CALLBACK MyTable::DialogProc(
    HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        MyTable* self = reinterpret_cast<MyTable*>(lParam);
        self->dialog = hDlg;
        SetWindowLongPtrW(hDlg, DWLP_USER, reinterpret_cast<LONG_PTR>(self));

        HWND list = GetDlgItem(hDlg, IDC_OBJECT_LIST);
        ListView_SetExtendedListViewStyle(
            list, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

        wchar_t titles[][8] = { L"Name", L"x1", L"y1", L"x2", L"y2" };
        const int widths[] = { 150, 55, 55, 55, 55 };

        for (int i = 0; i < 5; ++i)
        {
            LVCOLUMNW column{};
            column.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
            column.pszText = titles[i];
            column.cx = widths[i];
            column.iSubItem = i;

            SendMessageW(list, LVM_INSERTCOLUMNW, i,
                reinterpret_cast<LPARAM>(&column));
        }

        for (const auto& row : self->rows)
            self->ShowRow(row);

        return TRUE;
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == IDCANCEL)
        {
            DestroyWindow(hDlg);
            return TRUE;
        }
        break;

    case WM_CLOSE:
        DestroyWindow(hDlg);
        return TRUE;

    case WM_NCDESTROY:
    {
        MyTable* self = reinterpret_cast<MyTable*>(
            GetWindowLongPtrW(hDlg, DWLP_USER));

        if (self != nullptr)
            self->dialog = nullptr;

        SetWindowLongPtrW(hDlg, DWLP_USER, 0);
        break;
    }
    }

    return FALSE;
}

void MyTable::Add(const std::string& name,
    long x1, long y1, long x2, long y2)
{
    // Class names in the file format use ASCII characters.
    rows.push_back({
        std::wstring(name.begin(), name.end()),
        std::to_wstring(x1),
        std::to_wstring(y1),
        std::to_wstring(x2),
        std::to_wstring(y2)
        });

    if (dialog != nullptr)
        ShowRow(rows.back());
}

void MyTable::ShowRow(const Row& row)
{
    HWND list = GetDlgItem(dialog, IDC_OBJECT_LIST);

    // The Windows API expects writable text buffers.
    Row text = row;

    LVITEMW item{};
    item.mask = LVIF_TEXT;
    item.iItem = ListView_GetItemCount(list);
    item.pszText = &text[0][0];

    const int index = static_cast<int>(SendMessageW(
        list, LVM_INSERTITEMW, 0, reinterpret_cast<LPARAM>(&item)));

    if (index < 0)
        return;

    for (int column = 1; column < 5; ++column)
    {
        LVITEMW cell{};
        cell.iSubItem = column;
        cell.pszText = &text[column][0];

        SendMessageW(list, LVM_SETITEMTEXTW, index,
            reinterpret_cast<LPARAM>(&cell));
    }
}