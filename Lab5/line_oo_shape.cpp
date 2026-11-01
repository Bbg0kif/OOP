#include "framework.h"
#include "line_oo_shape.h"

#include <cmath>

Shape* LineOOShape::Clone() const
{
    return new LineOOShape(*this);
}

void LineOOShape::Show(HDC hdc)
{
    // Save the circle centers.
    const long x1 = xs1;
    const long y1 = ys1;
    const long x2 = xs2;
    const long y2 = ys2;

    const long radius = 16;

    const double dx = static_cast<double>(x2) - x1;
    const double dy = static_cast<double>(y2) - y1;
    const double length = std::sqrt(dx * dx + dy * dy);

    // Draw the segment between the circle boundaries.
    // Overlapping circles do not need a connecting segment.
    if (length > 2.0 * radius)
    {
        const double offsetX = radius * dx / length;
        const double offsetY = radius * dy / length;

        const long lineStartX = std::lround(x1 + offsetX);
        const long lineStartY = std::lround(y1 + offsetY);
        const long lineEndX = std::lround(x2 - offsetX);
        const long lineEndY = std::lround(y2 - offsetY);

        Set(lineStartX, lineStartY, lineEndX, lineEndY);
        LineShape::Show(hdc);
    }

    // Draw the first circle using the parent implementation.
    Set(x1, y1, x1 + radius, y1 + radius);
    EllipseShape::Show(hdc);

    // Draw the second circle using the parent implementation.
    Set(x2, y2, x2 + radius, y2 + radius);
    EllipseShape::Show(hdc);

    // Restore the original coordinates.
    Set(x1, y1, x2, y2);
}