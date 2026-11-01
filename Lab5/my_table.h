#pragma once

#include <windows.h>
#include <array>
#include <string>
#include <vector>
#include <functional>

class MyTable
{
public:
    bool Toggle(HWND owner, HINSTANCE instance);
    void Close();
    bool ProcessDialogMessage(MSG& message) const;

    void Add(const std::string& name, long x1, long y1, long x2, long y2);
    void Clear();

    void SetSelectionCallback(std::function<void(int)> callback);
    void SetDeleteCallback(std::function<bool(int)> callback);

private:
    HWND dialog = nullptr;

    static INT_PTR CALLBACK DialogProc(
        HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

    using Row = std::array<std::wstring, 5>;
    std::vector<Row> rows;

    void ShowRow(const Row& row);

    std::function<void(int)> onSelectionChanged;
    std::function<bool(int)> onDeleteRequested;

};