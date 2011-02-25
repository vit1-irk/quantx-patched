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

class WavePacketKView;

class MomentumDistributionCurve : public QGraphicsPolygonItem
{
    WavePacketKView *view;
public:
    MomentumDistributionCurve(const QPolygonF& p, WavePacketKView *_view, QGraphicsItem *parent=0,QGraphicsScene *scene=0)
        : QGraphicsPolygonItem(p,parent,scene), view(_view)
    {
    }
    void paint (QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
//    void mousePressEvent(QGraphicsSceneMouseEvent * event);
};

class WavePacketKView : public QGraphicsView
{
    Q_OBJECT
public:

    WavePacketKView(PhysicalModel *m, QWidget *parent = 0);
    void setCurve(int id,const QPolygonF&, const QPen& = QPen());
    MomentumDistributionCurve *getCurve(int id) const { return curves[id]; }
    void removeCurve(int id);
    void contextMenuEvent(QContextMenuEvent *event);

     
public slots:
    void setViewportMapping();
    void resizePicture();
    void slot_WavePacket_of_t();
signals:
    void infoMouseMovedTo(QPointF);

protected:
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void scaleView(qreal scaleFactor);

public:
    void showDialogScaleY();

private:
    double widthLineV;
    double widthLineH;
    double widthLineE;
    MyParamD phiMax, phiMin; 
    MyParamD kmin,kmax;
    QGraphicsLineItem *lineh,*linev;

    PhysicalModel *model;
    QGroupBox  *gbScaleXY;

    QMap<int,MomentumDistributionCurve*> curves;

};
