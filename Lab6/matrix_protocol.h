#pragma once

#include <cstdint>

namespace MatrixProtocol
{
    constexpr wchar_t Object2Class[] = L"LAB6_OBJECT2";

    constexpr std::uintptr_t GenerateMatrixCommand = 1;

    struct Parameters
    {
        std::int32_t size = 3;
        std::int32_t minValue = -10;
        std::int32_t maxValue = 10;
    };

    static_assert(sizeof(Parameters) == 12,
        "Unexpected parameters size.");

    inline bool IsValid(const Parameters& params)
    {
        return params.size >= 1 &&
            params.size <= 10 &&
            params.minValue >= -1000 &&
            params.minValue <= 1000 &&
            params.maxValue >= -1000 &&
            params.maxValue <= 1000 &&
            params.minValue <= params.maxValue;
    }

    constexpr wchar_t Object3Class[] = L"LAB6_OBJECT3";

    // Application messages: WM_APP + 1 and WM_APP + 2.
    constexpr unsigned int ReadMatrixMessage = 0x8001;
    constexpr unsigned int MatrixResultMessage = 0x8002;

}