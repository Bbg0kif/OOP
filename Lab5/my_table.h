#pragma once

#include <windows.h>

class MyTable
{
public:
    bool Toggle(HWND owner, HINSTANCE instance);
    void Close();
    bool ProcessDialogMessage(MSG& message) const;

private:
    HWND dialog = nullptr;

    static INT_PTR CALLBACK DialogProc(
        HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
};