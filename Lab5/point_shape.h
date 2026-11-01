#pragma once

#include "shape.h"

// A point defined by its initial coordinates.
class PointShape : public virtual Shape
{
public:
    Shape* Clone() const override;

    const char* GetName() const override;

    void Show(HDC hdc) override;

    void Set(long x1, long y1, long x2, long y2) override;
};