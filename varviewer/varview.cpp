#include "varview.h"

#include <iostream>

#include <QElapsedTimer>

VarView::VarView(QWidget *parent)
    : QTableView(parent)
{
    m_model = new QStandardItemModel();
    m_model->setHorizontalHeaderLabels({ tr("Имя"), tr("Тип"), tr("Значение") });
    setModel(m_model);

    horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    horizontalHeader()->setStretchLastSection(true);
}

VarView::~VarView()
{
    delete m_model;
}

void VarView::dropEvent(QDropEvent *event)
{
    QTableView::dropEvent(event);
}

void VarView::dragEnterEvent(QDragEnterEvent *event)
{
    QTableView::dragEnterEvent(event);
}

void VarView::dragMoveEvent(QDragMoveEvent *event)
{
    QTableView::dragMoveEvent(event);
}

void VarView::focusInEvent(QFocusEvent *event)
{
    QTableView::focusInEvent(event);
}

void VarView::focusOutEvent(QFocusEvent *event)
{
    QTableView::focusOutEvent(event);
}

void VarView::keyPressEvent(QKeyEvent *event)
{
    QTableView::keyPressEvent(event);
}

void VarView::data_ready(QList<Variable> data)
{
    if (m_model->rowCount() == 0)
    {
        for (int row = 0; row < data.size(); ++row)
        {
            const Variable &v = data.at(row);

            QStandardItem *name_item = new QStandardItem;
            QStandardItem *type_item = new QStandardItem;
            QStandardItem *value_item = new QStandardItem;

            name_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            type_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            value_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);

            name_item->setText(v.name);
            type_item->setText(v.type_str);
            value_item->setData(v.value, Qt::DisplayRole);

            m_model->setItem(row, 0, name_item);
            m_model->setItem(row, 1, type_item);
            m_model->setItem(row, 2, value_item);
        }
    }
    else
    {
        for (int row = 0; row < data.size(); ++row)
        {
            if (row == m_edit_index)
                continue;

            const Variable &v = data.at(row);

            QStandardItem *value_item = m_model->item(row, 2);
            value_item->setData(v.value, Qt::DisplayRole);
        }
    }
}

void VarView::set_edit_index(int index)
{
    m_edit_index = index;
}

void VarView::item_data_changed(QStandardItem *item)
{

}
