/*!
    \file
    \brief Библиотека marty::expressions - простой разбор булевых выражений. Допустимы операторы '|', '&' и '!', а также группирующие скобки
 */
#pragma once

//
#include "types.h"
//
#include "enum_descriptions.h"
//
#include "utils.h"
#include "truth_table.h"
//
#include "undef_min_max.h"
//
#include "truth_table.h"

//
#include <stdexcept>
#include <set>


// marty::expressions::
namespace marty {
namespace expressions {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
enum class SimpleBoolExpressionItemKind
{
    unknown,
    opOpen,
    opClose,
    opNot,
    opAnd,
    // opXor,
    opOr,
    tokenIdent,
    tokenFalse,
    tokenTrue
};

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename OperatorTokenTypeT>
struct SimpleBoolExpressionOperatorTraits
{
    OperatorTokenTypeT   openBracketOp;
    OperatorTokenTypeT   closeBracketOp;
    OperatorTokenTypeT   notOp;
    OperatorTokenTypeT   andOp;
    // OperatorTokenTypeT   xorOp;
    OperatorTokenTypeT   orOp ;
};

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template< typename OperatorTraitsTypeT // = SimpleBoolExpressionOperatorTraits<OperatorTokenTypeT>
        , typename PositionInfoTypeT
        , typename OperatorTokenTypeT
        , typename IntegerTypeT
        , typename FloatingPointTypeT
        , typename StringTypeT
        >
struct SimpleBoolExpressionEvaluator;

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template< typename OperatorTraitsTypeT // = SimpleBoolExpressionOperatorTraits<OperatorTokenTypeT>
        , typename PositionInfoTypeT
        , typename OperatorTokenTypeT
        , typename IntegerTypeT=MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE
        , typename FloatingPointTypeT=MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE
        , typename StringTypeT=MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE
        >
struct SimpleBoolExpressionParser
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


    using Kind = SimpleBoolExpressionItemKind;


public: // member types

    // typedef Error (ExpressionParser::*ParserStateHandlerType)
    //     ( const ExpressionItemType &tk
    //     , ItemType itemType
    //     , OperationInfoType opInfo // Только для itemType == ItemType::operator_, частично заполнено, нужно доустановить operatorPair, associativity, precedence
    //     );


protected: // members

    enum class State
    {
        readVar,
        readNextVar,
        waitOperator,
        err
    };


    const OperatorTraitsType  m_opTraits  ;

    State      m_state        = State::readVar;
    //int        m_nestingLevel = 0;
    Error      m_error        = Error::noError; // gotUnexpectedClosingBacket

    std::vector<OperatorType>          m_opStack  ;
    std::vector<ExpressionNodeType>    m_varStack ;
    std::vector<std::size_t >          m_varCounts; // Счетчики добавленных переменных на текущем уровне вложенности скобок

    
    ExpressionNodeType  m_expression; // Сюда кладётся результат после финализации


protected: // helpers

    static
    ExpressionItemType expressionItemFromInputItem(const ExpressionInputItemType &i)
    {
        return std::visit( [](const auto ai) { return ExpressionItemType(ai); }, i);
    }

    Kind getOperatorKind(const OperatorType &op) const
    {
        return op.value==m_opTraits.openBracketOp
             ? Kind::opOpen
    
             : op.value==m_opTraits.closeBracketOp
             ? Kind::opClose
    
             : op.value==m_opTraits.notOp
             ? Kind::opNot
    
             : op.value==m_opTraits.andOp
             ? Kind::opAnd
    
             // : op.value==m_opTraits.xorOp
             // ? Kind::opXor
    
             : op.value==m_opTraits.orOp
             ? Kind::opOr
    
             // : op.value==m_opTraits.
             // ? Kind::tokenIdent
             //  
             // : op.value==m_opTraits.
             // ? Kind::tokenFalse
             //  
             // : op.value==m_opTraits.
             // ? Kind::tokenTrue
    
             : Kind::unknown;
    }

    Kind getExpressionItemKind(const ExpressionItemType &v) const
    {
        return std::visit( [&](const auto &a) -> Kind
                           {
                               using ArgType = std::decay_t<decltype(a)>;

                               if constexpr (std::is_same_v <ArgType, OperatorType >)
                                   return getOperatorKind(a);

                               else if constexpr (std::is_same_v <ArgType, BoolLiteralType >)
                                   return a.value ? Kind::tokenTrue : Kind::tokenFalse;

                               // Целочисленные типы допустимы в логических выражениях, при этом 0 - обозначает false, а не ноль (обычно 1) - обозначает true
                               // Такие замены false/true на 0/1 часто используются в математической логике, дискретной математике, теории автоматов, и подобных дисциплинах.
                               else if constexpr (std::is_same_v <ArgType, IntegerLiteralType >) 
                                   return a.value ? Kind::tokenTrue : Kind::tokenFalse;

                               // if constexpr (std::is_same_v <ArgType, FloatingPointLiteral<PositionInfoType, FloatingPointType> >) return ItemType::floatingPointLiteral;
                               // if constexpr (std::is_same_v <ArgType, StringLiteral<PositionInfoType, StringType>               >) return ItemType::stringLiteral       ;
                               // if constexpr (std::is_same_v <ArgType, SymbolLiteral<PositionInfoType, StringType>               >) return ItemType::symbolLiteral       ;

                               else if constexpr (std::is_same_v <ArgType, IdentifierLiteralType >)
                                   return Kind::tokenIdent;

                               // if constexpr (std::is_same_v <ArgType, ExpressionEntry<PositionInfoType>                         >) return ItemType::expressionEntry     ;
                               // if constexpr (std::is_same_v <ArgType, FunctionCall<PositionInfoType, StringType>                >) return ItemType::functionCall        ;
                               // if constexpr (std::is_same_v <ArgType, FunctionalCast<PositionInfoType, StringType>              >) return ItemType::functionalCast      ;
                               // if constexpr (std::is_same_v <ArgType, Cast<PositionInfoType, StringType>                        >) return ItemType::cast                ;
                               // if constexpr (std::is_same_v <ArgType, VoidValue<PositionInfoType, StringType>                   >) return ItemType::void_               ;

                               else 
                                   return Kind::unknown;
                           }
                         , v
                         );
    
    }

