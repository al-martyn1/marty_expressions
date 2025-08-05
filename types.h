/*!
    \file
    \brief Библиотека marty::expressions
 */
#pragma once


#include "enums.h"
#include "defs.h"
//
#include "base_types.h"

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


    ExpressionItemType            nodeValue; // литерал - аргументов нет (они игнорируются), идентификатор - ссылка на переменную, FunctionCall - много аргументов, FunctionalCast - один аргумент, или Operator - количество аргументов зависит от оператора
    std::vector<ExpressionNode>   argList;

    // Требуются для вычисления значения выражения, передаются в вычислитель
    // В зависимости от формы - префиксная/постфиксная, а также арности 
    // операции могут производится по-разному.
    OperatorAffixation            affixation    = OperatorAffixation::none;    //  none/prefix/postfix
    OperatorArity                 arity         = OperatorArity::none;         // unary/binary/etc

    // associativity - right/left не требуется для вычисления выражения, она используется только для вычисления порядка операндов


}; // struct ExpressionNode

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// Информация об операторе для стека операторов
template< typename PositionInfoType
        , typename OperatorTokenType
        , typename IntegerType       = MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE
        , typename FloatingPointType = MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE
        , typename StringType        = MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE
        >
struct OperationInfo
{

    // OperatorTokenType, содержащийся в OperatorType, может обозначать functionCall, functionalCast, cast
    // Ещё надо бы хранить имя, если у нас хранится functionCall, functionalCast, cast
    // using OperatorType = Operator<PositionInfoType, OperatorTokenType, StringType>;

    using ExpressionItemType = ExpressionItem<PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType>;


    using IdentifierLiteralType = IdentifierLiteral<PositionInfoType, StringType>;

    // OperatorType          opInfo;
    ExpressionItemType       opInfo;
    IdentifierLiteralType callableObjectName;

    // Или таки хранить ExpressionItem? Ведь для операторов токены могут быть, а вот для callable сущностей токены могут быть не предусмотрены?

    // Для не операторов (ExpressionEntry/FunctionCall/FunctionalCast/Cast/VoidValue)
    // задаём какое-то значение?
    OperatorFeatures      opFeatures = OperatorFeatures::none; // none/regular/groupping/simpleCast/functionalCast/functionCall/indexation/templateInstantiation

    // Для скобок
    BracketKind           bracketKind  = BracketKind::none; // none/open/close - если none, то не скобка, а обычный оператор

    // Для операторов
    OperatorAffixation    affixation    = OperatorAffixation::none;    //  none/prefix/postfix
    OperatorArity         arity         = OperatorArity::none;         // unary/binary/etc
    OperatorAssociativity associativity = OperatorAssociativity::none; // right/left

    OperatorTokenType     operatorPair = 0; // For ternary operators - second part separator, for brackets - bracket pair

    int                   precedence   = 0; // Приоритет операции делаем int, задолбало всё выносить в параметры шаблона

    std::size_t           argIdx       = 0;
    std::size_t           argsNum      = std::size_t(-1); // любое число раз


    int makeFullPrecedence() const
    {
        return (precedence<<1) + (associativity==OperatorTokenType::right ? 0 : 1);
    }

    // Если приоритет численно меньше, значит, операция приоритетнее
    int precidenceCompare(const OperationInfo &other) const
    {
        auto fp  = makeFullPrecedence();
        auto ofp = other.makeFullPrecedence();

        if (fp<ofp)
            return 1;

        if (fp>ofp)
            return -1;

        return 0;
    }

    bool precedenceGreater  (const OperationInfo &other) const { return precidenceCompare(other)> 0; }
    bool precedenceGreaterEq(const OperationInfo &other) const { return precidenceCompare(other)>=0; }
    bool precedenceLess     (const OperationInfo &other) const { return precidenceCompare(other)< 0; }
    bool precedenceLessEq   (const OperationInfo &other) const { return precidenceCompare(other)<=0; }


}; // struct OperationInfo





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

