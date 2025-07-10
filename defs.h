/*! \file
    \brief Definitions for marty_format
 */


#pragma once

// #ifndef MARTY_UTF_MARTY_DECIMAL_H
// #define MARTY_UTF_MARTY_DECIMAL_H

// #ifndef MARTY_UTF_ASSERT_FAIL
//  
//     //#if defined(UMBA_ASSERT_FAIL)
//  
//         #define MARTY_UTF_ASSERT_FAIL(msg)     throw std::runtime_error(msg)
//  
//     //#else
//  
//     //    #define MARTY_UTF_ASSERT_FAIL(msg)    UMBA_ASSERT_FAIL(msg)
//  
//     //#endif
//  
// #endif
//  
//----------------------------------------------------------------------------
#ifndef MARTY_ARG_USED

    //! Подавление варнинга о неиспользованном аргументе
    #define MARTY_ARG_USED(x)                   (void)(x)

#endif


#if !defined(USE_MARTY_BIGINT)
    #define USE_MARTY_BIGINT 1
#endif

#if !defined(USE_MARTY_DECIMAL)
    #define USE_MARTY_DECIMAL 1
#endif


#if defined(USE_MARTY_BIGINT) && USE_MARTY_BIGINT!=0
    #include "marty_bigint/marty_bigint.h"
    #if !defined(MARTY_EXPRESSIONS_MARTY_BIGINT_USED)
        #define MARTY_EXPRESSIONS_MARTY_BIGINT_USED
    #endif
#else
    #if defined(MARTY_EXPRESSIONS_MARTY_BIGINT_USED)
        #undef MARTY_EXPRESSIONS_MARTY_BIGINT_USED
    #endif
#endif

#if defined(USE_MARTY_DECIMAL) && USE_MARTY_DECIMAL!=0
    #include "marty_decimal/marty_decimal.h"
    #if !defined(MARTY_EXPRESSIONS_MARTY_DECIMAL_USED)
        #define MARTY_EXPRESSIONS_MARTY_DECIMAL_USED
    #endif
#else
    #if defined(MARTY_EXPRESSIONS_MARTY_DECIMAL_USED)
        #undef MARTY_EXPRESSIONS_MARTY_DECIMAL_USED
    #endif
#endif
