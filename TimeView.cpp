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

#include "TimeView.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>


TimeView::TimeView(QWidget *parent, Qt::WindowFlags f)
: QDialog(parent,f), model(0)
{
    this->setWindowTitle(tr("Time parameters:"));
        this->setFont(QFont("Serif", 12, QFont::Bold ));
        QVBoxLayout *vl = new QVBoxLayout(this);

    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("time"),this));
        h->addWidget(this->leTime = new QLineEdit(this));
        this->leTime->setEnabled(true);
        this->leTime->setToolTip(tr("Current value of time"));
        connect(this->leTime,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("tmin"),this));
        h->addWidget(this->leTmin= new QLineEdit(this));
        this->leTmin->setToolTip(tr("Lower bound of time interval"));
        connect(this->leTmin,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("tmax"),this));
        h->addWidget(this->leTmax= new QLineEdit(this));
        this->leTmax->setToolTip(tr("High bound of time interval"));
        connect(this->leTmax,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("ht"),this));
        h->addWidget(this->leHTime= new QLineEdit(this));
        this->leHTime->setToolTip(tr("time increment"));
        connect(this->leHTime,SIGNAL(editingFinished()),model,SLOT(updateModel()));
        vl->addWidget(line);
    }
    this->setLayout(vl);
    this->modelChanged();
}

TimeView::~TimeView(void)
{
}

void TimeView::setCurrentTime(double d)
{
    QString s;
    s.sprintf("%lg",d);
    this->leTime->setText(s);
}

void TimeView::setModel(PhysicalModel *_model)
{

    if (model != _model)
    {
        disconnect(model,0,this,0);
        model = _model;
        modelChanged();
    }
        connect(model,SIGNAL(signalTimeChanged(double)),this,SLOT(setCurrentTime(double)));
        connect(this->leTime,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        connect(this->leTmin,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        connect(this->leTmax,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        connect(this->leHTime,SIGNAL(editingFinished()),this,SLOT(updateModel()));
//        connect(this->leTime,SIGNAL(editingFinished()),this,SLOT(change_Time()));
//        connect(this->leHTime,SIGNAL(editingFinished()),this,SLOT(change_Time()));
}

void TimeView::change_Time()
{
    TimeParameters tp = { 0 };

    tp.time = this->leTime->text().toDouble();
    tp.tmin = this->leTmin->text().toDouble();
    tp.tmax = this->leTmax->text().toDouble();
    tp.ht = this->leHTime->text().toDouble();
    model->setTimeParam(tp);
//    model->change_Time(tp.time);
}
void TimeView::modelChanged()
{
    if (!model)
    {
        this->leTime->setDisabled(true);
        this->leTmin->setDisabled(true);
        this->leTmax->setDisabled(true);
        this->leHTime->setDisabled(true);
        return;
    }
    else
    {
        this->leTime->setEnabled(true);
        this->leTmin->setEnabled(true);
        this->leTmax->setEnabled(true);
        this->leHTime->setEnabled(true);
    }

    TimeParameters tp = model->getTimeParam();
    last.time = -1000;

    QString buf;
    buf.sprintf("%lg",tp.time);
    this->leTime->setText(buf);

    buf.sprintf("%lg",tp.tmin);
    this->leTmin->setText(buf);

    buf.sprintf("%lg",tp.tmax);
    this->leTmax->setText(buf);

    buf.sprintf("%lg",tp.ht);
    this->leHTime->setText(buf);
}

void TimeView::updateModel()
{
    if (!model) return;
    TimeParameters tp = { 0 };

    tp.time = this->leTime->text().toDouble();
    tp.tmin = this->leTmin->text().toDouble();
    tp.tmax = this->leTmax->text().toDouble();
    tp.ht = this->leHTime->text().toDouble();
//    if (tp != last)
//    {
        model->setTimeParam(tp);
//        last = tp;
//    }
}
