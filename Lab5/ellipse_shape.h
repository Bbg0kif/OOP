#pragma once

#include "shape.h"

// An ellipse defined by its center and a bounding-box corner.
class EllipseShape : public virtual Shape
{
public:
    Shape* Clone() const override;
    void Show(HDC hdc) override;
};