    Error setError(Error e)
    {
        if (e!=Error::noError)
            m_state = State::err;
        m_error = e;
        return e;
    }


    ExpressionNodeType expressionItemToNode(const ExpressionItemType &v)
    {
        return ExpressionNodeType{v}; // Заполняется только nodeValue
    }

    void pushOperator(const ExpressionItemType &v)
    {
        std::visit( [&](const auto &a)
                    {
                        if constexpr (std::is_same_v <std::decay_t<decltype(a)>, OperatorType >)
                            m_opStack.push_back(a);
                        //else
                    }
                  , v
                  );
    }

    Kind getTopOperatorKind() const
    {
        if (m_opStack.empty())
            return Kind::unknown;

        const auto &topOp = m_opStack.back();

        return getOperatorKind(topOp);
    }

    Kind popOperator()
    {
        auto kind = getTopOperatorKind();

        if (kind!=Kind::unknown)
            m_opStack.pop_back();

        return kind;
    }

    // Помещает в value стек значение n, или проверяет наличие отрицания на стеке операторов и помещает n в value стек с его учетом
    void pushValue(const ExpressionNodeType &n, bool checkUseInvertOnOperatorStackTop=false)
    {
        if (!m_varCounts.empty())
            m_varCounts.back()++;

        if (!checkUseInvertOnOperatorStackTop)
        {
            m_varStack.push_back(n);
        }
        else 
        {
            // Тут мы проверяем наличие оператора отрицания на вершине стека операторов
            Kind k = getTopOperatorKind();
            if (k!=Kind::opNot) // На стеке нет оператора отрицания
            {
                m_varStack.push_back(n);
            }
            else
            {
                // Запихиваем value не как value, а как операторныую ноду с одним аргументом value

                // Задаём nodeValue восстановленным из оператора ExpressionItemType
                auto node = ExpressionNodeType{ ExpressionItemType{m_opStack.back()} };

                // Задаём аффиксность и арность
                // Это нужно для вычислителя выражений
                node.affixation = OperatorAffixation::prefix; //  none/prefix/postfix
                node.arity      = OperatorArity::unary;       // unary/binary/etc    

                // В аргументы операции кладём переданный аргумент
                node.argList.emplace_back(n);

                m_varStack.push_back(node);
                m_opStack.pop_back(); // Удаляем отрицание со стека операторов

            }
        }
    }

    void pushValue(const ExpressionItemType &v, bool checkUseInvertOnOperatorStackTop=false)
    {
        std::visit( [&](const auto &a)
                    {
                        if constexpr (std::is_same_v <std::decay_t<decltype(a)>, IdentifierLiteralType>)
                            pushValue(expressionItemToNode(v), checkUseInvertOnOperatorStackTop);
                        else if constexpr (std::is_same_v <std::decay_t<decltype(a)>, BoolLiteralType >)
                            pushValue(expressionItemToNode(v), checkUseInvertOnOperatorStackTop);
                        //else
                    }
                  , v
                  );
    }

    // Следует вызывать после добавления идентификатора или константы
    // Если AND оператора нет на стеке операторов, то ничего не делаем
    // Если AND оператор есть на стеке операторов, но нет двух операндов, то это ошибка
    // Иначе всё сходится, и два операнда преобразовываем в один AND-операнд

    bool checkCollapseForLogicAndOnTopOfOperatorsStack()
    {
        Kind k = getTopOperatorKind();

        if (k!=Kind::opAnd) // На стеке нет оператора AND
            return true;
    
        // На стеке есть оператор AND
        if (m_varStack.size()<2)
            return false; // Но нет для AND операндов - это какая-то беда

        // Всё нормас, можно делать. Из оператора на вершине стека операторов взяли оператор (AND) и сделали из него ноду
        auto node = ExpressionNodeType{ ExpressionItemType{m_opStack.back()} };
        // Задаём аффиксность и арность
        // Это нужно для вычислителя выражений
        node.affixation = OperatorAffixation::none; //  none/prefix/postfix
        node.arity      = OperatorArity::binary;    // unary/binary/etc    

        m_opStack.pop_back(); // Удаляем AND со стека операторов

        auto right = m_varStack.back(); m_varStack.pop_back();
        auto left  = m_varStack.back(); m_varStack.pop_back();

        node.argList.emplace_back(left);
        node.argList.emplace_back(right);

        m_varStack.push_back(node);

        return true;
    }

    bool checkCollapseForLogicOrOnTopOfOperatorsStack()
    {
        Kind k = getTopOperatorKind();

        if (k!=Kind::opOr) // На стеке нет оператора OR
            return true;
    
        // На стеке есть оператор OR
        if (m_varStack.size()<2)
            return false; // Но нет для OR операндов - это какая-то беда

        // Всё нормас, можно делать. Из оператора на вершине стека операторов взяли оператор (AND) и сделали из него ноду
        auto node = ExpressionNodeType{ ExpressionItemType{m_opStack.back()} };
        // Задаём аффиксность и арность
        // Это нужно для вычислителя выражений
        node.affixation = OperatorAffixation::none; //  none/prefix/postfix
        node.arity      = OperatorArity::binary;    // unary/binary/etc    

        m_opStack.pop_back(); // Удаляем OR со стека операторов

        auto right = m_varStack.back(); m_varStack.pop_back();
        auto left  = m_varStack.back(); m_varStack.pop_back();

        node.argList.emplace_back(left);
        node.argList.emplace_back(right);

        m_varStack.push_back(node);

        return true;
    }

