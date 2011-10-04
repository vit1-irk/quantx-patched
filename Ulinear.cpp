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

#include "Ulinear.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

Ulinear::Ulinear(QWidget *parent, Qt::WindowFlags f)
: QDialog(parent,f), model(0)
{
    this->setWindowTitle(tr("Linear addition to potential"));
    this->setFont(QFont("Serif", 12, QFont::Bold ));

    QVBoxLayout *vl = new QVBoxLayout(this);

    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("Bias Ub"),this));
        h->addWidget(this->leUBias = new QLineEdit(this));
        this->leUBias->setToolTip(tr("U bias over whole structure"));
        connect(this->leUBias,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    this->setLayout(vl);
    this->modelChanged();
}

Ulinear::~Ulinear(void)
{
}

void Ulinear::setModel(PhysicalModel *_model)
{
    model = _model;
    modelChanged();
}

void Ulinear::modelChanged()
{
    if (!model)
    {
        this->leUBias->setDisabled(true);
        return;
    }
    else
    {
        this->leUBias->setEnabled(true);
    }

    double Ubias = model->getUbias();
    Ubiaslast=-1;

    QString buf;
    buf.sprintf("%lg",Ubias);
    this->leUBias->setText(buf);
}

void Ulinear::updateModel()
{
    if (!model) return;
    double Ubias=0;

    Ubias = this->leUBias->text().toDouble();

    if (Ubias != Ubiaslast)
    {
        model->setUbias(Ubias);
        Ubiaslast = Ubias;
    }
}
