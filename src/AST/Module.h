#pragma once

#include "Root.h"

namespace kiwi {

template<typename NodeTrait>
using Module = Dict<String, generic::Root<Expression<NodeTrait>>>;

}