    bool checkCollapseGrouppingBrackets()
    {
        while(!m_opStack.empty())
        {
            if (getTopOperatorKind()==Kind::opAnd)
            {
                if (!checkCollapseForLogicAndOnTopOfOperatorsStack())
                    return false;
            }
            else if (getTopOperatorKind()==Kind::opOr)
            {
                if (!checkCollapseForLogicOrOnTopOfOperatorsStack())
                    return false;
            }
            else if (getTopOperatorKind()==Kind::opOpen)
            {
                if (m_varStack.size()<1)
                    return false; // какая-то беда

                auto node = ExpressionNodeType{ ExpressionItemType{m_opStack.back()} };
                node.affixation = OperatorAffixation::none; //  none/prefix/postfix
                node.arity      = OperatorArity::unary;     // unary/binary/etc    
                node.argList.emplace_back(m_varStack.back());
                m_varStack.pop_back();
                //m_varStack.push_back(node);
                m_opStack.pop_back(); // Удаляем '(' со стека операторов
                pushValue(node, true); // checkUseInvertOnOperatorStackTop
                return true;
            }
            else
            {
                return false;
            }
        }

        return true;
    }


    // bool isBoolConstantItem(const ExpressionItemType &v) const
    // {
    //     return
    //     std::visit( [&](const auto &a) -> bool
    //                 {
    //                     else if constexpr (std::is_same_v <std::decay_t<decltype(a)>, BoolLiteralType >)
    //                         return true;
    //                     else
    //                         return false;
    //                 }
    //               , v
    //               );
    // }

    // bool getBoolConstantItemValue(const ExpressionItemType &v) const
    // {
    //     auto k = getExpressionItemKind(v);
    //     return k==Kind::tokenFalse || k==Kind::tokenTrue;
    //     // return
    //     // std::visit( [&](const auto &a) -> bool
    //     //             {
    //     //                 else if constexpr (std::is_same_v <std::decay_t<decltype(a)>, BoolLiteralType >)
    //     //                     return a.value;
    //     //                 else
    //     //                     return false;
    //     //             }
    //     //           , v
    //     //           );
    // }
    //  
    // ExpressionItemType makeBoolExpressionItem(bool v) const
    // {
    //     return ExpressionItemType{marty::expressions::BoolLiteral<PositionInfoType>{posInfo, v}};
    // }




    std::size_t countGraphNodes(const ExpressionNodeType &n) const
    {
        //std::vector<ExpressionNode>   argList;
        std::size_t res = 1;

        for(const auto &a: n.argList)
            res += countGraphNodes(a);

        return res;
    }

    template<typename StreamType>
    void gvGraph(StreamType &oss, const ExpressionNodeType &node, std::size_t n) const
    {
        oss << "n" << n << ";\n";

        Kind k = getExpressionItemKind(node.nodeValue);

        std::string label;
        switch(k)
        {
            case Kind::tokenIdent: label = std::string(utils::expressionItemToString(node.nodeValue).c_str()); break;
            // case Kind::opOpen    : label = "(...)"   ; break;
            // case Kind::opClose   : label = "(...)"   ; break;
            case Kind::opOpen    : label = "( )"  ; break;
            case Kind::opClose   : label = "( )"  ; break;
            case Kind::opNot     : label = "NOT"  ; break;
            case Kind::opAnd     : label = "AND"  ; break;
            case Kind::opOr      : label = "OR"   ; break;
            case Kind::tokenFalse: label = "False"; break;
            case Kind::tokenTrue : label = "True" ; break;
            case Kind::unknown: [[fallthrough]];
            default: label = "<UNKNOWN>";
        }


    // ExpressionItemType            nodeValue; // литерал - аргументов нет (они игнорируются), идентификатор - ссылка на переменную, FunctionCall - много аргументов, FunctionalCast - один аргумент, или Operator - количество аргументов зависит от оператора
    // std::vector<ExpressionNode>   argList;


        // Kind getOperatorKind(const OperatorType &op) const
        
        // n003 [label="*"] ;
        oss << "n" << n << " [label=\"" << label << "\"];\n";

        std::size_t 
        nextBase = n+1;
        for(const auto &a: node.argList)
        {
            oss << "n" << n << " -- " << "n" << nextBase << ";\n";
            nextBase += countGraphNodes(a);
        }

        nextBase = n+1;
        for(const auto &a: node.argList)
        {
            gvGraph(oss, a, nextBase);
            nextBase += countGraphNodes(a);
        }

    }

/*
    using PositionInfoType            = PositionInfoTypeT;
    using BoolLiteralType             = BoolLiteral<PositionInfoType>;

*/    

    // ExpressionNodeType

    // bool precedenceLess(unsigned p1, unsigned p2) const     { return precedenceCompare(p1, p2)<0; }
    // bool precedenceGreater(unsigned p1, unsigned p2) const  { return precedenceCompare(p1, p2)>0; }
    // bool precedenceLessThanTop(unsigned p) const            { return precedenceCompare(getKindPrecedence(getTopOperatorKind()), p)<0; }
    // bool precedenceGreaterThanTop(unsigned p) const         { return precedenceCompare(getKindPrecedence(getTopOperatorKind()), p)>0; }


public: // ctors

    SimpleBoolExpressionParser(const OperatorTraitsType &opTraits) : m_opTraits(opTraits)
    {
        m_expression.nodeValue = BoolLiteralType{PositionInfoType{}, false};
    }

    SimpleBoolExpressionParser(const SimpleBoolExpressionParser &) = default;
    SimpleBoolExpressionParser(SimpleBoolExpressionParser &&) = default;


public: // assign

    SimpleBoolExpressionParser& operator=(const SimpleBoolExpressionParser &) = default;
    SimpleBoolExpressionParser& operator=(SimpleBoolExpressionParser &&) = default;


public: // methods

    //const ExpressionNodeType& getExpression() const
    ExpressionNodeType getExpression() const
    {
    //  
    // ExpressionNodeType  m_expression;
    //  
    // // ExpressionItemType            nodeValue; // литерал - аргументов нет (они игнорируются), идентификатор - ссылка на переменную, FunctionCall - много аргументов, FunctionalCast - один аргумент, или Operator - количество аргументов зависит от оператора
    // // std::vector<ExpressionNode>   argList;
    // //  
    // // // Требуются для вычисления значения выражения, передаются в вычислитель
    // // // В зависимости от формы - префиксная/постфиксная, а также арности 
    // // // операции могут производится по-разному.
    // // OperatorAffixation            affixation    = OperatorAffixation::none;    //  none/prefix/postfix
    // // OperatorArity                 arity         = OperatorArity::none;         // unary/binary/etc

        // ExpressionInputItem resItem = marty::expressions::BoolLiteral<PositionInfoType>{posInfo, tokenType==UMBA_TOKENIZER_TOKEN_BOOL_LITERAL_TRUE};
        // 

        return m_expression;
    }

