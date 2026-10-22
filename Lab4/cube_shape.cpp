#include "framework.h"
#include "cube_shape.h"

#include <algorithm>
#include <cstdlib>

CubeShape::CubeShape()
    : RectShape(false)
{
    // Cube faces must remain unfilled.
}

Shape* CubeShape::Clone() const
{
    return new CubeShape(*this);
}

void CubeShape::Show(HDC hdc)
{
    // Save the original mouse coordinates.
    const long x1 = xs1;
    const long y1 = ys1;
    const long x2 = xs2;
    const long y2 = ys2;

    // Keep the front face square.
    const long side = (std::max)(
        std::abs(x2 - x1),
        std::abs(y2 - y1)
        );

    if (side == 0)
    {
        return;
    }

    // Allow dragging in any direction.
    const long left = x2 >= x1 ? x1 : x1 - side;
    const long top = y2 >= y1 ? y1 : y1 - side;
    const long right = left + side;
    const long bottom = top + side;

    // Move the back face to the right and upward.
    const long depth = (std::max)(1L, side / 3);
    const long offsetX = depth;
    const long offsetY = -depth;

    // Draw the front face using the rectangle implementation.
    Set(left, top, right, bottom);
    RectShape::Show(hdc);

    // Draw the back face.
    Set(
        left + offsetX,
        top + offsetY,
        right + offsetX,
        bottom + offsetY
    );
    RectShape::Show(hdc);

    // Connect the top-left corners.
    Set(left, top, left + offsetX, top + offsetY);
    LineShape::Show(hdc);

    // Connect the top-right corners.
    Set(right, top, right + offsetX, top + offsetY);
    LineShape::Show(hdc);

    // Connect the bottom-right corners.
    Set(right, bottom, right + offsetX, bottom + offsetY);
    LineShape::Show(hdc);

    // Connect the bottom-left corners.
    Set(left, bottom, left + offsetX, bottom + offsetY);
    LineShape::Show(hdc);

    // Restore the coordinates for future repainting.
    Set(x1, y1, x2, y2);
}