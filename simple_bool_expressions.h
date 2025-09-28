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
#include <stdexcept>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <utility>

//
#include "undef_min_max.h"

// marty::expressions::
namespace marty {
namespace expressions {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
enum class SimpleBoolExpressionItemKind
{
    unknown,
    tokenFalse,
    tokenTrue,
    tokenIdent,
    opNot,
    opAnd,
    // opXor,
    opOr,
    opOpen,
    opClose
};

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename OperatorTokenTypeT>
struct SimpleBoolExpressionOperatorTraits
{
    OperatorTokenTypeT   openBracketOp ; // UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_OPEN    0x0031u
    OperatorTokenTypeT   closeBracketOp; // UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_CLOSE   0x0032u
    OperatorTokenTypeT   notOp         ; // UMBA_TOKENIZER_TOKEN_OPERATOR_LOGICAL_NOT  0x2150u  /  UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_NOT  0x2160u
    OperatorTokenTypeT   andOp         ; // UMBA_TOKENIZER_TOKEN_OPERATOR_LOGICAL_AND  0x2151u  /  UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_AND  0x2161u
    // OperatorTokenTypeT   xorOp         ;
    OperatorTokenTypeT   orOp          ; // UMBA_TOKENIZER_TOKEN_OPERATOR_LOGICAL_OR   0x2152u  /  UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_OR   0x2162u
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


    using ExpressionInputItemType     = ExpressionInputItem<PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType>;
    using expression_input_item_type  = ExpressionInputItemType;

    using ExpressionItemType          = ExpressionItem<PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType>;
    using expression_item_type        = ExpressionItemType;

    using ExpressionNodeType          = ExpressionNode<PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType>;
    using expression_node_type        = ExpressionNodeType;

    using OperationInfoType           = OperationInfo<PositionInfoType, OperatorTokenType, IntegerType, FloatingPointType, StringType>;
    using operation_info_type         = OperationInfoType;


    using kind_type = SimpleBoolExpressionItemKind;
    using KindType  = SimpleBoolExpressionItemKind;
    using Kind      = SimpleBoolExpressionItemKind;

    using error_type = Error;
    using ErrorType  = Error;


public: // member types



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
    
             : op.value==m_opTraits.orOp
             ? Kind::opOr
    
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

                               else if constexpr (std::is_same_v <ArgType, IdentifierLiteralType >)
                                   return Kind::tokenIdent;

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
        return m_expression;
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
    
             : op.value==m_opTraits.orOp
             ? Kind::opOr

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
    
                               else if constexpr (std::is_same_v <ArgType, IdentifierLiteralType >)
                                   return Kind::tokenIdent;
    
