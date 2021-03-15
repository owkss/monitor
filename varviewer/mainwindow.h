#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <deque>

#include "varview.h"
#include "executable.h"

#include <QThread>
#include <QComboBox>
#include <QSplitter>
#include <QListWidget>
#include <QMainWindow>
#include <QPushButton>
#include <QTableWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void timerEvent(QTimerEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void create_ui();
    void create_toolbar();
    QWidget *create_tab_widget(Executable *e);

    void save_geometry();
    void restore_geometry();

    std::deque<Executable*> m_executables;

    /* Интерфейс */
    QSplitter *m_splitter = nullptr;

    QTabWidget *m_tab = nullptr;

    QListWidget *m_list = nullptr;
    QPushButton *m_add_exe_button = nullptr;
    QPushButton *m_remove_exe_button = nullptr;

    QToolBar *m_toolbar = nullptr;
    QComboBox *m_exec_combobox = nullptr;
    QAction *m_add_exe_action = nullptr;
    QAction *m_remove_exe_action = nullptr;

public slots:

private slots:
    void add_executable();
    void remove_executable();

    void executable_started();
    void executable_finished(int exit_code);
    void executable_error(const QString &error_string);
    int get_executable_index(QObject *sender);

    void add_new_executable_in_list();
    void remove_executable_from_list();
    void current_executable_changed(const QString &text);

    void list_clicked(const QItemSelection &selected, const QItemSelection &);
    void tab_clicked(int index);

signals:

};

#endif // MAINWINDOW_H
