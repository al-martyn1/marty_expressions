#pragma once


template< typename OperatorTraitsTypeT
        , typename PositionInfoTypeT
        , typename OperatorTokenTypeT
        , typename IntegerTypeT
        , typename FloatingPointTypeT
        , typename StringTypeT
        >
template<typename StreamType>
void ExpressionParser<OperatorTraitsTypeT, PositionInfoTypeT, OperatorTokenTypeT, IntegerTypeT, FloatingPointTypeT, StringTypeT>::dump(StreamType &oss) const
{
    oss << "State: " << enum_serialize(getParserState()) << "\n\n";

    oss << "Operators:";
    if (m_ops.empty())
    {
        oss << " <empty>\n";
    }
    else
    {
        oss << "\n";
        for(std::size_t i=0u; i!=m_ops.size(); ++i )
        {
            const auto &op = m_ops[i];
            auto itemType = getExpressionItemType(op.opInfo);
            oss << "    " << enum_serialize(itemType);
            if (itemType==ItemType::operator_)
            {
                std::visit( [&](const auto &a)
                            {
                                using ArgType = std::decay_t<decltype(a)>;
                                if constexpr (std::is_same_v <ArgType, OperatorType>)
                                {
                                    oss << ", '" << a.text << "'" 
                                        << ", affixation: " << enum_serialize_flags(op.affixation)
                                        << ", arity: " << enum_serialize_flags(op.arity)
                                        << ", associativity: " << enum_serialize_flags(op.associativity)
                                        << ", precedence: " << op.precedence
                                        ;
                                }
                            }
                          , op.opInfo
                          );
            }
            oss << "\n";
        }
    }
    oss << "\n";

    oss << "Arguments:";
    if (m_ops.empty())
    {
        oss << " <empty>\n";
    }
    else
    {
        oss << "\n";
        for(std::size_t i=0u; i!=m_operands.size(); ++i )
        {
            const auto &op = m_operands[i];
            auto itemType = getExpressionItemType(op);
            oss << "    " << enum_serialize(itemType);
            std::visit( [&](const auto &a)
                      {
                          using ArgType = std::decay_t<decltype(a)>;
                          //if constexpr (std::is_same_v <ArgType, Operator<PositionInfoType, OperatorTokenType, StringType> >)
                          if constexpr (std::is_same_v <ArgType, BoolLiteralType>)
                          {
                              oss << ", " << (a.value ? "true" : "false");
                          }
                          else if constexpr (std::is_same_v <ArgType, IntegerLiteralType>)
                          {
                              oss << ", " << to_string(a.value) << a.suffix ;
                          }
                          else if constexpr (std::is_same_v <ArgType, FloatingPointLiteralType>)
                          {
                              oss << ", " << to_string(a.value) << a.suffix ;
                          }
                          else if constexpr (std::is_same_v <ArgType, StringLiteralType>)
                          {
                              oss << ", \"" << a.value << "\"" << a.suffix ;
                          }
                          else if constexpr (std::is_same_v <ArgType, SymbolLiteralType>)
                          {
                              oss << ", \'" << a.value << "\'" << a.suffix ;
                          }
                      }
                      , op
                      );

            oss << "\n";
        }
    }
    oss << "\n";

}
