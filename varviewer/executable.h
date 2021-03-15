#ifndef EXECUTABLE_H
#define EXECUTABLE_H

#include "../global.h"
#include "variable.h"

#include <memory>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>

#include <QDir>
#include <QFile>
#include <QTimer>
#include <QThread>
#include <QObject>
#include <QVariant>
#include <QProcess>
#include <QSharedMemory>

class Executable : public QObject
{
    Q_OBJECT
public:
    Executable(const QString &path, QObject *parent = nullptr);
    ~Executable();

    qint64 pid() const { return m_pid; }
    QString pid_str() const { return m_pid > 0 ? QString("%1").arg(m_pid) : QString(); }
    QString program() const { return m_executable_name; }
    QString program_path() const { return m_path; }
    QString executable_path() const { return QDir::toNativeSeparators(m_path + '/' + m_executable_name); }
    bool is_running() const { return m_process ? m_process->state() == QProcess::Running : false; }

private:
    qint64 m_pid = -1;
    QString m_path;
    QString m_executable_name;

    QTimer *m_timer = nullptr;
    QProcess *m_process = nullptr;
    QSharedMemory *m_sm = nullptr;
    QThread m_thread;

    QList<Variable> m_variables;

    void cleanup();

public slots:
    void execute();
    void value_changed(int index, QVariant new_value);

private slots:
    void started();
    void error_occured(QProcess::ProcessError error);
    void finished(int exit_code, QProcess::ExitStatus status);
    void read_stdout();
    void read_stderr();

    void read_settings_file();
    void read_shared_memory();

signals:
    void executable_started();
    void executable_finished(int exit_code);
    void executable_error(const QString &error_string);

    void stdout_msg(const QString &msg);
    void stderr_msg(const QString &msg);

    void data_ready(QList<Variable> data);
};

#endif // EXECUTABLE_H
