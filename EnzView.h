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
#include <QtGui>
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
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include "myparam.h"
#include "BreakStatus.h"
#include "Zview.h"
#include <QGroupBox>

class EnzView;
class ZDraggable;

class EnzCurve : public QGraphicsPolygonItem
{
    EnzView *view;
public:
    EnzCurve(const QPolygonF& p, EnzView *_view, QGraphicsItem *parent=0,QGraphicsScene *scene=0)
        : QGraphicsPolygonItem(p,parent), view(_view)
    {
    }
    void paint (QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
//    void mousePressEvent(QGraphicsSceneMouseEvent * event);
};

class EnzView : public QGraphicsView
{
    Q_OBJECT
public:

    EnzView(PhysicalModel *m, QWidget *parent = 0);
//    virtual ~EnzView();

    void setCurve(int id,const QPolygonF& pcurve,const QPolygonF& curve, const QPen& = QPen());
    void removeCurve(int id);
//    void redrawCurves();
public:
    void showDialogScaleY();
    bool Erase;

public slots:
    void redrawCurves();
    void setViewportMapping();
    void resizePicture();
    void slot_En_of_z();
    void slotZline();
    void updateScaleEnz();
//    void slot_drawEc_n();
signals:
    void infoMouseMovedTo(QPointF);
    void signalScaleEnzChanged();
protected:
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void scaleView(qreal scaleFactor);
    void setScalesFromModel();
    void clearAll();

public:
    void showDialogZ();
    QPair<double,double> getEnzMinMax();
    void setEnzMinMax(const QPair<double,double> &s);
    void setScaleEnz();

//    getScalesEnz();
private:
    void initDialogScaleY();
    QLineEdit *leEnzmin;
    QLineEdit *leEnzmax;
    double Enzmin, Enzmax;
    QGroupBox  *gbScaleXY;
    double lineWidth;
    int numberOfCurves;
    QVector<QPolygonF> adjCurves;
    QVector<QPolygonF> physCurves;
    QVector<QPolygonF> adjCurvesI;
    QVector<QPolygonF> physCurvesI;
    QVector<QPolygonF> adjCurvesR;
    QVector<QPolygonF> physCurvesR;
    QVector<QColor> colorOfCurves;

    int curve_number;
    double zmax, zmin, hz;
//    double Umin, Umax;
    QGraphicsLineItem *lineh,*linev;
    QGraphicsRectItem *rectEG;
//    QVector<QPointF*> pointEG;

    void contextMenuEvent(QContextMenuEvent *event);
    ZDraggable *lineZ;

    friend class ZDraggable;

    PhysicalModel *model;
    Zview *dialogZ;

    QMap<int,EnzCurve*> curves;
    QMap<int,QPolygonF> pcurves;

};
class EnzWidget : public QGroupBox
{
    Q_OBJECT
public:
    EnzWidget(PhysicalModel *model, QWidget * parent = 0);
    void readFromXml(QXmlStreamReader *r);
    void writeToXml(QXmlStreamWriter *w);
public slots:
    void slotRunEnz();
    void slotErase();
private:
    EnzView *enzView;
    QToolButton *bRunEnz;
};
