#pragma once

#include "shape.h"

// A line segment between two endpoints.
class LineShape : public virtual Shape
{
public:
    Shape* Clone() const override;
    void Show(HDC hdc) override;
};