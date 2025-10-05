#pragma once

#include <cstdio>
#include <cstdlib>
#include <cassert>

#define TOSTRINGIMPL(x) #x
#define TOSTRING(x) TOSTRINGIMPL(x)

#ifdef __FILE_NAME__
#define FILE_NAME __FILE_NAME__
#else
#define FILE_NAME __FILE__
#endif

#define VARGS_( _10, _9, _8, _7, _6, _5, _4, _3, _2, _1, N, ... ) N
#define VARGS( ... ) VARGS_( __VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 )

#define CONCAT_( a, b ) a##b
#define CONCAT( a, b ) CONCAT_( a, b )

// https://stackoverflow.com/questions/27049491/can-c-c-preprocessor-macros-have-default-parameter-values
#define STATIC_ASSERT_2( e, msg ) \
    static_assert( e, "\n\n[" FILE_NAME ":" TOSTRING( __LINE__ ) "] ASSERT: " msg "\n" )
#define STATIC_ASSERT_1( e ) STATIC_ASSERT_2( e, #e )
#define STATIC_ASSERT( ... ) CONCAT( STATIC_ASSERT_, VARGS( __VA_ARGS__ ) )( __VA_ARGS__ )
#define STATIC_ASSERT_MSG( e, msg ) STATIC_ASSERT_2( e, msg )

#define RUNTIME_ASSERT_2( e, msg ) \
    do \
    { \
        if( !( e ) ) \
        { \
            printf( "[" FILE_NAME ":" TOSTRING( __LINE__ ) "] ASSERT: %s\n", msg ); \
            exit( EXIT_FAILURE ); \
        } \
    } while( 0 )
#define RUNTIME_ASSERT_1( e ) RUNTIME_ASSERT_2( e, #e )
#define RUNTIME_ASSERT( ... ) CONCAT( RUNTIME_ASSERT_, VARGS( __VA_ARGS__ ) )( __VA_ARGS__ )

#define ASSERT( ... ) assert( __VA_ARGS__ );
#define ASSERT_MSG( e, msg ) assert((void(msg), e))

#define PANIC(m, ...) \
    do { \
        printf("[" FILE_NAME ":" TOSTRING(__LINE__) "] PANIC: " m "\n", ##__VA_ARGS__); \
        abort(); \
    } while (0)

#ifndef NDEBUG
#define DEBUG(m, ...) \
    do { \
        printf("[" FILE_NAME ":" TOSTRING(__LINE__) "] DEBUG: " m "\n", ##__VA_ARGS__); \
    } while (0)
#else
#define DEBUG(m, ...)
#endif
