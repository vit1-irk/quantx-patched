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

#include "Uch2x.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>


Uch2x::Uch2x(QWidget *parent, Qt::WindowFlags f)
: QDialog(parent,f), model(0)
{
        this->setWindowTitle(tr("U0/ch^2(x/a)"));
        this->setFont(QFont("Serif", 12, QFont::Bold ));
        QVBoxLayout *vl = new QVBoxLayout(this);

    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel("N",this));
        h->addWidget(this->leNumberOfIntervals = new QLineEdit(this));
        this->leNumberOfIntervals->setEnabled(true);
        this->leNumberOfIntervals->setToolTip(tr("Number of intervals"));
        connect(this->leNumberOfIntervals,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel("U0",this));
        h->addWidget(this->leU0= new QLineEdit(this));
        this->leU0->setToolTip(tr("Depth of the well"));
        connect(this->leU0,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("Width"),this));
        h->addWidget(this->leWidth= new QLineEdit(this));
        this->leWidth->setToolTip(tr("Width of parabolic potential"));
        connect(this->leWidth,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    this->setLayout(vl);
    this->modelChanged();
}
Uch2x::~Uch2x(void)
{
}

void Uch2x::setModel(PhysicalModel *_model)
{
    model = _model;
    modelChanged();
}

void Uch2x::modelChanged()
{
    if (!model)
    {
        this->leNumberOfIntervals->setDisabled(true);
        this->leU0->setDisabled(true);
        this->leWidth->setDisabled(true);
        return;
    }
    else
    {
        this->leNumberOfIntervals->setEnabled(true);
        this->leU0->setEnabled(true);
        this->leWidth->setEnabled(true);
    }

    U_ch2x up = model->getU_ch2x();
    last.numberOfIntervals = -1;

    QString buf;
    buf.sprintf("%i",up.numberOfIntervals);
    this->leNumberOfIntervals->setText(buf);
    buf.sprintf("%lg",up.U0);
    this->leU0->setText(buf);
    buf.sprintf("%lg",up.Width);
    this->leWidth->setText(buf);
}

void Uch2x::updateModel()
{
    if (!model) return;
    U_ch2x up = { 0 };

    up.numberOfIntervals = this->leNumberOfIntervals->text().toInt();
    up.U0 = this->leU0->text().toDouble();
    up.Width = this->leWidth->text().toDouble();
//    if (up != last)
    {
        model->setU_ch2x(up);
        last = up;
    }
}

