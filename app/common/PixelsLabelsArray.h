#pragma once

#include <memory>

namespace Common
{
	/************************************************************************/
	/*  Label-Row-Column major array of similarity weights. That is, the    */
	/*  similarity of pixel at row r and column c with the label l is given */
	/*  by:                                                                 */
	/*         Data[c * (Rows * Labels) + r * Labels + l].                  */
	/************************************************************************/
	class PixelsLabelsArray
	{
        typedef std::unique_ptr<double[]> DataPtr;

		size_t _labels;
		size_t _rows;
		size_t _cols;
		DataPtr _data;

		static double* AllocateStorage(size_t rows, size_t cols, size_t labels)
		{
            return new double[rows * cols * labels];
		}

	public:
		PixelsLabelsArray()
			: _labels(0)
			, _rows(0)
			, _cols(0)
			, _data(nullptr)
		{}

		PixelsLabelsArray(PixelsLabelsArray&& other)
			: _labels(other._labels)
			, _rows(other._rows)
			, _cols(other._cols)
			, _data(std::move(other._data))
		{
		}

        PixelsLabelsArray(const PixelsLabelsArray& other)
            : _labels(other._labels)
            , _rows(other._rows)
            , _cols(other._cols)
            , _data(AllocateStorage(other._rows, other._cols, other._labels))
        {
            std::copy(other.Data(), other.Data() + other.Size(), Data());
        }

		PixelsLabelsArray(size_t rows, size_t cols, size_t labels)
			: _labels(labels)
			, _rows(rows)
			, _cols(cols)
			, _data(AllocateStorage(rows, cols, labels))
		{
		}

		PixelsLabelsArray Copy() const
		{
			PixelsLabelsArray result(_rows, _cols, _labels);
			std::copy(Data(), Data() + Size(), result.Data());
			return result;
		}

		PixelsLabelsArray& operator=(const PixelsLabelsArray&) = delete;
		
		
		PixelsLabelsArray& operator=(PixelsLabelsArray&& other)
		{
			if (this != &other)
			{
				_labels = other._labels;
				_rows = other._rows;
				_cols = other._cols;
				_data = std::move(other._data);
			}

			return *this;
		}

		const double& At(size_t r, size_t c, size_t l) const
		{
			return const_cast<PixelsLabelsArray*>(this)->At(r, c, l);
		}

		double& At(size_t r, size_t c, size_t l)
		{
			return _data.get()[c * (_rows * _labels) + r * _labels + l];
		}

		const double* Data() const { return _data.get(); }
		double* Data()  { return _data.get(); }

		size_t Rows() const { return _rows; }
		size_t Cols() const { return _cols; }
		size_t Labels() const { return _labels; }
		size_t Size() const { return _rows * _cols * _labels; }
		bool IsEmpty() const { return Size() == 0; }
	};
}
