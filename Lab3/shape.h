#pragma once

#include <windows.h>

// Abstract base class for all geometric shapes.
class Shape
{
protected:
    // Coordinates that describe the geometric object.
    long xs1;
    long ys1;
    long xs2;
    long ys2;

public:
    Shape();
    virtual ~Shape();

    // Sets the coordinates of the object.
    void Set(long x1, long y1, long x2, long y2);

    // Pure virtual method for polymorphic drawing.
    virtual void Show(HDC hdc) = 0;
};

// A point.
class PointShape : public Shape
{
public:
    void Show(HDC hdc) override;
};

// A line segment.
class LineShape : public Shape
{
public:
    void Show(HDC hdc) override;
};

// A rectangle.
class RectShape : public Shape
{
public:
    void Show(HDC hdc) override;
};

// An ellipse.
class EllipseShape : public Shape
{
public:
    void Show(HDC hdc) override;
};