/*!
    \file
    \brief Библиотека marty::expressions
 */
#pragma once


#include "enums.h"
#include "defs.h"

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



#if defined(MARTY_EXPRESSIONS_MARTY_BIGINT_USED)
    #define MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE    marty::BigInt
#else
    #define MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE    std::int64_t
#endif

#if defined(MARTY_EXPRESSIONS_MARTY_DECIMAL_USED)
    #define MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE    marty::Decimal
#else
    #define MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE    double
#endif


#if !defined(MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE)
    #define MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE    std::string
#endif



// #include "marty_expressions/types.h"
// marty::expressions::
namespace marty {
namespace expressions {



template<typename PositionInfoType>
struct BoolLiteral
{
    PositionInfoType     positionInfo;
    bool                 value;

}; // BoolLiteral


template<typename PositionInfoType, typename IntegerType=MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE, typename StringType=MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE>
struct IntegerLiteral
{
    PositionInfoType     positionInfo;
    IntegerType          value;
    StringType           suffix;

    // bool isNegative() const { return negSign; }

}; // IntegerLiteral


template<typename PositionInfoType, typename FloatingPointType=MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE, typename StringType=MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE>
struct FloatingPointLiteral
{
    PositionInfoType     positionInfo;
    FloatingPointType    value;
    StringType           suffix;

    // bool isNegative() const { return value.sign()<0; }

}; // FloatingPointLiteral


template<typename PositionInfoType, typename StringType=MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE>
struct StringLiteral
{
    PositionInfoType     positionInfo;
    StringType           value;
    StringType           suffix;

}; // StringLiteral


template<typename PositionInfoType, typename StringType=MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE>
struct SymbolLiteral
{
    PositionInfoType     positionInfo;
    StringType           value;
    StringType           suffix;

}; // SymbolLiteral


template<typename PositionInfoType, typename StringType=MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE>
struct IdentifierLiteral
{
    PositionInfoType     positionInfo;
    StringType           value;

}; // IdentifierLiteral


template<typename PositionInfoType, typename OperatorTokenType, typename StringType=MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE>
struct OperatorToken
{
    PositionInfoType     positionInfo;
    OperatorTokenType    value;
    StringType           text;

}; // OperatorToken




template<typename PositionInfoType, typename StringType=MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE>
struct FunctionCall
{
    PositionInfoType     positionInfo;
    StringType           value; // function name

}; // FunctionCall


template<typename PositionInfoType, typename StringType=MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE>
struct FunctionalCast
{
    PositionInfoType     positionInfo;
    StringType           value; // type name

}; // FunctionalCast


// C-style cast
template<typename PositionInfoType, typename StringType=MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE>
struct Cast
{
    PositionInfoType     positionInfo;
    StringType           value; // type name

}; // Cast


template<typename PositionInfoType, typename StringType=MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE>
struct VoidValue
{
    PositionInfoType     positionInfo;
    StringType           value; // for uniformity

}; // VoidValue



//----------------------------------------------------------------------------
template< typename PositionInfoType
        , typename OperatorTokenType
        , typename IntegerType       =MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE
        , typename FloatingPointType =MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE
        , typename StringType        =MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE
        >
using ExpressionInputItem = std::variant< OperatorToken<PositionInfoType, OperatorTokenType, StringType>
                                        , BoolLiteral<PositionInfoType>
                                        , IntegerLiteral<PositionInfoType, IntegerType, StringType>
                                        , FloatingPointLiteral<PositionInfoType, FloatingPointType, StringType>
                                        , StringLiteral<PositionInfoType, StringType>
                                        , SymbolLiteral<PositionInfoType, StringType>
                                        , IdentifierLiteral<PositionInfoType, StringType>
                                        >;

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template< typename PositionInfoType
        , typename OperatorTokenType
        , typename IntegerType       = MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE
        , typename FloatingPointType = MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE
        , typename StringType        = MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE
        >
using ExpressionItem = std::variant< OperatorToken<PositionInfoType, OperatorTokenType, StringType>
                                   , BoolLiteral<PositionInfoType>
                                   , IntegerLiteral<PositionInfoType, IntegerType>
                                   , FloatingPointLiteral<PositionInfoType, FloatingPointType>
                                   , StringLiteral<PositionInfoType, StringType>
                                   , SymbolLiteral<PositionInfoType, StringType>
                                   , IdentifierLiteral<PositionInfoType, StringType>
                                   , FunctionCall<PositionInfoType, StringType>
                                   , FunctionalCast<PositionInfoType, StringType>
                                   , Cast<PositionInfoType, StringType>
                                   , VoidValue<PositionInfoType, StringType>
                                   >;
//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template< typename PositionInfoType
        , typename OperatorTokenType
        , typename IntegerType=MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE
        , typename FloatingPointType=MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE
        , typename StringType=MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE
        >
struct ExpressionNode
{
    using ExpressionItemType = ExpressionItem< PositionInfoType 
                                             , OperatorTokenType
                                             , IntegerType      
                                             , FloatingPointType
                                             , StringType       
                                             >;


