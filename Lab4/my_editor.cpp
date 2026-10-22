#include "framework.h"
#include "my_editor.h"

MyEditor::MyEditor()
    : pshape(nullptr),
    shapeCount(0),
    capacity(0),
    selectedShape(nullptr),
    temporaryShape(nullptr),
    startX(0),
    startY(0)
{
}

MyEditor::~MyEditor()
{
    for (int i = 0; i < shapeCount; ++i)
    {
        delete pshape[i];
    }

    delete[] pshape;
    delete temporaryShape;
    delete selectedShape;
}

void MyEditor::Start(Shape* shape)
{
    if (shape == selectedShape)
    {
        return;
    }

    delete selectedShape;
    selectedShape = shape;
}

void MyEditor::AddShape(Shape* shape)
{
    if (shape == nullptr)
    {
        return;
    }

    // Allocate the array only when needed, then grow it.
    if (shapeCount == capacity)
    {
        const int newCapacity = capacity == 0
            ? 8
            : capacity * 2;

        Shape** newArray = new Shape * [newCapacity] {};

        for (int i = 0; i < shapeCount; ++i)
        {
            newArray[i] = pshape[i];
        }

        // The shapes remain alive; only the old array is released.
        delete[] pshape;

        pshape = newArray;
        capacity = newCapacity;
    }

    pshape[shapeCount] = shape;
    ++shapeCount;
}

void MyEditor::OnLBdown(HWND hWnd, long x, long y)
{
    if (selectedShape == nullptr)
    {
        return;
    }

    CancelDrawing(hWnd);

    startX = x;
    startY = y;

    // Clone preserves the actual type of the selected shape.
    temporaryShape = selectedShape->Clone();
    temporaryShape->Set(startX, startY, x, y);
    temporaryShape->SetPreview(true);

    SetCapture(hWnd);
    InvalidateRect(hWnd, nullptr, TRUE);
}

void MyEditor::OnMouseMove(HWND hWnd, long x, long y)
{
    if (temporaryShape == nullptr)
    {
        return;
    }

    temporaryShape->Set(startX, startY, x, y);
    InvalidateRect(hWnd, nullptr, TRUE);
}

void MyEditor::OnLBup(HWND hWnd, long x, long y)
{
    if (temporaryShape == nullptr)
    {
        return;
    }

    temporaryShape->Set(startX, startY, x, y);
    temporaryShape->SetPreview(false);

    AddShape(temporaryShape);

    // Ownership has moved to the array.
    temporaryShape = nullptr;

    if (GetCapture() == hWnd)
    {
        ReleaseCapture();
    }

    InvalidateRect(hWnd, nullptr, TRUE);
}

void MyEditor::OnPaint(HDC hdc)
{
    for (int i = 0; i < shapeCount; ++i)
    {
        pshape[i]->Show(hdc);
    }

    // Preview uses the same polymorphic drawing method.
    if (temporaryShape != nullptr)
    {
        temporaryShape->Show(hdc);
    }
}

void MyEditor::CancelDrawing(HWND hWnd)
{
    const bool hadPreview = temporaryShape != nullptr;

    delete temporaryShape;
    temporaryShape = nullptr;

    if (GetCapture() == hWnd)
    {
        ReleaseCapture();
    }

    if (hadPreview)
    {
        InvalidateRect(hWnd, nullptr, TRUE);
    }
}