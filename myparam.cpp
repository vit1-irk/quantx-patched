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

#include "MainWindow.h"
//#include <Q3Vbox.h>
#include <qlabel.h>
#include "myparam.h"
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
const int maxSizeInt=300;
const int maxSizeDouble=300;

void
//MyParamD::setDisplay(const QString& label, QLayout *layout)
MyParamD::setDisplay(const QString& label, const QString& labelTip, QLayout *layout)
{
    if (this->ledit) delete this->ledit;
    QWidget *parent = new QWidget;
    QHBoxLayout *hl = new QHBoxLayout(parent);
//    hl->addWidget(new QLabel(label));
    QLabel *script= new QLabel(label);
    script->setFont(QFont("Serif", 10, QFont::DemiBold ));
    hl->addWidget(script);
    parent->setToolTip(labelTip);
    hl->addWidget(this->ledit = new QLineEdit);
    parent->setMaximumWidth(maxSizeDouble);
    connect(ledit,SIGNAL(editingFinished()),this,SLOT(textToValue()));
    updateDisplay();
    layout ->addWidget(parent);
}

    /*
void
MyParamD::setDisplay(const QString& label, QLayout *parent)
{
    if (this->ledit) delete this->ledit;
    QHBoxLayout *hl = new QHBoxLayout(parent);
    hl->addWidget(new QLabel(label));
    hl->addWidget(this->ledit = new QLineEdit);
    connect(ledit,SIGNAL(returnPressed()),this,SLOT(textToValue()));
    updateDisplay();
}
*/
void MyParamD::textToValue()
{
    if (!ledit) return;
    double t = this->ledit->text().toDouble();
    if (t != this->v)
    {
        this->v = t;
        emit valueChanged();
    }
}

void MyParamD::updateDisplay()
{
    if (!ledit) return;
    QString aText;
    aText.sprintf("%lg",this->v);
//    this->ledit->setAlignment(Qt::AlignLeft);
    this->ledit->setText(aText);
    this->ledit->home(false);
}

//--------------------------------------------------------------------
/*void
MyParamI::setDisplay(const QString& label, QLayout *parent)
{
    if (ledit) delete ledit;
    QHBoxLayout *hl = new QHBoxLayout(parent);
    hl->addWidget(new QLabel(label));
    hl->addWidget(this->ledit = new QLineEdit);
    connect(ledit,SIGNAL(returnPressed()),this,SLOT(textToValue()));
//    connect(this,SIGNAL(valueChanged()),this,SLOT(updateDisplay()));
    updateDisplay();
}
*/
/*
void
MyParamUxN::setDisplay(const QString& label, QLayout *layout)
//MyParamI::setDisplay(const QString& label, Widget *parent)
{
    if (this->ledit) delete this->ledit;
    QWidget *parent = new QWidget;
    QHBoxLayout *hl = new QHBoxLayout(parent);
    hl->addWidget(new QLabel(label));
    hl->addWidget(this->ledit = new QLineEdit);
    connect(ledit,SIGNAL(returnPressed()),this,SLOT(textToValue()));
//    connect(ledit,SIGNAL(returnPressed()),this,SLOT(this->showU()));
//    connect(butUx, SIGNAL(clicked()), this, SLOT(showU()));
   connect(this,SIGNAL(valueChanged()),this,SLOT(showU()));
    updateDisplay();
    layout ->addWidget(parent);
}
void MyParamUxN::textToValue()
{
    if (!ledit) return;
    int t = this->ledit->text().toInt();
    if (t != this->v)
    {
        this->v = t;
        emit valueChanged();
    }
}

void MyParamUxN::updateDisplay()
{
    if (!ledit) return;
    QString aText;
    aText.sprintf("%i",this->v);
    this->ledit->setText(aText);
}*/
/*void MyParamUxN::drawUx()
{
    std::vector<double> data;
    double x=this->xmin;
    double y=0;
    data.push_back(x);
    data.push_back(y);
    x=0;
    data.push_back(x);
    data.push_back(y);
    for(int n=1;n<=Nwb;n++)
    {
        double y=MainWindow->Ua;
        data.push_back(x);
        data.push_back(y);
        MainWindow->plotterUx->setCurveData(MainWindow->numOfCurve,data);
        x +=MainWindow->aa;
        data.push_back(x);
        data.push_back(y);
        MainWindow->plotterUx->setCurveData(MainWindow->numOfCurve,data);
        y=MainWindow->Ub;
        data.push_back(x);
        data.push_back(y);
        MainWindow->plotterUx->setCurveData(MainWindow->numOfCurve,data);
        x +=MainWindow->->bb;
        if(n==MainWindow->Nwb&&MainWindow->xmax>(x-MainWindow->bb)) x=MainWindow->xmax;
        data.push_back(x);
        data.push_back(y);
        MainWindow->plotterUx->setCurveData(MainWindow->numOfCurve,data);
    }
    //    this->plotterU->setCurveData(this->numOfCurve,data);
    MainWindow->numOfCurve++;

}
*/
void
MyParamI::setDisplay(const QString& label, const QString& labelTip, QLayout *layout)
//MyParamI::setDisplay(const QString& label, Widget *parent)
{
    if (this->ledit) delete this->ledit;
    QWidget *parent = new QWidget;
    parent->setMaximumWidth(maxSizeInt);
    QHBoxLayout *hl = new QHBoxLayout(parent);
    QLabel *script= new QLabel(label);
//    script->setFrameStyle(QFrame::Panel | QFrame::Sunken);
//    script->setFont(QFont("Serif", 10, QFont::DemiBold ));
    parent->setFont(QFont("Serif", 10, QFont::DemiBold ));
    hl->addWidget(script);
    parent->setToolTip(labelTip);
    hl->addWidget(this->ledit = new QLineEdit);
    connect(ledit,SIGNAL(editingFinished()),this,SLOT(textToValue()));
    updateDisplay();
    layout ->addWidget(parent);
}
void MyParamI::textToValue()
{
    if (!ledit) return;
    int t = this->ledit->text().toInt();
    if (t != this->v)
    {
        this->v = t;
        emit valueChanged();
    }
}

void MyParamI::updateDisplay()
{
    if (!ledit) return;
    QString aText;
    aText.sprintf("%i",this->v);
    this->ledit->setText(aText);
}