    template<typename StreamType>
    void dump(StreamType &oss) const
    {
        MARTY_ARG_USED(oss);
    }

    template<typename StreamType>
    void gvGraph(StreamType &oss, const ExpressionNodeType &n, const std::string &label="") const
    {
        oss << "graph \"\"\n";
        oss << "{\n";
        if (!label.empty())
        {
            oss << "label=\"" << label << "\"\n";
            oss << "labelloc=\"t\";\n";
        }
        //oss << "{\n";

        gvGraph(oss, n, 0);

        oss << "}\n";
    }

    std::string getErrorMessage(Error err, bool addErrCode=true) const
    {
        std::string resStr;
        if (addErrCode)
            resStr = enum_serialize(err) + ": ";
        resStr += enum_get_description(err);

        // if (err==Error::gotNonUnaryOrNonPrefixOp || err==Error::gotNonUnaryOp || err==Error::gotNonPrefixOp)
        // {
        //     resStr += ", but expected " + enum_get_description(getParserState());
        // }

        return resStr;
    }

    std::string getErrorMessage(bool addErrCode=true) const
    {
        return getErrorMessage(m_error);
    }

    SimpleBoolExpressionEvaluator< OperatorTraitsType, PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType>
    getEvaluator() const;
    
    

protected: // parsing helpers

    // ExpressionItemType            nodeValue; // литерал - аргументов нет (они игнорируются), идентификатор - ссылка на переменную, FunctionCall - много аргументов, FunctionalCast - один аргумент, или Operator - количество аргументов зависит от оператора
    // std::vector<ExpressionNode>   argList;
    //  
    // // Требуются для вычисления значения выражения, передаются в вычислитель
    // // В зависимости от формы - префиксная/постфиксная, а также арности 
    // // операции могут производится по-разному.
    // OperatorAffixation            affixation    = OperatorAffixation::none;    //  none/prefix/postfix
    // OperatorArity                 arity         = OperatorArity::none;         // unary/binary/etc


public: // parsing methods

    Error initialize()
    {
        m_state        = State::readVar;
        //m_nestingLevel = 0;
        m_error        = Error::noError; // gotUnexpectedClosingBacket

        m_expression.nodeValue = BoolLiteralType{PositionInfoType{}, false};

        m_opStack  .clear();
        m_varStack .clear();
        m_varCounts.clear();

        return Error::noError;
    }

    Error finalize()
    {
        if (m_state==State::readNextVar)
            return setError(Error::unexpectedEnd); // Был прочитан оператор, ожидали переменную или скобку, а получили неожиданный конец выражения

        if (!m_varCounts.empty())
            return setError(Error::parenthesisBalanceBroken); // У нас есть открытые скобки, а пришло неожиданное окончание

        if (!checkCollapseGrouppingBrackets())
            return setError(Error::somethingGoesWrong); // Ваще-то такого не должно происходить

        if (m_varStack.size()!=1)
            return setError(Error::somethingGoesWrong); // Что-то пошло не так

        m_expression = m_varStack.front();

        return Error::noError;
    }

