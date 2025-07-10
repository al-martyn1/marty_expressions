/*!
    \file
    \brief Пример трейтов операторов
 */
#pragma once

#include "types.h"
//
#include "umba/tokenizer/tokenizer.h"
//
#include "undef_min_max.h"


// #include "marty_expressions/sample_op_traits.h"
// marty::expressions::
namespace marty {
namespace expressions {

//----------------------------------------------------------------------------


template<typename OperatorTokenType>
struct SampleOperatorTraits
{
    using PrecedenceType = unsigned;

    // Надо уметь по типу токена оператора понимать, что это скобка, понимать - открывающая или нет (isBrace, isOpen).


    constexpr
    BracketKind getBracketKind(OperatorTokenType tokenType) const
    {
        return tokenType==UMBA_TOKENIZER_TOKEN_CURLY_BRACKET_OPEN
             ? BracketKind::open
             : tokenType==UMBA_TOKENIZER_TOKEN_CURLY_BRACKET_CLOSE
             ? BracketKind::close
             : tokenType==UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_OPEN
             ? BracketKind::open
             : tokenType==UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_CLOSE
             ? BracketKind::close
             : tokenType==UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_OPEN
             ? BracketKind::open
             : tokenType==UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_CLOSE
             ? BracketKind::close
             : tokenType==UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_OPEN
             ? BracketKind::open
             : tokenType==UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_CLOSE
             ? BracketKind::close
             : tokenType==UMBA_TOKENIZER_TOKEN_DBLSQUARE_BRACKET_OPEN
             ? BracketKind::open
             : tokenType==UMBA_TOKENIZER_TOKEN_DBLSQUARE_BRACKET_CLOSE
             ? BracketKind::close
             : BracketKind::none
             ;
    }

    constexpr
    OperatorType getOperatorType(OperatorTokenType tokenType) const
    {
        return tokenType==UMBA_TOKENIZER_TOKEN_CURLY_BRACKET_OPEN
             ? OperatorType::functionalCast
             : tokenType==UMBA_TOKENIZER_TOKEN_CURLY_BRACKET_CLOSE
             ? OperatorType::functionalCast
             : tokenType==UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_OPEN
             ? OperatorType::functionalCast | OperatorType::simpleCast | OperatorType::functionCall | OperatorType::groupping
             : tokenType==UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_CLOSE
             ? OperatorType::functionalCast | OperatorType::simpleCast | OperatorType::functionCall | OperatorType::groupping
             : tokenType==UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_OPEN
             ? OperatorType::templateInstantiation
             : tokenType==UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_CLOSE
             ? OperatorType::templateInstantiation
             : tokenType==UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_OPEN
             ? OperatorType::indexation
             : tokenType==UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_CLOSE
             ? OperatorType::indexation
             : tokenType==UMBA_TOKENIZER_TOKEN_DBLSQUARE_BRACKET_OPEN
             ? OperatorType::indexation
             : tokenType==UMBA_TOKENIZER_TOKEN_DBLSQUARE_BRACKET_CLOSE
             ? OperatorType::indexation
             : OperatorType::regular
             ;
    }

    // Очерёдность операций - https://ru.wikipedia.org/wiki/%D0%9E%D1%87%D0%B5%D1%80%D1%91%D0%B4%D0%BD%D0%BE%D1%81%D1%82%D1%8C_%D0%BE%D0%BF%D0%B5%D1%80%D0%B0%D1%86%D0%B8%D0%B9

    // https://en.cppreference.com/w/cpp/language/operator_precedence.html
    // Приоритет операторов зависит от:
    //   префикс/постфикс формы  - affixation
    //   унарности/бинарности    - arity
    //   лево-правостороннесть не роялит - она учитывается при обработке операторов с равным приоритетом

