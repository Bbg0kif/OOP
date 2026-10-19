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

// Draws a rectangle with a black border
// and a light-green fill according to variant 1.
void RectShape::Show(HDC hdc)
{
    HPEN newPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN oldPen = static_cast<HPEN>(SelectObject(hdc, newPen));

    HBRUSH newBrush = CreateSolidBrush(RGB(0, 255, 0));
    HBRUSH oldBrush =
        static_cast<HBRUSH>(SelectObject(hdc, newBrush));

    Rectangle(hdc, xs1, ys1, xs2, ys2);

    // The old GDI objects must be restored before deletion.
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);

    DeleteObject(newBrush);
    DeleteObject(newPen);
}

// Draws an ellipse with a black border
// and a white fill according to variant 1.
void EllipseShape::Show(HDC hdc)
{
    HPEN newPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN oldPen = static_cast<HPEN>(SelectObject(hdc, newPen));

    HBRUSH newBrush = CreateSolidBrush(RGB(255, 255, 255));
    HBRUSH oldBrush =
        static_cast<HBRUSH>(SelectObject(hdc, newBrush));

    Ellipse(hdc, xs1, ys1, xs2, ys2);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);

    DeleteObject(newBrush);
    DeleteObject(newPen);
}