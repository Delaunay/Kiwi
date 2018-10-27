/*#include <vector>
#include <cassert>

template <typename E>
class MatrixExpression {
public:
	typedef typename E::Scalar Scalar;

	Scalar operator()(size_t x, size_t y) const { return static_cast<E const&>(*this)(x, y); }
	
	size_t cols() const { return static_cast<E const&>(*this).cols(); }
	size_t rows() const { return static_cast<E const&>(*this).rows(); }
	size_t size() const { return cols() * size(); }

	// The following overload conversions to E, the template argument type;
	// e.g., for VecExpression<VecSum>, this is a conversion to VecSum.
	operator E& () { return static_cast<E&>(*this); }
	operator const E& () const { return static_cast<const E&>(*this); }
};

template <typename T>
class Matrix: public MatrixExpression<Matrix<T>> {
	size_t _rows;
	size_t _cols;
	std::vector<T> _elems;

public:
	typedef typename T Scalar;

	Matrix(size_t n, size_t m):
		_rows(n), _cols(m), _elems(n * m)
	{}

	// A Vec can be constructed from any VecExpression, forcing its evaluation.
	template <typename E>
	Matrix(MatrixExpression<E> const& vec): 
		_rows(vec.rows()), _cols(vec.cols()), _elems(vec.size())
	{

		for (size_t i = 0; i < _rows; ++i) {
			for (size_t j = 0; j < _cols; ++j) {
				_elems[i + j * _rows] = vec(i, j);
			}
		}
	}

	T operator ()(size_t x, size_t y) const { return _elems[x + y * _rows]; }
	T &operator()(size_t x, size_t y)       { return _elems[x + y * _rows]; }

	size_t cols() const { return _cols; }
	size_t rows() const { return _rows; }
	size_t size() const { return _elems.size(); }
};

template <typename E1, typename E2>
class MatrixSum: public MatrixExpression<MatrixSum<E1, E2> > {
	E1 const& _u;
	E2 const& _v;

public:
	typedef typename E1::Scalar Scalar;

	MatrixSum(E1 const& u, E2 const& v):
		_u(u), _v(v) 
	{
		std::cout << "SUM" << std::endl;
		assert(u.cols() == v.cols() && u.rows() == v.rows());
	}

	Scalar operator ()(size_t x, size_t y) const { return _u(x, y) + _v(x, y); }

	size_t cols() const { return _v.cols(); }
	size_t rows() const { return _v.rows(); }
	size_t size() const { return _v.size(); }
};


template <typename E1, typename E2>
class MatrixMult : public MatrixExpression<MatrixMult<E1, E2> > {
	E1 const& _u;
	E2 const& _v;

public:
	typedef typename E1::Scalar Scalar;

	MatrixMult(E1 const& u, E2 const& v) : _u(u), _v(v) {
		assert(u.cols() == u.rows());
	}

	Scalar operator ()(size_t x, size_t y) const { 
		T value = 0;

		for (size_t i = 0; i < _u.cols(); ++i)
			value += _u(x, i) * _v(i, y);

		return value;
	}

	size_t cols() const { return _u.cols(); }
	size_t rows() const { return _v.rows(); }
	size_t size() const { return _u.size(); }
};

template<typename E1, typename E2>
MatrixMult<E1, E2> const operator*(E1 const& u, E2 const& v) {
	return MatrixMult<E1, E2>(u, v);
}

template<typename E1, typename E2>
MatrixSum<E1, E2> const operator+(E1 const& u, E2 const& v) {
	return MatrixSum<E1, E2>(u, v);
}


#include<iostream>

int main(){
	
	Matrix<double> a(10, 10);
	Matrix<double> b(10, 10);

	Matrix<double>::Scalar g;

	for (int i = 0; i < 10; ++i){
		for (int j = 0; j < 10; ++j) {
			a(i, j) = i + j;
			b(i, j) = i * j;
		}
	}

	auto c = b + a + b;
	//Matrix<double> c = a + b;


	for (int i = 0; i < 10; ++i){
		for (int j = 0; j < 10; ++j) {
			std::cout << c(i, j) << "\t";
		}
		std::cout << "\n";
	}

	return 0;
}*/
