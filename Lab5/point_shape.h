#pragma once

#include "shape.h"

// A point defined by its initial coordinates.
class PointShape : public virtual Shape
{
public:
    Shape* Clone() const override;
    void Show(HDC hdc) override;
};