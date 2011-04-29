#include <QString>
#include <QLineEdit>
#include "LineEditDelegate.h"


LineEditDelegate::LineEditDelegate(QObject *parent)
: QItemDelegate(parent)
{
}

QWidget *LineEditDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem& option,
                                        const QModelIndex &index) const
{
    (void)option;
    (void)index;
    QLineEdit *editor = new QLineEdit(parent);
    editor->setFocusPolicy(Qt::StrongFocus);
    editor->setAutoFillBackground(true);
    return editor;
}

void LineEditDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
{
    double value = index.model()->data(index, Qt::EditRole).toDouble();

    QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
    QString str;
    str.sprintf("%lg",value);
    lineEdit->setText(str);
    lineEdit->setContextMenuPolicy(Qt::DefaultContextMenu);
}

void LineEditDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
{
    QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
    double value = lineEdit->text().toDouble();

    model->setData(index, value, Qt::EditRole);
}

void LineEditDelegate::updateEditorGeometry(QWidget *editor,
                                            const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}
