#include "mainwindow.h"
#include "application.h"
#include "delegate.h"

#include <QToolBar>
#include <QSettings>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QProcessEnvironment>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    qRegisterMetaType<QList<Variable>>("QList<Variable>");
    qRegisterMetaType<QList<Variable>>("QList<Variable>const&");
    setObjectName("MainWindow");

    create_ui();
    restore_geometry();
}

MainWindow::~MainWindow()
{
    save_geometry();

    for (auto &e : m_executables)
    {
        delete e;
    }

    m_executables.clear();
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    QMainWindow::timerEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    QMainWindow::keyPressEvent(event);
}

void MainWindow::create_ui()
{
    setMinimumSize(640, 480);

    m_splitter = new QSplitter(this);
    m_splitter->setObjectName("splitter");

    m_tab = new QTabWidget(m_splitter);
    m_tab->setObjectName("tabWidget");

    QWidget *list_container = new QWidget(m_splitter);
    m_list = new QListWidget(list_container);
    m_add_exe_button = new QPushButton(QIcon(":/images/add.png"), tr("Добавить"), list_container);
    m_remove_exe_button = new QPushButton(QIcon(":/images/remove.png"), tr("Удалить"), list_container);

    list_container->setObjectName("list_container");
    m_list->setObjectName("listWidget");
    m_add_exe_button->setObjectName("add_button");
    m_remove_exe_button->setObjectName("remove_button");

    QHBoxLayout *list_btn_layout = new QHBoxLayout;
    list_btn_layout->addWidget(m_add_exe_button);
    list_btn_layout->addWidget(m_remove_exe_button);

    QVBoxLayout *list_layout = new QVBoxLayout;
    list_layout->addWidget(m_list);
    list_layout->addLayout(list_btn_layout);

    if (list_container->layout())
        delete list_container->layout();
    list_container->setLayout(list_layout);

    m_splitter->addWidget(list_container);
    m_splitter->addWidget(m_tab);
    setCentralWidget(m_splitter);

    connect(m_add_exe_button, &QPushButton::clicked, this, &MainWindow::add_executable);
    connect(m_remove_exe_button, &QPushButton::clicked, this, &MainWindow::remove_executable);
    connect(m_list->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::list_clicked);
    connect(m_tab, &QTabWidget::tabBarClicked, this, &MainWindow::tab_clicked);

    create_toolbar();
}

void MainWindow::create_toolbar()
{
    m_toolbar = new QToolBar(this);
    m_toolbar->setObjectName("toolbar");
    m_toolbar->setMovable(false);

    /* Список запускаемых исполняемых файлов */
    m_exec_combobox = new QComboBox(m_toolbar);
    m_add_exe_action = new QAction(QIcon(":/images/add.png"), tr("Добавить исполняемый файл в список"), m_toolbar);
    m_remove_exe_action = new QAction(QIcon(":/images/remove.png"), tr("Удалить исполняемый файл из списка"), m_toolbar);

    m_exec_combobox->setMinimumWidth(250);

    m_toolbar->addWidget(m_exec_combobox);
    m_toolbar->addSeparator();
    m_toolbar->addAction(m_add_exe_action);
    m_toolbar->addAction(m_remove_exe_action);
    m_toolbar->addSeparator();

    QWidget *spacer = new QWidget(m_toolbar);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_toolbar->addWidget(spacer);

    /* Заполнение выпадающего списка */
    QSettings sett("Umbrella", "Monitor");
    QStringList executables = sett.value("executables").toStringList();
    m_exec_combobox->addItems(executables);

    const QString current_executable = sett.value("current_executable").toString();
    const int index = m_exec_combobox->findText(current_executable);
    if (index == -1)
    {
        sett.setValue("current_executable", QString());
    }
    else
    {
        m_exec_combobox->setCurrentIndex(index);
    }

    connect(m_add_exe_action, &QAction::triggered, this, &MainWindow::add_new_executable_in_list);
    connect(m_remove_exe_action, &QAction::triggered, this, &MainWindow::remove_executable_from_list);
    connect(m_exec_combobox, static_cast<void(QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged), this, &MainWindow::current_executable_changed);

    addToolBar(Qt::TopToolBarArea, m_toolbar);
}

