/*!
    \file
    \brief Библиотека marty::expressions
 */
#pragma once


#include "marty_decimal/marty_decimal.h"

#include "enums.h"

//
#include <vector>
#include <cstddef>
#include <functional>
#include <string>
#include <type_traits>
#include <tuple>
#include <variant>
#include <vector>
#include <memory>

//
#include "undef_min_max.h"


// #include "marty_expressions/types.h"
// marty::expressions::
namespace marty {
namespace expressions {

/*
    Что у нас может быть в качестве входного токена?

    - Целое число - std::uint64_t - беззнаковое, 
      но в качестве результата выражения беззнаковое может превратиться в знаковое, поэтому надо завернуть
      в структуру, в которой также храним знак.
      Вообще, хорошо бы сделать отдельный тип BigInt, по типу Decimal, но пока обойдусь

*/


/*
enum OperatorType
{
    opOperator
    opFunction

};

template<typename OpTokenType, typename StringType>
struct ExpressionNode
{
    OperatorArity                  arity; //! Арность оператора - unary, binary, ternary или nAry - только эти значения

    OperatorType                   operatorType   ;
    OpTokenType                    opToken        ; // operatorType==opOperator
    std::vector<StringType>        opFunctionName ; // operatorType==opFunction


    std::vector<ExpressionNode>    args;


};
*/



} // namespace expressions
} // namespace marty

// marty::expressions::
// #include "marty_expressions/types.h"

