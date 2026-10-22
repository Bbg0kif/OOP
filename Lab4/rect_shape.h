#pragma once

#include "shape.h"

// A rectangle defined by two opposite corners.
class RectShape : public virtual Shape
{
protected:
    bool filled;

public:
    explicit RectShape(bool useFill = true);

    Shape* Clone() const override;
    void Show(HDC hdc) override;
};