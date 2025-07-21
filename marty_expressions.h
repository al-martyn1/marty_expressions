/*!
    \file
    \brief Библиотека marty::expressions
 */
#pragma once

// Старое тут - F:\tmp\2\omf51_1\omf51\src\


/*
  Парсер формул с помощью метода рекурсивного спуска - https://habr.com/ru/post/122397/
  Алгоритм парсинга арифметических выражений         - https://habr.com/ru/post/263775/
  Обратная польская запись                           - https://habr.com/ru/post/100869/
  Парсер выражений                                   - http://slovesnov.users.sourceforge.net/index.php?parser,russian
  Парсер математических выражений                    - https://ru.stackoverflow.com/questions/23842/%D0%9F%D0%B0%D1%80%D1%81%D0%B5%D1%80-%D0%BC%D0%B0%D1%82%D0%B5%D0%BC%D0%B0%D1%82%D0%B8%D1%87%D0%B5%D1%81%D0%BA%D0%B8%D1%85-%D0%B2%D1%8B%D1%80%D0%B0%D0%B6%D0%B5%D0%BD%D0%B8%D0%B9
  Обратная польская запись                           - https://ru.wikipedia.org/wiki/%D0%9E%D0%B1%D1%80%D0%B0%D1%82%D0%BD%D0%B0%D1%8F_%D0%BF%D0%BE%D0%BB%D1%8C%D1%81%D0%BA%D0%B0%D1%8F_%D0%B7%D0%B0%D0%BF%D0%B8%D1%81%D1%8C
 */


#define MARTY_EXPRESSIONS_CHECK_RETURN_IF_NOT_OK(expr)   \
            do                                           \
            {                                            \
                auto res = expr;                         \
                if (res!=Error::ok)                      \
                    return res;                          \
                                                         \
            } while(0)

//
#include "types.h"
//
#include "enum_descriptions.h"
//
#include "undef_min_max.h"


// #include "marty_expressions/marty_expressions.h"
// marty::expressions::
namespace marty {
namespace expressions {




template< typename OperatorTraitsTypeT
        , typename PositionInfoTypeT
        , typename OperatorTokenTypeT
        , typename IntegerTypeT=MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE
        , typename FloatingPointTypeT=MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE
        , typename StringTypeT=MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE
        >
struct ExpressionParser
{

public: // template parameter types

    using OperatorTraitsType          = OperatorTraitsTypeT;
    using operator_traits_type        = OperatorTraitsType ;

    using PositionInfoType            = PositionInfoTypeT;
    using position_info_type          = PositionInfoType ;

    using OperatorTokenType           = OperatorTokenTypeT;
    using operator_token_type         = OperatorTokenType ;

    using IntegerType                 = IntegerTypeT;
    using integer_type                = IntegerType ;

    using FloatingPointType           = FloatingPointTypeT;
    using floating_point_type         = FloatingPointType ;

    using StringType                  = StringTypeT;
    using string_type                 = StringType ;


public: // depended types

    using BoolLiteralType             = BoolLiteral<PositionInfoType>;
    using bool_literal_type           = BoolLiteralType;

    using IntegerLiteralType          = IntegerLiteral<PositionInfoType, IntegerType, StringType>;
    using integer_literal_type        = IntegerLiteralType;

    using FloatingPointLiteralType    = FloatingPointLiteral<PositionInfoType, IntegerType, StringType>;
    using floating_point_literal_type = FloatingPointLiteralType;

    using StringLiteralType           = StringLiteral<PositionInfoType, StringType>;
    using string_literal_type         = StringLiteralType;

    using SymbolLiteralType           = SymbolLiteral<PositionInfoType, StringType>;
    using symbol_literal_type         = SymbolLiteralType;

    using IdentifierLiteralType       = IdentifierLiteral<PositionInfoType, StringType>;
    using identifier_literal_type     = IdentifierLiteralType;

    using OperatorType                = Operator<PositionInfoType, OperatorTokenType, StringType>;
    using operator_type               = OperatorType;

    using FunctionCallType            = FunctionCall<PositionInfoType, StringType>;
    using function_call_type          = FunctionCallType;

    using FunctionalCastType          = FunctionalCast<PositionInfoType, StringType>;
    using functional_cast_type        = FunctionalCastType;

    using CastType                    = Cast<PositionInfoType, StringType>;
    using cast_type                   = CastType;



// template<typename PositionInfoType, typename StringType=MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE>
// struct VoidValue
// {
//     PositionInfoType     positionInfo;
//     StringType           value; // for uniformity
//  
// }; // VoidValue


    using ExpressionInputItemType     = ExpressionInputItem<PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType>;
    using expression_input_item_type  = ExpressionInputItemType;

    using ExpressionItemType          = ExpressionItem<PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType>;
    using expression_item_type        = ExpressionItemType;