    Error parse(const ExpressionInputItemType &tk_)
    {
        auto tk = expressionItemFromInputItem(tk_);

        Kind kind = getExpressionItemKind(tk);

        MARTY_ARG_USED(tk);
        MARTY_ARG_USED(kind);


        // Preccedence (C++)

        // 3   !a, ~a - логическое и битовое отрицание

        // 10  a == b, a != b - эти не используются, но XOR хотелось заменить на !=

        // Битовые операторы C++, но я их использую как логические операторы
        // 11  &
        // 12  ^
        // 13  |

        // Логические операторы C++ - можно использовать наравне с символами битовых операторов
        // 14  &&
        // 15  ||

        // Для простоты не будем использовать XOR - при желании его можно использовать вручную в виде формулы 
        // A XOR B = (A & ~B) | (~A & B)

        // Тут ещё такой нюанс. XOR эквивалентен оператору "не равно".
        // В C++ это !=.
        // Но приоритет оператора != выше всех логических операторов.
        // Эквивалентного преобразования логического выражения в плюсы (да и в другие языки) мы сделать не можем.
        // Но во всех языках есть гарантия, что приоритеты располагаются так pri(!) > pri(&) > pri(|)
        // Поэтому используем только то, что у нас гарантированно будет работать одинаково везде.


        // Что мы делаем.
        // Читаем первый аргумент (со всеми его отрицаниями, заканчиваем после идентификатора, переходим в ожидание бинарного оператора).
        // Читаем символ операции.
        // Читаем второй аргумент (со всеми его отрицаниями, заканчиваем после идентификатора, переходим в ожидание следующего бинарного оператора).
        // Читаем символ операции.
        // Если 
        //   операторы равны, то все последующие операнды складываются в один список
        //   тот, кто у нас уже лежит, приоритетнее, то кладём его в первый аргумент, а пришедший - заменяет его.
        //   тот, кто у нас уже лежит, менее приоритетный, то последний операнд удаляем, помещаем вместо него операторную ноду, в которую помещаем первый операнд.
        //   последующие добавления того же оператора кладём в эту оператортную ноду.

        // Что-то как-то сложно получается, не попробовать ли просто польскую нотацию?

        // А если польская нотация?
        // Перво-наперво - аннигилируем двойные отрицания, когда они появляются на стеке операторов.
        // А потом - кладём операторы и скобки в стек операторов, а операнды - в стек операндов.
        // Скобки - дополнительно меняем счётчик скобок. Скобки - кладём и открывающую, и закрывающую.

        // Не ясно, как собирать в выражение потом.

        // Алгоритм сортировочной станции - https://ru.wikipedia.org/wiki/%D0%90%D0%BB%D0%B3%D0%BE%D1%80%D0%B8%D1%82%D0%BC_%D1%81%D0%BE%D1%80%D1%82%D0%B8%D1%80%D0%BE%D0%B2%D0%BE%D1%87%D0%BD%D0%BE%D0%B9_%D1%81%D1%82%D0%B0%D0%BD%D1%86%D0%B8%D0%B8
        // Вроде сразу собирает, при финализации остатки только подсобрать надо

        switch(m_state)
        {
            case State::readVar: [[fallthrough]];
            case State::readNextVar:
            {
                switch(kind)
                {
                    case Kind::opOpen     : // Ждём переменную, получили открывающую скобку - норм, состояние не меняется
                        // ++m_nestingLevel;
                        m_varCounts.emplace_back(0);
                        pushOperator(tk);
                        m_state = State::readVar;
                        break;

                    case Kind::opClose    :
                        return setError(Error::gotUnexpectedClosingParenthesis);

                    case Kind::opNot      :

                        // Двойное отрицание схлопывается.
                        // Стек значений мы не трогаем, так как оператор отрицания - префиксный, и может попадаться
                        // только перед переменной или булевой константой.
                        if (getTopOperatorKind()==Kind::opNot)
                           popOperator(); 
                        else
                           pushOperator(tk);
                        break;

                    case Kind::opAnd      : [[fallthrough]];
                    case Kind::opOr       :
                        return setError(Error::gotUnexpectedOperator);

                    case Kind::tokenIdent : [[fallthrough]];
                    case Kind::tokenFalse : [[fallthrough]];
                    case Kind::tokenTrue  :
                        pushValue(tk, true); // С проверкой/учетом возможного отрицания на стеке операторов
                        if (!checkCollapseForLogicAndOnTopOfOperatorsStack())
                            return setError(Error::somethingGoesWrong); // Ваще-то такого не должно происходить
                        m_state = State::waitOperator;
                        break;

                    case Kind::unknown    : [[fallthrough]];
                    default:
                        return setError(Error::gotUnexpectedToken);

                } // switch(kind)
            }
            break;

            // В данном состоянии мы находимся тогда, когда мы ожидаем бинарный оператор
            case State::waitOperator: 
            {
                switch(kind)
                {
                    case Kind::opOpen     : // Вместо ожидаемого оператора пришла открывающая скобка - беда, это ситуация `!A (B | !C)`, а должно быть `!A & (B | !C)`
                        return setError(Error::gotUnexpectedOpeningParenthesis);

                    case Kind::opClose    :
                        if (m_varCounts.empty())
                            return setError(Error::gotUnbalancedParenthesis);

                        // Ошибкой является нахождение на стеке оператора отрицания - это значит, что у отрицания нет аргумента
                        if (getTopOperatorKind()==Kind::opNot) 
                            return setError(Error::gotUnexpectedClosingParenthesis);

                        // Скобками могут быть сгруппированы только подвыражения из одного или нескольких бинарных операторов
                        // Или переменная или булева константа
                        // Пустые скобки - это ошибка
                        if ( /* getTopOperatorKind()==Kind::opOpen &&  */ m_varCounts.back()==0)
                            return setError(Error::gotUnexpectedClosingParenthesis);

                        m_varCounts.pop_back();
                        if (!checkCollapseGrouppingBrackets())
                            return setError(Error::somethingGoesWrong); // Ваще-то такого не должно происходить

                        // Состояние не меняется - после закрывающей скобки ожидаем оператор
                        break;

                    case Kind::opNot      :
                        return setError(Error::gotUnexpectedOperator);

                    case Kind::opAnd      :
                        pushOperator(tk);
                        m_state = State::readNextVar;
                        break;

                    case Kind::opOr       :
                        pushOperator(tk);
                        m_state = State::readNextVar;
                        break;

                    case Kind::tokenIdent :
                        return setError(Error::gotUnexpectedVariable);

                    case Kind::tokenFalse : [[fallthrough]];
                    case Kind::tokenTrue  :
                        return setError(Error::gotUnexpectedConstant);

                    case Kind::unknown    : [[fallthrough]];
                    default:
                        return setError(Error::gotUnexpectedToken);

                } // switch(kind)
            }
            break;

            case State::err:
                return setError(Error::errorParserState);

            default:
                return setError(Error::unknownParserState);
                
        }

    return m_error;

    // enum class State
    // {
    //     readVar,
    //     waitOperator,
    //     err
    // };
    //  
    // enum class Kind
    // {
    //     unknown,
    //     opOpen,
    //     opClose,
    //     opNot,
    //     opAnd,
    //     // opXor,
    //     opOr,
    //     tokenIdent,
    //     tokenFalse,
    //     tokenTrue
    // };

    // State      m_state        = State::readVar;
    // int        m_nestingLevel = 0;
    // Error      m_error        = Error::noError; // gotUnexpectedClosingBacket
    //  
    // std::vector<OperatorType>    m_opStack ;
    // std::vector<ExpressionItem>  m_varStack;


// gotUnexpectedVariable              // got unexpected variable
// gotUnexpectedOperator              // got unexpected operator
// gotUnexpectedOpeningBacket         // got unexpected opening backet
// gotUnexpectedClosingBacket         // got unexpected closing backet
// errorParserState                   // parser already is in an eror state
// unknownParserState                 // parser is in an unknown state


        // return Error::noError;
    }


protected: // state handlers

}; // struct SimpleBoolExpressionParser

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template< typename OperatorTraitsTypeT // = SimpleBoolExpressionOperatorTraits<OperatorTokenTypeT>
        , typename PositionInfoTypeT
        , typename OperatorTokenTypeT
        , typename IntegerTypeT=MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE
        , typename FloatingPointTypeT=MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE
        , typename StringTypeT=MARTY_EXPRESSIONS_DEFAULT_STRING_LITERAL_VALUE_TYPE
        >
struct SimpleBoolExpressionEvaluator
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


    using ExpressionInputItemType     = ExpressionInputItem<PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType>;
    using expression_input_item_type  = ExpressionInputItemType;

    using ExpressionItemType          = ExpressionItem<PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType>;
    using expression_item_type        = ExpressionItemType;

    using ExpressionNodeType          = ExpressionNode<PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType>;
    using expression_node_type        = ExpressionNodeType;

