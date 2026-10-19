#pragma once

#include <windows.h>
#include "editor.h"
#include "shape.h"

// Controls the collection of shapes and the currently selected editor.
class ShapeObjectsEditor
{
private:
    // A static array required by the laboratory assignment.
    Shape* shapes[SHAPE_ARRAY_SIZE];

    // Number of shapes currently stored in the array.
    int shapeCount;

    // Editor used for processing the selected object type.
    ShapeEditor* currentEditor;

    // Replaces the current editor with another one.
    void ChangeEditor(ShapeEditor* newEditor);

public:
    ShapeObjectsEditor();
    ~ShapeObjectsEditor();

    // Selects the required drawing mode.
    void StartPointEditor(HWND hWnd);
    void StartLineEditor(HWND hWnd);
    void StartRectEditor(HWND hWnd);
    void StartEllipseEditor(HWND hWnd);

    // Passes window events to the current editor.
    void OnLBdown(HWND hWnd);
    void OnLBup(HWND hWnd);
    void OnMouseMove(HWND hWnd);
    void OnPaint(HWND hWnd);
};