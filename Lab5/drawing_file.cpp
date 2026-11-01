#include "drawing_file.h"

#include "point_shape.h"
#include "line_shape.h"
#include "rect_shape.h"
#include "ellipse_shape.h"
#include "line_oo_shape.h"
#include "cube_shape.h"

#include <fstream>
#include <limits>
#include <sstream>
#include <utility>

namespace
{
    std::unique_ptr<Shape> CreateShape(const std::string& name)
    {
        if (name == "PointShape")   return std::make_unique<PointShape>();
        if (name == "LineShape")    return std::make_unique<LineShape>();
        if (name == "RectShape")    return std::make_unique<RectShape>();
        if (name == "EllipseShape") return std::make_unique<EllipseShape>();
        if (name == "LineOOShape")  return std::make_unique<LineOOShape>();
        if (name == "CubeShape")    return std::make_unique<CubeShape>();

        return nullptr;
    }

    bool ReadCoordinate(const std::string& field, long& value)
    {
        std::istringstream input(field);
        input >> std::noskipws >> value;

        // Reject empty fields, overflow and extra characters.
        // Leave room for coordinate arithmetic when drawing shapes.
        return input && input.eof() &&
            value >= -1000000 && value <= 1000000;
    }
}

bool ReadDrawingFile(
    const wchar_t* path,
    std::vector<std::unique_ptr<Shape>>& loaded,
    std::wstring& error)
{
    // Visual C++ supports opening a file by its Windows wide path.
    std::ifstream file(path);

    if (!file)
    {
        error = L"Could not open the selected file.";
        return false;
    }

    std::vector<std::unique_ptr<Shape>> parsed;
    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line))
    {
        ++lineNumber;
        std::istringstream fields(line);
        std::string name;
        long coordinates[4]{};

        std::getline(fields, name, '\t');
        auto shape = CreateShape(name);
        bool valid = (shape != nullptr);

        for (int i = 0; valid && i < 4; ++i)
        {
            std::string field;
            valid = static_cast<bool>(std::getline(fields, field, '\t'))
                && ReadCoordinate(field, coordinates[i]);
        }

        // After the fourth coordinate there must be no extra field.
        if (!valid || !fields.eof())
        {
            error = L"Invalid shape record on line " +
                std::to_wstring(lineNumber) + L".";
            return false;
        }

        if (parsed.size() >= static_cast<size_t>(
            (std::numeric_limits<int>::max)()))
        {
            error = L"The drawing contains too many shapes.";
            return false;
        }

        shape->Set(coordinates[0], coordinates[1],
            coordinates[2], coordinates[3]);

        parsed.push_back(std::move(shape));
    }

    if (file.bad() || !file.eof())
    {
        error = L"Could not finish reading the file.";
        return false;
    }

    loaded = std::move(parsed);
    return true;
}