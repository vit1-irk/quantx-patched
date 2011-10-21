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

#include "ScaleWPK.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>


ScaleWPK::ScaleWPK(QWidget *parent, Qt::WindowFlags f)
: QDialog(parent,f), model(0)
{
    this->setWindowTitle(tr("Scales for wavepacket(k,t)"));
        this->setFont(QFont("Serif", 12, QFont::Bold ));
        QVBoxLayout *vl = new QVBoxLayout(this);
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("kmin"),this));
        h->addWidget(this->leXmin= new QLineEdit(this));
        this->leXmin->setToolTip(tr("Lower value of k-interval"));
        connect(this->leXmin,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("kmax"),this));
        h->addWidget(this->leXmax= new QLineEdit(this));
        this->leXmax->setToolTip(tr("High value of k-interval"));
        connect(this->leXmax,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("hk"),this));
        h->addWidget(this->leHx= new QLineEdit(this));
        this->leHx->setToolTip(tr("k increment"));
        connect(this->leHx,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("Phi_min"),this));
        h->addWidget(this->lePsimin= new QLineEdit(this));
        this->lePsimin->setToolTip(tr("Lower value of Phi"));
        connect(this->lePsimin,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("Phi_max"),this));
        h->addWidget(this->lePsimax= new QLineEdit(this));
        this->lePsimax->setToolTip(tr("High value of Phi"));
        connect(this->lePsimax,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    this->setLayout(vl);
    this->modelChanged();
}

ScaleWPK::~ScaleWPK(void)
{
}


void ScaleWPK::setModel(PhysicalModel *_model)
{
    if (model != _model)
    {
        disconnect(model,0,this,0);
        model = _model;
        modelChanged();
    }
}

void ScaleWPK::modelChanged()
{
    if (!model)
    {
        this->leHx->setDisabled(true);
        this->leXmin->setDisabled(true);
        this->leXmax->setDisabled(true);
        this->lePsimin->setDisabled(true);
        this->lePsimax->setDisabled(true);
        return;
    }
    else
    {
        this->leHx->setEnabled(true);
        this->leXmin->setEnabled(true);
        this->leXmax->setEnabled(true);
        this->lePsimin->setEnabled(true);
        this->lePsimax->setEnabled(true);
    }

    ScaleWPKParameters tp = model->getScaleWPKParam();
    last.Hk = -1000;

    QString buf;
    buf.sprintf("%lg",tp.Kmin);
    this->leXmin->setText(buf);

    buf.sprintf("%lg",tp.Kmax);
    this->leXmax->setText(buf);

    buf.sprintf("%lg",tp.WPKmax);
    this->lePsimax->setText(buf);

    buf.sprintf("%lg",tp.WPKmin);
    this->lePsimin->setText(buf);

    buf.sprintf("%lg",tp.Hk);
    this->leHx->setText(buf);
}

void ScaleWPK::updateModel()
{
    if (!model) return;
    ScaleWPKParameters tp = { 0, 0, 0, 0, 0 };

    tp.Hk = this->leHx->text().toDouble();
    tp.Kmin = this->leXmin->text().toDouble();
    tp.Kmax = this->leXmax->text().toDouble();
    tp.WPKmin = this->lePsimin->text().toDouble();
    tp.WPKmax = this->lePsimax->text().toDouble();
    if (tp != last)
    {
        model->setScaleWPKParam(tp);
        last = tp;
    }
}
