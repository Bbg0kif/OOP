#pragma once

#include "line_shape.h"
#include "rect_shape.h"

// A wireframe cube built from two squares and four connecting edges.
class CubeShape : public LineShape, public RectShape
{
public:
    CubeShape();

    Shape* Clone() const override;
    void Show(HDC hdc) override;
};