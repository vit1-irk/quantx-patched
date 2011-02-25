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

class EnzView;

class EnzCurve : public QGraphicsPolygonItem
{
    EnzView *view;
public:
    EnzCurve(const QPolygonF& p, EnzView *_view, QGraphicsItem *parent=0,QGraphicsScene *scene=0)
        : QGraphicsPolygonItem(p,parent,scene), view(_view)
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
    void setCurve(int id,const QPolygonF&, const QPen& = QPen());
//    EnzCurve *getCurve(int id) const { return curves[id]; }
    void removeCurve(int id);
     
public slots:
    void setViewportMapping();
    void resizePicture();
    void slot_En_of_z();
signals:
    void infoMouseMovedTo(QPointF);

protected:
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void scaleView(qreal scaleFactor);
    void setScalesFromModel();

public:
//    void showDialogScaleY();

private:
    double widthLine;
    double zmax, zmin; 
    double Umin, Umax;
    QGraphicsLineItem *lineh,*linev;

    PhysicalModel *model;
    QGroupBox  *gbScaleXY;

    QMap<int,EnzCurve*> curves;

};
