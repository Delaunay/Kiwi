#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace kiwi
{

typedef std::uint64_t uint64;
typedef std::int64_t int64;

typedef std::uint32_t uint32;
typedef std::int32_t int32;

typedef std::uint16_t uint15;
typedef std::int16_t int16;

typedef std::uint8_t uint8;
typedef std::int8_t int8;

typedef float float32;
typedef double float64;

typedef unsigned char uchar;

typedef std::uint64_t u64;
typedef std::int64_t i64;

typedef std::uint32_t u32;
typedef std::int32_t i32;

typedef std::uint16_t u15;
typedef std::int16_t i16;

typedef std::uint8_t u8;
typedef std::int8_t i8;

typedef float f32;
typedef double f64;

template<typename... Args>
using Tuple = std::tuple<Args...>;
using String = std::string;

template<typename Key, typename Value>
using Dict = std::unordered_map<Key, Value>;

template<typename Value>
using Array = std::vector<Value>;

}
