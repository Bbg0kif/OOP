#pragma once

#include "shape.h"

class MyEditor
{
private:
    Shape** pshape;
    int shapeCount;
    int capacity;

    Shape* selectedShape;
    Shape* temporaryShape;

    long startX;
    long startY;

    void AddShape(Shape* shape);

public:
    MyEditor();
    ~MyEditor();

    // The editor owns its array and shapes.
    MyEditor(const MyEditor&) = delete;
    MyEditor& operator=(const MyEditor&) = delete;

    // Takes ownership of the selected shape prototype.
    void Start(Shape* shape);

    void OnLBdown(HWND hWnd, long x, long y);
    void OnLBup(HWND hWnd, long x, long y);
    void OnMouseMove(HWND hWnd, long x, long y);
    void OnPaint(HDC hdc);

    void CancelDrawing(HWND hWnd);
};