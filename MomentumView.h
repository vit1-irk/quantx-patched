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

class MomentumView;

class MomentumDistribution : public QGraphicsPolygonItem
{
    MomentumView *view;
public:
    MomentumDistribution(const QPolygonF& p, MomentumView *_view, QGraphicsItem *parent=0,QGraphicsScene *scene=0)
        : QGraphicsPolygonItem(p,parent,scene), view(_view)
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
    void setCurve(int id,const QPolygonF&, const QPen& = QPen());
    MomentumDistribution *getCurve(int id) const { return curves[id]; }
    void removeCurve(int id);
    void contextMenuEvent(QContextMenuEvent *event);
 
public slots:
    void setViewportMapping();
    void resizePicture();
    void slot_Phi_n_of_k();
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

    QMap<int,MomentumDistribution*> curves;

};
