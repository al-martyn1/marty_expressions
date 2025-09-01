/*!
    \file
    \brief Библиотека marty::expressions - таблица истинности (для использования с логическими выражениями)
 */

/*

a   0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1
b   0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1
c   0 0 0 0 1 1 1 1 0 0 0 0 1 1 1 1
d   0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1
Fn  X X X X X X X X X X X X X X X X


Для вывода таблицы истинности в горизонтальном виде надо:
- для каждой переменной в цикле от 0 до stateMax() получить и вывести значение переменной
  Значение переменной извлекаем при помощи ф-ии varGetValue, которая извлекает бит переменной из вектора значений.

*/


#pragma once

//
#include <cstdint>
#include <map>
#include <stdexcept>
#include <string>
#include <set>
#include <unordered_map>
#include <vector>

//----------------------------------------------------------------------------
// marty::expressions::
namespace marty {
namespace expressions {

//----------------------------------------------------------------------------

/*

Переменные храним в векторе строк.
Результат храним в векторе результатов (неожиданно, да?). Или в мапе?
Индекс результата побитно определяет значения переменных: бит 0 - значение первой переменной, бит 1 - значение второй переменной, и тп.
Всего элементов в таблице истинности - 2**N, где N - количество переменных
63 переменных - максимум. В здравом уме логические выражения с 63+ входными переменными не могут даже привидится.

*/

template<typename StringTypeT>
struct TruthTable
{

    using StringType         = StringTypeT   ;
    using string_type        = StringTypeT   ;

    using VarsStateType      = std::uint64_t ;
    using vars_state_type    = std::uint64_t ;

    using PayloadType        = std::uintptr_t;
    using payload_type       = std::uintptr_t;

    // Logic function result
    struct Result
    {
        bool           value   = false;
        PayloadType    payload = 0;
    };


protected:

    std::vector<StringType>                           m_vars;
    std::unordered_map<StringType, std::size_t >      m_varIndices; // Для поиска индекса переменной
    //VarsStateType                                     m_curVals = 0; // Текущие значения переменных - а нужно ли?

    std::unordered_map<VarsStateType, Result>         m_results; // Значения функции


public: // ctors

    TruthTable() = default;
    TruthTable(const TruthTable &) = default;
    TruthTable(TruthTable &&) = default;

    template<typename IterType>
    TruthTable(IterType b, IterType e)
    {
        for(; b!=e; ++b)
           varAdd(*b);
    }

public: // assign

    TruthTable& operator=(const TruthTable &) = default;
    TruthTable& operator=(TruthTable &&) = default;


public:

    // А нужно ли?
    // VarsStateType varsStateGet() const { return m_curVals; }
    // void varsStateReset() { m_curVals = 0; }
    // void varsStateNext()  { ++m_curVals; }

    VarsStateType varsStateMax(const std::vector<StringType> &vars) const
    {
        if (vars.size()>63u)
            throw std::runtime_error("marty::expressions::TruthTable::varsStateMax: too many vars");

        return VarsStateType(1) << int(vars.size());
    }

    VarsStateType varsStateMax() const
    {
        return varsStateMax(m_vars);
    }

    VarsStateType varGetMask(std::size_t varIdx, const std::vector<StringType> &vars) const
    {
        if (varIdx >= vars.size())
            throw std::runtime_error("marty::expressions::TruthTable::varGetMask: too big var index");
        return std::uint64_t(1) << int(varIdx);
    }
    
    VarsStateType varGetMask(std::size_t varIdx) const
    {
        return varGetMask(varIdx, m_vars);
    }
    
    //! Возвращает максимальную длину имени переменной
    std::size_t varGetMaxLen(const std::vector<StringType> &vars) const
    {
        std::size_t m = 0;
        for(const auto & n : vars)
        {
            if (n.size()>m)
                m = n.size();
        }

        return m;
    }

    //! Возвращает максимальную длину имени переменной из набора TT
    std::size_t varGetMaxLen() const
    {
        return varGetMaxLen(m_vars);
    }

    std::size_t       varGetNumber(const std::vector<StringType> &vars) const { return vars.size(); }
    std::size_t       varGetNumber() const { return varGetNumber(m_vars); }

    const StringType& varGetName(std::size_t varIdx, const std::vector<StringType> &vars) const { return vars[varIdx]; }
    const StringType& varGetName(std::size_t varIdx) const { return varGetName(m_vars, varIdx); }

    std::size_t varGetNameFillSize(const StringType &vn, std::size_t maxNameLen) const
    {
        if (vn.size()>maxNameLen)
            return 0u;
        return maxNameLen - vn.size();
    }

