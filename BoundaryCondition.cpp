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

#include "BoundaryCondition.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>

BoundaryCondition::BoundaryCondition(QWidget *parent, Qt::WindowFlags f)
: QDialog(parent,f), model(0), last(UNDEFINED)
{
    this->setWindowTitle(tr("Boundary conditions"));
    this->setFont(QFont("Serif", 12, QFont::Bold ));
    //int index;
    QVBoxLayout *vl = new QVBoxLayout(this);
    {
        QRadioButton *rad1= new QRadioButton("Bound States/Scattering");
        QRadioButton *rad2= new QRadioButton("Periodic");
        QRadioButton *rad3= new QRadioButton("Quasistationary");
        vl->addWidget(rad1);
        vl->addWidget(rad2);
        vl->addWidget(rad3);
        bgR = new QButtonGroup(this);
        bgR->setExclusive(true);
        bgR->addButton(rad1,0);
        bgR->addButton(rad2,1);
        bgR->addButton(rad3,2);
        bgR->button(0)->setChecked(true);
        connect(bgR,SIGNAL(buttonClicked(int)),this,SLOT(updateModel()));
    }
    this->setLayout(vl);
    this->modelChanged();
}

BoundaryCondition::~BoundaryCondition(void)
{
}

void BoundaryCondition::setModel(PhysicalModel *_model)
{
    if (model != _model)
    {
        disconnect(model,0,this,0);
        model = _model;
        modelChanged();
    }
        connect(model,SIGNAL(signalTypeOfUChanged()),this,SLOT(modelChanged()));

}

void BoundaryCondition::modelChanged()
{
    if (!model)
    {
        return;
    }
    int index = 0;
    PotentialType u = model->getPotentialType();
    if(u==FINITE) index=0;
    if(u==PERIODIC) index=1;
    if(u==QUASISTATIONARY) index=2;
    if(u==SEMIPERIODIC) index=3;
    bgR->button(index)->setChecked(true);
//    bgR->setId(bgR->button(index),index);
}

void BoundaryCondition::updateModel()
{
    if (!model) return;
    int index = bgR->checkedId();

    PotentialType u = FINITE;
    if(index==0) u=FINITE;
    if(index==1) u=PERIODIC;
    if(index==2) u=QUASISTATIONARY;
    if (u != last)
    {
        model->setPotentialType(u);
        last = u;
    }
}
