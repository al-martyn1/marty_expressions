#pragma once

template< typename OperatorTraitsTypeT, typename PositionInfoTypeT, typename OperatorTokenTypeT
        , typename IntegerTypeT, typename FloatingPointTypeT, typename StringTypeT >
Error
ExpressionParser<OperatorTraitsTypeT, PositionInfoTypeT, OperatorTokenTypeT, IntegerTypeT, FloatingPointTypeT, StringTypeT>::
waitOperand
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
        case ItemType::operator_           : [[fallthrough]];
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

    //return Error::ok;
}