    std::size_t varGetNameFillSize(std::size_t varIdx, std::size_t maxNameLen) const
    {
        return varGetNameFillSize(varGetName(varIdx), maxNameLen);
    }

    StringType varGetNameFill(const StringType &vn, std::size_t maxNameLen) const
    {
        auto fillSz = varGetNameFillSize(vn, maxNameLen);
        return StringType(fillSz, ' ');
    }

    // const std::vector<StringType> &vars

    std::size_t varGetIndex(const StringType &varName) const
    {
        auto it = m_varIndices.find(varName);
        if (it==m_varIndices.end())
            return std::size_t(-1);
        return it->second;
    }

    std::size_t varGetIndex(const StringType &varName, const std::vector<StringType> &vars) const
    {
        auto it = std::find(vars.begin(), vars.end(), varName);
        if (it==vars.end())
            return std::size_t(-1);
        return it->second;
    }

    bool varGetValue(std::size_t varIdx, VarsStateType varsState) const
    {
        auto mask = varGetMask(varIdx);
        return (varsState & mask) != 0;
    }
    
    bool varGetValue(std::size_t varIdx, VarsStateType varsState, const std::vector<StringType> &vars) const
    {
        auto mask = varGetMask(varIdx, vars);
        return (varsState & mask) != 0;
    }
    
    bool varGetValue(const StringType &varName, VarsStateType varsState) const
    {
        auto it = m_varIndices.find(varName);
        return it==m_varIndices.end() ? false : varGetValue(it->second, varsState);
    }

    bool varGetValue(const StringType &varName, VarsStateType varsState, const std::vector<StringType> &vars) const
    {
        auto it = std::find(vars.begin(), vars.end(), varName);
        return it==vars.end() ? false : varGetValue(std::size_t(it-vars.begin()), varsState, vars);
    }

    VarsStateType varSetValue(std::size_t varIdx, VarsStateType varsState, bool b) const
    {
        auto mask = varGetMask(varIdx);
        return b ? (varsState | mask) : varsState;
    }
    
    VarsStateType varSetValue(const StringType &varName, VarsStateType varsState, bool b) const
    {
        auto it = m_varIndices.find(varName);
        if (it==m_varIndices.end())
            throw std::runtime_error("marty::expressions::TruthTable::varSetValue: unknown variable");
        return varSetValue(*it, varsState, b);
    }

    std::size_t varAdd(const StringType &varName, bool *pNewVar=0)
    {
        if (m_vars.size()>63u)
            throw std::runtime_error("marty::expressions::TruthTable::addVar: too many vars");

        auto it = m_varIndices.find(varName);
        if (it!=m_varIndices.end())
        {
            if (pNewVar)
               *pNewVar = false;
            return it->second;
        }

        auto res = m_vars.size();
        m_varIndices[varName] = res;
        m_vars.emplace_back(varName);

        if (pNewVar)
           *pNewVar = true;

        return res;
    }


    void resultClear() { m_results.clear(); }

    int resultGet(VarsStateType varsState) const
    {
        auto it = m_results.find(varsState);
        if (it==m_results.end())
            return -1;

        return it->second.value ? 1 : 0;
    }

    char resultGetChar(VarsStateType varsState) const
    {
        auto r = resultGet(varsState);
        if (r<0)
            return 'X';
        return r>0 ? '1' : '0';
    }

    // Возвращает true, если результат был добавлен, и false - если обновлен
    bool resultSet(VarsStateType varsState, bool res, PayloadType plt, PayloadType *pPltPrev=0)
    {
        auto it = m_results.find(varsState);
        if (it!=m_results.end())
        {
            // перезапись
            if (pPltPrev)
                *pPltPrev = it->second.payload;

            it->second.value    = res;
            it->second.payload  = plt;
            return false;
        }

        m_results[varsState] = Result{res, plt};

        return true;
    }

    bool resultSet(VarsStateType varsState, bool res)
    {
        return resultSet(varsState, res, PayloadType{0u});
    }

