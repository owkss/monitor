#include "var_p.h"

#include <bitset>
#include <fstream>
#include <sstream>
#include <functional>

#ifdef _WIN32
#   include <windows.h>
#else
#   include <unistd.h>
#   include <sys/syscall.h>
#endif

int VarManager::add_variable(void *var_ptr, const char *var_name, const char *var_type, int type_size)
{
    /* Флаг об инициализации необходим из-за того, что монитор не будет больше читать файл, создаваемый во время инициализации.
     * К тому же, функции чтения и установления значений завязаны на размере массива данных, и его изменение после инициализации некорректно отразится на данных */
    if (was_launched)
        return 1;

    static int offset = 0; /* Смещение добавляемого параметра */
    const int index = static_cast<int>(m_variables.size());

    if (index == 0)
        offset = 0;

    m_variables.emplace_back(var_ptr, var_name, var_type, type_size, index, offset);
    offset += type_size;

    return 0;
}

int VarManager::init()
{
    was_launched = true;

    if (m_variables.empty())
    {
        std::cerr << "there is no added variables" << std::endl;
        return -1;
    }

    if (!m_pid)
    {
        std::cerr << "pid == 0" << std::endl;
        return 1;
    }

    if (!create_info_file())
        return 2;

    m_sm.setKey(QString("%1").arg(m_pid));
    if (m_sm.create(SHARED_MEM_SIZE, QSharedMemory::ReadWrite))
    {
        m_sm.lock();

        std::memset(m_sm.data(), 0, SHARED_MEM_SIZE);
        *reinterpret_cast<uint32_t*>(m_sm.data()) = SET_VALUES_FLAG; /* Первоначальная установка флага */

        m_sm.unlock();
    }
    else
    {
        std::cerr << "creating shared memory failed" << std::endl;
        return 3;
    }

    m_work = true;
    m_thread = std::thread(&VarManager::read_shared_memory, this);

    return m_thread.joinable() ? 0 : 4;
}

VarManager::VarManager()
{
#ifdef _WIN32
    m_pid = (int64_t)GetCurrentProcessId();
#else
    m_pid = (int64_t)syscall(SYS_getpid);
#endif
}

VarManager::~VarManager()
{
    m_work = false;
    if (m_thread.joinable())
        m_thread.join();

    m_sm.unlock();
    if (m_sm.isAttached())
        m_sm.detach();
    m_variables.clear();
}

void VarManager::set_initial_value_and_zero(uint8_t *begin_data_ptr)
{
    std::memset(begin_data_ptr, 0, SERVICE_DATA_OFFSET);
    *reinterpret_cast<uint32_t*>(begin_data_ptr) = SET_VALUES_FLAG;
}

bool VarManager::create_info_file()
{
    std::ofstream out;
    out.open("./variables.dat");
    if (!out)
    {
        std::cerr << "can not create file with settings" << std::endl;
        return false;
    }

    for (const auto &v : m_variables)
    {
        std::stringstream ss;

        ss << v.name << "|" << v.type << "|" << v.type_size << "|" << v.index << "|" << v.offset << "\n";
        out << ss.str();
    }

    out.close();
    return true;
}

void VarManager::read_shared_memory()
{
    while (m_work)
    {
        uint32_t command_word = 0;
        uint32_t value = 0;

        m_sm.lock();

        uint8_t *begin_data_ptr = reinterpret_cast<uint8_t*>(m_sm.data());

        std::memcpy(&command_word, begin_data_ptr, 4);
        std::memcpy(&value, begin_data_ptr + 4, 4);

        switch (command_word)
        {
        case SET_VALUES_FLAG: /* Установка значений в общую память */
            set_variables_values(begin_data_ptr);
            break;
        case READ_VALUES_FLAG: /* Изменение значений в основной программе монитора */
            read_variables_values(begin_data_ptr);
            break;
        default:
            std::cerr << "wrong command word" << std::endl;
            break;
        }

        set_initial_value_and_zero(begin_data_ptr);

        m_sm.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(m_duration));
    }
}

void VarManager::set_variables_values(uint8_t *begin_data_ptr)
{
    for (const auto &v : m_variables)
    {
        std::memcpy(begin_data_ptr + SERVICE_DATA_OFFSET + v.offset, reinterpret_cast<const uint8_t*>(v.ptr), std::size_t(v.type_size));
    }
}

void VarManager::read_variables_values(uint8_t *begin_data_ptr)
{
    uint8_t var_indexes[MAX_VARIABLES] = {0};
    std::memcpy(&var_indexes[0], begin_data_ptr + 4 + 4, MAX_VARIABLES);

    for (std::size_t index = 0; index < MAX_VARIABLES; ++index)
    {
        std::bitset<8> bits(var_indexes[index]);
        for (std::size_t i = 0; i < bits.size(); ++i)
        {
            if (bits.test(i)) /* Установленный бит значит, что параметр с данным индексом изменён */
            {
                const std::size_t parameter_index = i + index * 8;
                if (parameter_index < m_variables.size())
                {
                    Variable &v = m_variables.at(parameter_index);
                    std::memcpy(reinterpret_cast<uint8_t*>(v.ptr), begin_data_ptr + SERVICE_DATA_OFFSET + v.offset, std::size_t(v.type_size));
                }
                else
                {
                    return;
                }
            }
        }
    }
}
