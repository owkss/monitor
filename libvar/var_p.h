#ifndef VAR_P_H
#define VAR_P_H

#include "variable.h"

#include <vector>
#include <thread>
#include <iostream>

#include <QSharedMemory>

class VarManager
{
public:
    VarManager();
    ~VarManager();

    int add_variable(void *var_ptr, const char *var_name, const char *var_type, int type_size);
    int init();

private:
    void set_initial_value_and_zero(uint8_t *begin_data_ptr);
    bool create_info_file();

    void read_shared_memory();

    void set_variables_values(uint8_t *begin_data_ptr); /* Установка значений переменных в общую память */
    void read_variables_values(uint8_t *begin_data_ptr); /* Чтение новых значений переменных из общей памяти */

    int m_duration = 100; /* Время ожидания в мс */
    std::atomic_bool m_work{false};
    std::thread m_thread;
    bool was_launched = false;

    int64_t m_pid = 0;
    QSharedMemory m_sm;

    std::vector<Variable> m_variables;
};

#endif // VAR_P_H
