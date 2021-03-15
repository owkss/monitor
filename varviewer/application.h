#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>

class Application : public QApplication
{
    Q_OBJECT
public:
    Application(int &argc, char *argv[]);

    bool notify(QObject *receiver, QEvent *event) override;

    static void save_log(const char *what, const QObject *receiver = nullptr, const QEvent *event = nullptr);
};

#endif // APPLICATION_H
