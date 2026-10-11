#pragma once

#include <cstdio>
#include <cstdlib>
#include <cassert>

#define BOLT_TOSTRINGIMPL(x) #x
#define BOLT_TOSTRING(x) BOLT_TOSTRINGIMPL(x)

#ifdef __FILE_NAME__
#define BOLT_FILE_NAME __FILE_NAME__
#else
#define BOLT_FILE_NAME __FILE__
#endif

#define BOLT_VARGS_( _10, _9, _8, _7, _6, _5, _4, _3, _2, _1, N, ... ) N
#define BOLT_VARGS( ... ) BOLT_VARGS_( __VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 )

#define BOLT_CONCAT_( a, b ) a##b
#define BOLT_CONCAT( a, b ) BOLT_CONCAT_( a, b )

// https://stackoverflow.com/questions/27049491/can-c-c-preprocessor-macros-have-default-parameter-values
#define BOLT_STATIC_ASSERT_2( e, msg ) \
    static_assert( e, "\n\n[" BOLT_FILE_NAME ":" BOLT_TOSTRING( __LINE__ ) "] ASSERT: " msg "\n" )
#define BOLT_STATIC_ASSERT_1( e ) BOLT_STATIC_ASSERT_2( e, #e )
#define BOLT_STATIC_ASSERT( ... ) BOLT_CONCAT( BOLT_STATIC_ASSERT_, BOLT_VARGS( __VA_ARGS__ ) )( __VA_ARGS__ )
#define BOLT_STATIC_ASSERT_MSG( e, msg ) BOLT_STATIC_ASSERT_2( e, msg )

#define BOLT_RUNTIME_ASSERT_2( e, msg ) \
    do \
    { \
        if( !( e ) ) \
        { \
            printf( "[" BOLT_FILE_NAME ":" BOLT_TOSTRING( __LINE__ ) "] ASSERT: %s\n", msg ); \
            exit( EXIT_FAILURE ); \
        } \
    } while( 0 )
#define BOLT_RUNTIME_ASSERT_1( e ) BOLT_RUNTIME_ASSERT_2( e, #e )
#define BOLT_RUNTIME_ASSERT( ... ) BOLT_CONCAT( BOLT_RUNTIME_ASSERT_, BOLT_VARGS( __VA_ARGS__ ) )( __VA_ARGS__ )

#define BOLT_ASSERT( ... ) assert( __VA_ARGS__ );
#define BOLT_ASSERT_MSG( e, msg ) assert((void(msg), e))

#define BOLT_PANIC(m, ...) \
    do { \
        printf("[" BOLT_FILE_NAME ":" BOLT_TOSTRING(__LINE__) "] PANIC: " m "\n", ##__VA_ARGS__); \
        abort(); \
    } while (0)

#ifndef NDEBUG
#define BOLT_DEBUG(m, ...) \
    do { \
        printf("[" BOLT_FILE_NAME ":" BOLT_TOSTRING(__LINE__) "] DEBUG: " m "\n", ##__VA_ARGS__); \
    } while (0)
#else
#define BOLT_DEBUG(m, ...)
#endif

#define BOLT_ERROR(m, ...) \
    do { \
        fprintf(stderr, "[" BOLT_FILE_NAME ":" BOLT_TOSTRING(__LINE__) "] ERROR: " m "\n", ##__VA_ARGS__); \
    } while (0)

#define BOLT_INFO(m, ...) \
    do { \
        printf("[" BOLT_FILE_NAME ":" BOLT_TOSTRING(__LINE__) "] INFO: " m "\n", ##__VA_ARGS__); \
    } while (0)
