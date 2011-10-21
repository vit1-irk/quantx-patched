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

#include "WPparametersM.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>


WPparametersM::WPparametersM(QWidget *parent, Qt::WindowFlags f)
: QDialog(parent,f), model(0)
{
        this->setWindowTitle(tr("Superposition of wavefunctions E_n<0"));
        this->setFont(QFont("Serif", 12, QFont::Bold ));
        QVBoxLayout *vl = new QVBoxLayout(this);
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel("nmin",this));
        h->addWidget(this->leNmin = new QLineEdit(this));
        this->leNmin->setEnabled(true);
        this->leNmin->setToolTip(tr("lower bond of the level number interval>=0"));
        connect(this->leNmin,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel("nmax",this));
        h->addWidget(this->leNmax = new QLineEdit(this));
        this->leNmax->setEnabled(true);
        this->leNmax->setToolTip(tr("upper bond of the level number interval<N"));
        connect(this->leNmax,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
        {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel("hn",this));
        h->addWidget(this->leHn = new QLineEdit(this));
        this->leHn->setEnabled(true);
        this->leHn->setToolTip(tr("level number increment"));
        connect(this->leHn,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }

    this->setLayout(vl);
    this->modelChanged();
}

WPparametersM::~WPparametersM(void)
{
}

void WPparametersM::setModel(PhysicalModel *_model)
{
    model = _model;
    modelChanged();
}

void WPparametersM::modelChanged()
{
    if (!model)
    {
        this->leNmax->setDisabled(true);
        this->leNmin->setDisabled(true);
        this->leHn->setDisabled(true);
        return;
    }
    else
    {
        this->leNmax->setEnabled(true);
        this->leNmin->setEnabled(true);
        this->leHn->setEnabled(true);
    }

    EmWP wp = model->getEmWP();
    last.nmin = -1;

    QString buf;
    buf.sprintf("%i",wp.nmin);
    this->leNmin->setText(buf);
    buf.sprintf("%i",wp.nmax);
    this->leNmax->setText(buf);
    buf.sprintf("%i",wp.hn);
    this->leHn->setText(buf);
}

void WPparametersM::updateModel()
{
    if (!model) return;
    EmWP wp = { 0, 0, 0 };

    wp.nmin = this->leNmin->text().toInt();
    wp.nmax = this->leNmax->text().toInt();
    wp.hn   = this->leHn->text().toInt();
    if (wp != last)
    {
        model->setEmWP(wp);
        last = wp;
    }
}

