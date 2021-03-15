#ifndef USERGRAPHICSDELEGATE_H
#define USERGRAPHICSDELEGATE_H

#include <QStyledItemDelegate>

class Delegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    Delegate(QWidget *parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *itemModel, const QModelIndex &index) const override;

private:
    mutable QVariant m_prev_value;

public slots:

private slots:

signals:
    void set_edit_index(int index) const;
    void value_changed(int row, QVariant value) const;
};

#endif // USERGRAPHICSDELEGATE_H
