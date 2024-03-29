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

#include "WPparametersP.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>


WPparametersP::WPparametersP(QWidget *parent, Qt::WindowFlags f)
: QDialog(parent,f), model(0)
{
        this->setWindowTitle(tr("Wavepacket definition E>0"));
        this->setFont(QFont("Serif", 12, QFont::Bold ));
        QVBoxLayout *vl = new QVBoxLayout(this);

    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("Number of the wavepacket modes"),this));
        h->addWidget(this->leNumberOfModes = new QLineEdit(this));
        this->leNumberOfModes->setEnabled(true);
//        this->leNumberOfModes->setToolTip(tr("Number of wavepacket modes"));
        connect(this->leNumberOfModes,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("E_lowest"),this));
        h->addWidget(this->leE_low= new QLineEdit(this));
        this->leE_low->setToolTip(tr("Lowest bound of the wp energy"));
        connect(this->leE_low,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("E_highest"),this));
        h->addWidget(this->leE_high= new QLineEdit(this));
        this->leE_high->setToolTip(tr("Highest bound of the wp energy"));
        connect(this->leE_high,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    this->setLayout(vl);
    this->modelChanged();
}

WPparametersP::~WPparametersP(void)
{
}

void WPparametersP::setModel(PhysicalModel *_model)
{
    model = _model;
    modelChanged();
}

void WPparametersP::modelChanged()
{
    if (!model)
    {
        this->leNumberOfModes->setDisabled(true);
        this->leE_low->setDisabled(true);
        this->leE_high->setDisabled(true);
        return;
    }
    else
    {
        this->leNumberOfModes->setEnabled(true);
        this->leE_low->setEnabled(true);
        this->leE_high->setEnabled(true);
    }

    EpWP wp = model->getEpWP();
    last.numberOfModes = -1;

    QString buf;
    buf.sprintf("%i",wp.numberOfModes);
    this->leNumberOfModes->setText(buf);
    buf.sprintf("%lg",wp.E_low);
    this->leE_low->setText(buf);
    buf.sprintf("%lg",wp.E_high);
    this->leE_high->setText(buf);
}

void WPparametersP::updateModel()
{
    if (!model) return;
    EpWP wp = { 0, 0, 0 };

    wp.numberOfModes = this->leNumberOfModes->text().toInt();
    wp.E_low = this->leE_low->text().toDouble();
    wp.E_high = this->leE_high->text().toDouble();
    if (wp != last)
    {
        model->setEpWP(wp);
        last = wp;
    }
}
