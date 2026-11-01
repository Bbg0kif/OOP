#include "framework.h"
#include "my_editor.h"

MyEditor& MyEditor::GetInstance()
{
    static MyEditor instance;
    return instance;
}

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
    InvalidateRect(hWnd, nullptr, FALSE);
}

void MyEditor::OnMouseMove(HWND hWnd, long x, long y)
{
    if (temporaryShape == nullptr)
    {
        return;
    }

    temporaryShape->Set(startX, startY, x, y);
    InvalidateRect(hWnd, nullptr, FALSE);
}

const Shape* MyEditor::OnLBup(HWND hWnd, long x, long y)
{
    if (temporaryShape == nullptr)
        return nullptr;

    temporaryShape->Set(startX, startY, x, y);
    temporaryShape->SetPreview(false);

    Shape* completed = temporaryShape;
    AddShape(completed);
    temporaryShape = nullptr;

    if (GetCapture() == hWnd)
        ReleaseCapture();

    InvalidateRect(hWnd, nullptr, FALSE);
    return completed;
}

void MyEditor::OnPaint(HDC hdc)
{
    // Draw all completed shapes in their original style.
    for (int i = 0; i < shapeCount; ++i)
    {
        pshape[i]->Show(hdc);
    }

    // Draw a dotted outline over the selected shape.
    if (highlightedIndex >= 0 && highlightedIndex < shapeCount)
    {
        Shape* highlighted = pshape[highlightedIndex];
        highlighted->SetPreview(true);
        highlighted->Show(hdc);
        highlighted->SetPreview(false);
    }

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
        InvalidateRect(hWnd, nullptr, FALSE);
    }
}

void MyEditor::HighlightShape(int index)
{
    highlightedIndex =
        (index >= 0 && index < shapeCount) ? index : -1;
}

bool MyEditor::RemoveShape(int index)
{
    if (index < 0 || index >= shapeCount)
        return false;

    delete pshape[index];

    for (int i = index; i + 1 < shapeCount; ++i)
    {
        pshape[i] = pshape[i + 1];
    }

    --shapeCount;
    pshape[shapeCount] = nullptr;
    highlightedIndex = -1;

    return true;
}

void MyEditor::ReplaceShapes(
    HWND hWnd, std::vector<std::unique_ptr<Shape>>& loaded)
{
    // Prepare the new array before releasing the current drawing.
    const int newCount = static_cast<int>(loaded.size());
    Shape** newArray = newCount > 0 ? new Shape * [newCount] : nullptr;

    CancelDrawing(hWnd);

    for (int i = 0; i < newCount; ++i)
        newArray[i] = loaded[i].release();

    for (int i = 0; i < shapeCount; ++i)
        delete pshape[i];

    delete[] pshape;

    pshape = newArray;
    shapeCount = newCount;
    capacity = newCount;
    highlightedIndex = -1;

    InvalidateRect(hWnd, nullptr, FALSE);
}
