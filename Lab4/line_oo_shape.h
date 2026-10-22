#pragma once

#include "line_shape.h"
#include "ellipse_shape.h"

// A line with a circle at each endpoint.
class LineOOShape : public LineShape, public EllipseShape
{
public:
    Shape* Clone() const override;
    void Show(HDC hdc) override;
};