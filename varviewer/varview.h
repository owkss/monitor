#ifndef VARVIEW_H
#define VARVIEW_H

#include "variable.h"

#include <QScrollBar>
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>

class VarView : public QTableView
{
    Q_OBJECT
public:
    VarView(QWidget *parent = nullptr);
    ~VarView() override;

protected:
    void dropEvent(QDropEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    QStandardItemModel *m_model = nullptr;

    int m_edit_index = -1;

public slots:
    void data_ready(QList<Variable> data);
    void set_edit_index(int index);

private slots:
    void item_data_changed(QStandardItem *item);

signals:

};

#endif // VARVIEW_H