    using OperationInfoType           = OperationInfo<PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType>;
    using operation_info_type         = OperationInfoType;


    using Kind = SimpleBoolExpressionItemKind;


protected: // members

    const OperatorTraitsType  m_opTraits  ;


    Kind getOperatorKind(const OperatorType &op) const
    {
        return op.value==m_opTraits.openBracketOp
             ? Kind::opOpen
    
             : op.value==m_opTraits.closeBracketOp
             ? Kind::opClose
    
             : op.value==m_opTraits.notOp
             ? Kind::opNot
    
             : op.value==m_opTraits.andOp
             ? Kind::opAnd
    
             // : op.value==m_opTraits.xorOp
             // ? Kind::opXor
    
             : op.value==m_opTraits.orOp
             ? Kind::opOr
    
             // : op.value==m_opTraits.
             // ? Kind::tokenIdent
             //  
             // : op.value==m_opTraits.
             // ? Kind::tokenFalse
             //  
             // : op.value==m_opTraits.
             // ? Kind::tokenTrue
    
             : Kind::unknown;
    }

    PositionInfoType getPositionInfo(const ExpressionItemType &v) const
    {
        return std::visit( [&](const auto &a) -> PositionInfoType
                           {
                               using ArgType = std::decay_t<decltype(a)>;

                               if constexpr (std::is_same_v <ArgType, OperatorType >)            return a.positionInfo;
                               else if constexpr (std::is_same_v <ArgType, BoolLiteralType >)    return a.positionInfo;
                               else if constexpr (std::is_same_v <ArgType, IntegerLiteralType >) return a.positionInfo;
                               else if constexpr (std::is_same_v <ArgType, FloatingPointLiteral<PositionInfoType, FloatingPointType> >) return a.positionInfo;
                               else if constexpr (std::is_same_v <ArgType, StringLiteral<PositionInfoType, StringType>               >) return a.positionInfo;
                               else if constexpr (std::is_same_v <ArgType, SymbolLiteral<PositionInfoType, StringType>               >) return a.positionInfo;
                               else if constexpr (std::is_same_v <ArgType, IdentifierLiteralType >) return a.positionInfo;
                               // if constexpr (std::is_same_v <ArgType, ExpressionEntry<PositionInfoType>                         >) return ItemType::expressionEntry     ;
                               else if constexpr (std::is_same_v <ArgType, FunctionCall<PositionInfoType, StringType>                >) return a.positionInfo;
                               else if constexpr (std::is_same_v <ArgType, FunctionalCast<PositionInfoType, StringType>              >) return a.positionInfo;
                               else if constexpr (std::is_same_v <ArgType, Cast<PositionInfoType, StringType>                        >) return a.positionInfo;
                               else if constexpr (std::is_same_v <ArgType, VoidValue<PositionInfoType, StringType>                   >) return a.positionInfo;

                               else 
                                   return PositionInfoType{};
                           }
                         , v
                         );
    }

    Kind getExpressionItemKind(const ExpressionItemType &v) const
    {
        return std::visit( [&](const auto &a) -> Kind
                           {
                               using ArgType = std::decay_t<decltype(a)>;

                               if constexpr (std::is_same_v <ArgType, OperatorType >)
                                   return getOperatorKind(a);

                               else if constexpr (std::is_same_v <ArgType, BoolLiteralType >)
                                   return a.value ? Kind::tokenTrue : Kind::tokenFalse;

                               // Целочисленные типы допустимы в логических выражениях, при этом 0 - обозначает false, а не ноль (обычно 1) - обозначает true
                               // Такие замены false/true на 0/1 часто используются в математической логике, дискретной математике, теории автоматов, и подобных дисциплинах.
                               else if constexpr (std::is_same_v <ArgType, IntegerLiteralType >) 
                                   return a.value ? Kind::tokenTrue : Kind::tokenFalse;

                               // if constexpr (std::is_same_v <ArgType, FloatingPointLiteral<PositionInfoType, FloatingPointType> >) return ItemType::floatingPointLiteral;
                               // if constexpr (std::is_same_v <ArgType, StringLiteral<PositionInfoType, StringType>               >) return ItemType::stringLiteral       ;
                               // if constexpr (std::is_same_v <ArgType, SymbolLiteral<PositionInfoType, StringType>               >) return ItemType::symbolLiteral       ;

                               else if constexpr (std::is_same_v <ArgType, IdentifierLiteralType >)
                                   return Kind::tokenIdent;

                               // if constexpr (std::is_same_v <ArgType, ExpressionEntry<PositionInfoType>                         >) return ItemType::expressionEntry     ;
                               // if constexpr (std::is_same_v <ArgType, FunctionCall<PositionInfoType, StringType>                >) return ItemType::functionCall        ;
                               // if constexpr (std::is_same_v <ArgType, FunctionalCast<PositionInfoType, StringType>              >) return ItemType::functionalCast      ;
                               // if constexpr (std::is_same_v <ArgType, Cast<PositionInfoType, StringType>                        >) return ItemType::cast                ;
                               // if constexpr (std::is_same_v <ArgType, VoidValue<PositionInfoType, StringType>                   >) return ItemType::void_               ;

                               else 
                                   return Kind::unknown;
                           }
                         , v
                         );
    }

    StringType getExpressionItemText(const ExpressionItemType &v) const
    {
        return std::visit( [&](const auto &a) -> StringType
                           {
                               using ArgType = std::decay_t<decltype(a)>;

                               if constexpr (std::is_same_v <ArgType, IdentifierLiteralType >)
                                   return a.value;
                               else 
                                   return StringType();
                           }
                         , v
                         );
    
    }

    static
    StringType ltrim(const StringType &s)
    {
        using CharType = typename StringType::value_type;
        auto it = s.begin();
        for(; it!=s.end() && *it==(CharType)' '; ++it) {}
        return StringType(it, s.end());
    }

