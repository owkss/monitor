#include "executable.h"
#include "application.h"

#include <QDebug>

Executable::Executable(const QString &path, QObject *parent)
    : QObject(parent)
{
    const QFileInfo fi(path);
    m_path = fi.absolutePath();
    m_executable_name = fi.fileName();

    this->moveToThread(&m_thread);
    connect(&m_thread, &QThread::started, this, &Executable::execute);
    connect(&m_thread, &QThread::finished, this, &Executable::cleanup);

    m_thread.start();
}

Executable::~Executable()
{
    m_thread.quit();
    m_thread.wait(30000);
}

void Executable::cleanup()
{
    if (m_process)
    {
#ifdef _WIN32
        //const char *command = QString("taskkill /f /pid %1 /t").arg(m_process->processId()).toLocal8Bit().data();
        //std::system(command);
#endif

        m_process->kill();
        m_process->waitForFinished();

        delete m_process;
        m_process = nullptr;
    }

    if (m_sm)
    {
        if (m_sm->isAttached())
            m_sm->detach();

        delete m_sm;
        m_sm = nullptr;
    }

    if (m_timer)
    {
        delete m_timer;
        m_timer = nullptr;
    }
}

void Executable::execute()
{
    m_process = new QProcess(this);

    connect(m_process, &QProcess::started, this, &Executable::started);
    connect(m_process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &Executable::finished);
    connect(m_process, &QProcess::readyReadStandardOutput, this, &Executable::read_stdout);
    connect(m_process, &QProcess::readyReadStandardError, this, &Executable::read_stderr);
    connect(m_process, &QProcess::errorOccurred, this, &Executable::error_occured);

    m_process->setProgram(executable_path());
    m_process->setWorkingDirectory(program_path()); /* Рабочая директория вызываемого процесса будет рабочей директорией вызывающего. На виндовсе из-за этого не читаются стандартные каналы */
    m_process->start();
}

void Executable::value_changed(int index, QVariant new_value)
{
    if (index < 0 || index >= m_variables.size())
    {
        std::cerr << "incorrect index " << index << std::endl;
        return;
    }

    m_sm->lock();

    uint8_t *d = reinterpret_cast<uint8_t*>(m_sm->data());
    std::memcpy(reinterpret_cast<uint8_t*>(d), reinterpret_cast<const uint8_t*>(&READ_VALUES_FLAG), 4);

    const int byte_index = index / 8;
    const int index_in_byte = index % 8;
    *reinterpret_cast<uint8_t*>(d + 4 + 4 + byte_index) = static_cast<uint8_t>(1 << index_in_byte);

    const Variable &v = m_variables.at(index);
    switch (v.type)
    {
    case Variable::CHAR:
    {
        const int8_t value = new_value.toInt();
        std::memcpy(d + SERVICE_DATA_OFFSET + v.offset, reinterpret_cast<const uint8_t*>(&value), std::size_t(v.type_size));
    }
        break;
    case Variable::UCHAR:
    {
        const uint8_t value = new_value.toUInt();
        std::memcpy(d + SERVICE_DATA_OFFSET + v.offset, reinterpret_cast<const uint8_t*>(&value), std::size_t(v.type_size));
    }
        break;
    case Variable::SHORT:
    {
        const int16_t value = new_value.toInt();
        std::memcpy(d + SERVICE_DATA_OFFSET + v.offset, reinterpret_cast<const uint8_t*>(&value), std::size_t(v.type_size));
    }
        break;
    case Variable::USHORT:
    {
        const uint16_t value = new_value.toUInt();
        std::memcpy(d + SERVICE_DATA_OFFSET + v.offset, reinterpret_cast<const uint8_t*>(&value), std::size_t(v.type_size));
    }
        break;
    case Variable::INT:
    {
        const int32_t value = new_value.toInt();
        std::memcpy(d + SERVICE_DATA_OFFSET + v.offset, reinterpret_cast<const uint8_t*>(&value), std::size_t(v.type_size));
    }
        break;
    case Variable::UINT:
    {
        const uint32_t value = new_value.toUInt();
        std::memcpy(d + SERVICE_DATA_OFFSET + v.offset, reinterpret_cast<const uint8_t*>(&value), std::size_t(v.type_size));
    }
        break;
    case Variable::LONG:
    {
        const qint64 value = new_value.toLongLong();
        std::memcpy(d + SERVICE_DATA_OFFSET + v.offset, reinterpret_cast<const uint8_t*>(&value), std::size_t(v.type_size));
    }
        break;
    case Variable::ULONG:
    {
        const quint64 value = new_value.toULongLong();
        std::memcpy(d + SERVICE_DATA_OFFSET + v.offset, reinterpret_cast<const uint8_t*>(&value), std::size_t(v.type_size));
    }
        break;
    case Variable::FLOAT:
    {
        const float value = new_value.toFloat();
        std::memcpy(d + SERVICE_DATA_OFFSET + v.offset, reinterpret_cast<const uint8_t*>(&value), std::size_t(v.type_size));
    }
        break;
    case Variable::DOUBLE:
    {
        const double value = new_value.toDouble();
        std::memcpy(d + SERVICE_DATA_OFFSET + v.offset, reinterpret_cast<const uint8_t*>(&value), std::size_t(v.type_size));
    }
        break;
    default:
        std::cerr << "Unknown type" << std::endl;
        break;
    } // end switch type

    m_sm->unlock();
}

