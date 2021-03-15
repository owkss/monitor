#include "delegate.h"

#include <iostream>

#include <QDoubleSpinBox>

Delegate::Delegate(QWidget *parent)
    : QStyledItemDelegate(parent)
{
    this->setObjectName("VarDelegate");
}

QWidget *Delegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const
{
    const QAbstractItemModel *model = index.model();
    const QString type = model->data(model->index(index.row(), 1)).toString();

    QWidget *edit = nullptr;

    if (type.contains("float", Qt::CaseInsensitive) || type.contains("double", Qt::CaseInsensitive) || type.contains("int64", Qt::CaseInsensitive))
    {
        QDoubleSpinBox *spinbox = new QDoubleSpinBox(parent);
        spinbox->setDecimals(12);
        spinbox->setRange(type.startsWith("u", Qt::CaseInsensitive) ? 0 : std::numeric_limits<int64_t>::min(), std::numeric_limits<double>::max());
        edit = spinbox;
    }
    else
    {
        QSpinBox *spinbox = new QSpinBox(parent);
        spinbox->setRange(type.startsWith("u", Qt::CaseInsensitive) ? 0 : std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
        edit = spinbox;
    }

    m_prev_value = QVariant();
    emit set_edit_index(index.row());

    return edit;
}

void Delegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    const QVariant data = index.data();
    if (QDoubleSpinBox *e = qobject_cast<QDoubleSpinBox*>(editor))
    {
        e->setValue(data.toDouble());
    }
    else if (QSpinBox *e = qobject_cast<QSpinBox*>(editor))
    {
        e->setValue(data.toInt());
    }

    m_prev_value = data;
}

void Delegate::setModelData(QWidget *editor, QAbstractItemModel *itemModel, const QModelIndex &index) const
{
    QVariant new_value;
    if (QDoubleSpinBox *e = qobject_cast<QDoubleSpinBox*>(editor))
    {
        new_value = e->value();
    }
    else if (QSpinBox *e = qobject_cast<QSpinBox*>(editor))
    {
        new_value = e->value();
    }

    if (m_prev_value != new_value)
    {
        emit value_changed(index.row(), new_value);
    }

    itemModel->setData(index, new_value);

    emit set_edit_index(-1);
}
