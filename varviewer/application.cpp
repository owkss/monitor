#include "application.h"

#include <iostream>

#include <QDir>
#include <QFile>
#include <QIcon>
#include <QProcess>
#include <QDateTime>
#include <QStyleFactory>
#include <QStandardPaths>

Application::Application(int &argc, char *argv[]) : QApplication(argc, argv)
{
    setObjectName("monitor_application");
    setStyle(QStyleFactory::create("Fusion"));
    setOrganizationName("Umbrella");
    setApplicationName("Monitor");
    setWindowIcon(QIcon(":/images/oscilloscope.png"));
}

bool Application::notify(QObject *receiver, QEvent *event)
{
    try
    {
        return QApplication::notify(receiver, event);
    }
    catch (std::exception &e)
    {
        save_log(e.what(), receiver, event);
    }
    catch(...)
    {
        save_log("<unknown>", receiver, event);
    }

    return false;
}

void Application::save_log(const char *what, const QObject *receiver, const QEvent *event)
{
    const QDateTime dt = QDateTime::currentDateTime();

    const QString filename = "Monitor_crash_log_" + dt.toString("dd-MM-yyyy_hh-mm-ss") + ".txt";
    const QDir dir(QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first());
    const QString filepath = QDir::toNativeSeparators(dir.absolutePath() + "/" + filename);

    std::FILE *f = std::fopen(filepath.toLocal8Bit().constData(), "w");
    if (f)
    {
        char msg[2048] = {0};
        std::snprintf(msg, sizeof(msg), "Monitor crash log! Time: %s\n\nReason:\n%s\n\n", qPrintable(dt.toString("dd.MM.yyyy hh:mm:ss.zzz")), what);

        if (event != nullptr)
        {
            unsigned long long len = strlen(msg);
            std::snprintf(msg + len, sizeof(msg) - len, "Sending event:\n%s\n\n", typeid(*event).name());
        }

        if (receiver != nullptr)
        {
            unsigned long long len = strlen(msg);
            std::snprintf(msg + len, sizeof(msg) - len, "Receiving object:\n%s (%s)\n\n", typeid(*receiver).name(), qPrintable(receiver->objectName()));
        }

        std::fwrite(msg, strlen(msg), sizeof(char), f);
        std::fclose(f);
    }
}
