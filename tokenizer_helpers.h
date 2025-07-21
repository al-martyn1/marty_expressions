/*!
    \file
    \brief Хелперы для обработки событий токенизера
 */
#pragma once

#include "types.h"

//
#include "umba/tokenizer.h"
//
#include "umba/tokenizer/utils.h"


//
#include "undef_min_max.h"


//----------------------------------------------------------------------------
// #include "marty_expressions/marty_expressions.h"
// marty::expressions::tokenizer_helpers::
namespace marty {
namespace expressions {
namespace tokenizer_helpers {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template< typename TokenizerType, typename InputIteratorType, typename TokenParsedDataType, typename PositionInfoGeneratorType

        , typename PositionInfoType, typename OperatorTokenType
        , typename IntegerType       = MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE
        , typename FloatingPointType = MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE
        , typename StringType        = MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE
        >
bool
checkConvertIdentifier( ExpressionInputItem<PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType> &resItem
                      , OperatorTokenType tokenType
                      , InputIteratorType b, InputIteratorType e
                      , const TokenParsedDataType &parsedData
                      , PositionInfoGeneratorType positionInfoGenerator
                      )
{
    if (!( tokenType>=UMBA_TOKENIZER_TOKEN_IDENTIFIER_FIRST && tokenType<=UMBA_TOKENIZER_TOKEN_IDENTIFIER_LAST
        || tokenType>=UMBA_TOKENIZER_TOKEN_KEYWORDS_FIRST   && tokenType<=UMBA_TOKENIZER_TOKEN_KEYWORDS_LAST
       ))
       return false;

    bool bRes = false;

    std::visit( [&](auto && val)
                {
                    if constexpr (std::is_same_v< std::decay_t<decltype(val)>, typename TokenizerType::IdentifierDataHolder>)
                    {
                        auto strVal       = val.pData->value;
                        using StringType  = std::decay_t<decltype(strVal)>;
                        
                        auto posInfo = positionInfoGenerator(b,e);

                        // !!! Тут надо переделать
                        if (strVal=="true" || strVal=="false" || strVal=="TRUE" || strVal=="FALSE" || strVal=="True" || strVal=="False")
                        {
                            bool bTrue = strVal=="true" || strVal=="TRUE" || strVal=="True";
                            resItem = marty::expressions::BoolLiteral<PositionInfoType>{posInfo, bTrue};
                        }
                        else
                        {
                            resItem = marty::expressions::IdentifierLiteral<PositionInfoType,StringType>{posInfo, strVal};
                        }

                        bRes = true;
                    }
                }
                , parsedData
                );

    return bRes;
}

//----------------------------------------------------------------------------
template< typename TokenizerType, typename InputIteratorType, typename TokenParsedDataType, typename PositionInfoGeneratorType

        , typename PositionInfoType, typename OperatorTokenType
        , typename IntegerType       = MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE
        , typename FloatingPointType = MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE
        , typename StringType        = MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE
        >
bool
checkConvertBoolLiteral( ExpressionInputItem<PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType> &resItem
                       , OperatorTokenType tokenType
                       , InputIteratorType b, InputIteratorType e
                       , const TokenParsedDataType &parsedData
                       , PositionInfoGeneratorType positionInfoGenerator
                       )
{
    MARTY_ARG_USED(parsedData);

    if (!(tokenType>=UMBA_TOKENIZER_TOKEN_BOOL_LITERAL_FIRST && tokenType<=UMBA_TOKENIZER_TOKEN_BOOL_LITERAL_LAST))
       return false;

    //bool bRes = false;
    auto posInfo = positionInfoGenerator(b,e);
    resItem = marty::expressions::BoolLiteral<PositionInfoType>{posInfo, tokenType==UMBA_TOKENIZER_TOKEN_BOOL_LITERAL_TRUE};
    return true;
}

//----------------------------------------------------------------------------
template< typename TokenizerType, typename InputIteratorType, typename TokenParsedDataType, typename PositionInfoGeneratorType

        , typename PositionInfoType, typename OperatorTokenType
        , typename IntegerType       = MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE
        , typename FloatingPointType = MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE
        , typename StringType        = MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE
        >
bool
checkConvertIntegerLiteral( ExpressionInputItem<PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType> &resItem
                          , OperatorTokenType tokenType
                          , InputIteratorType b, InputIteratorType e
                          , const TokenParsedDataType &parsedData
                          , PositionInfoGeneratorType positionInfoGenerator
                          )
{
    if (!(tokenType>=UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_FIRST && tokenType<=UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_LAST))
        return false;

    bool bRes = false;

    std::visit( [&](auto && val)
                {
                    auto posInfo = positionInfoGenerator(b,e);
                    
                    if constexpr (std::is_same_v< std::decay_t<decltype(val)>, typename TokenizerType::IntegerNumericLiteralDataHolder>)
                    {
                        StringType suffix;
                        if (val.pData->hasSuffix)
                            suffix = umba::tokenizer::utils::makeTokenText(tokenType, val.pData->suffixStartPos, e);
    
                        resItem = marty::expressions::IntegerLiteral<PositionInfoType,IntegerType,StringType>{posInfo, val.pData->value, suffix};
                        bRes = true;
                    }
                }
                , parsedData
                );

    return bRes;
}    
    
//----------------------------------------------------------------------------
template< typename TokenizerType, typename InputIteratorType, typename TokenParsedDataType, typename PositionInfoGeneratorType

