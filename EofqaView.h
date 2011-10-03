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

class EEDrag;
class EofqaView;
struct ScalesEQ
{
    double Emin,Emax,hE;
    double qaMin,qaMax;
    bool operator != (const ScalesEQ& o)
    {
        return hE != o.hE
            ||Emin != o.Emin
            ||Emax != o.Emax
            ||qaMin != o.qaMin
            ||qaMax != o.qaMax;
    }
};

class EofqaCurve : public QGraphicsPolygonItem
{
    EofqaView *view;
public:
    EofqaCurve(const QPolygonF& p, EofqaView *_view, QGraphicsItem *parent=0,QGraphicsScene *scene=0)
        : QGraphicsPolygonItem(p,parent,scene), view(_view)
    {
    }
    void paint (QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
//    void mousePressEvent(QGraphicsSceneMouseEvent * event);

};

class EofqaView : public QGraphicsView
{
    Q_OBJECT
public:

    ScalesEQ getScalesEQ() const;
    void setScalesEQ(const ScalesEQ&);

    EofqaView(PhysicalModel *m, QWidget *parent = 0);
//    virtual ~EofqaView();
    void setCurve(int id,const QPolygonF&, const QPen& = QPen());
    EofqaCurve *getCurve(int id) const { return curves[id]; }
    void removeCurve(int id);
public slots:
    void setViewportMapping();
    void resizePicture();
    void slot_Eofqa();
    void slotEline();
    void clearAll();
    void updateScaleQE();
signals:
    void infoMouseMovedTo(QPointF);
//    void signalScalesChanged();
    void signalScaleQAChanged();
protected:
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void scrollView(int dx, int dy);
    void setScaleQE();
    void scaleView(qreal scaleFactor);
    void setScalesFromModel();
    bool Erase;
    void contextMenuEvent(QContextMenuEvent *event);
public:
    void showDialogScaleY();

private:
    void initDialogScaleY();
    void updateRubberBandRegion();
    double qaMax, qaMin;
    double Emin,Emax,hE;
    QGraphicsLineItem *lineh,*linev;
    EEDrag *lineE;

    QLineEdit *leTmin;
    QLineEdit *leTmax;
    QLineEdit *leEmin;
    QLineEdit *leEmax;
    QLineEdit *leHE;
    double lineWidth;

    PhysicalModel *model;
    QGroupBox  *gbScaleXY;
    QMap<int,EofqaCurve*> curves;
    friend class EEDrag;

    int curve_number;
};
class QEWidget : public QGroupBox
{
    Q_OBJECT
public:
    QEWidget(PhysicalModel *model, QWidget * parent = 0);
    void readFromXml(QXmlStreamReader *r);
    void writeToXml(QXmlStreamWriter *w);
private:
    EofqaView *eofqaView;
//    QPushButton *bRunEqa;

public slots:
    void slotRunEqa();

};
