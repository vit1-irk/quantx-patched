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

#include "Zview.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>


Zview::Zview(QWidget *parent, Qt::WindowFlags f)
: QDialog(parent,f), model(0)
{
        this->setWindowTitle(tr("z parameters:"));
        this->setFont(QFont("Serif", 12, QFont::Bold ));
        QVBoxLayout *vl = new QVBoxLayout(this);

    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel("z",this));
        h->addWidget(this->leZ = new QLineEdit(this));
        this->leZ->setEnabled(true);
        this->leZ->setToolTip(tr("Current value of z"));
        connect(this->leZ,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("zmin"),this));
        h->addWidget(this->leZmin= new QLineEdit(this));
        this->leZmin->setToolTip(tr("Lower bound of z interval"));
        connect(this->leZmin,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("zmax"),this));
        h->addWidget(this->leZmax= new QLineEdit(this));
        this->leZmax->setToolTip(tr("High bound of z interval"));
        connect(this->leZmax,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("hz"),this));
        h->addWidget(this->leHz= new QLineEdit(this));
        this->leHz->setToolTip(tr("z increment"));
        connect(this->leHz,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
/*      {
        QGroupBox *gb = new QGroupBox("U(x,z)=U_1(x)*(1-z)+U_2(x)*z");
        QVBoxLayout *vlb = new QVBoxLayout;
        QHBoxLayout *hl = new QHBoxLayout;
        bInit = new QPushButton(tr("U_1(x,z=0) = U(x)"));
 //       connect(bInit, SIGNAL(clicked()), model, SLOT(slotU1()));
        hl->addWidget(bInit);

        bFin = new QPushButton(tr("U_2(x,z=1) = U(x)"));
//        connect(bFin, SIGNAL(clicked()), model, SLOT(slotU2()));
        hl->addWidget(bFin);

        vlb->addLayout(hl);
        gb->setLayout(vlb);
        vl->addWidget(gb);
    }
*/
    this->setLayout(vl);
    this->modelChanged();
}

Zview::~Zview(void)
{
}
//void Zview::invalidateModel()
//{
//    reset();
//}
void Zview::setCurrentZ(double d)
{
    QString s;
    s.sprintf("%lg",d);
    this->leZ->setText(s);
}

void Zview::setModel(PhysicalModel *_model)
{
    if (model != _model)
    {
        disconnect(model,0,this,0);
        model = _model;
        modelChanged();
    }
        connect(model,SIGNAL(signalZChanged(double)),this,SLOT(setCurrentZ(double)));
//        connect(model,SIGNAL(signalScaleZChanged()),this,SLOT(modelChanged())); //??????????
  }

void Zview::modelChanged()
{
    if (!model)
    {
        this->leZ->setDisabled(true);
        this->leZmin->setDisabled(true);
        this->leZmax->setDisabled(true);
        this->leHz->setDisabled(true);
        return;
    }
    else
    {
        this->leZ->setEnabled(true);
        this->leZmin->setEnabled(true);
        this->leZmax->setEnabled(true);
        this->leHz->setEnabled(true);
    }

    zParameters tp = model->getzParam();
    last.z = -1000;

    QString buf;
    buf.sprintf("%lg",tp.z);
    this->leZ->setText(buf);

    buf.sprintf("%lg",tp.zmin);
    this->leZmin->setText(buf);

    buf.sprintf("%lg",tp.zmax);
    this->leZmax->setText(buf);

    buf.sprintf("%lg",tp.hz);
    this->leHz->setText(buf);
}

void Zview::updateModel()
{
    if (!model) return;
    zParameters tp = { 0, 0, 0, 0 };

    tp.z = this->leZ->text().toDouble();
    tp.zmin = this->leZmin->text().toDouble();
    tp.zmax = this->leZmax->text().toDouble();
    tp.hz = this->leHz->text().toDouble();
    if (tp != last)
    {
        model->setzParam(tp);
        last = tp;
    }
}