    using ExpressionNodeType          = ExpressionNode<PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType>;
    using expression_node_type        = ExpressionNodeType;

    using OperationInfoType           = OperationInfo<PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType>;
    using operation_info_type         = OperationInfoType;


public: // member types

    typedef Error (ExpressionParser::*ParserStateHandlerType)
        ( const ExpressionItemType &tk
        , ItemType itemType
        , OperationInfoType opInfo // Только для itemType == ItemType::operator_, частично заполнено, нужно доустановить operatorPair, associativity, precedence
        );


protected: // members

    const OperatorTraitsType            m_opTraits  ;
    const OperatorFeatures              m_opFeatures  ; // flags of allowed OperatorType

    // ParserState                         m_state;
    ParserStateHandlerType              m_stateHandler = 0;
    ExpressionNodeType                  m_expression;
    std::vector<OperationInfoType>      m_ops       ;
    std::vector<ExpressionItemType>     m_operands  ;


protected: // helpers

    static
    ExpressionItemType expressionItemFromInputItem(const ExpressionInputItemType &i)
    {
        return std::visit( [](const auto ai) { return ExpressionItemType(ai); }, i);
    }

    TokenContext findContext(TokenContext lookFor) const
    {
        if (lookFor==TokenContext::functionCall)
        {
            // Надо будет обдумать
            for(std::size_t i=m_ops.size(); i-->0; )
            {
                const auto &op = m_ops[i];
                auto itemType = getExpressionItemType(op.opInfo);

                if (itemType==ItemType::expressionEntry)
                    return TokenContext::normal;

                if (itemType==ItemType::functionCall)
                    return TokenContext::functionCall;
            }
        }
     
        return TokenContext::normal;
    }

    OperationInfoType& updateAssociativityAndPreceedence(OperationInfoType &opi) const;

    //! Проверяется пришедший оператор. Исследуется то, что у нас лежит в стеке аргументов
    Error checkOperatorRestrictions(const OperationInfoType &opi) const;

    //! Проверяются ограничения для оператора на вершине стека по приходу чего-то, что не является оператором
    Error checkOperatorRestrictions(const ExpressionItemType &ei) const;


public: // ctors

    ExpressionParser(const OperatorTraitsType &opTraits, OperatorFeatures features=OperatorFeatures::regular)
    : m_opTraits(opTraits)
    , m_opFeatures(features | OperatorFeatures::regular) // regular operators are always supported
    , m_stateHandler(&ExpressionParser::waitUnaryPrefixOperatorOrValue)
    {
        m_expression.nodeValue = IntegerLiteralType{PositionInfoType{}, IntegerType{0}};
    }

    ExpressionParser(const ExpressionParser &) = default;
    ExpressionParser(ExpressionParser &&) = default;


public: // assign

    ExpressionParser& operator=(const ExpressionParser &) = default;
    ExpressionParser& operator=(ExpressionParser &&) = default;


public: // methods

    const ExpressionNodeType& getExpression() const
    {
        return m_expression;
    }

    ParserState getParserState() const 
    {
        if (m_stateHandler==&ExpressionParser::waitUnaryPrefixOperatorOrValue) return ParserState::waitUnaryPrefixOpOrValue;
        if (m_stateHandler==&ExpressionParser::waitOperator)                   return ParserState::waitOperator;
        if (m_stateHandler==&ExpressionParser::waitOperatorOrFunctionCall)     return ParserState::waitOperatorOrFunctionCall;
        if (m_stateHandler==&ExpressionParser::waitOperand)                    return ParserState::waitOperand;
        // if (m_stateHandler==&ExpressionParser::) return ;
        // if (m_stateHandler==&ExpressionParser::) return ;
        // if (m_stateHandler==&ExpressionParser::) return ;
        return ParserState::unknown;
    }


    template<typename StreamType>
    void dump(StreamType &oss) const;

    std::string getErrorMessage(Error err, bool addErrCode=true) const
    {
        std::string resStr;
        if (addErrCode)
            resStr = enum_serialize(err) + ": ";
        resStr += enum_get_description(err);

        if (err==Error::gotNonUnaryOrNonPrefixOp || err==Error::gotNonUnaryOp || err==Error::gotNonPrefixOp)
        {
            resStr += ", but expected " + enum_get_description(getParserState());
        }

        return resStr;
    }

    Error initialize();

    Error finalize();