    template<typename ErrorHandler>
    TruthTable merge(const TruthTable &ttOther, ErrorHandler errHandler) const
    {
        auto names = std::set(m_vars.begin(), m_vars.end());
        names.insert(ttOther.m_vars.begin(), ttOther.m_vars.end());

        auto ttRes = TruthTable(names.begin(), names.end());


        VarsStateType stMax = ttRes.varsStateMax();
        VarsStateType st = 0u;

        std::size_t varsNum = ttRes.varGetNumber();

        // Перечисляем все состояния новой таблицы истинности
        for(; st!=stMax; ++st)
        {
            // Нужно создать наборы переменных для каждой из двух таблиц истинности

            VarsStateType stLeft  = 0u;
            VarsStateType stRight = 0u;

            // Перечисляем все переменные в новой таблице истинности
            for(std::size_t iVar=0u; iVar!=varsNum; ++iVar)
            {
                // Нужно получить значение переменной в строчке новой таблици истинности.
                // Индекс переменной у нас уже есть, поиска по имени не нужно
                auto ttResVarValue = ttRes.varGetValue(iVar, st);

                // Для исходных TT нужен поиск по имени

                auto varName = ttRes.varGetName(iVar);

                std::size_t varIdxLeft  = this  ->varGetIndex(varName);
                std::size_t varIdxRight = ttOther.varGetIndex(varName);

                if (varIdxLeft!=std::size_t(-1))
                {
                    stLeft  = this   ->varSetValue(varIdxLeft, stLeft, ttResVarValue);
                }

                if (varIdxRight!=std::size_t(-1))
                {
                    stRight  = ttOther.varSetValue(varIdxRight, stRight, ttResVarValue);
                }
            }

            // Тут у нас есть 
            // st - набор переменных результирующей ТТ
            // stLeft, stRight - наборы пеерменных составляющих ТТ

            int resLeft  = this  ->resultGet(stLeft);
            int resRight = ttOther.resultGet(stRight);

            if (resLeft>=0 && resRight>=0)
            {
                // if (resLeft!=resRight)
                int result = errHandler(st, stLeft, stRight, resLeft, resRight, *this, ttOther);
                if (result>=0)
                {
                    // Если хандлер разрешил, то устанавливаем то значение, которое он вернул
                    ttRes.resultSet(st, result>0);
                }
            }

            if (resLeft>=0)
                ttRes.resultSet(st, resLeft>0);

            if (resRight>=0)
                ttRes.resultSet(st, resRight>0);
        }

        return ttRes;
    }

    TruthTable merge(const TruthTable &ttOther) const
    {
        auto errHandler = [](VarsStateType st, VarsStateType stLeft, VarsStateType stRight, int resLeft, int resRight, const TruthTable &ttLeft, const TruthTable &ttRight)
        {
            MARTY_ARG_USED(st      );
            MARTY_ARG_USED(stLeft  );
            MARTY_ARG_USED(stRight );
            //MARTY_ARG_USED(resLeft );
            //MARTY_ARG_USED(resRight);
            MARTY_ARG_USED(ttLeft  );
            MARTY_ARG_USED(ttRight );
            //return stLeft;
            //return stRight; // новые значения перезаписывают, в случае tt =| ttNew;
            return resRight>=0 ? resRight : resLeft;
        };

        return merge(ttOther, errHandler);
    }

    TruthTable operator|   (const TruthTable &ttOther) const { return  merge(ttOther); }
    TruthTable operator||  (const TruthTable &ttOther) const { return  merge(ttOther); }
    TruthTable operator+   (const TruthTable &ttOther) const { return  merge(ttOther); }
    TruthTable& operator|= (const TruthTable &ttOther)       { *this = merge(ttOther); return *this; }
    // TruthTable& operator||=(const TruthTable &ttOther)       { *this = merge(ttOther); return *this; } // Нет такого оператора в плюсиках
    TruthTable& operator+= (const TruthTable &ttOther)       { *this = merge(ttOther); return *this; }


    template<typename StreamType>
    StreamType& print(StreamType &oss, const std::vector<StringType> &vars) const
    {
        auto maxVarLen  = varGetMaxLen(vars);
        if (maxVarLen<3)
            maxVarLen = 3;

        std::size_t numVars = varGetNumber(vars);
        for(std::size_t varIdx=0; varIdx!=numVars; ++varIdx)
        {
            auto varName = varGetName(varIdx, vars);
            oss <<  /* "    " << */ varName << varGetNameFill(varName, maxVarLen) << " :" << " ";
            VarsStateType vsMax = varsStateMax(vars);
            for(TruthTable::VarsStateType vs=0u; vs!=vsMax; ++vs)
                oss << varGetValue(varIdx, vs, vars);
            oss << "\n";
        }

        return oss;
    }

    template<typename StreamType>
    StreamType& print(StreamType &oss) const
    {
        return print(oss, m_vars);
    }

    // std::vector<StringType>                           m_vars;




}; // struct TruthTable




//----------------------------------------------------------------------------


} // namespace expressions
} // namespace marty

