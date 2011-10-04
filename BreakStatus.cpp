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

#include <QApplication>
#include "BreakStatus.h"

static volatile int localBreakStatus = 0;

void BreakStatus::set1()
{
    localBreakStatus = 1;
}

bool BreakStatus::onButton(QToolButton *button)
//bool BreakStatus::onButton(QPushButton *button)
{
    localBreakStatus = 0;
    return connect(button,SIGNAL(clicked()),this,SLOT(set1()),Qt::DirectConnection);
}

bool BreakStatus::noButton(QToolButton *button)
//bool BreakStatus::noButton(QPushButton *button)
{
    return disconnect(button,SIGNAL(clicked()),this,SLOT(set1()));
}

BreakStatus breakStatus;

int getBreakStatus(int newStatus)
{
    qApp->processEvents(QEventLoop::AllEvents);
    int res = localBreakStatus;
    localBreakStatus = newStatus;
    return res;
}

void setBreakStatus(int newStatus)
{
    localBreakStatus = newStatus;
}