        , typename PositionInfoType, typename OperatorTokenType
        , typename IntegerType       = MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE
        , typename FloatingPointType = MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE
        , typename StringType        = MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE
        >
bool
checkConvertFloatLiteral( ExpressionInputItem<PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType> &resItem
                        , OperatorTokenType tokenType
                        , InputIteratorType b, InputIteratorType e
                        , const TokenParsedDataType &parsedData
                        , PositionInfoGeneratorType positionInfoGenerator
                        )
{
    if (!(tokenType>=UMBA_TOKENIZER_TOKEN_FLOAT_NUMBER_FIRST && tokenType<=UMBA_TOKENIZER_TOKEN_FLOAT_NUMBER_LAST))
        return false;

    bool bRes = false;

    std::visit( [&](auto && val)
                {
                    auto posInfo = positionInfoGenerator(b,e);
                    
                    if constexpr (std::is_same_v< std::decay_t<decltype(val)>, typename TokenizerType::FloatNumericLiteralDataHolder>)
                    {
                        StringType suffix;
                        if (val.pData->hasSuffix)
                            suffix = umba::tokenizer::utils::makeTokenText(tokenType, val.pData->suffixStartPos, e);
    
                        resItem = marty::expressions::FloatingPointLiteral<PositionInfoType,FloatingPointType,StringType>{posInfo, val.pData->value, suffix};
                        bRes = true;
                    }
                }
                , parsedData
                );

    return bRes;
}    
    
//----------------------------------------------------------------------------
template< typename TokenizerType, typename InputIteratorType, typename TokenParsedDataType, typename PositionInfoGeneratorType

        , typename PositionInfoType, typename OperatorTokenType
        , typename IntegerType       = MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE
        , typename FloatingPointType = MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE
        , typename StringType        = MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE
        >
bool
checkConvertStringLiteral( ExpressionInputItem<PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType> &resItem
                         , OperatorTokenType tokenType
                         , InputIteratorType b, InputIteratorType e
                         , const TokenParsedDataType &parsedData
                         , PositionInfoGeneratorType positionInfoGenerator
                         )
{
    if (!(tokenType==UMBA_TOKENIZER_TOKEN_STRING_LITERAL && tokenType==UMBA_TOKENIZER_TOKEN_CHAR_LITERAL))
        return false;

    bool bRes = false;

    std::visit( [&](auto && val)
                {
                    if constexpr (std::is_same_v< std::decay_t<decltype(val)>, typename TokenizerType::StringLiteralDataHolder>)
                    {
                        auto posInfo = positionInfoGenerator(b,e);
                        //using StringType       = std::decay_t<decltype(val.pData->value)>;
                        //using SuffixType       = std::decay_t<decltype( umba::tokenizer::utils::makeTokenText(tokenType, val.pData->suffixStartPos, e) )>;
                        StringType suffix;
                        if (val.pData->hasSuffix)
                            suffix = umba::tokenizer::utils::makeTokenText(tokenType, val.pData->suffixStartPos, e);

                        if (tokenType==UMBA_TOKENIZER_TOKEN_STRING_LITERAL)
                            resItem = marty::expressions::StringLiteral<PositionInfoType,StringType>{posInfo, val.pData->value, suffix};
                        else
                            resItem = marty::expressions::SymbolLiteral<PositionInfoType,StringType>{posInfo, val.pData->value, suffix};

                        bRes = true;
                    }
                }
                , parsedData
                );

    return bRes;
}    
    
//----------------------------------------------------------------------------
template< typename TokenizerType, typename InputIteratorType, typename TokenParsedDataType, typename PositionInfoGeneratorType

