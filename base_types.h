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
    PositionInfoType     positionInfo = {};
    bool                 value = false;

}; // BoolLiteral


template<typename PositionInfoType, typename IntegerType=MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE, typename StringType=MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE>
struct IntegerLiteral
{
    PositionInfoType     positionInfo = {};
    IntegerType          value = {};
    StringType           suffix = {};

}; // IntegerLiteral


template<typename PositionInfoType, typename FloatingPointType=MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE, typename StringType=MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE>
struct FloatingPointLiteral
{
    PositionInfoType     positionInfo = {};
    FloatingPointType    value = {};
    StringType           suffix = {};

}; // FloatingPointLiteral


template<typename PositionInfoType, typename StringType=MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE>
struct StringLiteral
{
    PositionInfoType     positionInfo = {};
    StringType           value = {};
    StringType           suffix = {};

}; // StringLiteral


template<typename PositionInfoType, typename StringType=MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE>
struct SymbolLiteral
{
    PositionInfoType     positionInfo = {};
    StringType           value = {};
    StringType           suffix = {};

}; // SymbolLiteral


template<typename PositionInfoType, typename StringType=MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE>
struct IdentifierLiteral
{
    PositionInfoType     positionInfo = {};
    StringType           value = {}; // !!! сделать вектор

}; // IdentifierLiteral


template<typename PositionInfoType, typename OperatorTokenType, typename StringType=MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE>
struct Operator
{
    PositionInfoType     positionInfo = {};
    OperatorTokenType    value = {};
    StringType           text = {};

}; // Operator

//----------------------------------------------------------------------------
// Закончились допустимые входные типы 
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// Начинаются выходные типы
//----------------------------------------------------------------------------
// Для того, чтобы отделять отдельное подвыражение от остального - скобочное выражение
template<typename PositionInfoType>
struct ExpressionEntry
{
    PositionInfoType     positionInfo = {};

    // !!! Добавить тип скобки

}; // ExpressionEntry


template<typename PositionInfoType, typename StringType=MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE>
struct FunctionCall
{
    PositionInfoType     positionInfo = {};
    StringType           value = {}; // function name // !!! сделать вектор

    // !!! Надо как-то пометить окончание - пустое имя?

}; // FunctionCall


template<typename PositionInfoType, typename StringType=MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE>
struct FunctionalCast
{
    PositionInfoType     positionInfo = {};
    StringType           value = {}; // type name // !!! сделать вектор

    // !!! Надо как-то пометить окончание - пустое имя?

}; // FunctionalCast


// C-style cast
template<typename PositionInfoType, typename StringType=MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE>
struct Cast
{
    PositionInfoType     positionInfo = {};
    StringType           value = {}; // type name // !!! сделать вектор

}; // Cast


template<typename PositionInfoType, typename StringType=MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE>
struct VoidValue
{
    PositionInfoType     positionInfo = {};
    StringType           value = {}; // for uniformity

}; // VoidValue



//----------------------------------------------------------------------------
template< typename PositionInfoType
        , typename OperatorTokenType
        , typename IntegerType       =MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE
        , typename FloatingPointType =MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE
        , typename StringType        =MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE
        >
using ExpressionInputItem = std::variant< Operator<PositionInfoType, OperatorTokenType, StringType>
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
using ExpressionItem = std::variant< Operator<PositionInfoType, OperatorTokenType, StringType>
                                   , BoolLiteral<PositionInfoType>
                                   , IntegerLiteral<PositionInfoType, IntegerType>
                                   , FloatingPointLiteral<PositionInfoType, FloatingPointType>
                                   , StringLiteral<PositionInfoType, StringType>
                                   , SymbolLiteral<PositionInfoType, StringType>
                                   , IdentifierLiteral<PositionInfoType, StringType>
                                   //, ExpressionEntry<PositionInfoType>
                                   , FunctionCall<PositionInfoType, StringType>
                                   , FunctionalCast<PositionInfoType, StringType>
                                   , Cast<PositionInfoType, StringType>
                                   , VoidValue<PositionInfoType, StringType>
                                   >;
//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template< typename PositionInfoType
        , typename OperatorTokenType
        , typename IntegerType       = MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE
        , typename FloatingPointType = MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE
        , typename StringType        = MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE
        >
ItemType getExpressionItemType(const ExpressionItem<PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType> &v)
{
    return std::visit( [](const auto &a) -> ItemType
                       {
                           using ArgType = std::decay_t<decltype(a)>;
                           if constexpr (std::is_same_v <ArgType, Operator<PositionInfoType, OperatorTokenType, StringType> >) return ItemType::operator_           ;
                           if constexpr (std::is_same_v <ArgType, BoolLiteral<PositionInfoType>                             >) return ItemType::boolLiteral         ;
                           if constexpr (std::is_same_v <ArgType, IntegerLiteral<PositionInfoType, IntegerType>             >) return ItemType::integerLiteral      ;
                           if constexpr (std::is_same_v <ArgType, FloatingPointLiteral<PositionInfoType, FloatingPointType> >) return ItemType::floatingPointLiteral;
                           if constexpr (std::is_same_v <ArgType, StringLiteral<PositionInfoType, StringType>               >) return ItemType::stringLiteral       ;
                           if constexpr (std::is_same_v <ArgType, SymbolLiteral<PositionInfoType, StringType>               >) return ItemType::symbolLiteral       ;
                           if constexpr (std::is_same_v <ArgType, IdentifierLiteral<PositionInfoType, StringType>           >) return ItemType::identifier          ;
                           //if constexpr (std::is_same_v <ArgType, ExpressionEntry<PositionInfoType>                         >) return ItemType::expressionEntry     ;
                           if constexpr (std::is_same_v <ArgType, FunctionCall<PositionInfoType, StringType>                >) return ItemType::functionCall        ;
                           if constexpr (std::is_same_v <ArgType, FunctionalCast<PositionInfoType, StringType>              >) return ItemType::functionalCast      ;
                           if constexpr (std::is_same_v <ArgType, Cast<PositionInfoType, StringType>                        >) return ItemType::cast                ;
                           if constexpr (std::is_same_v <ArgType, VoidValue<PositionInfoType, StringType>                   >) return ItemType::void_               ;
                           // return ItemType::unknown;
                       }
                     , v
                     );
}

