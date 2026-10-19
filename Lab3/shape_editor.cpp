#include "framework.h"
#include "shape_editor.h"

// Initializes an empty array and selects the point editor by default.
ShapeObjectsEditor::ShapeObjectsEditor()
    : shapeCount(0),
    currentEditor(nullptr)
{
    // All elements must initially contain null pointers.
    for (int i = 0; i < SHAPE_ARRAY_SIZE; ++i)
    {
        shapes[i] = nullptr;
    }

    currentEditor = new PointEditor(shapes, &shapeCount);
}

// Releases the editor and all dynamically created shapes.
ShapeObjectsEditor::~ShapeObjectsEditor()
{
    delete currentEditor;

    for (int i = 0; i < shapeCount; ++i)
    {
        delete shapes[i];
    }
}

// Replaces the previous editor with the selected editor.
void ShapeObjectsEditor::ChangeEditor(ShapeEditor* newEditor)
{
    if (newEditor == nullptr)
        return;

    delete currentEditor;
    currentEditor = newEditor;
}

void ShapeObjectsEditor::StartPointEditor(HWND hWnd)
{
    UNREFERENCED_PARAMETER(hWnd);
    ChangeEditor(new PointEditor(shapes, &shapeCount));
}

void ShapeObjectsEditor::StartLineEditor(HWND hWnd)
{
    UNREFERENCED_PARAMETER(hWnd);
    ChangeEditor(new LineEditor(shapes, &shapeCount));
}

void ShapeObjectsEditor::StartRectEditor(HWND hWnd)
{
    UNREFERENCED_PARAMETER(hWnd);
    ChangeEditor(new RectEditor(shapes, &shapeCount));
}

void ShapeObjectsEditor::StartEllipseEditor(HWND hWnd)
{
    UNREFERENCED_PARAMETER(hWnd);
    ChangeEditor(new EllipseEditor(shapes, &shapeCount));
}

// Passes the left-button press to the current editor.
void ShapeObjectsEditor::OnLBdown(HWND hWnd)
{
    if (currentEditor != nullptr)
        currentEditor->OnLBdown(hWnd);
}

// Passes the left-button release to the current editor.
void ShapeObjectsEditor::OnLBup(HWND hWnd)
{
    if (currentEditor != nullptr)
        currentEditor->OnLBup(hWnd);
}

// Passes mouse movement to the current editor.
void ShapeObjectsEditor::OnMouseMove(HWND hWnd)
{
    if (currentEditor != nullptr)
        currentEditor->OnMouseMove(hWnd);
}

// Draws all previously created objects.
void ShapeObjectsEditor::OnPaint(HWND hWnd)
{
    if (currentEditor != nullptr)
        currentEditor->OnPaint(hWnd);
}