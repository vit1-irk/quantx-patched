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

#include "ScalePhin.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>


ScalePhin::ScalePhin(QWidget *parent, Qt::WindowFlags f)
: QDialog(parent,f), model(0)
{
    this->setWindowTitle("Scales for momentum distribution:");
        this->setFont(QFont("Serif", 12, QFont::Bold ));
        QVBoxLayout *vl = new QVBoxLayout(this);
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("kmin"),this));
        h->addWidget(this->leKmin= new QLineEdit(this));
        this->leKmin->setToolTip(tr("Lower value of k-interval"));
        connect(this->leKmin,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("kmax"),this));
        h->addWidget(this->leKmax= new QLineEdit(this));
        this->leKmax->setToolTip(tr("High value of k-interval"));
        connect(this->leKmax,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("hk"),this));
        h->addWidget(this->leHk= new QLineEdit(this));
        this->leHk->setToolTip(tr("k increment"));
        connect(this->leHk,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("Phi_min"),this));
        h->addWidget(this->lePhimin= new QLineEdit(this));
        this->lePhimin->setToolTip(tr("Lower value of Phi"));
        connect(this->lePhimin,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("Phi_max"),this));
        h->addWidget(this->lePhimax= new QLineEdit(this));
        this->lePhimax->setToolTip(tr("High value of Phi"));
        connect(this->lePhimax,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    this->setLayout(vl);
    this->modelChanged();
}

ScalePhin::~ScalePhin(void)
{
}


void ScalePhin::setModel(PhysicalModel *_model)
{
    if (model != _model)
    {
        disconnect(model,0,this,0);
        model = _model;
        modelChanged();
    }
        connect(model,SIGNAL(signalScalePhinChanged()),this,SLOT(modelChanged()));
}

void ScalePhin::modelChanged()
{
    if (!model)
    {
        this->leHk->setDisabled(true);
        this->leKmin->setDisabled(true);
        this->leKmax->setDisabled(true);
        this->lePhimin->setDisabled(true);
        this->lePhimax->setDisabled(true);
        return;
    }
    else
    {
        this->leHk->setEnabled(true);
        this->leKmin->setEnabled(true);
        this->leKmax->setEnabled(true);
        this->lePhimin->setEnabled(true);
        this->lePhimax->setEnabled(true);
    }

    ScalePhinParameters tp = model->getScalePhinParam();
    last.Hk = -1000;

    QString buf;
    buf.sprintf("%lg",tp.Kmin);
    this->leKmin->setText(buf);

    buf.sprintf("%lg",tp.Kmax);
    this->leKmax->setText(buf);

    buf.sprintf("%lg",tp.Phinmax);
    this->lePhimax->setText(buf);

    buf.sprintf("%lg",tp.Phinmin);
    this->lePhimin->setText(buf);

    buf.sprintf("%lg",tp.Hk);
    this->leHk->setText(buf);
}

void ScalePhin::updateModel()
{
    if (!model) return;
    ScalePhinParameters tp = { 0, 0, 0, 0, 0 };

    tp.Hk = this->leHk->text().toDouble();
    tp.Kmin = this->leKmin->text().toDouble();
    tp.Kmax = this->leKmax->text().toDouble();
    tp.Phinmin = this->lePhimin->text().toDouble();
    tp.Phinmax = this->lePhimax->text().toDouble();
    if (tp != last)
    {
        model->setScalePhinParam(tp);
        last = tp;
    }
}
