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
#include "ScalePhin.h"
#include <QGroupBox>

class MomentumView;

class MomentumDistribution : public QGraphicsPolygonItem
{
    MomentumView *view;
public:
    MomentumDistribution(const QPolygonF& p, MomentumView *_view, QGraphicsItem *parent=0,QGraphicsScene *scene=0)
        : QGraphicsPolygonItem(p,parent), view(_view)
    {
    }
    void paint (QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
//    void mousePressEvent(QGraphicsSceneMouseEvent * event);
};

class MomentumView : public QGraphicsView
{
    Q_OBJECT
public:

    MomentumView(PhysicalModel *m, QWidget *parent = 0);
//    virtual ~MomentumView();

    void setCurve(int id,const QPolygonF&, const QPen& = QPen());
private:
    MomentumDistribution *getCurve(int id) const { return curves[id]; }
    void removeCurve(int id);
    void contextMenuEvent(QContextMenuEvent *event);

public slots:
    void setViewportMapping();
    void resizePicture();
    void slot_Phi_n_of_k();
    void slotEnergyChanged();
signals:
    void infoMouseMovedTo(QPointF);

protected:
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void scaleView(qreal scaleFactor);
    void clearAll();

public:
    void showDialogScale();

private:
    ScalePhin  *dialogScalePhin;

    double widthLineV;
    double widthLineH;
    double widthLineE;
    double phiMax, phiMin;
    double kmin,kmax,dk;
    double  lineWidth;
//    MyParamD phiMax, phiMin;
//    MyParamD kmin,kmax;
    QGraphicsLineItem *lineh,*linev;

    PhysicalModel *model;
//    QGroupBox  *gbScaleXY;

    QMap<int,MomentumDistribution*> curves;

};

class MomentumDistibutionWidget : public QGroupBox
{
    Q_OBJECT
public:
    MomentumDistibutionWidget(PhysicalModel *model, QWidget * parent = 0);
};
