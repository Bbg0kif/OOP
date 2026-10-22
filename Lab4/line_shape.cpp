#include "framework.h"
#include "line_shape.h"

Shape* LineShape::Clone() const
{
    return new LineShape(*this);
}

void LineShape::Show(HDC hdc)
{
    // Temporary lines are dotted; completed lines are solid.
    int penStyle = preview ? PS_DOT : PS_SOLID;

    COLORREF penColor = preview
        ? RGB(0, 0, 255)
        : RGB(0, 0, 0);

    HPEN pen = CreatePen(penStyle, 1, penColor);

    if (pen == nullptr)
        return;

    HGDIOBJ oldPen = SelectObject(hdc, pen);
    int oldBackgroundMode = SetBkMode(hdc, TRANSPARENT);

    MoveToEx(hdc, xs1, ys1, nullptr);
    LineTo(hdc, xs2, ys2);

    // Restore the device context before releasing the pen.
    SetBkMode(hdc, oldBackgroundMode);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}