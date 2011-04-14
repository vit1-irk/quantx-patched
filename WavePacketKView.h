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
#include "ScaleWPK.h"

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
    virtual ~WavePacketKView();
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

private:
    ScaleWPK *dialogScaleWPK;

public:
    void showDialogScaleY();

private:
    double widthLineV;
    double widthLineH;
    double widthLineE;
    double widthLineEn;
    double phiMax, phiMin; 
    double kmin,kmax,dk;
    QGraphicsLineItem *lineh,*linev;

    PhysicalModel *model;

    QMap<int,MomentumDistributionCurve*> curves;

};
class WavePacketKWidget : public QGroupBox
{
    Q_OBJECT
public:
    WavePacketKWidget(PhysicalModel *model, QWidget * parent = 0);
private:
    WavePacketKView *wavePacketKView;
//    QPushButton *bRunPsiKT;	

//public slots:
//    void slotRunWP();

};

