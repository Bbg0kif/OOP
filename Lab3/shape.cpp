#include "framework.h"
#include "shape.h"

// Initializes all coordinates.
Shape::Shape()
    : xs1(0), ys1(0), xs2(0), ys2(0)
{
}

// The virtual destructor allows derived objects
// to be deleted through a Shape pointer.
Shape::~Shape()
{
}

// Stores the coordinates of the geometric object.
void Shape::Set(long x1, long y1, long x2, long y2)
{
    xs1 = x1;
    ys1 = y1;
    xs2 = x2;
    ys2 = y2;
}

// Draws a point in black.
void PointShape::Show(HDC hdc)
{
    SetPixel(hdc, xs1, ys1, RGB(0, 0, 0));
}

// Draws a black line segment.
void LineShape::Show(HDC hdc)
{
    HPEN newPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN oldPen = static_cast<HPEN>(SelectObject(hdc, newPen));

    MoveToEx(hdc, xs1, ys1, nullptr);
    LineTo(hdc, xs2, ys2);

    // Restores the previous pen and releases the created resource.
    SelectObject(hdc, oldPen);
    DeleteObject(newPen);
}

void RectShape::Show(HDC hdc)
{
    // Variant 2: black outline and cyan fill.
    HPEN newPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN oldPen = static_cast<HPEN>(SelectObject(hdc, newPen));

    HBRUSH newBrush = CreateSolidBrush(RGB(0, 255, 255));
    HBRUSH oldBrush =
        static_cast<HBRUSH>(SelectObject(hdc, newBrush));

    Rectangle(hdc, xs1, ys1, xs2, ys2);

    // Restore previous objects before deleting our GDI resources.
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);

    DeleteObject(newBrush);
    DeleteObject(newPen);
}

void EllipseShape::Show(HDC hdc)
{
    // Variant 2: black outline without filling the interior.
    HPEN newPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN oldPen = static_cast<HPEN>(SelectObject(hdc, newPen));

    // A stock hollow brush leaves existing drawings visible inside.
    HBRUSH oldBrush = static_cast<HBRUSH>(
        SelectObject(hdc, GetStockObject(NULL_BRUSH)));

    Ellipse(hdc, xs1, ys1, xs2, ys2);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);

    // The stock brush belongs to Windows and must not be deleted.
    DeleteObject(newPen);
}