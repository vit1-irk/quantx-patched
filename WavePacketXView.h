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
#include "TimeView.h"
#include "PhysicalModel.h"
#include <QGraphicsPolygonItem>
#include <QPolygonF>
#include <QMap>
#include "myparam.h"
#include "WPparametersP.h"
#include "WPparametersM.h"
#include "ScaleWPX.h"
#include <QButtonGroup>
#include <QGroupBox>

class WavePacketXView;

class CoordinateDistributionCurve : public QGraphicsPolygonItem
{
    WavePacketXView *view;
public:
    CoordinateDistributionCurve(const QPolygonF& p, WavePacketXView *_view, QGraphicsItem *parent=0,QGraphicsScene *scene=0)
        : QGraphicsPolygonItem(p,parent), view(_view)
    {
    }
    void paint (QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
//    void mousePressEvent(QGraphicsSceneMouseEvent * event);
};

class WavePacketXView : public QGraphicsView
{
    Q_OBJECT
public:

    WavePacketXView(PhysicalModel *m, QWidget *parent = 0);
//    virtual ~WavePacketXView();
    void setCurve(int id,const QPolygonF&, const QPen& = QPen());
    CoordinateDistributionCurve *getCurve(int id) const { return curves[id]; }
    void removeCurve(int id);
    void contextMenuEvent(QContextMenuEvent *event);

public slots:
    void setViewportMapping();
    void resizePicture();
    void slot_WavePacket_of_t();
    void slotSetWPEm();
    void slotSetWPEp();
    void slotSetTime();
//    void updateWidth();
    void reDraw();
signals:
    void infoMouseMovedTo(QPointF);
    void signalWidthChanged();

protected:
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void scaleView(qreal scaleFactor);
public:
    void showDialogScaleY();
    void showDialogWavePacket();
//    void showDialogViewPsiX();
//    void showDialogDefWP();
private:
    void clearAll();
    QPointF getPoint(bool first, double x, complex y);
    void setInitialTime();
    void reDrawAxes();

    WPparametersM *dialogWPEm;
    WPparametersP *dialogWPEp;
    int lineWidth;
    double psiMax, psiMin;
    double xmin,xmax,dx;
    double xn,yn,xk,yk,cs,sn;
    ScaleWPX *dialogScaleWPX;
    double tmin,tmax, time, htime;
    QGraphicsLineItem *lineh,*linev,*vectorFirst,*linez;
    PhysicalModel *model;
    TimeView *dialogTime;
    QMap<int,CoordinateDistributionCurve*> curves;
    QVector<double> waveFunction;
    QVector<complex> waveFunctionC;
    QPen p,pl;

    int whatToDraw;
public slots:
    void setWhatToDraw(int);
signals:
    void whatToDrawChanged(int);
public:
    int getWhatToDraw();

};
class WavePacketXWidget : public QGroupBox
{
    Q_OBJECT
public:
    WavePacketXWidget(PhysicalModel *model, QWidget * parent = 0);
public slots:
    void checkButton(int);
private:
    QButtonGroup *bgR;
    WavePacketXView *wavePacketXView;
    QToolButton *bRunPsiXT;

public slots:
    void slotRunWP();

};

