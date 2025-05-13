/*!
    \file
    \brief Библиотека marty::expressions
 */
#pragma once



//
#include "undef_min_max.h"


// #include "marty_expressions/types.h"
// marty::expressions::
namespace marty {
namespace expressions {


// https://en.wikipedia.org/wiki/Arity
// https://ru.wikipedia.org/wiki/%D0%9A%D0%B0%D1%82%D0%B5%D0%B3%D0%BE%D1%80%D0%B8%D1%8F:%D0%90%D1%80%D0%BD%D0%BE%D1%81%D1%82%D1%8C
// https://ru.wikipedia.org/wiki/%D0%90%D1%80%D0%BD%D0%BE%D1%81%D1%82%D1%8C

enum class OperatorArity
{
    unaryOp       = 1,
    binaryOp      = 2,
    unaryBinaryOp = 3
};


} // namespace expressions
} // namespace marty

// marty::expressions::
// #include "marty_expressions/types.h"

