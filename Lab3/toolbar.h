#pragma once

#include <windows.h>

class Toolbar
{
private:
    HWND hToolbar;

public:
    Toolbar();

    bool Create(HWND hParent, HINSTANCE hInstance);
    void Resize();
};