    ExpressionItemType            nodeValue; // литерал - аргументов нет (они игнорируются), идентификатором - ссылка на переменную, FunctionCall - много аргументов, FunctionalCast - один аргумент, или OperatorToken - количество аргументов зависит от оператора

    std::vector<ExpressionNode>   argList;

}; // struct ExpressionNode

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template< typename PositionInfoType
        , typename OperatorTokenType
        , typename IntegerType       = MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE
        , typename FloatingPointType = MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE
        , typename StringType        = MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE
        >
struct OperatorInfo
{
    using ExpressionItemType = 
    ExpressionItem< PositionInfoType 
                  , OperatorTokenType
                  , IntegerType      
                  , FloatingPointType
                  , StringType       
                  >;

    ExpressionItemType    operatorItem;

    OperatorType          operatorType = OperatorType::none; // none/regular/groupping/simpleCast/functionalCast/functionCall/indexation/templateInstantiation

    // Для скобок
    BracketKind           bracketKind  = BracketKind::none; // none/open/close - если none, то не скобка, а обычный оператор

    // Для операторов
    OperatorAffixation    affixation    = OperatorAffixation::none;    //  none/prefix/postfix
    OperatorArity         arity         = OperatorArity::none;         // unary/binary/etc
    OperatorAssociativity associativity = OperatorAssociativity::none; // right/left

    OperatorTokenType     operatorPair = 0; // For ternary operators - second part separator, for brackets - bracket pair

    int                   precedence   = 0; // Приоритет операцииб делаем int, задолбало всё выносить в параметры шаблона


}; // struct OperatorInfo





#if 0

enum class OperatorAssociativity : std::uint32_t
{
    unknown   = (std::uint32_t)(-1) /*!< ! */,
    invalid   = (std::uint32_t)(-1) /*!< ! */,
    none      = 0x00 /*!<  */,
    left      = 0x01 /*!< left-hand operator */,
    right     = 0x02 /*!< right-hand operator */

}; // enum 


enum class OperatorAffixation : std::uint32_t
{
    unknown   = (std::uint32_t)(-1) /*!< ! */,
    invalid   = (std::uint32_t)(-1) /*!< ! */,
    none      = 0x00 /*!< ! */,
    prefix    = 0x01 /*!< Prefix mode allowed for operator */,
    postfix   = 0x02 /*!< Postfix mode allowed for operator */

}; // enum 


enum class BracketKind : std::uint32_t
{
    unknown   = (std::uint32_t)(-1) /*!< ! */,
    invalid   = (std::uint32_t)(-1) /*!< ! */,
    none      = 0x00 /*!< Returned for non-bracket symbols */,
    open      = 0x01 /*!< Open bracket */,
    close     = 0x02 /*!< Close bracket */

}; // enum 


enum class BracketType : std::uint32_t
{
    unknown                 = (std::uint32_t)(-1) /*!< ! */,
    invalid                 = (std::uint32_t)(-1) /*!< ! */,
    none                    = 0x00 /*!< Returned for non-bracket symbols */,
    groupping               = 0x01 /*!< Operations groupping for priority raising */,
    functionalCast          = 0x02 /*!< Functional cast */,
    functionCall            = 0x03 /*!< Function call */,
    indexation              = 0x04 /*!< Indexation */,
    templateInstantiation   = 0x05 /*!< Template instantiation */

}; // enum 


enum class OperatorArity : std::uint32_t
{
    unknown          = (std::uint32_t)(-1) /*!< ! */,
    invalid          = (std::uint32_t)(-1) /*!< ! */,
    none             = 0x00 /*!< Zero number of operands */,
    nonary           = 0x00 /*!< Zero number of operands */,
    unary            = 0x01 /*!< Unary operator. Mutually exclusive with `ternary` and `nAry` flags. Can be combined with `binary`. */,
    binary           = 0x02 /*!< Binary operator. Mutually exclusive with `ternary` and `nAry` flags. Can be combined with `unary`. */,
    ternary          = 0x04 /*!< Ternary operator. Mutually exclusive with other arity flags. */,
    nAry             = 0x08 /*!< N-arity. Mutually exclusive with other arity flags. */,
    unaryBinary      = unary  | binary /*!< Unary or binary operator */,
    fixedArityMask   = unary | binary | ternary /*!< Mask to pick out fixed size arity */,
    arityMask        = fixedArityMask | nAry /*!< Mask to pick out arity */

}; // enum 





#endif

/*






Напиши кода парсера.
Распиши поподробнее, что и как ты делал.

*/




} // namespace expressions
} // namespace marty

// #include "marty_expressions/types.h"
// marty::expressions::



// marty::expressions::
// #include "marty_expressions/types.h"

