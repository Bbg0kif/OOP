#include "framework.h"
#include "ellipse_shape.h"

#include <cstdlib>

Shape* EllipseShape::Clone() const
{
    return new EllipseShape(*this);
}

void EllipseShape::Show(HDC hdc)
{
    // The first point is the center of the ellipse.
    const long radiusX = std::abs(xs2 - xs1);
    const long radiusY = std::abs(ys2 - ys1);

    const long left = xs1 - radiusX;
    const long top = ys1 - radiusY;
    const long right = xs1 + radiusX;
    const long bottom = ys1 + radiusY;

    const int penStyle = preview ? PS_DOT : PS_SOLID;

    const COLORREF penColor =
        preview ? RGB(0, 0, 255) : RGB(0, 0, 0);

    HPEN pen = CreatePen(penStyle, 1, penColor);

    if (pen == nullptr)
    {
        return;
    }

    HGDIOBJ oldPen = SelectObject(hdc, pen);

    HGDIOBJ oldBrush = SelectObject(
        hdc,
        GetStockObject(NULL_BRUSH)
    );

    const int oldBackgroundMode =
        SetBkMode(hdc, TRANSPARENT);

    Ellipse(hdc, left, top, right, bottom);

    // Restore the device context before releasing the pen.
    SetBkMode(hdc, oldBackgroundMode);
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);

    DeleteObject(pen);
}