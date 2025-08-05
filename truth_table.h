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

*/


#pragma once

//
#include <cstdint>
#include <map>
#include <stdexcept>
#include <string>
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

    VarsStateType varsStateMax() const
    {
        if (m_vars.size()>63u)
            throw std::runtime_error("marty::expressions::TruthTable::varsStateMax: too many vars");

        return VarsStateType(1) << int(m_vars.size());
    }

    VarsStateType varGetMask(std::size_t varIdx) const
    {
        if (varIdx >= m_vars.size())
            throw std::runtime_error("marty::expressions::TruthTable::varGetMask: too big var index");
        return std::uint64_t(1) << int(varIdx);
    }
    
    std::size_t varGetMaxLen() const
    {
        std::size_t m = 0;
        for(const auto & n : m_vars)
        {
            if (n.size()>m)
                m = n.size();
        }

        return m;
    }

    std::size_t       varGetNumber() const { return m_vars.size(); }
    const StringType& varGetName(std::size_t varIdx) const { return m_vars[varIdx]; }

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


    bool varGetValue(std::size_t varIdx, VarsStateType varsState) const
    {
        auto mask = varGetMask(varIdx);
        return (varsState & mask) != 0;
    }
    
    bool varGetValue(const StringType &varName, VarsStateType varsState) const
    {
        auto it = m_varIndices.find(varName);
        return it==m_varIndices.end() ? false : varGetValue(it->second, varsState);
    }

    std::size_t varAdd(const StringType &varName, bool *pNewVar=0)
    {
        if (m_vars.size()>(63u-1u))
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


}; // struct TruthTable




//----------------------------------------------------------------------------


} // namespace expressions
} // namespace marty

