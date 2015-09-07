#include "PixelsLabelsArray.BIF.h"
#include "PixelsLabelsArray.h"
#include <fstream>
#include <boost/endian/conversion.hpp>
#include <cstdint>

namespace Common
{
	namespace
	{
		std::uint64_t ToIo(std::uint64_t num) { return boost::endian::native_to_little(num); }
		std::uint64_t FromIo(std::uint64_t num) { return boost::endian::little_to_native(num); }
		
		template<typename T>
		const char* Buf(const T* ptr) { return reinterpret_cast<const char*>(ptr); }

		template<typename T>
		char* Buf(T* ptr) { return reinterpret_cast<char*>(ptr); }

		void SaveBIF(const PixelsLabelsArray& pla, std::ostream& os)
		{
			auto rows = ToIo(pla.Rows());
			auto cols = ToIo(pla.Cols());
			auto labels = ToIo(pla.Labels());

			os.write(Buf(&rows), sizeof(rows));
			os.write(Buf(&cols), sizeof(cols));
			os.write(Buf(&labels), sizeof(labels));
			os.write(Buf(pla.Data()), pla.Size() * sizeof(double));
		}

		PixelsLabelsArray LoadBIF(std::istream& is)
		{
			std::uint64_t rows, cols, labels;

			is.read(Buf(&rows), sizeof(rows));
			is.read(Buf(&cols), sizeof(cols));
			is.read(Buf(&labels), sizeof(labels));

			PixelsLabelsArray pla(rows, cols, labels);
			is.read(Buf(pla.Data()), pla.Size() * sizeof(double));

			return pla;
		}
	}


void SaveBIF(const PixelsLabelsArray& pla, const std::string& fileName)
{
    std::ofstream os(fileName, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
	SaveBIF(pla, os);
}

PixelsLabelsArray LoadBIF(const std::string& fileName)
{
    std::ifstream is(fileName, std::ios_base::in | std::ios_base::binary);
	return LoadBIF(is);
}

}
