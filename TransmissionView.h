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
#include "myparam.h"
#include <QPaintEvent>

class EnergyDraggable;
class TransmissionView;

class TransmissionCurve : public QGraphicsPolygonItem
{
    TransmissionView *view;
public:
    TransmissionCurve(const QPolygonF& p, TransmissionView *_view, QGraphicsItem *parent=0,QGraphicsScene *scene=0)
        : QGraphicsPolygonItem(p,parent,scene), view(_view)
    {
    }
    void paint (QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
//    void mousePressEvent(QGraphicsSceneMouseEvent * event);

};

class TransmissionView : public QGraphicsView
{
    Q_OBJECT
public:

    TransmissionView(PhysicalModel *m, QWidget *parent = 0);
//    virtual ~TransmissionView();
    void setCurve(int id,const QPolygonF&, const QPen& = QPen());
    TransmissionCurve *getCurve(int id) const { return curves[id]; }
    void removeCurve(int id);
    bool Erase;
public slots:
    void setViewportMapping();
    void resizePicture();
    void reDraw();
    void slot_whole_T_of_E();
    void slotEline();
    void clearAll();
    void updateScaleTE();
signals:
    void infoMouseMovedTo(QPointF);
    void signalScaleTEChanged();
protected:
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void scrollView(int dx, int dy);
//    void setScaleTE();
    void scaleView(qreal scaleFactor);
    void setScalesFromModel();
    void contextMenuEvent(QContextMenuEvent *event);

public:
    void showDialogScaleY();
    void setScaleTE();

    double tMax, tMin;
    double Emin,Emax,hE;

private:

    void initDialogScaleY();
    void updateRubberBandRegion();
//    double tMax, tMin;
//    double Emin,Emax,hE;
    QGraphicsLineItem *lineh,*linev;
    EnergyDraggable *lineE;

    QLineEdit *leTmin;
    QLineEdit *leTmax;
    QLineEdit *leEmin;
    QLineEdit *leEmax;
    QLineEdit *leHE;
    double lineWidth;

    PhysicalModel *model;
    QGroupBox  *gbScaleXY;
    QMap<int,TransmissionCurve*> curves;
    QVector<QPolygonF> physCurves;
    friend class EnergyDraggable;
    QVector<QColor> colorOfCurves;
    int curve_number;

};
class TEWidget : public QGroupBox
{
    Q_OBJECT
public:
    TEWidget(PhysicalModel *model, QWidget * parent = 0);
    void readFromXml(QXmlStreamReader *r);
    void writeToXml(QXmlStreamWriter *w);
private:
    TransmissionView *transmissionView;
    QPushButton *bRunTE;
    QCheckBox *ch;
public slots:
    void slotRunTE();
    void slotErase(int);

};

