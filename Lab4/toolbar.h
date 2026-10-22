#pragma once

#include <windows.h>
#include <commctrl.h>

class Toolbar
{
private:
    HWND hToolbar;
    HIMAGELIST hImages;

public:
    Toolbar();
    ~Toolbar();

    Toolbar(const Toolbar&) = delete;
    Toolbar& operator=(const Toolbar&) = delete;

    bool Create(HWND hParent, HINSTANCE hInstance);
    void Resize();
    int GetHeight() const;
    void SetActiveTool(UINT commandId);
    LRESULT OnNotify(LPARAM lParam);
};