                               else 
                                   return Kind::unknown;
                           }
                         , v
                         );
    }
    
    bool getBoolConstantItemValue(const ExpressionItemType &v) const
    {
        auto k = getExpressionItemKind(v);
        return k==Kind::tokenTrue; // k==Kind::tokenFalse;
    }
    
    ExpressionItemType makeBoolExpressionItem(PositionInfoType p, bool v) const
    {
        return ExpressionItemType{BoolLiteralType{p, v}};
    }
    
    // Constant Absorption - поглощение констант
    // Expression simplification - упрощение выражения
    // Redundant condition elimination - удаление избыточных условий
    // Logic minimization - минимизация логических функций
    // Примеры:
    // 'A&true' -> 'A', 'A&false' -> 'false', 'A|true' -> 'true', 'A|false' -> 'A'.
    //
    // Упрощением это назвать нельзя, хотя оно имеет место быть.
    // Упрощение - под этим термином обычно подразумевают минимизацию, а этого мы тут не будем делать.
    
    bool performConstantAbsorptionImpl(ExpressionNodeType &node) const
    {
        bool bRes = false;
        for(auto &a: node.argList)
        {
            if (performConstantAbsorptionImpl(a))
                bRes = true;
        }

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
                        bRes = true;
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
                        bRes = true;
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
                    bRes = true;
                    break;
                }
    
                // В выражении OR найдена константа true - всё выражение истинно
                if (nodeValueKind==Kind::opOr && foundTrue)
                {
                    node.nodeValue = makeBoolExpressionItem(getPositionInfo(node.nodeValue), true);
                    node.argList.clear();
                    bRes = true;
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
                    bRes = true;
                }
    
                break;
            
            case Kind::tokenIdent: [[fallthrough]];
            case Kind::tokenFalse: [[fallthrough]];
            case Kind::tokenTrue : [[fallthrough]];
            case Kind::unknown   : [[fallthrough]];
            default: {}
        }

        return bRes;
    }

    bool makeMultiAryImpl(ExpressionNodeType &node) const
    {
        bool res = false;

        for(auto &childNode: node.argList)
        {
            if (makeMultiAryImpl(childNode))
                res = true;
        }

        auto nodeValueKind = getExpressionItemKind(node.nodeValue);

        if (nodeValueKind!=Kind::opAnd && nodeValueKind!=Kind::opOr)
            return res;

        bool foundSameOp = false;
        for(const auto &childNode: node.argList)
        {
            auto childNodeValueKind = getExpressionItemKind(childNode.nodeValue);
            if (childNodeValueKind==nodeValueKind)
            {
                foundSameOp = true;
            }
        }

        if (!foundSameOp)
            return res;

        std::vector<ExpressionNodeType> newArgList;

        for(const auto &childNode: node.argList)
        {
            auto childNodeValueKind = getExpressionItemKind(childNode.nodeValue);
            if (childNodeValueKind==nodeValueKind)
            {
                newArgList.insert(newArgList.end(), childNode.argList.begin(), childNode.argList.end());
            }
            else
            {
                newArgList.push_back(childNode);
            }
        }

        node.argList = newArgList;

        return true;
    }

    // Схлопывание вложенных скобок
    // Collapsing nested parentheses
    bool collapseNestedParenthesesImpl(ExpressionNodeType &node) const
    {
        bool res = false;

        for(auto &childNode: node.argList)
        {
            if (collapseNestedParenthesesImpl(childNode))
                res = true;
        }

        auto nodeValueKind = getExpressionItemKind(node.nodeValue);
        if (nodeValueKind!=Kind::opOpen && nodeValueKind!=Kind::opClose)
            return res;

        if (node.argList.size()!=1)
            return res;

        auto childValueKind = getExpressionItemKind(node.argList[0].nodeValue);
        if (childValueKind!=Kind::opOpen && childValueKind!=Kind::opClose)
            return res;

        auto tmpArgList = node.argList[0].argList;

        node.argList = tmpArgList;

        return true;
    }

    bool removeAllParenthesesImpl(ExpressionNodeType &node) const
    {
        bool res = false;

        for(auto &childNode: node.argList)
        {
            if (removeAllParenthesesImpl(childNode))
                res = true;
        }

        auto nodeValueKind = getExpressionItemKind(node.nodeValue);
        if (nodeValueKind!=Kind::opOpen && nodeValueKind!=Kind::opClose)
            return res;

        if (node.argList.size()!=1)
            return res;

        node = ExpressionNodeType(node.argList[0]);
        return true;
    }

    void invertNodeAndOrOperator(ExpressionNodeType &node, Kind nk) const
    {
        auto &opNode = std::get<OperatorType>(node.nodeValue);
        opNode.value = nk==Kind::opAnd ? m_opTraits.orOp : m_opTraits.andOp;
        opNode.text  = nk==Kind::opAnd ?       "|"       :       "&"       ;
    }

    // Продвинуть отрицания (Привести к нормальной форме отрицания)
    // Promote Negations (Bring to Normal Form of Negation)
    bool promoteNegationsImpl2(ExpressionNodeType &node) const
    {
        bool res = false;

        for(auto &childNode: node.argList)
        {
            if (promoteNegationsImpl(childNode))
                res = true;
        }

        auto promoteImpl = [&]( ExpressionNodeType notNodeTpl, std::vector<ExpressionNodeType> &argList )
        {
            notNodeTpl.argList.clear();
            std::vector<ExpressionNodeType> newArgList;
            for(auto &ch : argList)
            {                                      
                auto notNode = notNodeTpl;         
                notNode.argList.push_back(ch);     
                newArgList.push_back(notNode);     
            }                                      

            argList = newArgList;
        };

        auto nodeValueKind = getExpressionItemKind(node.nodeValue);
        if (nodeValueKind!=Kind::opNot)
            return res;

        if (node.argList.size()!=1)
            return res;

        auto &parenthesesNode = node.argList[0];
        auto parenthesesNodeValueKind = getExpressionItemKind(parenthesesNode.nodeValue);
        if (parenthesesNodeValueKind==Kind::opAnd || parenthesesNodeValueKind==Kind::opOr || parenthesesNodeValueKind==Kind::opNot)
        {
            // Мы ожидали ноду со скобками, а получили ноду с лог оператором

            if (parenthesesNodeValueKind==Kind::opNot)
            {
                if (parenthesesNode.argList.size()!=1)
                    return res;
                node = ExpressionNodeType(parenthesesNode.argList[0]);
                return true;
            }

            // Тут у нас либо AND, либо OR

            invertNodeAndOrOperator(parenthesesNode, parenthesesNodeValueKind);
            //invertNodeOperator(parenthesesNode, parenthesesNodeValueKind);

            promoteImpl(node, parenthesesNode.argList);
            node = ExpressionNodeType(parenthesesNode);
            return true;
        }

        if (parenthesesNodeValueKind!=Kind::opOpen && parenthesesNodeValueKind!=Kind::opClose)
            return res;

        if (parenthesesNode.argList.size()!=1)
            return res;

        auto &parenthesesChildNode = parenthesesNode.argList[0];
        auto parenthesesChildNodeValueKind = getExpressionItemKind(parenthesesChildNode.nodeValue);
        if (parenthesesChildNodeValueKind!=Kind::opAnd && parenthesesChildNodeValueKind!=Kind::opOr && parenthesesChildNodeValueKind!=Kind::opNot)
            return res;

        if (parenthesesChildNodeValueKind==Kind::opNot)
        {
            if (parenthesesChildNode.argList.size()!=0)
                return res;
            parenthesesChildNode = ExpressionNodeType(parenthesesChildNode.argList[0]);
            node = parenthesesNode;
            return true;
        }

        invertNodeAndOrOperator(parenthesesChildNode, parenthesesChildNodeValueKind);
        promoteImpl(node, parenthesesChildNode.argList);

        node = ExpressionNodeType(parenthesesNode);

        return true;
    }

    bool promoteNegationsImpl(ExpressionNodeType &node) const
    {
        std::size_t cnt = 0;
        while(promoteNegationsImpl2(node))
            ++cnt;
        return cnt!=0;
    }

    bool removeUnnecessaryParenthesesImpl(ExpressionNodeType &node) const
    {
        bool res = false;

        for(auto &childNode: node.argList)
        {
            if (removeUnnecessaryParenthesesImpl(childNode))
                res = true;
        }

        auto nodeValueKind = getExpressionItemKind(node.nodeValue);
        if (nodeValueKind!=Kind::opAnd && nodeValueKind!=Kind::opOr)
            return res;

        // if (node.argList.size()!=1)
        //     return res;

        for(auto &childNode: node.argList)
        {
            auto childNodeValueKind = getExpressionItemKind(childNode.nodeValue);
            if (childNodeValueKind!=Kind::opOpen && childNodeValueKind==Kind::opClose)
                continue;

            if (childNode.argList.size()!=1)
                continue;

            auto &childChildNode = childNode.argList[0];
            auto childChildNodeValueKind = getExpressionItemKind(childChildNode.nodeValue);
            if (childChildNodeValueKind!=Kind::opAnd && childChildNodeValueKind!=Kind::opOr)
                continue;

            if (nodeValueKind!=childChildNodeValueKind)
            {
                if (nodeValueKind==Kind::opOr && childChildNodeValueKind==Kind::opAnd)
                {
                    childNode = ExpressionNodeType(childChildNode);
                    continue;
                }
                if (nodeValueKind==Kind::opAnd && childChildNodeValueKind==Kind::opOr)
                {
                    childNode = ExpressionNodeType(childChildNode);
                    continue;
                }
                continue;
            }

            childNode = ExpressionNodeType(childChildNode);

            res = true;
        }
    
        return res;
    }

    //! Возвращает 0, если не идентификатор, 1 - если идентификатор, и -1, если идентификатор с отрицанием
    int isIdentNode(const ExpressionNodeType &node) const
    {
        auto nodeValueKind = getExpressionItemKind(node.nodeValue);
        if (nodeValueKind==Kind::tokenIdent) 
            return 1; // Для идентификатора возвращаем прямое вхождение

        if (nodeValueKind!=Kind::opNot)
            return 0; // Если не отрицание, то это явно не идентификатор с отрицанием
        
        if (node.argList.size()!=1)
            return 0; // Отрицание, но некорректное число потомков

        auto &child = node.argList[0];
        auto childValueKind = getExpressionItemKind(child.nodeValue);
        if (childValueKind==Kind::tokenIdent)
            return -1; // Дочерный элемент у отрицания - идентификатор, возвращаем инверсное вхождение

        return 0;
    }

    //! Возвращает идентификатор, если у нас node tokenIdent или opNot с дочерним tokenIdent
    StringType getIdentNodeString(const ExpressionNodeType &node) const
    {
        auto nodeValueKind = getExpressionItemKind(node.nodeValue);
        if (nodeValueKind==Kind::tokenIdent) 
            return getExpressionItemString(node.nodeValue);

        if (nodeValueKind!=Kind::opNot)
            return StringType();
        
        if (node.argList.size()!=1)
            return StringType();

        auto &child = node.argList[0];
        auto childValueKind = getExpressionItemKind(child.nodeValue);
        if (childValueKind==Kind::tokenIdent)
            return getExpressionItemString(child.nodeValue);

        return StringType(); // Что-то пошло не так
    }


    // Нужно избавится от одинаковых переменных (удаление дубликатов)
    // Нужно избавится от прямого и инверсного вхождения переменных:
    //   для OR всё выражение будет равно единице;
    //   для AND всё выражение будет равно нулю;

    bool collapseSameVarsAndObvioslyConstantsImpl(ExpressionNodeType &node) const
    {
        bool res = false;

        for(auto &childNode: node.argList)
        {
            if (collapseSameVarsAndObvioslyConstantsImpl(childNode))
                res = true;
        }

        auto nodeValueKind = getExpressionItemKind(node.nodeValue);

        if (nodeValueKind!=Kind::opAnd && nodeValueKind!=Kind::opOr)
            return res;

        using OccurrenceCounters = std::pair<std::size_t, std::size_t>; // first - прямые вхождения, second - инверсные

        std::unordered_map<StringType, OccurrenceCounters> occurrenceCounters;

        bool hasTheSameVar = false;

        auto replaceNodeWithBoolConstant = [&]()
        {
            auto posInfo   = getExpressionItemPositionInfo(node.nodeValue);
            // Для AND результат тождественно равен false. 
            // Для OR  результат тождественно равен true.
            node.nodeValue = makeBoolExpressionItem(posInfo, nodeValueKind==Kind::opAnd ? false : true );
            node.argList.clear();
            return true;
        };


        for(const auto &child: node.argList)
        {
            auto isIdent = isIdentNode(child);

            if (isIdent==0)
                continue;

            StringType name = getIdentNodeString(child);
            if (name.empty())
                continue;

            if (isIdent>0) // Прямое вхождение
            {
                occurrenceCounters[name].first++;
                if (occurrenceCounters[name].second>0) // Есть прямое вхождение, и есть инверсное.
                    return replaceNodeWithBoolConstant();

                if (occurrenceCounters[name].first>1)
                    hasTheSameVar = true;
            }
            else // Инверсное вхождение
            {
                occurrenceCounters[name].second++;
                if (occurrenceCounters[name].first>0) // Есть инверсное вхождение, и есть прямое.
                    return replaceNodeWithBoolConstant();

                if (occurrenceCounters[name].second>1)
                    hasTheSameVar = true;
            }
        }

        if (!hasTheSameVar)
            return res;


        // У нас есть повторения
        // Повторения могут быть как прямые, так и с инверснией
        // Не может быть вхождения прямого одновременно с инверсным - это мы отсекли ранее
        // Теперь нам нужно сформировать newArgList из уникальных идентификаторов

        std::vector<ExpressionNodeType> newArgList;
        std::unordered_set<StringType>  usedIdents;


        for(const auto &child: node.argList)
        {
            auto isIdent = isIdentNode(child);

            if (isIdent==0)
            {
                newArgList.emplace_back(child);
                continue;
            }

            StringType name = getIdentNodeString(child);
            if (name.empty())
            {
                newArgList.emplace_back(child);
                continue;
            }

            auto it = usedIdents.find(name);
            if (it!=usedIdents.end())
                continue; // Пропускаем уже использованный идентификатор

            newArgList.emplace_back(child); // Добавляем в первый раз
            usedIdents.insert(name); // Помечаем, как использованный
        }

        node.argList = newArgList;

        return true;
    }

    //! Возвращает 0, если не идентификатор, 1 - если идентификатор, и -1, если идентификатор с отрицанием
    bool isValidNegationNode(const ExpressionNodeType &node) const
    {
        auto nodeValueKind = getExpressionItemKind(node.nodeValue);
        if (nodeValueKind!=Kind::opNot) 
            return false;

        if (node.argList.size()!=1)
            return false; // Отрицание, но некорректное число потомков

        return true;
    }

    bool convertNegativeOperandsImpl(ExpressionNodeType &node) const
    {
        bool res = false;

        for(auto &childNode: node.argList)
        {
            if (convertNegativeOperandsImpl(childNode))
                res = true;
        }

        auto nodeValueKind = getExpressionItemKind(node.nodeValue);

        if (nodeValueKind!=Kind::opAnd && nodeValueKind!=Kind::opOr)
            return res;

        bool allNegatives = true;

        for(auto &child: node.argList)
        {
            if (!isValidNegationNode(child))
            {
                allNegatives = false;
                break;
            }
        }

        if (!allNegatives)
            return res;

        // node = ExpressionNodeType(parenthesesNode.argList[0]);

        invertNodeAndOrOperator(node, nodeValueKind);

        for(auto &child: node.argList)
        {
            child = ExpressionNodeType(child.argList[0]);
        }

        return res;
    }


    // Окружаем узел скобками
    void addParenthesesImpl(ExpressionNodeType &node) const
    {
        auto exprNode = node;

        auto &opNode = std::get<OperatorType>(node.nodeValue);
        opNode.value = m_opTraits.openBracketOp;
        opNode.text  = "(";

        node.argList.clear();
        node.argList.push_back(exprNode);
    }

    bool addRequiredParenthesesImpl(ExpressionNodeType &node) const
    {
        bool res = false;

        for(auto &childNode: node.argList)
        {
            if (addRequiredParenthesesImpl(childNode))
                res = true;
        }

        auto nodeValueKind = getExpressionItemKind(node.nodeValue);

        if (nodeValueKind!=Kind::opAnd /* && nodeValueKind!=Kind::opNot */ )
            return res;

        // if (nodeValueKind==Kind::opOr)
        //     return res;

        for(auto &child: node.argList)
        {
            auto childKind = getExpressionItemKind(child.nodeValue);
            if (childKind!=Kind::opOr)
                continue;
            addParenthesesImpl(child);
            res = true;
        }

        return res;
    }

    StringType internalGetNodeTextForSorting(const ExpressionNodeType &node) const
    {
        auto nodeKind = getExpressionItemKind(node.nodeValue);

        if (nodeKind==Kind::tokenIdent || nodeKind==Kind::opNot)
        {
            // А нужно ли отрицание добавлять при формировании строки для сорта?
            // Всё равно, в симплифицированном выражении прямое и обратное вхождения невозможны
            return getIdentNodeString(node); // оно само умеет в NOT
        }

        // Тут что-то может пойти не так, но обычно у приоритизирующих скобок
        // ровно один аргумент. Ассерты лень вставлять
        if (nodeKind==Kind::opOpen || nodeKind==Kind::opClose)
           return StringType("(") + internalGetNodeTextForSorting(node.argList[0]) + StringType(")");

        // Тут бы лучше, если вокруг OR были уже расставлены приоритизирующие скобки.
        // Но если их нет, то наверное пережить можно.
        // А лучше - явно добавить, просто будут двойные скобки, если что
        if (nodeKind==Kind::opAnd || nodeKind==Kind::opOr)
        {
            StringType res;
            for(auto &childNode: node.argList)
            {
                if (!res.empty())
                {
                    res += StringType(nodeKind==Kind::opAnd ? "&" : "|");
                }

                res.append(internalGetNodeTextForSorting(childNode));

            }
            return StringType("(") + res + StringType(")");
        }

        if (nodeKind==Kind::tokenFalse)
           return StringType("0");

        if (nodeKind==Kind::tokenTrue)
           return StringType("1");

        return StringType(); // Что-то пошло не так
    }

    // internalGetNodeTextForSorting

    void sortArgumentsImpl(ExpressionNodeType &node) const
    {
        //bool res = false;

        for(auto &childNode: node.argList)
        {
            auto childKind = getExpressionItemKind(childNode.nodeValue);
            if (childKind==Kind::tokenIdent || childKind==Kind::opNot)
                continue; // идентификатор и отрицание сортировать бесполезно

            sortArgumentsImpl(childNode);
            // if (sortArgumentsImpl(childNode))
            //     res = true;
        }

        auto nodeLess = [&](const ExpressionNodeType &n1, const ExpressionNodeType &n2)
        {
            auto nk1 = getExpressionItemKind(n1.nodeValue);
            auto nk2 = getExpressionItemKind(n2.nodeValue);
        
            if (nk1==nk2)
                return internalGetNodeTextForSorting(n1) < internalGetNodeTextForSorting(n2);

            if ( (nk1==Kind::tokenIdent || nk1==Kind::opNot)
              && (nk2==Kind::tokenIdent || nk2==Kind::opNot)
               )
                return internalGetNodeTextForSorting(n1) < internalGetNodeTextForSorting(n2);

            return (unsigned)nk1 < (unsigned)nk2;
        };

        std::sort(node.argList.begin(), node.argList.end(), nodeLess);
    }

    PositionInfoType getPositionInfo(const ExpressionItemType &v) const
    {
           return getExpressionItemPositionInfo(v);
    }

    StringType getExpressionItemText(const ExpressionItemType &v) const
    {
        return getExpressionItemString(v);
    }

    static
    StringType ltrim(const StringType &s)
    {
        using CharType = typename StringType::value_type;
        auto it = s.begin();
        for(; it!=s.end() && *it==(CharType)' '; ++it) {}
        return StringType(it, s.end());
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

        Kind k = Kind::unknown;
        try
        {
            // Kind 
            k = getExpressionItemKind(node.nodeValue);
        }
        catch(...)
        {
        }

        std::string label;
        switch(k)
        {
            case Kind::tokenIdent: label = std::string(utils::expressionItemToString(node.nodeValue).c_str()); break;
            case Kind::opOpen    : label = "( )"    ; break; // label = "(...)"
            case Kind::opClose   : label = "( )"    ; break; // label = "(...)"
            case Kind::opNot     : label = "NOT"    ; break;
            case Kind::opAnd     : label = "AND"    ; break;
            case Kind::opOr      : label = "OR"     ; break;
            case Kind::tokenFalse: label = "<False>"; break;
            case Kind::tokenTrue : label = "<True>" ; break;
            case Kind::unknown: [[fallthrough]];
            default: label = "<UNKNOWN>";
        }

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
            oss.flush();
            gvGraph(oss, a, nextBase);
            nextBase += countGraphNodes(a);
        }

        oss.flush();
    }