    // При вызове получения приоритета affixation и arity - не набор флагов, а только один из них
    // В примере не поддерживаем плюсовое унарное умножение Indirection (dereference), и унарное битовое AND - Address-of
    constexpr
    PrecedenceType getOperatorPrecedence(OperatorTokenType tokenType, OperatorAffixation affixation, OperatorArity arity) const
    {
        return tokenType==UMBA_TOKENIZER_TOKEN_CURLY_BRACKET_OPEN     || tokenType==UMBA_TOKENIZER_TOKEN_CURLY_BRACKET_CLOSE ||
               tokenType==UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_OPEN     || tokenType==UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_CLOSE ||
               tokenType==UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_OPEN     || tokenType==UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_CLOSE ||
               tokenType==UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_OPEN    || tokenType==UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_CLOSE ||
               tokenType==UMBA_TOKENIZER_TOKEN_DBLSQUARE_BRACKET_OPEN || tokenType==UMBA_TOKENIZER_TOKEN_DBLSQUARE_BRACKET_CLOSE
             ? 0u

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_SCOPE_RESOLUTION  /* a::b */
             ? 1u // Приоритет scope resolution operator'а не зависит ни от чего

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_INCREMENT || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_DECREMENT
             ? (affixation==OperatorAffixation::postfix ? 2u: 3u) // postfix - 2, prefix - 3, арность не роялит, они всегда унарные

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_DOT || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_MEMBER_OF_POINTER /* UMBA_TOKENIZER_TOKEN_OPERATOR_STRUCTURE_DEREFERENCE */  /* a.b, a->b */ // Всегда бинарные
             ? 2u

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_ADDITION || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_SUBTRACTION
             ? (arity==OperatorArity::unary ? 3u : 6u) // Унарные - приоритетнее

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_LOGICAL_NOT || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_NOT
             ? 3u

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_MULTIPLICATION
             // ? (arity==OperatorArity::unary ? 3u : 5u) // Унарное умножение - это Indirection (dereference)
             ? 5u // Унарное умножение - Indirection (dereference) - не поддерживаем

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_AND
             // ? (arity==OperatorArity::unary ? 3u : 11u) // В унарном варианте - это Address-of
             ? 11u // Address-of - не поддерживаем

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_POINTER_TO_MEMBER_OF_POINTER || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_POINTER_TO_MEMBER_OF_OBJECT /* ->*  .*  */
             ? 4u

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_DIVISION || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_MODULO /* division and remainder/modulo */
             ? 5u

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_SHIFT_LEFT || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_SHIFT_RIGHT
             ? 7u

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_THREE_WAY_COMPARISON
             ? 8u

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_GREATER || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_LESS ||
               tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_GREATER_EQUAL || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_LESS_EQUAL
             ? 9u

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_EQUAL || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_NOT_EQUAL
             ? 10u

             // : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_AND
             // ? 11u

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_XOR
             ? 12u

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_OR
             ? 13u

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_LOGICAL_AND
             ? 14u

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_LOGICAL_OR
             ? 15u

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_CC_TERNARY ||
               tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_ASSIGNMENT ||
               tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_ADDITION_ASSIGNMENT || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_SUBTRACTION_ASSIGNMENT ||
               tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_MULTIPLICATION_ASSIGNMENT || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_DIVISION_ASSIGNMENT || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_MODULO_ASSIGNMENT ||
               tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_SHIFT_LEFT_ASSIGNMENT || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_SHIFT_RIGHT_ASSIGNMENT ||
               tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_AND_ASSIGNMENT || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_OR_ASSIGNMENT || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_XOR_ASSIGNMENT ||
               tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_PASCAL_ASSIGNMENT
             ? 16u


             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_COMMA
             ? 17u

             : PrecedenceType(255) // Вряд ли есть больше уровней приоритетов
             ;
    }

    constexpr
    OperatorArity getOperatorArity(OperatorTokenType tokenType) const
    {
        return tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_INCREMENT || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_DECREMENT ||
               tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_LOGICAL_NOT || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_NOT
             ? OperatorArity::unary

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_CC_TERNARY
             ? OperatorArity::ternary

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_COMMA
             ? OperatorArity::nAry

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_ADDITION || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_SUBTRACTION
               // || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_MULTIPLICATION || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_AND // Поддерживаем только binary форму
             ? OperatorArity::unaryBinary

             : OperatorArity::binary // Все прочие операторы - бинарные
             ;
    }

    constexpr
    OperatorTokenType getTernarySecond(OperatorTokenType tokenType) const
    {
        return tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_CC_TERNARY ? UMBA_TOKENIZER_TOKEN_OPERATOR_COLON : 0;
    }

    // Валидно только для унарных операторов/для оператров, используемых в унарной форме, при наличии нескольких доступных форм
    constexptr
    OperatorAffixation getOperatorAffixation(OperatorTokenType tokenType) const 
    {
        return tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_INCREMENT || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_DECREMENT ||
             ? OperatorAffixation::prefix | OperatorAffixation::postfix
             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_LOGICAL_NOT || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_NOT
             ? OperatorAffixation::prefix
             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_ADDITION || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_SUBTRACTION
             ? OperatorAffixation::prefix
             // : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_MULTIPLICATION
             // ? OperatorAffixation::prefix
             // : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_AND
             // ? OperatorAffixation::prefix
             : tokenType==UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_OPEN    || tokenType==UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_CLOSE
             ? OperatorAffixation::prefix | OperatorAffixation::postfix // Индексация может быть как до, так и после имени - C/C++ правила
             : OperatorAffixation::none
             ;
    }



    // Очерёдность операций - https://ru.wikipedia.org/wiki/%D0%9E%D1%87%D0%B5%D1%80%D1%91%D0%B4%D0%BD%D0%BE%D1%81%D1%82%D1%8C_%D0%BE%D0%BF%D0%B5%D1%80%D0%B0%D1%86%D0%B8%D0%B9

    // https://en.cppreference.com/w/cpp/language/operator_precedence.html
    // Приоритет операторов зависит от:
    //   префикс/постфикс формы  - affixation
    //   унарности/бинарности    - arity
    //   лево-правостороннесть не роялит - она учитывается при обработке операторов с равным приоритетом