//----------------------------------------------------------------------------
template< typename PositionInfoType
        , typename OperatorTokenType
        , typename IntegerType       = MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE
        , typename FloatingPointType = MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE
        , typename StringType        = MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE
        >
ItemType getExpressionItemType(const ExpressionInputItem<PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType> &v)
{
    return std::visit( [](const auto &a) -> ItemType
                       {
                           using ArgType = std::decay_t<decltype(a)>;
                           if constexpr (std::is_same_v <ArgType, Operator<PositionInfoType, OperatorTokenType, StringType> >) return ItemType::operator_           ;
                           if constexpr (std::is_same_v <ArgType, BoolLiteral<PositionInfoType>                             >) return ItemType::boolLiteral         ;
                           if constexpr (std::is_same_v <ArgType, IntegerLiteral<PositionInfoType, IntegerType>             >) return ItemType::integerLiteral      ;
                           if constexpr (std::is_same_v <ArgType, FloatingPointLiteral<PositionInfoType, FloatingPointType> >) return ItemType::floatingPointLiteral;
                           if constexpr (std::is_same_v <ArgType, StringLiteral<PositionInfoType, StringType>               >) return ItemType::stringLiteral       ;
                           if constexpr (std::is_same_v <ArgType, SymbolLiteral<PositionInfoType, StringType>               >) return ItemType::symbolLiteral       ;
                           if constexpr (std::is_same_v <ArgType, IdentifierLiteral<PositionInfoType, StringType>           >) return ItemType::identifier          ;

                           //if constexpr (std::is_same_v <ArgType, ExpressionEntry<PositionInfoType>                         >) return ItemType::expressionEntry     ;
                           //if constexpr (std::is_same_v <ArgType, FunctionCall<PositionInfoType, StringType>                >) return ItemType::functionCall        ;
                           //if constexpr (std::is_same_v <ArgType, FunctionalCast<PositionInfoType, StringType>              >) return ItemType::functionalCast      ;
                           //if constexpr (std::is_same_v <ArgType, Cast<PositionInfoType, StringType>                        >) return ItemType::cast                ;
                           //if constexpr (std::is_same_v <ArgType, VoidValue<PositionInfoType, StringType>                   >) return ItemType::void_               ;

                           // return ItemType::unknown;
                       }
                     , v
                     );
}

//----------------------------------------------------------------------------
template< typename PositionInfoType
        , typename OperatorTokenType
        , typename IntegerType       = MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE
        , typename FloatingPointType = MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE
        , typename StringType        = MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE
        >
PositionInfoType getExpressionItemPositionInfo(const ExpressionItem<PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType> &v)
{
    return std::visit( [](const auto &a) -> PositionInfoType
                       {
                           return a.positionInfo;
                       }
                     , v
                     );
}

//----------------------------------------------------------------------------
template< typename PositionInfoType
        , typename OperatorTokenType
        , typename IntegerType       = MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE
        , typename FloatingPointType = MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE
        , typename StringType        = MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE
        >
StringType getExpressionItemString(const ExpressionItem<PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType> &v)
{
    return std::visit( [](const auto &a) -> StringType
                       {
                           using ArgType = std::decay_t<decltype(a)>;
                           if constexpr ( std::is_same_v <ArgType, StringLiteral<PositionInfoType, StringType>     >
                                       || std::is_same_v <ArgType, SymbolLiteral<PositionInfoType, StringType>     >
                                       || std::is_same_v <ArgType, IdentifierLiteral<PositionInfoType, StringType> >
                                       || std::is_same_v <ArgType, FunctionCall<PositionInfoType, StringType>      >
                                       || std::is_same_v <ArgType, FunctionalCast<PositionInfoType, StringType>    >
                                       || std::is_same_v <ArgType, Cast<PositionInfoType, StringType>              >
                                       || std::is_same_v <ArgType, VoidValue<PositionInfoType, StringType>         >
                                        )
                               return a.value;
                           else
                               return StringType();
                       }
                     , v
                     );
}

//----------------------------------------------------------------------------
inline
std::string getExpressionItemTypeName(ItemType ii)
{
    switch(ii)
    {
        case ItemType::operator_           : return "operator";              
        case ItemType::boolLiteral         : return "bool literal";          
        case ItemType::integerLiteral      : return "integer literal";       
        case ItemType::floatingPointLiteral: return "floating point literal";
        case ItemType::stringLiteral       : return "string literal";        
        case ItemType::symbolLiteral       : return "symbol literal";        
        case ItemType::identifier          : return "identifier";            
        case ItemType::expressionEntry     : return "expression";         
        case ItemType::functionCall        : return "function call";         
        case ItemType::functionalCast      : return "functional cast";       
        case ItemType::cast                : return "cast";                  
        case ItemType::void_               : return "void value";            
        case ItemType::unknown             : [[fallthrough]];
        default                            : return "ExpressionItem of unknown type";
    }
}

//----------------------------------------------------------------------------





} // namespace expressions
} // namespace marty

// marty::expressions::
// #include "marty_expressions/types.h"

