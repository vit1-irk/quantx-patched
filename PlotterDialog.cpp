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

#include "PlotterDialog.h"
#include <QVBoxLayout>
#include "MainWindow.h"
PlotterDialog::PlotterDialog(QWidget *parent, Qt::WindowFlags flags)
: QWidget(parent,flags)
//: QDialog(parent,flags)
{
    QVBoxLayout *vb = new QVBoxLayout(this);
    vb->addWidget( _plotter = new Plotter(this) );
    vb->addWidget( label = new QLabel(this) );
    connect(_plotter,SIGNAL(infoMouseMovedTo(QPointF)),
        this,SLOT(processInfoMouseMovedTo(QPointF)));
}

void PlotterDialog::processInfoMouseMovedTo(QPointF p)
{
    QString s;
    s.sprintf("x=%lg y=%lg",p.x(),p.y());
    label->setText(s);
}
