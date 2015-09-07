#pragma once

#include <string>

namespace Common
{
	class PixelsLabelsArray;

    void SaveBIF(const PixelsLabelsArray& pla, const std::string& fileName);
    PixelsLabelsArray LoadBIF(const std::string& fileName);
}
