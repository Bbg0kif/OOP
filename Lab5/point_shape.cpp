#include "framework.h"
#include "point_shape.h"

Shape* PointShape::Clone() const
{
    return new PointShape(*this);
}

void PointShape::Show(HDC hdc)
{
    if (!preview)
    {
        // A completed point is a single black pixel.
        SetPixel(hdc, xs1, ys1, RGB(0, 0, 0));
        return;
    }

    // A temporary dotted cross makes the point preview visible.
    HPEN pen = CreatePen(PS_DOT, 1, RGB(0, 0, 255));

    if (pen == nullptr)
        return;

    HGDIOBJ oldPen = SelectObject(hdc, pen);
    int oldBackgroundMode = SetBkMode(hdc, TRANSPARENT);

    MoveToEx(hdc, xs1 - 4, ys1, nullptr);
    LineTo(hdc, xs1 + 5, ys1);

    MoveToEx(hdc, xs1, ys1 - 4, nullptr);
    LineTo(hdc, xs1, ys1 + 5);

    SetBkMode(hdc, oldBackgroundMode);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}