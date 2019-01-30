#pragma once

#include <exception>

#include "../Logging/Log.h"

namespace kiwi {
	
	template<typename T>
	class Option;
	
	template<typename T>
	Option<T> some(const T& v);
	
	template<typename T>
	Option<T> none();
	
	class EmptyOptionException: public std::exception {
	public:
        EmptyOptionException() {
			log_debug(what());
		}

		const char* what() const throw () {
			return "`get` on an empty Optional";
		}
	};

	template<typename T>
	class Option {
	public:
		Option():
			_is_defined(false), _dummy(Dummy())
		{}

        Option(const T& v) :
            _is_defined(true), _data(v)
        {}

        Option(const Option<T>& v) :
            _is_defined(v._is_defined)
        {
            if (v._is_defined){
                _data = v._data;
            }
        }

        ~Option() {
            if (_is_defined) {
                _data.T::~T();
            }
        }

        bool operator==(T const& b) const {
            return is_defined() && b.is_defined() && get() == b.get();
        }

        bool operator!=(T const& b) const {
            return !(*this == b);
        }

		T& get() const {
			if (is_defined()) {
				return const_cast<T&>(_data);
			}
			throw EmptyOptionException();
		}

		bool is_defined() const {
			return _is_defined;
		}

		bool is_empty() const {
			return !is_defined();
		}

		void foreach(void (*func)(const T&)) {
			if (is_defined()) {
				return func(get());
			}
		}

		template<typename V>
		Option<V> map(V (*func)(const T&)) {
			if (is_defined()) {
				return some<V>(func(_data));
			}
			return none<V>();
		}

		template<typename V>
		V fold(V (*empty)(), V (*defined)(const T&)) {
			if (is_defined()) {
				return defined(get());
			}
			return empty();
		}

	private:
		bool _is_defined = false;

		struct Dummy {};

		union {
			T _data;
			Dummy _dummy;
		};
	};

    template<typename T>
    bool operator==(Option<T> const& a, Option<T> const& b) {
        return a.is_defined() && b.is_defined() && a.get() == b.get();
    }

    template<typename T>
    bool operator!=(Option<T> const& a, Option<T> const& b) {
        return !(a == b);
    }

	template<typename T>
	Option<T> some(const T& v){
	return Option<T>(v);
	} 

	template<typename T>
	Option<T> none(){
		return Option<T>();
	}

	class EitherError : public std::exception {
	public:
		EitherError(const char* message):
			message(message)
		{
			log_debug(what());
		}

		const char* what() const throw () {
			return message;
		}

	private:
		const char* message;
	};

	template<typename A, typename B>
	class Either {
	public:
		Either(const A& a) :
			_flag(true), _a(a)
		{}

		Either(const B& b) :
			_flag(false), _b(b)
		{}

		bool is_right() { return _flag; }
		bool is_left() { return !_flag;  }

		A& right() {
			if (is_right())
				return _a;
			throw EitherError("Either does not hold `right` value");
		}

		B& left() {
			if (is_left())
				return _b;
			throw EitherError("Either does not hold `left` value");
		}

		template<typename T>
		T fold(T(*a_fun)(const A&), T(*b_fun)(const B&)) {
			if (_flag)
				return a_fun(_a);
			return b_fun(_b);
		}

		void foreach(void(*a_fun)(const A&), void(*b_fun)(const B&)) {
			if (_flag)
				return a_fun(_a);
			return b_fun(_b);
		}

	private:
		bool _flag;
		union {
			A _a;
			B _b;
		};
	};

	template<typename A, typename B>
	Either<A, B> right(const A& v) {
		return Either<A, B>(v);
	}

	template<typename A, typename B>
	Either<A, B> left(const B& v) {
		return Either<A, B>(v);
	}
}
