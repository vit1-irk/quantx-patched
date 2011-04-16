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
#include "BreakStatus.h"
#include "Zview.h"

class EnzView;
class ZDraggable;

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
    virtual ~EnzView();

    void setCurve(int id,const QPolygonF&, const QPen& = QPen());
    void removeCurve(int id);
public:
    void showDialogScaleY();
     
public slots:
    void setViewportMapping();
    void resizePicture();
    void slot_En_of_z();
    void slotZline();
    void updateScaleEnz();

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

private:
    void initDialogScaleY();
    QLineEdit *leEnzmin;
    QLineEdit *leEnzmax;
    double Enzmin, Enzmax;
    void setScaleEnz();
    QGroupBox  *gbScaleXY;
    double widthLine; 

    double zmax, zmin, hz; 
//    double Umin, Umax;
    QGraphicsLineItem *lineh,*linev;
    void contextMenuEvent(QContextMenuEvent *event);
    ZDraggable *lineZ;

    friend class ZDraggable;

    PhysicalModel *model;
    Zview *dialogZ;

    QMap<int,EnzCurve*> curves;

};
class EnzWidget : public QGroupBox
{
    Q_OBJECT
public:
    EnzWidget(PhysicalModel *model, QWidget * parent = 0);
private:
    EnzView *enzView;
    QToolButton *bRunEnz;
public slots:
void slotRunEnz();
};
