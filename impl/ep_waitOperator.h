#pragma once

template< typename OperatorTraitsTypeT, typename PositionInfoTypeT, typename OperatorTokenTypeT
        , typename IntegerTypeT, typename FloatingPointTypeT, typename StringTypeT >
Error
ExpressionParser<OperatorTraitsTypeT, PositionInfoTypeT, OperatorTokenTypeT, IntegerTypeT, FloatingPointTypeT, StringTypeT>::
waitOperator
    ( const ExpressionParser<OperatorTraitsTypeT, PositionInfoTypeT, OperatorTokenTypeT, IntegerTypeT, FloatingPointTypeT, StringTypeT>::ExpressionItemType &tk
    , ItemType itemType
    , ExpressionParser<OperatorTraitsTypeT, PositionInfoTypeT, OperatorTokenTypeT, IntegerTypeT, FloatingPointTypeT, StringTypeT>::OperationInfoType opInfo
    )
{
    MARTY_ARG_USED(tk);
    MARTY_ARG_USED(itemType);
    MARTY_ARG_USED(opInfo);

    switch(itemType)
    {
        case ItemType::operator_           :
        {
            if ((opInfo.opFeatures&OperatorFeatures::regular)!=0)
            {
                opInfo.affixation &= OperatorAffixation::postfix; // тут может быть только постфиксный оператор

                if ((opInfo.arity&OperatorArity::binary)!=0)
                {
                    opInfo.arity     &= OperatorArity::binary; // отсекаем все другие формы
                    opInfo.affixation = OperatorAffixation::none;
                    MARTY_EXPRESSIONS_CHECK_RETURN_IF_NOT_OK(checkOperatorRestrictions(opInfo));
                    m_ops.push_back(updateAssociativityAndPreceedence(opInfo));
                    // m_state = ParserState::waitUnaryPrefixOpOrValue;
                    m_stateHandler = &ExpressionParser::waitUnaryPrefixOperatorOrValue;
                    return Error::ok;
                }
                else if ((opInfo.arity&OperatorArity::ternary)!=0)
                {
                    return Error::notImplemented;
                }
                else if ((opInfo.arity&OperatorArity::unary)!=0)
                {
                    if (opInfo.affixation==0)
                        return Error::gotUnaryNonPostfixOp;

                    opInfo.arity      &= OperatorArity::unary;
                    MARTY_EXPRESSIONS_CHECK_RETURN_IF_NOT_OK(checkOperatorRestrictions(opInfo));
                    m_ops.push_back(updateAssociativityAndPreceedence(opInfo));
                    // Состояние не меняем
                    return Error::ok;
                }

                return Error::notImplemented;
            }

            return Error::notImplemented;
        }

        case ItemType::boolLiteral         : [[fallthrough]];
        case ItemType::integerLiteral      : [[fallthrough]];
        case ItemType::floatingPointLiteral: [[fallthrough]];
        case ItemType::stringLiteral       : [[fallthrough]];
        case ItemType::symbolLiteral       : [[fallthrough]];
        case ItemType::identifier          : [[fallthrough]];
        case ItemType::void_               : [[fallthrough]];
        case ItemType::expressionEntry     : [[fallthrough]];
        case ItemType::functionCall        : [[fallthrough]];
        case ItemType::functionalCast      : [[fallthrough]];
        case ItemType::cast                : [[fallthrough]];
        case ItemType::unknown             : [[fallthrough]];
        default                            : return Error::notImplemented;
    }

    // return Error::ok;
}
