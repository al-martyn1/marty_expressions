#pragma once

template< typename OperatorTraitsTypeT, typename PositionInfoTypeT, typename OperatorTokenTypeT
        , typename IntegerTypeT, typename FloatingPointTypeT, typename StringTypeT >
Error
ExpressionParser<OperatorTraitsTypeT, PositionInfoTypeT, OperatorTokenTypeT, IntegerTypeT, FloatingPointTypeT, StringTypeT>::
waitUnaryPrefixOperatorOrValue
    ( const ExpressionParser<OperatorTraitsTypeT, PositionInfoTypeT, OperatorTokenTypeT, IntegerTypeT, FloatingPointTypeT, StringTypeT>::ExpressionItemType &tk
    , ItemType itemType
    , ExpressionParser<OperatorTraitsTypeT, PositionInfoTypeT, OperatorTokenTypeT, IntegerTypeT, FloatingPointTypeT, StringTypeT>::OperationInfoType opInfo
    )
{
    MARTY_ARG_USED(tk);
    MARTY_ARG_USED(itemType);
    MARTY_ARG_USED(opInfo);

    // Ожидаем унарный префиксный оператор, литерал, или идентификатор

    switch(itemType)
    {
        case ItemType::operator_           :
        {
            if ((opInfo.opFeatures&OperatorFeatures::regular)!=0)
            {
                // У нас обычный оператор
                opInfo.affixation &= OperatorAffixation::prefix;
                opInfo.arity      &= OperatorArity::unary;

                if (opInfo.arity==0)
                    return Error::gotNonPrefixOp;

                if (opInfo.affixation==0)
                    return Error::gotNonUnaryOp;

                MARTY_EXPRESSIONS_CHECK_RETURN_IF_NOT_OK(checkOperatorRestrictions(opInfo));

                // opInfo.argIdx  = m_operands.size();
                // opInfo.argsNum = 1;
                m_ops.push_back(updateAssociativityAndPreceedence(opInfo));

                //TODO: !!! не очень понятно, как и где обрабатывать унарные префиксные операторы
                return Error::ok;
            }

            return Error::notImplemented;
        }


        case ItemType::boolLiteral         : [[fallthrough]];
        case ItemType::integerLiteral      : [[fallthrough]];
        case ItemType::floatingPointLiteral: [[fallthrough]];
        case ItemType::stringLiteral       : [[fallthrough]];
        case ItemType::symbolLiteral       :
        {
            MARTY_EXPRESSIONS_CHECK_RETURN_IF_NOT_OK(checkOperatorRestrictions(tk));
            m_operands.push_back(tk);
            // После литерала ждём оператор
            m_stateHandler = &ExpressionParser::waitOperator;
            return Error::ok;
        }

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
