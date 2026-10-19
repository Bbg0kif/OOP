#pragma once

#include <windows.h>
#include "shape.h"

// Number in the student list: 1.
// Required array size: 1 + 100 = 101.
constexpr int SHAPE_ARRAY_SIZE = 101;

// Abstract interface for all object editors.
class Editor
{
public:
    virtual ~Editor();

    virtual void OnLBdown(HWND hWnd) = 0;
    virtual void OnLBup(HWND hWnd) = 0;
    virtual void OnMouseMove(HWND hWnd) = 0;
    virtual void OnPaint(HWND hWnd) = 0;
};

// Base class containing common drawing logic.
class ShapeEditor : public Editor
{
protected:
    Shape** shapes;
    int* shapeCount;

    bool drawing;
    bool previewVisible;

    long xStart;
    long yStart;
    long xEnd;
    long yEnd;

    // Adds a completed object to the shared array.
    void AddShape(Shape* shape);

    // Draws or erases the rubber-band preview using XOR mode.
    void TogglePreview(HWND hWnd);

    // Implemented differently for every geometric form.
    virtual void DrawPreview(HDC hdc) = 0;
    virtual Shape* CreateShape() = 0;

public:
    ShapeEditor(Shape** shapeArray, int* count);
    virtual ~ShapeEditor();

    void OnLBdown(HWND hWnd) override;
    void OnLBup(HWND hWnd) override;
    void OnMouseMove(HWND hWnd) override;
    void OnPaint(HWND hWnd) override;
};

// Editor for point objects.
class PointEditor : public ShapeEditor
{
protected:
    void DrawPreview(HDC hdc) override;
    Shape* CreateShape() override;

public:
    PointEditor(Shape** shapeArray, int* count);
    void OnLBdown(HWND hWnd) override;
};

// Editor for line objects.
class LineEditor : public ShapeEditor
{
protected:
    void DrawPreview(HDC hdc) override;
    Shape* CreateShape() override;

public:
    LineEditor(Shape** shapeArray, int* count);
};

// Editor for rectangles entered from the centre to a corner.
class RectEditor : public ShapeEditor
{
protected:
    void DrawPreview(HDC hdc) override;
    Shape* CreateShape() override;

public:
    RectEditor(Shape** shapeArray, int* count);
};

// Editor for ellipses entered by two opposite corners.
class EllipseEditor : public ShapeEditor
{
protected:
    void DrawPreview(HDC hdc) override;
    Shape* CreateShape() override;

public:
    EllipseEditor(Shape** shapeArray, int* count);
};