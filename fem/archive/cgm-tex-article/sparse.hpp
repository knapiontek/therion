
namespace sp
{
	int size = 0;

	class Vector
	{
	public:
		Vector()
		{
			data.resize(sp::size);
		}
		double& operator[](int i)
		{
			return data[i];
		}
		const double& operator[](int i) const
		{
			return data[i];
		}
		static double norm2(const Vector& v1, const Vector& v2)
		{
			double out = 0.0;
			for(int i = 0; i < sp::size; i++)
			{
				double d = v2[i] - v1[i];
				out = d * d;
			}
			return out;
		}
		friend Vector operator-(const Vector& v1, const Vector& v2)
		{
			Vector out;
			for(int i = 0; i < sp::size; i++)
			{
				out[i] = v1[i] - v2[i];
			}
			return out;
		}
		friend double operator*(const Vector& v1, const Vector& v2)
		{
			double out = 0.0;
			for(int i = 0; i < sp::size; i++)
			{
				out += v1[i] * v2[i];
			}
			return out;
		}
		friend Vector operator*(double val, const Vector& in)
		{
			Vector out;
			for(int i = 0; i < sp::size; i++)
			{
				out[i] = val * in[i];
			}
			return out;
		}
		void operator+=(const Vector& vec)
		{
			for(int i = 0; i < sp::size; i++)
			{
				data[i] += vec[i];
			}
		}
		void operator-=(const Vector& vec)
		{
			for(int i = 0; i < sp::size; i++)
			{
				data[i] -= vec[i];
			}
		}
		void operator*=(double val)
		{
			for(int i = 0; i < sp::size; i++)
			{
				data[i] *= val;
			}
		}
	private:
		QVector<double> data;
	};

	struct Index
	{
		int row, col;
		friend bool operator<(const Index& i1, const Index& i2)
		{
			if(i1.row != i2.row) return i1.row < i2.row;
			else return i1.col < i2.col;
		}
	};
	class Matrix : public QMap<Index, double>
	{
	public:
		void set(int row, int col, double val)
		{
			(*this)[Index { row, col }] += val;
		}
	};

	class SymetryMatrix
	{
	public:
		SymetryMatrix(const Matrix& mx)
		{
			shift_vec.resize(sp::size);
			elem_vec.resize(mx.size());

			int e = 0, prev_row = 0;
			for(auto m = mx.begin(); m != mx.end(); m++)
			{
				auto& index = m.key();

				Q_ASSERT(index.row - prev_row <= 1);
				if(prev_row < index.row)
				{
					shift_vec[index.row - 1] = e;
					prev_row = index.row;
				}

				elem_vec[e] = Element { index.col, m.value() };
				e++;
			}
			shift_vec[sp::size - 1] = mx.size();
		}
		friend Vector operator*(const Vector& vec, const SymetryMatrix& mx)
		{
			Vector out;
			mx.mul(out, vec);
			return out;
		}
		friend Vector operator*(const SymetryMatrix& mx, const Vector& vec)
		{
			Vector out;
			mx.mul(out, vec);
			return out;
		}
	private:
		struct Element
		{
			int col;
			double value;
		};
	private:
		void mul(Vector& out, const Vector& in) const
		{
			int e = 0;
			for(int i = 0; i < sp::size; i++)
			{
				out[i] = 0.0;
				while(e < shift_vec[i])
				{
					auto& elem = elem_vec[e];
					out[i] += in[elem.col] * elem.value;
					e++;
				}
			}
		}
	private:
		QVector<Element> elem_vec;
		QVector<int> shift_vec;
	};

	void copy(QVector<double>& out, const Matrix& mx)
	{
		for(auto d = mx.begin(); d != mx.end(); d++)
		{
			auto& index = d.key();
			double value = d.value();

			out[index.row * sp::size + index.col] = value;
		}
	}

	void mul(QVector<double>& out, const Matrix& mx, const Vector& in)
	{
		for(auto d = mx.begin(); d != mx.end(); d++)
		{
			auto& index = d.key();
			double value = d.value();

			out[index.row] += value * in[index.col];
		}
	}
}