        , typename PositionInfoType, typename OperatorTokenType
        , typename IntegerType       = MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE
        , typename FloatingPointType = MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE
        , typename StringType        = MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE
        >
bool
checkConvertOperatorToken( ExpressionInputItem<PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType> &resItem
                         , OperatorTokenType tokenType
                         , InputIteratorType b, InputIteratorType e
                         , const TokenParsedDataType &parsedData
                         , PositionInfoGeneratorType positionInfoGenerator
                         )
{
    if (!( tokenType>=UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST         && tokenType<=UMBA_TOKENIZER_TOKEN_OPERATOR_LAST
        || tokenType==UMBA_TOKENIZER_TOKEN_CURLY_BRACKET_OPEN     || tokenType==UMBA_TOKENIZER_TOKEN_CURLY_BRACKET_CLOSE     
        || tokenType==UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_OPEN     || tokenType==UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_CLOSE     
        || tokenType==UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_OPEN     || tokenType==UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_CLOSE     
        || tokenType==UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_OPEN    || tokenType==UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_CLOSE    
        || tokenType==UMBA_TOKENIZER_TOKEN_DBLSQUARE_BRACKET_OPEN || tokenType==UMBA_TOKENIZER_TOKEN_DBLSQUARE_BRACKET_CLOSE 
       ))
       return false;
    
    StringType operatorText;

    bool bRes = false;

    std::visit( [&](auto && val)
                {
                    auto posInfo = positionInfoGenerator(b,e);
                    if constexpr (std::is_same_v< std::decay_t<decltype(val)>, typename TokenizerType::StringLiteralDataHolder>)
                        operatorText = val.pData->value;
                    else
                        operatorText = umba::tokenizer::utils::makeTokenText(tokenType, b, e);

                    resItem = marty::expressions::Operator<PositionInfoType,OperatorTokenType,StringType>{posInfo, tokenType, operatorText};
                    bRes = true;
                }
                , parsedData
                );

    return bRes;
}    

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
template< typename TokenizerType, typename InputIteratorType, typename TokenParsedDataType, typename PositionInfoGeneratorType

