#ifndef VAR_H
#define VAR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)
    #if defined(VAR_LIBRARY)
        #define VAR_LIBSPEC __declspec(dllexport)
    #else
        #define VAR_LIBSPEC /* При динамической компоновке добавить __declspec(dllimport) */
    #endif
#else
    #if defined(__GNUC__) && (__GNUC__ >= 4)
        #define VAR_LIBSPEC __attribute__((visibility("default")))
    #else
        #define VAR_LIBSPEC
    #endif
#endif // _WIN32

/**
 * @brief Добавляет во внутренний список указатель на переменную var_ptr типа var_type под именем var_name
 * @param var_ptr - указатель на переменную
 * @param var_name - имя переменной, отображаемое в мониторе
 * @param var_type - тип переменной
 * @return Не ноль в случае ошибки
 */
int add_variable(void *var_ptr, const char *var_name, const char *var_type);

/**
 * @brief Начало работы. Все последующие вызовы add_variable игнорируются
 * @return Не ноль в случае ошибки
 */
int init();

#ifdef __cplusplus
}
#endif


#endif // VAR_H
