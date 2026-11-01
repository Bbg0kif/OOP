#pragma once

#include "shape.h"

#include <memory>
#include <string>
#include <vector>

bool ReadDrawingFile(
    const wchar_t* path,
    std::vector<std::unique_ptr<Shape>>& loaded,
    std::wstring& error);