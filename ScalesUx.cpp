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

#include "ScalesUx.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>


ScalesUx::ScalesUx(QWidget *parent, Qt::WindowFlags f)
: QDialog(parent,f), model(0)
{
    this->setWindowTitle(tr("Scales for U(x)"));
        this->setFont(QFont("Serif", 12, QFont::Bold ));
        QVBoxLayout *vl = new QVBoxLayout(this);

    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("Umin"),this));
        h->addWidget(this->leUmin= new QLineEdit(this));
        this->leUmin->setToolTip(tr("Lower value of potential"));
        connect(this->leUmin,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("Umax"),this));
        h->addWidget(this->leUmax= new QLineEdit(this));
        this->leUmax->setToolTip(tr("High value of potential"));
        connect(this->leUmax,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("xmin"),this));
        h->addWidget(this->leXmin= new QLineEdit(this));
        this->leXmin->setToolTip(tr("Lower value of x-interval"));
        connect(this->leXmin,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("xmax"),this));
        h->addWidget(this->leXmax= new QLineEdit(this));
        this->leXmax->setToolTip(tr("High value of x-interval"));
        connect(this->leXmax,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    this->setLayout(vl);
    this->modelChanged();
}

ScalesUx::~ScalesUx(void)
{
}


void ScalesUx::setModel(PhysicalModel *_model)
{
    if (model != _model)
    {
        disconnect(model,0,this,0);
        model = _model;
        modelChanged();
    }
        connect(model,SIGNAL(signalScalesUChanged()),this,SLOT(modelChanged()));
}

void ScalesUx::modelChanged()
{
    if (!model)
    {
        this->leXmin->setDisabled(true);
        this->leXmax->setDisabled(true);
        this->leUmin->setDisabled(true);
        this->leUmax->setDisabled(true);
        return;
    }
    else
    {
        this->leXmin->setEnabled(true);
        this->leXmax->setEnabled(true);
        this->leUmin->setEnabled(true);
        this->leUmax->setEnabled(true);
    }

    ScalesUParameters tp = model->getScalesUParam();
    last.Xmin = -1000;

    QString buf;
    buf.sprintf("%lg",tp.Xmin);
    this->leXmin->setText(buf);

    buf.sprintf("%lg",tp.Xmax);
    this->leXmax->setText(buf);

    buf.sprintf("%lg",tp.Umin);
    this->leUmin->setText(buf);

    buf.sprintf("%lg",tp.Umax);
    this->leUmax->setText(buf);
}

void ScalesUx::updateModel()
{
    if (!model) return;
    ScalesUParameters tp = { 0, 0, 0, 0 };

    tp.Xmin = this->leXmin->text().toDouble();
    tp.Xmax = this->leXmax->text().toDouble();
    tp.Umin = this->leUmin->text().toDouble();
    tp.Umax = this->leUmax->text().toDouble();
    if (tp != last)
    {
        model->setScalesUParam(tp);
        last = tp;
    }
}
