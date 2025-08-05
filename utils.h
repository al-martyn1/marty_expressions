/*!
    \file
    \brief Утилзы
 */
#pragma once

#include "types.h"

#include <string>


//----------------------------------------------------------------------------
namespace marty {
namespace expressions {
namespace utils {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// Нафиг оптимизации через словарь, делаем тупо на сравнениях
inline
std::string getBracketPair(const std::string &brStr)
{
    if      (brStr=="(") return ")";
    else if (brStr=="{") return "}";
    else if (brStr=="[") return "]";
    else if (brStr=="<") return ">";
    else if (brStr==")") return "(";
    else if (brStr=="}") return "{";
    else if (brStr=="]") return "[";
    else if (brStr==">") return "<";
    else return std::string();
}

//----------------------------------------------------------------------------
inline
std::string getBracketPair(const char *pBrStr)
{
    return pBrStr ? getBracketPair(pBrStr) : std::string();
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template< typename PositionInfoType
        , typename OperatorTokenType
        , typename IntegerType       = MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE
        , typename FloatingPointType = MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE
        , typename StringType        = MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE
        >
StringType expressionItemToString(const ExpressionInputItem<PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType> &v)
{
    return std::visit( [](const auto &a) -> StringType
                       {
                           using ArgType = std::decay_t<decltype(a)>;
                           if constexpr (std::is_same_v <ArgType, Operator<PositionInfoType, OperatorTokenType, StringType> >) return a.text;
                           if constexpr (std::is_same_v <ArgType, BoolLiteral<PositionInfoType>                             >) return StringType(a.value ? "true" : "false");
                           if constexpr (std::is_same_v <ArgType, IntegerLiteral<PositionInfoType, IntegerType>             >) return StringType(to_string(a.value));
                           if constexpr (std::is_same_v <ArgType, FloatingPointLiteral<PositionInfoType, FloatingPointType> >) return StringType(to_string(a.value));
                           if constexpr (std::is_same_v <ArgType, StringLiteral<PositionInfoType, StringType>               >) return StringType("\"") + a.value + StringType("\"");
                           if constexpr (std::is_same_v <ArgType, SymbolLiteral<PositionInfoType, StringType>               >) return StringType("'")  + a.value + StringType("'");
                           if constexpr (std::is_same_v <ArgType, IdentifierLiteral<PositionInfoType, StringType>           >) return a.value;
                           //if constexpr (std::is_same_v <ArgType, ExpressionEntry<PositionInfoType>                         >) return ;
                           // if constexpr (std::is_same_v <ArgType, FunctionCall<PositionInfoType, StringType>                >) return a.value + StringType("()");
                           // if constexpr (std::is_same_v <ArgType, FunctionalCast<PositionInfoType, StringType>              >) return a.value + StringType("()");;
                           // if constexpr (std::is_same_v <ArgType, Cast<PositionInfoType, StringType>                        >) return StringType("(") + a.value + StringType(")");
                           // if constexpr (std::is_same_v <ArgType, VoidValue<PositionInfoType, StringType>                   >) return StringType("void");
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
StringType expressionItemToString(const ExpressionItem<PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType> &v)
{
    return std::visit( [](const auto &a) -> StringType
                       {
                           using ArgType = std::decay_t<decltype(a)>;
                           if constexpr (std::is_same_v <ArgType, Operator<PositionInfoType, OperatorTokenType, StringType> >) return a.text;
                           if constexpr (std::is_same_v <ArgType, BoolLiteral<PositionInfoType>                             >) return StringType(a.value ? "true" : "false");
                           if constexpr (std::is_same_v <ArgType, IntegerLiteral<PositionInfoType, IntegerType>             >) return StringType(to_string(a.value));
                           if constexpr (std::is_same_v <ArgType, FloatingPointLiteral<PositionInfoType, FloatingPointType> >) return StringType(to_string(a.value));
                           if constexpr (std::is_same_v <ArgType, StringLiteral<PositionInfoType, StringType>               >) return StringType("\"") + a.value + StringType("\"");
                           if constexpr (std::is_same_v <ArgType, SymbolLiteral<PositionInfoType, StringType>               >) return StringType("'")  + a.value + StringType("'");
                           if constexpr (std::is_same_v <ArgType, IdentifierLiteral<PositionInfoType, StringType>           >) return a.value;
                           //if constexpr (std::is_same_v <ArgType, ExpressionEntry<PositionInfoType>                         >) return ;
                           if constexpr (std::is_same_v <ArgType, FunctionCall<PositionInfoType, StringType>                >) return a.value + StringType("()");
                           if constexpr (std::is_same_v <ArgType, FunctionalCast<PositionInfoType, StringType>              >) return a.value + StringType("()");;
                           if constexpr (std::is_same_v <ArgType, Cast<PositionInfoType, StringType>                        >) return StringType("(") + a.value + StringType(")");
                           if constexpr (std::is_same_v <ArgType, VoidValue<PositionInfoType, StringType>                   >) return StringType("void");
                           // return ItemType::unknown;
                       }
                     , v
                     );
}



//----------------------------------------------------------------------------
} // namespace utils
} // namespace expressions
} // namespace marty
