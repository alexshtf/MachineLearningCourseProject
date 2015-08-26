#pragma once

#include <string>

namespace Common
{
	class PixelsLabelsArray;

	void SaveBIF(const PixelsLabelsArray& pla, const std::wstring& fileName);
	PixelsLabelsArray LoadBIF(const std::wstring& fileName);
}