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

    - Число с плавающей точкой - используем marty::Decimal

    - Строковый литерал - храним в виде строки

    - Символьный литерал - храним в виде строки

    - Идентификатор - имя или список имён - список имён получается из последовательности идентификаторов,
      разделённых NS-разделителем.

    - Оператор. Оператор задаётся токеном оператора. Тип токена - шаблонный.

    - Скобка. Скобка - тоже оператор. Открывающая, закрывающая - разные оператор-токены.
      Надо уметь по типу токена оператора понимать, что это скобка, понимать - открывающая или нет (isBrace, isOpen).

    Что ещё?

*/

template<typename PositionInfoType, typename UnsignedType=std::uint64_t>
struct IntegerLiteral
{
    PositionInfoType     positionInfo;

    UnsignedType         value;
    bool                 negSign; // if true, value is negative

    bool isNegative() const { return negSign; }

}; // IntegerLiteral


template<typename PositionInfoType, typename FloatingPointType=marty::Decimal>
struct FloatingPointLiteral
{
    PositionInfoType     positionInfo;

    FloatingPointType    value;

    bool isNegative() const { return value.sign()<0; }

}; // FloatingPointLiteral


template<typename PositionInfoType, typename StringType>
struct StringLiteral
{
    PositionInfoType     positionInfo;

    StringType           value;

}; // StringLiteral


template<typename PositionInfoType, typename StringType>
struct SymbolLiteral
{
    PositionInfoType     positionInfo;

    StringType           value;

}; // SymbolLiteral


template<typename PositionInfoType, typename OperatorTokenType>
struct OperatorToken
{
    PositionInfoType     positionInfo;

    OperatorTokenType    value;

}; // SymbolLiteral






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