    bool getBoolConstantItemValue(const ExpressionItemType &v) const
    {
        auto k = getExpressionItemKind(v);
        return k==Kind::tokenTrue; // k==Kind::tokenFalse;
        // return
        // std::visit( [&](const auto &a) -> bool
        //             {
        //                 else if constexpr (std::is_same_v <std::decay_t<decltype(a)>, BoolLiteralType >)
        //                     return a.value;
        //                 else
        //                     return false;
        //             }
        //           , v
        //           );
    }

    ExpressionItemType makeBoolExpressionItem(PositionInfoType p, bool v) const
    {
        return ExpressionItemType{BoolLiteralType{p, v}};
    }

    // bool isBoolConstantItem(const ExpressionItemType &v) const
    // bool getBoolConstantItemValue(const ExpressionItemType &v) const
    // Kind getExpressionItemKind(const ExpressionItemType &v) const

    // Constant Absorption - поглощение констант
    // Expression simplification - упрощение выражения
    // Redundant condition elimination - удаление избыточных условий
    // Logic minimization - минимизация логических функций
    // Примеры:
    // 'A&true' -> 'A', 'A&false' -> 'false', 'A|true' -> 'true', 'A|false' -> 'A'.
    //
    // Упрощением это назвать нельзя, хотя оно имеет место быть.
    // Упрощение - под этим термином обычно подразумевают минимизацию, а этого мы тут не будем делать.

    void performConstantAbsorptionImpl(ExpressionNodeType &node) const
    {
        for(auto &a: node.argList)
            performConstantAbsorptionImpl(a);

        auto nodeValueKind = getExpressionItemKind(node.nodeValue);

        bool foundFalse = false;
        bool foundTrue  = false;

        switch(nodeValueKind)
        {
            case Kind::opOpen : [[fallthrough]];
            case Kind::opClose:
                if (node.argList.size()==1)
                {
                    auto childKind = getExpressionItemKind(node.argList[0].nodeValue);
                    if (childKind==Kind::tokenFalse || childKind==Kind::tokenTrue)
                    {
                        node.nodeValue = node.argList[0].nodeValue;
                        node.argList.clear();
                    }
                }
                break;

            case Kind::opNot:
                if (node.argList.size()==1)
                {
                    auto childKind = getExpressionItemKind(node.argList[0].nodeValue);
                    if (childKind==Kind::tokenFalse || childKind==Kind::tokenTrue)
                    {
                        node.nodeValue = makeBoolExpressionItem(getPositionInfo(node.argList[0].nodeValue), !getBoolConstantItemValue(node.argList[0].nodeValue));
                        node.argList.clear();
                    }
                }
                break;

            case Kind::opAnd: [[fallthrough]];
            case Kind::opOr:
                for(std::size_t i=0; i!=node.argList.size();  /* ++i */ )
                {
                    auto childKind = getExpressionItemKind(node.argList[i].nodeValue);
                    if (childKind==Kind::tokenFalse || childKind==Kind::tokenTrue)
                    {
                        if (childKind==Kind::tokenFalse)
                            foundFalse = true;

                        if (childKind==Kind::tokenTrue)
                            foundTrue  = true;

                        node.argList.erase(node.argList.begin()+std::ptrdiff_t(i));
                    }
                    else
                    {
                        ++i;
                    }
                }

                // Константы не обнаружены - ничего не трогаем
                if (!foundFalse && !foundTrue)
                    break;

                // В выражении AND найдена константа false - всё выражение ложно
                if (nodeValueKind==Kind::opAnd && foundFalse)
                {
                    node.nodeValue = makeBoolExpressionItem(getPositionInfo(node.nodeValue), false);
                    node.argList.clear();
                    break;
                }

                // В выражении OR найдена константа true - всё выражение истинно
                if (nodeValueKind==Kind::opOr && foundTrue)
                {
                    node.nodeValue = makeBoolExpressionItem(getPositionInfo(node.nodeValue), true);
                    node.argList.clear();
                    break;
                }

                // Что-то было найдено, но на результат выражения оно не повлияло

                if (node.argList.size()==1) // остался один элемент, просто перемещаем его на уровень выше, т.к. и OR и AND для одного элемента равны самому элементу
                {
                    node.nodeValue = node.argList[0].nodeValue;
                    // node.argList.clear();
                    auto tmp = node.argList[0].argList; // Тут какой-то косяк, появляется value_less_by_exception, исправляем через копию
                    node.argList = tmp;
                    break;
                }

                // Больше одного элемента быть не должно никак (поставить какой-нибудь assert?)
                // Но может быть 0 элементов в остатке - это значит, 
                // что для OR все элементы false,
                // или для AND все элементы true

                // А если таки у нас как-то получилось, что AND/OR не двухместные, а многоместные?
                // Тогда, если у нас получилось 0 аргументов, то в результате должна быть константа true/false в зависимости от AND/OR
                // А если получилось больше элементов, то просто их оставляем, так как удаление составляющих выражения
                // на предыдущем шаге в целом не привело к однозначному результату

                if (node.argList.empty())
                {
                    if (nodeValueKind==Kind::opAnd)
                        node.nodeValue = makeBoolExpressionItem(getPositionInfo(node.nodeValue), true);
                    else
                        node.nodeValue = makeBoolExpressionItem(getPositionInfo(node.nodeValue), false);
                }

                break;
            
            case Kind::tokenIdent: [[fallthrough]];
            case Kind::tokenFalse: [[fallthrough]];
            case Kind::tokenTrue : [[fallthrough]];
            case Kind::unknown   : [[fallthrough]];
            default: {}
        }
    }

    template<typename ValueGetter>
    void getVariables(ExpressionNodeType &node, ValueGetter valueGetter) const
    {
        auto nodeValueKind = getExpressionItemKind(node.nodeValue);
        if (nodeValueKind==Kind::tokenIdent)
        {
            auto varName = getExpressionItemText(node.nodeValue);
            node.nodeValue = makeBoolExpressionItem(getPositionInfo(node.nodeValue), valueGetter(varName));
        }
        else
        {
            for(auto &a: node.argList)
                getVariables(a, valueGetter);
        }
    
    }
    //getExpressionItemText



public: // ctors

    SimpleBoolExpressionEvaluator(const OperatorTraitsType &opTraits) : m_opTraits(opTraits)
    {
    }

