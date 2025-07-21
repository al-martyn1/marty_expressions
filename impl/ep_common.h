#pragma once

//----------------------------------------------------------------------------
template< typename OperatorTraitsTypeT, typename PositionInfoTypeT, typename OperatorTokenTypeT
        , typename IntegerTypeT, typename FloatingPointTypeT, typename StringTypeT >
typename ExpressionParser<OperatorTraitsTypeT, PositionInfoTypeT, OperatorTokenTypeT, IntegerTypeT, FloatingPointTypeT, StringTypeT>::OperationInfoType&
ExpressionParser<OperatorTraitsTypeT, PositionInfoTypeT, OperatorTokenTypeT, IntegerTypeT, FloatingPointTypeT, StringTypeT>::
updateAssociativityAndPreceedence(typename ExpressionParser<OperatorTraitsTypeT, PositionInfoTypeT, OperatorTokenTypeT, IntegerTypeT, FloatingPointTypeT, StringTypeT>::OperationInfoType &opi) const
{
    // auto opToken = opi.opInfo.value;
    auto opToken = std::get<OperatorType>(opi.opInfo).value;
    opi.associativity    = m_opTraits.getOperatorAssociativity(opToken, opi.affixation, opi.arity);   
    opi.precedence       = int(m_opTraits.getOperatorPrecedence(opToken, opi.affixation, opi.arity));
    return opi;
};

//----------------------------------------------------------------------------
/*
    Ограничения работают только при нормальном порядке следования операторов.
    А если у нас было закрытие скобки (группирующей, или функциональной, или индексной) - оно лежит на стеке операторов,
    то тогда requiresNameLeft файлится сразу.
*/
//----------------------------------------------------------------------------

//! Проверяется пришедший оператор. Исследуется то, что у нас лежит в стеке аргументов
template< typename OperatorTraitsTypeT, typename PositionInfoTypeT, typename OperatorTokenTypeT
        , typename IntegerTypeT, typename FloatingPointTypeT, typename StringTypeT >
Error
ExpressionParser<OperatorTraitsTypeT, PositionInfoTypeT, OperatorTokenTypeT, IntegerTypeT, FloatingPointTypeT, StringTypeT>::
checkOperatorRestrictions(const typename ExpressionParser<OperatorTraitsTypeT, PositionInfoTypeT, OperatorTokenTypeT, IntegerTypeT, FloatingPointTypeT, StringTypeT>::OperationInfoType &opi) const
{
    // auto opToken = opi.opInfo.value;
    // auto opRestrictions = m_opTraits.getOperatorRestrictions(tokenType, opi.affixation, opi.arity);

    MARTY_ARG_USED(opi);

    return Error::ok;
}

//----------------------------------------------------------------------------
//! Проверяются ограничения для оператора на вершине стека по приходу чего-то, что не является оператором
template< typename OperatorTraitsTypeT, typename PositionInfoTypeT, typename OperatorTokenTypeT
        , typename IntegerTypeT, typename FloatingPointTypeT, typename StringTypeT >
Error
ExpressionParser<OperatorTraitsTypeT, PositionInfoTypeT, OperatorTokenTypeT, IntegerTypeT, FloatingPointTypeT, StringTypeT>::
checkOperatorRestrictions(const typename ExpressionParser<OperatorTraitsTypeT, PositionInfoTypeT, OperatorTokenTypeT, IntegerTypeT, FloatingPointTypeT, StringTypeT>::ExpressionItemType &ei) const
{
    MARTY_ARG_USED(ei);

    return Error::ok;
}

//----------------------------------------------------------------------------
template< typename OperatorTraitsTypeT, typename PositionInfoTypeT, typename OperatorTokenTypeT
        , typename IntegerTypeT, typename FloatingPointTypeT, typename StringTypeT >
Error
ExpressionParser<OperatorTraitsTypeT, PositionInfoTypeT, OperatorTokenTypeT, IntegerTypeT, FloatingPointTypeT, StringTypeT>::
initialize()
{
    m_stateHandler = &ExpressionParser::waitUnaryPrefixOperatorOrValue;
    m_expression.nodeValue = IntegerLiteralType{PositionInfoType{}, IntegerType{0}};
    m_expression.argList.clear();
    m_ops.clear();
    m_operands.clear();

    return Error::ok;
}

//----------------------------------------------------------------------------
template< typename OperatorTraitsTypeT, typename PositionInfoTypeT, typename OperatorTokenTypeT
        , typename IntegerTypeT, typename FloatingPointTypeT, typename StringTypeT >
Error
ExpressionParser<OperatorTraitsTypeT, PositionInfoTypeT, OperatorTokenTypeT, IntegerTypeT, FloatingPointTypeT, StringTypeT>::
finalize()
{
    /* Надо будет не забыть, что в некоторых состояниях автомата его работа не может быть завершена

       Надо проверить парность всех скобок с учётом их вложенности.

     */

    return Error::ok;
}



// requiresNameRight                  // Requires name (identifier) at right
// requiresNameLeft                   // Requires name (identifier) at left
// requiresScopeRight                 // Requires scope operator at right
// requiresScopeLeft                  // Requires scope operator at left
//  
// requiresScopeOrNameRight = requiresNameRight|requiresScopeRight  // Requires scope operator or name (identifier) at right
// requiresScopeOrNameLeft  = requiresNameLeft|requiresScopeLeft    // Requires scope operator or name (identifier) at left