void Executable::started()
{
    m_pid = m_process->processId();
    setObjectName(QString("%1").arg(m_pid));

    emit executable_started();
    QTimer::singleShot(500, this, &Executable::read_settings_file); /* Этого времени должно хватить для создания файла */
}

void Executable::error_occured(QProcess::ProcessError error)
{
    switch (error)
    {
    case QProcess::FailedToStart:
        std::cerr << "FailedToStart" << std::endl;
        break;
    case QProcess::Crashed:
        std::cerr << "Crashed" << std::endl;
        break;
    case QProcess::Timedout:
        std::cerr << "Timedout" << std::endl;
        break;
    case QProcess::WriteError:
        std::cerr << "WriteError" << std::endl;
        break;
    case QProcess::ReadError:
        std::cerr << "ReadError" << std::endl;
        break;
    case QProcess::UnknownError:
        std::cerr << "UnknownError" << std::endl;
        break;
    }
}

void Executable::finished(int exit_code, QProcess::ExitStatus status)
{
    switch (status)
    {
    case QProcess::NormalExit:
        emit executable_finished(exit_code);
        break;
    case QProcess::CrashExit:
        emit executable_error(m_process ? m_process->errorString() : "Unknown error");
        break;
    }

    QTimer::singleShot(0, this, &Executable::cleanup);
}

void Executable::read_stdout()
{
    const QByteArray msg = m_process->readAllStandardOutput();
    emit stdout_msg(msg);
}

void Executable::read_stderr()
{
    const QByteArray msg = m_process->readAllStandardError();
    emit stderr_msg(msg);
}

void Executable::read_settings_file()
{
    if (!m_process || !is_running())
    {
        std::cerr << "failed to start" << std::endl;
        return;
    }

#if 0 //_WIN32
    QDir dir(Application::applicationDirPath());
    if (dir.absolutePath().section('/', -1).compare("debug", Qt::CaseInsensitive) || dir.absolutePath().section('/', -1).compare("release", Qt::CaseInsensitive))
    {
        dir.cdUp();
    }
#else
    QDir dir(program_path());
#endif

    QFile file(dir.absolutePath() + "/variables.dat");
    if (!file.open(QIODevice::ReadOnly))
    {
        std::cerr << "variables.dat does not exists or internal error" << std::endl;
        cleanup();
        return;
    }

    int line_number = 0;
    int prev_index = -1;
    int expected_offset = 0;
    while (!file.atEnd())
    {
        const QByteArray line = file.readLine();
        if (!line.size())
        {
            std::cerr << "empty line" << std::endl;
            continue;
        }

        ++line_number;
        const QByteArrayList parts = line.split('|');
        if (parts.size() == 5)
        {
            const char *var_name = parts.at(0).data();
            const char *var_type = parts.at(1).data();
            const int type_size  = parts.at(2).toInt();
            const int index      = parts.at(3).toInt();
            const int offset     = parts.at(4).toInt();

            if (prev_index + 1 != index)
            {
                std::cerr << "index out of order " << index << std::endl;
                cleanup();
                return;
            }

            if (expected_offset != offset)
            {
                std::cerr << "unexpected offset " << offset << std::endl;
                cleanup();
                return;
            }

            ++prev_index;
            expected_offset += type_size;

            Variable v(var_name, var_type, type_size, index, offset);
            if (v.valid())
            {
                m_variables.push_back(std::move(v));
            }
            else
            {
                std::cerr << "variable \"" << var_name << "\" is not valid" << std::endl;
            }
        }
        else
        {
            std::cerr << "invalid line " << line_number << std::endl;
        } // end if size == 5
    } // end while

    if (m_variables.empty())
    {
        std::cerr << "empty variable's list" << std::endl;
        cleanup();
        return;
    }

    /* Подключение к общей памяти */
    m_sm = new QSharedMemory(pid_str(), this);
    if (!m_sm->attach())
    {
        std::cerr << "attaching to shared memory failed" << std::endl;
        cleanup();
        return;
    }

    /* Запуск таймера для считывания значений */
    m_timer = new QTimer(this);
    m_timer->setTimerType(Qt::PreciseTimer);
    m_timer->setInterval(250);
    connect(m_timer, &QTimer::timeout, this, &Executable::read_shared_memory);

    m_timer->start();
}