        , typename PositionInfoType, typename OperatorTokenType
        , typename IntegerType       = MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE
        , typename FloatingPointType = MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE
        , typename StringType        = MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE
        >
bool
convertTokenizerEvent( std::vector<ExpressionInputItem<PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType> > &resVec
                     , OperatorTokenType tokenType
                     , InputIteratorType b, InputIteratorType e
                     , const TokenParsedDataType &parsedData
                     , PositionInfoGeneratorType positionInfoGenerator
                     )
{
    // TokenizerType, InputIteratorType, TokenParsedDataType, PositionInfoGeneratorType, PositionInfoType, IntegerType, FloatingPointType, StringType

    ExpressionInputItem<PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType> exprInputItem;

    if (tokenizer_helpers::checkConvertIdentifier<TokenizerType, InputIteratorType, TokenParsedDataType, PositionInfoGeneratorType, PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType>(exprInputItem, tokenType, b, e, parsedData, positionInfoGenerator))
    {
        resVec.emplace_back(exprInputItem);
        return true;
    }

    if (tokenizer_helpers::checkConvertBoolLiteral<TokenizerType, InputIteratorType, TokenParsedDataType, PositionInfoGeneratorType, PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType>(exprInputItem, tokenType, b, e, parsedData, positionInfoGenerator))
    {
        resVec.emplace_back(exprInputItem);
        return true;
    }

    if (tokenizer_helpers::checkConvertIntegerLiteral<TokenizerType, InputIteratorType, TokenParsedDataType, PositionInfoGeneratorType, PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType>(exprInputItem, tokenType, b, e, parsedData, positionInfoGenerator))
    {
        resVec.emplace_back(exprInputItem);
        return true;
    }

    if (tokenizer_helpers::checkConvertFloatLiteral<TokenizerType, InputIteratorType, TokenParsedDataType, PositionInfoGeneratorType, PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType>(exprInputItem, tokenType, b, e, parsedData, positionInfoGenerator))
    {
        resVec.emplace_back(exprInputItem);
        return true;
    }

    if (tokenizer_helpers::checkConvertStringLiteral<TokenizerType, InputIteratorType, TokenParsedDataType, PositionInfoGeneratorType, PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType>(exprInputItem, tokenType, b, e, parsedData, positionInfoGenerator))
    {
        resVec.emplace_back(exprInputItem);
        return true;
    }

    if (tokenizer_helpers::checkConvertOperatorToken<TokenizerType, InputIteratorType, TokenParsedDataType, PositionInfoGeneratorType, PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType>(exprInputItem, tokenType, b, e, parsedData, positionInfoGenerator))
    {
        resVec.emplace_back(exprInputItem);
        return true;
    }
    
    return true;

}    

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template< typename PositionInfoType
        , typename OperatorTokenType
        , typename IntegerType       = MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE
        , typename FloatingPointType = MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE
        , typename StringType        = MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE
        >
std::vector<ExpressionInputItem<PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType> >
mergeFullQualifiedIdents( const std::vector<ExpressionInputItem<PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType> > &vec
                        , OperatorTokenType  nsSepOperator
                        , const StringType   &nsSepStr
                        )
{
    using ExpressionInputItemType = ExpressionInputItem<PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType>;
    std::vector<ExpressionInputItem<PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType> > resVec;
    resVec.reserve(vec.size());

    enum State
    {
        stNormal,
        stWaitNsSep,
        stWaitIdent
    };

    State st = stNormal;

    for(const auto &i : vec)
    {
        // https://en.cppreference.com/w/cpp/utility/variant/holds_alternative.html

        if (std::holds_alternative<IdentifierLiteral<PositionInfoType,StringType>>(i))
        {
            if (st==stNormal)
            {
                resVec.emplace_back(i);  // кладём идентификатор
                st = stWaitNsSep;        // начинаем ждать NS разделитель
            }
            else if (st==stWaitNsSep)
            {
                // У нас идентификатор уже положен, ждем разделитель NS или что-то другое
                // Но пришел снова идентификатор
                resVec.emplace_back(i);  // кладём идентификатор
                // ждём NS разделитель для нового идентификатора
            }
            else if (st==stWaitIdent)
            {
                // Ждём идентификатор, чтобы дополнить имя
                // Вектор тут точно не пуст
                // У нас там лежит идентификатор с NS разделителем в конце
                auto &identRef = std::get<IdentifierLiteral<PositionInfoType,StringType>>(resVec.back());
                identRef.value.append(std::get<IdentifierLiteral<PositionInfoType,StringType>>(i).value); // добавляем идентификатор
                st = stWaitNsSep;        // начинаем ждать NS разделитель
            }
            else
            {
                // Хз что произошло
                st = stNormal;
            }
        }
        else if (std::holds_alternative<Operator<PositionInfoType,OperatorTokenType,StringType>>(i))
        {
            if (st==stNormal)
            {
                const auto &opRef = std::get<Operator<PositionInfoType,OperatorTokenType,StringType>>(i);
                if (opRef.value==nsSepOperator)
                {
                    // Создаём идентификатор, вместо текста - текст замены NS-разделителя, заданный параметром функции
                    resVec.emplace_back(ExpressionInputItemType{IdentifierLiteral<PositionInfoType,StringType>{opRef.positionInfo, nsSepStr}});
                    st = stWaitIdent; // Теперь ждём идентификатор
                }
                else
                {
                    resVec.emplace_back(i); // Какой-то другой оператор, просто кладём его
                    st = stNormal;
                }
            }
            else if (st==stWaitNsSep)
            {
                const auto &opRef = std::get<Operator<PositionInfoType,OperatorTokenType,StringType>>(i);
                if (opRef.value==nsSepOperator)
                {
                    std::get<IdentifierLiteral<PositionInfoType,StringType>>(resVec.back()).value.append(nsSepStr); // добавляем разделитель
                    st = stWaitIdent; // Теперь ждём идентификатор
                }
                else
                {
                    resVec.emplace_back(i); // Какой-то другой оператор, просто кладём его
                    st = stNormal;
                }
            }
            else if (st==stWaitIdent)
            {
                const auto &opRef = std::get<Operator<PositionInfoType,OperatorTokenType,StringType>>(i);
                if (opRef.value==nsSepOperator)
                {
                    resVec.emplace_back(ExpressionInputItemType{IdentifierLiteral<PositionInfoType,StringType>{opRef.positionInfo, nsSepStr}}); // заменяем текст разделителя NS
                    st = stWaitIdent; // Теперь ждём идентификатор
                }
                else
                {
                    resVec.emplace_back(i); // Какой-то другой оператор, просто кладём его
                    st = stNormal;
                }
            }
            else
            {
            }
        }
        else
        {
            resVec.emplace_back(i);

            // if (st==stNormal)
            // {
            // }
            // else if (st==stWaitNsSep)
            // {
            //  
            // }
            // else if (st==stWaitIdent)
            // {
            // }
            // else
            // {
            // }
        }
    }

// template<typename PositionInfoType, typename StringType=MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE>
// struct IdentifierLiteral
// {
//     PositionInfoType     positionInfo;
//     StringType           value;
//  
// }; // IdentifierLiteral
//  
//  
// template<typename PositionInfoType, typename OperatorTokenType, typename StringType=MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE>
// struct OperatorToken
// {
//     PositionInfoType     positionInfo;
//     OperatorTokenType    value;
//     StringType           text;
//  
// }; // OperatorToken


    return resVec;
}



//----------------------------------------------------------------------------

} // namespace tokenizer_helpers
} // namespace expressions
} // namespace marty

// marty::expressions::tokenizer_helpers::
// #include "marty_expressions/marty_expressions.h"

