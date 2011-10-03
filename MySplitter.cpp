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

#include <QPainter>
#include <QLinearGradient>
#include <QPaintEvent>
#include "MySplitter.h"

class MySplitterHandle : public QSplitterHandle
{
public:
    MySplitterHandle(Qt::Orientation orientation, QSplitter *parent = 0);
    void paintEvent(QPaintEvent *event);
private:
    QColor c0, c1;
};

MySplitterHandle::MySplitterHandle(Qt::Orientation orientation, QSplitter *parent)
: QSplitterHandle(orientation,parent), c0(Qt::gray), c1(Qt::gray)
{
}

void MySplitterHandle::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QLinearGradient gradient;
    gradient.setColorAt(0,c0);
    gradient.setColorAt(1,c1);
    if (orientation() == Qt::Vertical)
    {
        gradient.setStart(rect().left(), rect().height()/2);
        gradient.setFinalStop(rect().right(), rect().height()/2);
    } else {
        gradient.setStart(rect().width()/2, rect().top());
        gradient.setFinalStop(rect().width()/2, rect().bottom());
    }
    painter.fillRect(event->rect(), QBrush(gradient));
}

MySplitter::MySplitter(Qt::Orientation orientation, QWidget *parent)
: QSplitter(orientation,parent)
{
}

QSplitterHandle *MySplitter::createHandle()
{
    return new MySplitterHandle( orientation(), this );
}