    Error parse(const ExpressionInputItemType &tk_)
    {
        if (m_stateHandler==0)
            return Error::unknownParserState;

        auto tk = expressionItemFromInputItem(tk_);

        auto itemType = getExpressionItemType(tk);

        OperationInfoType opInfo;

        if (itemType==ItemType::operator_)
        {
            const auto &opRef = std::get<OperatorType>(tk);
            auto opToken = opRef.value;

            opInfo.opInfo        = opRef;
            // regular/groupping/simpleCast/functionalCast/functionCall/indexation/templateInstantiation
            opInfo.opFeatures    = m_opTraits.getOperatorFeatures(opToken) & m_opFeatures; // Оставляем только те фичи, которые указали как поддерживаемые
            opInfo.bracketKind   = m_opTraits.getBracketKind(opToken);
            opInfo.affixation    = m_opTraits.getOperatorAffixation(opToken); // Возможные варианты: prefix, postfix, prefix|postfix
            opInfo.arity         = m_opTraits.getOperatorArity(opToken); // Может быть установлено unary|binary, остальные флаги не комбинируются
            opInfo.operatorPair  = m_opTraits.getNulToken();
        
            // Ассоциативность и приоритет зависят от аффиксации (prefix/postfix) и арности, поэтому надо сначала выяснить их
            // opInfo.associativity    = m_opTraits.getOperatorAssociativity(opToken, opInfo.affixation, opInfo.arity)
            // opInfo.precedence       = int(m_opTraits.getOperatorPrecedence(opToken, opInfo.affixation, opInfo.arity));
    
            // Пара может быть у скобки или у тернарного оператора
            // OperatorTokenType getTernarySecond(OperatorTokenType tokenType) const
            // OperatorTokenType getBracketPair(OperatorTokenType tokenType) const
            // opInfo.operatorPair     = m_opTraits.
        }

        return (this->*m_stateHandler)(tk, itemType, opInfo);
    }


protected: // state handlers

    Error waitUnaryPrefixOperatorOrValue( const ExpressionItemType &tk, ItemType itemType, OperationInfoType opInfo );
    Error waitOperator( const ExpressionItemType &tk, ItemType itemType, OperationInfoType opInfo );
    Error waitOperatorOrFunctionCall( const ExpressionItemType &tk, ItemType itemType, OperationInfoType opInfo );
    Error waitOperand( const ExpressionItemType &tk, ItemType itemType, OperationInfoType opInfo );
    // Error ( const ExpressionItemType &tk, ItemType itemType, OperationInfoType opInfo );
    // Error ( const ExpressionItemType &tk, ItemType itemType, OperationInfoType opInfo );
    // Error ( const ExpressionItemType &tk, ItemType itemType, OperationInfoType opInfo );
    // Error ( const ExpressionItemType &tk, ItemType itemType, OperationInfoType opInfo );
    // Error ( const ExpressionItemType &tk, ItemType itemType, OperationInfoType opInfo );


// waitUnaryPrefixOpOrValue,initial                = 0 // unary prefix operator, variable, function or literal
// waitOperator                                        // unary postfix operator, binary, ternary or n-Ary operator
// waitOperatorOrFunctionCall                          // binary, ternary or n-Ary operator or function call
// waitOperand                                         // operand (literal or variable)



}; // struct ExpressionParser

    // using BoolLiteralType             = BoolLiteral<PositionInfoType>;
    // using bool_literal_type           = BoolLiteralType;
    //  
    // using IntegerLiteralType          = IntegerLiteral<PositionInfoType, IntegerType, StringType>;
    // using integer_literal_type        = IntegerLiteralType;
    //  
    // using FloatingPointLiteralType    = FloatingPointLiteral<PositionInfoType, IntegerType, StringType>;
    // using floating_point_literal_type = FloatingPointLiteralType;
    //  
    // using StringLiteralType           = StringLiteral<PositionInfoType, StringType>;
    // using string_literal_type         = StringLiteralType;
    //  
    // using SymbolLiteralType           = SymbolLiteral<PositionInfoType, StringType>;
    // using symbol_literal_type         = SymbolLiteralType;
    //  
    // using IdentifierLiteralType       = IdentifierLiteral<PositionInfoType, StringType>;
    // using identifier_literal_type     = IdentifierLiteralType;
    //  
    // using OperatorType                = Operator<PositionInfoType, OperatorTokenType, StringType>;
    // using operator_type               = OperatorType;
    //  
    // using FunctionCallType            = FunctionCall<PositionInfoType, StringType>;
    // using function_call_type          = FunctionCallType;
    //  
    // using FunctionalCastType          = FunctionalCast<PositionInfoType, StringType>;
    // using functional_cast_type        = FunctionalCastType;
    //  
    // using CastType                    = Cast<PositionInfoType, StringType>;
    // using cast_type                   = CastType;


// #include "impl/expression_parser_parse.h"
#include "impl/ep_dump.h"

#include "impl/ep_common.h"

#include "impl/ep_waitUnaryPrefixOperatorOrValue.h"
#include "impl/ep_waitOperator.h"
#include "impl/ep_waitOperatorOrFunctionCall.h"
#include "impl/ep_waitOperand.h"
// ep_.h
// ep_.h



} // namespace expressions
} // namespace marty

// marty::expressions::
// #include "marty_expressions/marty_expressions.h"

