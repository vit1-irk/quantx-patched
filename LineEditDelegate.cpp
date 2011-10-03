/*
 *  Copyright (C) 2011 Olga Tkachenko
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301 USA
 */

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
