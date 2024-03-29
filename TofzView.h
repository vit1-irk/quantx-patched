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
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPointF>
#include <QGraphicsItem>
#include "PhysicalModel.h"
#include <QGraphicsPolygonItem>
#include <QPolygonF>
#include <QMap>
#include "myparam.h"
#include "Zview.h"
#include <QPaintEvent>
#include <QGroupBox>

class ZTDraggable;
class TofzView;

class TofzCurve : public QGraphicsPolygonItem
{
    TofzView *view;
public:
    TofzCurve(const QPolygonF& p, TofzView *_view, QGraphicsItem *parent=0,QGraphicsScene *scene=0)
        : QGraphicsPolygonItem(p,parent), view(_view)
    {
    }
    void paint (QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
//    void mousePressEvent(QGraphicsSceneMouseEvent * event);

};

class TofzView : public QGraphicsView
{
    Q_OBJECT
public:

    TofzView(PhysicalModel *m, QWidget *parent = 0);
//    virtual ~TofzView();
    void setCurve(int id,const QPolygonF&, const QPen& = QPen());
    TofzCurve *getCurve(int id) const { return curves[id]; }
    void removeCurve(int id);
    void setScaleTZ();
public slots:
    void setViewportMapping();
    void resizePicture();
    void slot_whole_T_of_z();
    void slotZline();
    void clearAll();
    void updateScaleTZ();
signals:
    void infoMouseMovedTo(QPointF);
    void signalScaleTChanged();
protected:
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void scrollView(int dx, int dy);
//    void setScaleTZ();
    void scaleView(qreal scaleFactor);
    void setScalesFromModel();
    bool Erase;
    void contextMenuEvent(QContextMenuEvent *event);
public:
    void showDialogScaleY();
    void showDialogZ();
    double tMax, tMin;
private:
    void initDialogScaleY();
//    double tMax, tMin;
    double Zmin,Zmax,hZ;
    double lineWidth;
    QGraphicsLineItem *lineh,*linev;
    ZTDraggable *lineZ;

    QLineEdit *leTmin;
    QLineEdit *leTmax;

    PhysicalModel *model;
    Zview *dialogZ;
    QGroupBox  *gbScaleXY;
    QMap<int,TofzCurve*> curves;
    friend class ZTDraggable;
    int curve_number;
};
class TZWidget : public QGroupBox
{
    Q_OBJECT
public:
    TZWidget(PhysicalModel *model, QWidget * parent = 0);
    void readFromXml(QXmlStreamReader *r);
    void writeToXml(QXmlStreamWriter *w);
private:
    TofzView *tofzView;

};

