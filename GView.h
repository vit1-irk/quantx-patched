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

#pragma once
#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include "PhysicalModel.h"

class GView : public QDialog
{
    Q_OBJECT
public:
    GView(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~GView(void);

    void setModel(PhysicalModel*);

public slots:
    void updateModel();
    void modelChanged();
    void setCurrentG(double d);

private:
    PhysicalModel *model;
    gParameters last;
    QLineEdit *leG;
    QLineEdit *leGmin;
    QLineEdit *leGmax;
    QLineEdit *leHG;
};
