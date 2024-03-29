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

#include "PotentialViewMultiWell.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

PotentialViewMultiWell::PotentialViewMultiWell(QWidget *parent, Qt::WindowFlags f)
: QDialog(parent,f), model(0)
{
    this->setWindowTitle(tr("Multi-well/barrier potential"));
    this->setFont(QFont("Serif", 12, QFont::Bold ));

    QVBoxLayout *vl = new QVBoxLayout(this);

    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("Number of wells/barriers"),this));
        h->addWidget(this->leNumberOfWells = new QLineEdit(this));
        this->leNumberOfWells->setEnabled(true);
        this->leNumberOfWells->setToolTip(tr("Number of wells/barriers"));
        connect(this->leNumberOfWells,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("Width a"),this));
        h->addWidget(this->leWidthA = new QLineEdit(this));
        this->leWidthA->setToolTip(tr("Width of wells/barriers"));
        connect(this->leWidthA,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("Depth/Height a"),this));
        h->addWidget(this->leHeightA = new QLineEdit(this));
        this->leHeightA->setToolTip(tr("Value of depth/height of wells/barriers"));
        connect(this->leHeightA,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }

    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("Width b"),this));
        h->addWidget(this->leWidthB = new QLineEdit(this));
        this->leWidthB->setToolTip(tr("Distance between wells/barriers"));
        connect(this->leWidthB,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("Depth/Height B"),this));
        h->addWidget(this->leHeightB = new QLineEdit(this));
        this->leHeightB->setToolTip(tr("Value of potential between wells/barriers"));
        connect(this->leHeightB,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    this->setLayout(vl);
    this->modelChanged();
}

PotentialViewMultiWell::~PotentialViewMultiWell(void)
{
}

void PotentialViewMultiWell::setModel(PhysicalModel *_model)
{
    model = _model;
    modelChanged();
}

void PotentialViewMultiWell::modelChanged()
{
    if (!model)
    {
        this->leNumberOfWells->setDisabled(true);
        this->leWidthA->setDisabled(true);
        this->leWidthB->setDisabled(true);
        this->leHeightA->setDisabled(true);
        this->leHeightB->setDisabled(true);
//        this->leUBias->setDisabled(true);
        return;
    }
    else
    {
        this->leNumberOfWells->setEnabled(true);
        this->leWidthA->setEnabled(true);
        this->leWidthB->setEnabled(true);
        this->leHeightA->setEnabled(true);
        this->leHeightB->setEnabled(true);
//        this->leUBias->setEnabled(true);
    }

    UAsMW u = model->getUAsMW();
    last.numberOfWells = -1;

    QString buf;
    buf.sprintf("%i",u.numberOfWells);
    this->leNumberOfWells->setText(buf);
    buf.sprintf("%lg",u.wa);
    this->leWidthA->setText(buf);
    buf.sprintf("%lg",u.wb);
    this->leWidthB->setText(buf);
    buf.sprintf("%lg",u.ua);
    this->leHeightA->setText(buf);
    buf.sprintf("%lg",u.ub);
    this->leHeightB->setText(buf);
//    buf.sprintf("%lg",u.ubias);
//    this->leUBias->setText(buf);
}

void PotentialViewMultiWell::updateModel()
{
    if (!model) return;
    UAsMW u = { 0, 0, 0, 0, 0 };

    u.numberOfWells = this->leNumberOfWells->text().toInt();
    u.wa = this->leWidthA->text().toDouble();
    u.wb = this->leWidthB->text().toDouble();
    u.ua = this->leHeightA->text().toDouble();
    u.ub = this->leHeightB->text().toDouble();
//    u.ubias = this->leUBias->text().toDouble();

    if (u != last)
    {
        model->setUAsMW(u);
        last = u;
    }
}