    // При вызове получения приоритета affixation и arity - не набор флагов, а только один из них
    // В примере не поддерживаем плюсовое унарное умножение Indirection (dereference), и унарное битовое AND - Address-of

    // left   // left-hand operator  (Left-to-right >) левая ассоциативность, при которой вычисление выражения происходит слева направо
    // right  // right-hand operator (Right-to-left <) правая ассоциативность — справа налево
    constexptr
    OperatorAssociativity getOperatorAssociativity(OperatorTokenType tokenType, OperatorAffixation affixation, OperatorArity arity)
    {
        return tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_SCOPE_RESOLUTION  /* a::b */
             ? OperatorAssociativity::left

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_INCREMENT || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_DECREMENT
             ? (affixation==OperatorAffixation::postfix ? OperatorAssociativity::left: OperatorAssociativity::right) // postfix - 2, prefix - 3, арность не роялит, они всегда унарные

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_DOT || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_MEMBER_OF_POINTER /* UMBA_TOKENIZER_TOKEN_OPERATOR_STRUCTURE_DEREFERENCE */  /* a.b, a->b */ // Всегда бинарные
             ? OperatorAssociativity::left

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_ADDITION || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_SUBTRACTION
             ? (arity==OperatorArity::unary ? OperatorAssociativity::right : OperatorAssociativity::left) // Унарные - приоритетнее

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_LOGICAL_NOT || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_NOT
             ? OperatorAssociativity::right

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_MULTIPLICATION
             // ? (arity==OperatorArity::unary ? 3u : 5u) // Унарное умножение - это Indirection (dereference)
             ? OperatorAssociativity::left // Унарное умножение - Indirection (dereference) - не поддерживаем

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_AND
             // ? (arity==OperatorArity::unary ? 3u : 11u) // В унарном варианте - это Address-of
             ? OperatorAssociativity::left // Address-of - не поддерживаем

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_POINTER_TO_MEMBER_OF_POINTER || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_POINTER_TO_MEMBER_OF_OBJECT /* ->*  .*  */
             ? OperatorAssociativity::left

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_DIVISION || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_MODULO /* division and remainder/modulo */
             ? OperatorAssociativity::left

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_SHIFT_LEFT || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_SHIFT_RIGHT
             ? OperatorAssociativity::left

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_THREE_WAY_COMPARISON
             ? OperatorAssociativity::left

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_GREATER || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_LESS ||
               tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_GREATER_EQUAL || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_LESS_EQUAL
             ? OperatorAssociativity::left

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_EQUAL || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_NOT_EQUAL
             ? OperatorAssociativity::left

             // : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_AND
             // ? 11u

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_XOR
             ? OperatorAssociativity::left

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_OR
             ? OperatorAssociativity::left

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_LOGICAL_AND
             ? OperatorAssociativity::left

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_LOGICAL_OR
             ? OperatorAssociativity::left

             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_CC_TERNARY ||
               tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_ASSIGNMENT ||
               tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_ADDITION_ASSIGNMENT || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_SUBTRACTION_ASSIGNMENT ||
               tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_MULTIPLICATION_ASSIGNMENT || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_DIVISION_ASSIGNMENT || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_MODULO_ASSIGNMENT ||
               tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_SHIFT_LEFT_ASSIGNMENT || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_SHIFT_RIGHT_ASSIGNMENT ||
               tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_AND_ASSIGNMENT || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_OR_ASSIGNMENT || tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_XOR_ASSIGNMENT ||
               tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_PASCAL_ASSIGNMENT
             ? OperatorAssociativity::right


             : tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_COMMA
             ? OperatorAssociativity::left

             : OperatorAssociativity::none
             ;
    }


}; // struct SampleOperatorTraits

// OperatorAffixation
//     unknown,invalid             = -1   // !
//     none                        =  0   // !
//     prefix                             // Prefix mode allowed for operator
//     postfix                            // Postfix mode allowed for operator
//  
// OperatorArity
//     unknown,invalid             = -1   // !
//     
//     none,nonary                 = 0    // Zero number of operands
//     
//     unary                              // Unary operator. Mutually exclusive with `ternary` and `nAry` flags. Can be combined with `binary`.
//     binary                             // Binary operator. Mutually exclusive with `ternary` and `nAry` flags. Can be combined with `unary`.
//     ternary                            // Ternary operator. Mutually exclusive with other arity flags.
//     nAry                               // N-arity. Mutually exclusive with other arity flags.
//  
// OperatorAssociativity
//     unknown,invalid             = -1   // !
//     
//     none
//     left                               // left-hand operator
//     right                              // right-hand operator





//----------------------------------------------------------------------------

} // namespace expressions
} // namespace marty

// marty::expressions::
// #include "marty_expressions/sample_op_traits.h"