public: // ctors

    SimpleBoolExpressionEvaluator(const OperatorTraitsType &opTraits) : m_opTraits(opTraits)
    {
    }

    SimpleBoolExpressionEvaluator(const SimpleBoolExpressionEvaluator &) = default;
    SimpleBoolExpressionEvaluator(SimpleBoolExpressionEvaluator &&) = default;


public: // assign

    SimpleBoolExpressionEvaluator& operator=(const SimpleBoolExpressionEvaluator &) = default;
    SimpleBoolExpressionEvaluator& operator=(SimpleBoolExpressionEvaluator &&) = default;


public: // simplifications


    ExpressionNodeType performConstantAbsorption(const ExpressionNodeType &node) const
    {
        auto res = node;
        performConstantAbsorptionImpl(res);
        return res;
    }

    ExpressionNodeType makeMultiAry(const ExpressionNodeType &node) const
    {
        auto res = node;
        makeMultiAryImpl(res);
        return res;
    }

    ExpressionNodeType collapseNestedParentheses(const ExpressionNodeType &node) const
    {
        auto res = node;
        collapseNestedParenthesesImpl(res);
        return res;
    }

    ExpressionNodeType removeAllParentheses(const ExpressionNodeType &node) const
    {
        auto res = node;
        removeAllParenthesesImpl(res);
        return res;
    }

    ExpressionNodeType promoteNegations(const ExpressionNodeType &node) const
    {
        auto res = node;
        promoteNegationsImpl(res);
        return res;
    }

    ExpressionNodeType removeUnnecessaryParentheses(const ExpressionNodeType &node) const
    {
        auto res = node;
        removeUnnecessaryParenthesesImpl(res);
        return res;
    }

    ExpressionNodeType collapseSameVarsAndObvioslyConstants(const ExpressionNodeType &node) const
    {
        auto res = node;
        collapseSameVarsAndObvioslyConstantsImpl(res);
        return res;
    }

    ExpressionNodeType addRequiredParentheses(const ExpressionNodeType &node) const
    {
        auto res = node;
        addRequiredParenthesesImpl(res);
        return res;
    }

    ExpressionNodeType convertNegativeOperands(const ExpressionNodeType &node) const
    {
        auto res = node;
        convertNegativeOperandsImpl(res);
        return res;
    }

    ExpressionNodeType sortArguments(const ExpressionNodeType &node) const
    {
        auto res = node;
        sortArgumentsImpl(res);
        return res;
    }

    // Simplification is not minimization
    ExpressionNodeType simplify(const ExpressionNodeType &node) const
    {
        auto res = node;

        bool done = false;
        while(!done)
        {
            bool bContinue = false;

            if (performConstantAbsorptionImpl(res))
                bContinue = true;

            if (makeMultiAryImpl(res))
                bContinue = true;

            // if (collapseNestedParenthesesImpl(res))
            //     bContinue = true;

            if (removeAllParenthesesImpl(res))
                bContinue = true;

            if (promoteNegationsImpl(res))
                bContinue = true;

            if (removeUnnecessaryParenthesesImpl(res))
                bContinue = true;

            if (collapseSameVarsAndObvioslyConstantsImpl(res))
                bContinue = true;

            if (convertNegativeOperandsImpl(res))
                bContinue = true;

            done = !bContinue;
        }

        collapseNestedParenthesesImpl(res);
        sortArgumentsImpl(res);
        addRequiredParenthesesImpl(res);

        return res;
    }

    // To get DNF (Disjunctive Normal Form) or CNF (Conjunctive Normal Form), you need to use distributive laws (distributive laws).
    // distributivity of OR with respect to AND:
    // дистрибутивность И относительно ИЛИ: A & (B ∨ C) ≡ (A & B) ∨ (A & C)
    // дистрибутивность ИЛИ относительно И: A ∨ (B & C) ≡ (A ∨ B) & (A ∨ C)

    // Тестовая формула: o|p|q | (a1|b1&c)&(a2|b2|e&!f&g&h) | e&f&g&h
    // дистрибутивность И относительно ИЛИ: (a1|b1&c)&(a2|b2|e&!f&g&h) = (a1|b1&c)&a2 | (a1|b1&c)&b2 | (a1|b1&c)&(e&!f&g&h)

    // дистрибутивность ИЛИ относительно И

    //       OR                 AND                      AND
    //     /    \            /       \          /      /     \       \
    //   AND    AND         OR       OR       OR      OR      OR      OR
    //  /  \   /  \        /  \     /  \     /  \    /  \    /  \    /  \
    //  A  B   C  D      AND   C  AND   D    A  C    B  C    A  D    B  D
    //                  /  \     /  \    
    //                  A  B     A  B    
      
    // (A&B) | (C&D) = (A&B)|C & (A&B)|D = A|C & B|C & A|D & B|D
    // (A&B)|C = A|C & B|C
    // (A&B)|D = A|D & B|D

    // a1 | b1&c = a1|b1 & a1|c
    //  
    //     OR
    //    /  \
    //  a1   AND
    //      /   \
    //     b1   c
    //  


public: // members


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

    void getExpressionVars(ExpressionNodeType node, std::set<StringType> &s) const
    {
        s = getExpressionVars(node);
    }

    void getExpressionVars(ExpressionNodeType node, std::vector<StringType> &v) const
    {
        v.clear();
        auto s = getExpressionVars(node);
        v.insert(v.end(), s.begin(), s.end());
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
                // И правильно сделал!!!
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

        // Убрать try
        try
        {
            gvGraph(oss, n, 0);
        }
        catch(...)
        {}

        oss << "}\n";
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

