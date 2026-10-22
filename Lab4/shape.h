#pragma once

#include <windows.h>

// Abstract base class for all geometric shapes.
class Shape
{
protected:
    long xs1;
    long ys1;
    long xs2;
    long ys2;

    bool preview;

public:
    Shape();
    virtual ~Shape();

    // Stores the coordinates used by the shape.
    void Set(long x1, long y1, long x2, long y2);

    // Selects temporary or final drawing.
    void SetPreview(bool enabled);

    // Creates a copy of the actual derived object.
    virtual Shape* Clone() const = 0;

    // Draws either a preview or a completed shape.
    virtual void Show(HDC hdc) = 0;
};