void MainWindow::save_geometry()
{
    QSettings sett("Umbrella", "Monitor");
    sett.setValue("MainWindow/geometry", saveGeometry());
    sett.setValue("MainWindow/state", saveState());
    sett.setValue("MainWindow/splitter_sizes", m_splitter->saveState());
}

void MainWindow::restore_geometry()
{
    QSettings sett("Umbrella", "Monitor");
    restoreGeometry(sett.value("MainWindow/geometry").toByteArray());
    restoreState(sett.value("MainWindow/state").toByteArray());
    m_splitter->restoreState(sett.value("MainWindow/splitter_sizes").toByteArray());
}

void MainWindow::add_executable()
{
    const int index = m_exec_combobox->currentIndex();
    if (index < 0)
        return;

    const QString path = m_exec_combobox->currentText();
    Executable *e = new Executable(path);

    connect(e, &Executable::executable_started, this, &MainWindow::executable_started);
    connect(e, &Executable::executable_finished, this, &MainWindow::executable_finished);
    connect(e, &Executable::executable_error, this, &MainWindow::executable_error);

    m_executables.push_back(e);
}

void MainWindow::remove_executable()
{
    QList<QListWidgetItem*> selected_items = m_list->selectedItems();
    for (int i = 0; i < selected_items.size(); ++i)
    {
        QListWidgetItem *item = selected_items.at(i);
        const int index = m_list->row(item);

        Executable *e = nullptr;
        try
        {
            e = m_executables.at(std::deque<Executable*>::size_type(index));

            delete e;
            m_executables.erase(std::next(m_executables.begin(), index));

            m_list->removeItemWidget(item);
            m_tab->removeTab(index);

            delete item;
        }
        catch (std::out_of_range &e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
}

QWidget *MainWindow::create_tab_widget(Executable *e)
{
    QSplitter *splitter = new QSplitter(Qt::Vertical);
    VarView *view = new VarView(splitter);
    QPlainTextEdit *edit = new QPlainTextEdit(splitter);
    Delegate *delegate = new Delegate(view);

    view->setItemDelegateForColumn(2, delegate);

    edit->setReadOnly(true);
    edit->setMaximumBlockCount(10000); /* TODO должно задаваться в каких-то настройках, но необязательно */

    splitter->addWidget(view);
    splitter->addWidget(edit);

    connect(delegate, &Delegate::set_edit_index, view, &VarView::set_edit_index);
    connect(delegate, &Delegate::value_changed, e, &Executable::value_changed);
    connect(e, &Executable::data_ready, view, &VarView::data_ready);
    connect(e, &Executable::stdout_msg, this, [edit](const QString &msg) { edit->appendPlainText(msg); }, Qt::QueuedConnection);
    connect(e, &Executable::stderr_msg, this, [edit](const QString &msg) { edit->appendHtml(QString("<p style=\"color:red;\">%1</p>").arg(msg)); }, Qt::QueuedConnection);

    return splitter;
}


void MainWindow::executable_started()
{
    int index = get_executable_index(sender());
    if (index < 0)
        return;

    Executable *e = m_executables.at(std::deque<Executable*>::size_type(index));
    const QString str = QString("%1 (%2)").arg(e->program()).arg(e->pid());

    m_tab->addTab(create_tab_widget(e), str);

    QListWidgetItem *item = new QListWidgetItem(QIcon(":/images/green.png"), str);
    m_list->addItem(item);
}

void MainWindow::executable_finished(int exit_code)
{
    int index = get_executable_index(sender());
    if (index < 0)
        return;

    QWidget *w = m_tab->widget(index);
    if (!w)
    {
        std::cerr << "tab widget == nullptr for index " << index << std::endl;
        return;
    }

    const QObjectList &childs = w->children();
    for (int i = 0; i < childs.size(); ++i)
    {
        if (QPlainTextEdit *edit = qobject_cast<QPlainTextEdit*>(childs.at(i)))
        {
            edit->appendHtml(QString("<p style=\"color:%1;\">%2</p>").arg(exit_code == 0 ? "green" : "red").arg(tr("Процесс завершился с кодом %1").arg(exit_code)));
            break;
        }
    }

    QListWidgetItem *item = m_list->item(index);
    item->setIcon(QIcon(":/images/gray.png"));
}

void MainWindow::executable_error(const QString &error_string)
{
    int index = get_executable_index(sender());
    if (index < 0)
        return;

    QWidget *w = m_tab->widget(index);
    if (!w)
    {
        std::cerr << "tab widget == nullptr for index " << index << std::endl;
        return;
    }

    const QObjectList &childs = w->children();
    for (int i = 0; i < childs.size(); ++i)
    {
        if (QPlainTextEdit *edit = qobject_cast<QPlainTextEdit*>(childs.at(i)))
        {
            edit->appendHtml(QString("<p style=\"color:red;\">%1</p>").arg(tr("Процесс аварийно завершился (%1)").arg(error_string)));
            break;
        }
    }

    QListWidgetItem *item = m_list->item(index);
    item->setIcon(QIcon(":/images/red.png"));
}

int MainWindow::get_executable_index(QObject *sender)
{
    Executable *e = qobject_cast<Executable*>(sender);
    if (!e)
    {
        //std::cerr << "qobject_cast failed" << std::endl;
        return -1;
    }

    auto pos = std::find(m_executables.begin(), m_executables.end(), e);
    if (pos == m_executables.end())
    {
        std::cerr << "executable not found" << std::endl;
        return -1;
    }

    return static_cast<int>(std::distance(m_executables.begin(), pos));
}

void MainWindow::add_new_executable_in_list()
{
    QSettings sett("Umbrella", "Monitor");
    const QString dir = sett.value("exe_dir", Application::applicationDirPath()).toString();

    QFileDialog fd(this);
    fd.setFileMode(QFileDialog::AnyFile);
    fd.setAcceptMode(QFileDialog::AcceptOpen);
    fd.setDirectory(dir);
    fd.setWindowTitle(tr("Выбор исполняемого файла"));

    connect(&fd, &QFileDialog::directoryEntered, [&sett](const QString &directory) { sett.setValue("exe_dir", directory); });

    if (fd.exec() != QFileDialog::Accepted)
        return;

    const QString path = fd.selectedFiles().at(0);
    m_exec_combobox->addItem(path);

    QStringList executables = sett.value("executables").toStringList();
    executables.push_back(path);
    sett.setValue("executables", executables);
}

void MainWindow::remove_executable_from_list()
{
    if (m_exec_combobox->count() < 1)
        return;

    const QString path = m_exec_combobox->currentText();

    QMessageBox mbox;
    mbox.setWindowTitle(tr("Удаление из списка"));
    mbox.setText(tr("Удалить из списка \"%1\"?").arg(path));
    QPushButton *yes = mbox.addButton(tr("Да"), QMessageBox::YesRole);
    QPushButton *no = mbox.addButton(tr("Нет"), QMessageBox::NoRole);
    mbox.setDefaultButton(no);
    mbox.exec();

    if (mbox.clickedButton() == yes)
    {
        m_exec_combobox->removeItem(m_exec_combobox->currentIndex());

        QSettings sett("Umbrella", "Monitor");
        QStringList executables = sett.value("executables").toStringList();
        executables.removeOne(path);
        sett.setValue("executables", executables);

        const QString current_executable = sett.value("current_executable").toString();
        if (current_executable.compare(path, Qt::CaseInsensitive) == 0)
        {
            sett.setValue("current_executable", QString());
        }
    }
}

void MainWindow::current_executable_changed(const QString &text)
{
    QSettings sett("Umbrella", "Monitor");
    sett.setValue("current_executable", text);
}

void MainWindow::list_clicked(const QItemSelection &selected, const QItemSelection &)
{
    QModelIndexList list = selected.indexes();
    if (!list.empty())
    {
        const int index = list.front().row();
        m_tab->setCurrentIndex(index);
    }
}

void MainWindow::tab_clicked(int index)
{
    m_list->selectionModel()->clearSelection();
    m_list->selectionModel()->select(m_list->model()->index(index, 0), QItemSelectionModel::Select);
}