    SimpleBoolExpressionEvaluator(const SimpleBoolExpressionEvaluator &) = default;
    SimpleBoolExpressionEvaluator(SimpleBoolExpressionEvaluator &&) = default;


public: // assign

    SimpleBoolExpressionEvaluator& operator=(const SimpleBoolExpressionEvaluator &) = default;
    SimpleBoolExpressionEvaluator& operator=(SimpleBoolExpressionEvaluator &&) = default;


public: // members

    ExpressionNodeType performConstantAbsorption(const ExpressionNodeType &node) const
    {
        auto res = node;
        performConstantAbsorptionImpl(res);
        return res;
    }

    template<typename ValueGetter>
    bool evaluate(const ExpressionNodeType &node, ValueGetter valueGetter) const
    {
        auto res = node;
        getVariables(res, std::move(valueGetter));
        performConstantAbsorptionImpl(res);

        auto resKind = getExpressionItemKind(res.nodeValue);
        if (resKind==Kind::tokenFalse)
            return false;
        if (resKind==Kind::tokenTrue)
            return true;
        throw std::runtime_error("Something goes wrong in SimpleBoolExpressionEvaluator::evaluate");
    }

    template<typename ValueMap>
    bool evaluateMap(const ExpressionNodeType &node, const ValueMap &mVals) const
    {
        auto valueGetter = [&](const StringType &s) -> bool
        {
            auto it = mVals.find(s);
            if (it==mVals.end())
                return false;
            return it->second;
        };

        return evaluate(node, valueGetter);
    }

    std::set<StringType> getExpressionVars(ExpressionNodeType node) const
    {
        std::set<StringType> res;

        auto valueGetter = [&](const StringType &s) -> bool
        {
            res.insert(s);
            return false;
        };

        getVariables(node, valueGetter);

        return res;
    }

    TruthTable<StringType> makeTruthTable(const ExpressionNodeType &node) const
    {
        using TruthTableType = TruthTable<StringType>;
        auto vars = getExpressionVars(node);
        auto tt = TruthTableType(vars.begin(), vars.end());

        using StateType = typename TruthTableType::VarsStateType;

        StateType st = 0u;
        StateType stMax = tt.varsStateMax();

        auto valueGetter = [&](const StringType &s) -> bool
        {
            return tt.varGetValue(s, st);
        };

        for(; st!=stMax; ++st)
        {
            auto fnVal = evaluate(node, valueGetter);
            tt.resultSet(st, fnVal);
        }

        return tt;
    }

    TruthTable<StringType> makeTruthTable(const ExpressionNodeType &node, typename TruthTable<StringType>::PayloadType plt) const
    {
        using TruthTableType = TruthTable<StringType>;
        auto vars = getExpressionVars(node);
        auto tt = TruthTableType(vars.begin(), vars.end());

        using StateType = typename TruthTableType::VarsStateType;

        StateType st = 0u;
        StateType stMax = tt.varsStateMax();

        auto valueGetter = [&](const StringType &s) -> bool
        {
            return tt.varGetValue(s, st);
        };

        for(; st!=stMax; ++st)
        {
            auto fnVal = evaluate(node, valueGetter);
            tt.resultSet(st, fnVal, plt);
        }

        return tt;
    }

    


    StringType toString( const ExpressionNodeType &node
                       , bool useNamedOps=false // Также вместо 0/1 выводит false/true
                       ) const
    {
        auto nodeValueKind = getExpressionItemKind(node.nodeValue);

        switch(nodeValueKind)
        {
            case Kind::opOpen : [[fallthrough]];
            case Kind::opClose:
                if (node.argList.empty())
                    return StringType();
                else
                {
                    auto strArg = toString(node.argList.front(), useNamedOps);
                    return StringType("(") + strArg + StringType(")");
                }

            case Kind::opNot:
                if (node.argList.empty())
                    return StringType();
                else
                {
                    auto strArg = toString(node.argList.front(), useNamedOps);
                    return StringType(useNamedOps ? " not " : "!") + strArg;
                }

            case Kind::opAnd: [[fallthrough]];
            case Kind::opOr:

                // Будем поддерживать многоместные операции AND/OR, а не только двухместные
                if (node.argList.empty())
                    return StringType();
                else
                {
                    auto sep = StringType(nodeValueKind==Kind::opAnd ? (useNamedOps ? " and " : "&") : (useNamedOps ? " or " : "|"));

                    auto it  = node.argList.begin();
                    auto res = toString(*it, useNamedOps);
                    ++it;
                    for(; it!=node.argList.end(); ++it)
                    {
                        auto nextArg = ltrim(toString(*it, useNamedOps));
                        res += sep + nextArg;
                    }

                    return res;
                }

            case Kind::tokenIdent: 
                return getExpressionItemText(node.nodeValue);
            
            case Kind::tokenFalse: 
                return StringType(useNamedOps ? "false" : "0");

            case Kind::tokenTrue : 
                return StringType(useNamedOps ? "true" : "1");

            case Kind::unknown   : [[fallthrough]];
            default: return StringType();
        }
        
    }


}; // struct SimpleBoolExpressionEvaluator

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
#if 1
template< typename OperatorTraitsTypeT
        , typename PositionInfoTypeT
        , typename OperatorTokenTypeT
        , typename IntegerTypeT
        , typename FloatingPointTypeT
        , typename StringTypeT
        >
SimpleBoolExpressionEvaluator<OperatorTraitsTypeT, PositionInfoTypeT, OperatorTokenTypeT, IntegerTypeT, FloatingPointTypeT, StringTypeT>
SimpleBoolExpressionParser   <OperatorTraitsTypeT, PositionInfoTypeT, OperatorTokenTypeT, IntegerTypeT, FloatingPointTypeT, StringTypeT>::
getEvaluator() const
{
    return SimpleBoolExpressionEvaluator< OperatorTraitsTypeT, PositionInfoTypeT, OperatorTokenTypeT, IntegerTypeT, FloatingPointTypeT, StringTypeT>(m_opTraits);
}
#endif
//----------------------------------------------------------------------------



//----------------------------------------------------------------------------


} // namespace expressions
} // namespace marty
// marty::expressions::