void Executable::read_shared_memory()
{
    if (!m_sm || !m_sm->isAttached())
        return;

    QList<Variable> values;

    m_sm->lock();

    uint8_t *d = reinterpret_cast<uint8_t*>(m_sm->data());
    uint32_t command_word = *reinterpret_cast<uint32_t*>(d);

    switch (command_word)
    {
    case READ_VALUES_FLAG:
        break;
    case SET_VALUES_FLAG: /* Чтение данных из библиотек */
        for (Variable &v : m_variables)
        {
            switch (v.type)
            {
            case Variable::CHAR:
            {
                int8_t value = 0;
                std::memcpy(reinterpret_cast<uint8_t*>(&value), d + SERVICE_DATA_OFFSET + v.offset, std::size_t(v.type_size));

                v.value = value;
            }
                break;
            case Variable::UCHAR:
            {
                uint8_t value = 0;
                std::memcpy(reinterpret_cast<uint8_t*>(&value), d + SERVICE_DATA_OFFSET + v.offset, std::size_t(v.type_size));

                v.value = value;
            }
                break;
            case Variable::SHORT:
            {
                int16_t value = 0;
                std::memcpy(reinterpret_cast<uint8_t*>(&value), d + SERVICE_DATA_OFFSET + v.offset, std::size_t(v.type_size));

                v.value = value;
            }
                break;
            case Variable::USHORT:
            {
                uint16_t value = 0;
                std::memcpy(reinterpret_cast<uint8_t*>(&value), d + SERVICE_DATA_OFFSET + v.offset, std::size_t(v.type_size));

                v.value = value;
            }
                break;
            case Variable::INT:
            {
                int32_t value = 0;
                std::memcpy(reinterpret_cast<uint8_t*>(&value), d + SERVICE_DATA_OFFSET + v.offset, std::size_t(v.type_size));

                v.value = value;
            }
                break;
            case Variable::UINT:
            {
                uint32_t value = 0;
                std::memcpy(reinterpret_cast<uint8_t*>(&value), d + SERVICE_DATA_OFFSET + v.offset, std::size_t(v.type_size));

                v.value = value;
            }
                break;
            case Variable::LONG:
            {
                qint64 value = 0;
                std::memcpy(reinterpret_cast<uint8_t*>(&value), d + SERVICE_DATA_OFFSET + v.offset, std::size_t(v.type_size));

                v.value = value;
            }
                break;
            case Variable::ULONG:
            {
                quint64 value = 0;
                std::memcpy(reinterpret_cast<uint8_t*>(&value), d + SERVICE_DATA_OFFSET + v.offset, std::size_t(v.type_size));

                v.value = value;
            }
                break;
            case Variable::FLOAT:
            {
                float value = 0;
                std::memcpy(reinterpret_cast<uint8_t*>(&value), d + SERVICE_DATA_OFFSET + v.offset, std::size_t(v.type_size));

                v.value = value;
            }
                break;
            case Variable::DOUBLE:
            {
                double value = 0;
                std::memcpy(reinterpret_cast<uint8_t*>(&value), d + SERVICE_DATA_OFFSET + v.offset, std::size_t(v.type_size));

                v.value = value;
            }
                break;
            default:
                std::cerr << "Unknown type" << std::endl;
                break;
            } // end switch type

            values.push_back(v);

        } // end for
        break;
    }

    m_sm->unlock();

    if (m_variables.size() == values.size())
        emit data_ready(values);
    else
        std::cerr << "size not equal" << std::endl; /* Это сообщение также появляется, если библиотека не успела прочитать данные и командное слово это установка значений */

    return;

    QByteArray msg;
    msg = m_process->readAllStandardOutput();
    if (!msg.isEmpty())
        emit stdout_msg(msg);
    msg = m_process->readAllStandardError();
    if (!msg.isEmpty())
        emit stderr_msg(msg);
}
