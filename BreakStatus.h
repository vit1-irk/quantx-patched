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

#include <QObject>
#include <QToolButton>
//#include <QPushButton>

class BreakStatus : public QObject
{
    Q_OBJECT
public:
    //! Attach the break status to click of \a button
    bool onButton(QToolButton *button);
//    bool onButton(QPushButton *button);

    //! Detach the break status from \a button
    bool noButton(QToolButton *button);
//    bool noButton(QPushButton *button);

private slots:
    //! Helper slot
    void set1(void);
};

extern BreakStatus breakStatus;

int getBreakStatus(int newStatus = 0);
void setBreakStatus(int newStatus);
