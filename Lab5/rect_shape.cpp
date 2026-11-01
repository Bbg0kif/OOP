#include "framework.h"
#include "rect_shape.h"

#include <algorithm>

RectShape::RectShape(bool useFill)
    : filled(useFill)
{
}

Shape* RectShape::Clone() const
{
    return new RectShape(*this);
}

void RectShape::Show(HDC hdc)
{
    // Normalize the coordinates for any dragging direction.
    const long left = (std::min)(xs1, xs2);
    const long top = (std::min)(ys1, ys2);
    const long right = (std::max)(xs1, xs2);
    const long bottom = (std::max)(ys1, ys2);

    const int penStyle = preview ? PS_DOT : PS_SOLID;

    const COLORREF penColor =
        preview ? RGB(0, 0, 255) : RGB(0, 0, 0);

    HPEN pen = CreatePen(penStyle, 1, penColor);

    if (pen == nullptr)
    {
        return;
    }

    // Only completed, filled rectangles need a custom brush.
    HBRUSH fillBrush = nullptr;

    if (!preview && filled)
    {
        fillBrush = CreateSolidBrush(RGB(0, 255, 255));

        if (fillBrush == nullptr)
        {
            DeleteObject(pen);
            return;
        }
    }

    HGDIOBJ oldPen = SelectObject(hdc, pen);

    HGDIOBJ oldBrush = SelectObject(
        hdc,
        fillBrush != nullptr
        ? static_cast<HGDIOBJ>(fillBrush)
        : GetStockObject(NULL_BRUSH)
    );

    const int oldBackgroundMode =
        SetBkMode(hdc, TRANSPARENT);

    Rectangle(hdc, left, top, right, bottom);

    // Restore the device context before releasing GDI objects.
    SetBkMode(hdc, oldBackgroundMode);
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);

    if (fillBrush != nullptr)
    {
        DeleteObject(fillBrush);
    }

    DeleteObject(pen);
}