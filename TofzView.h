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
#include "Zview.h"
#include <QPaintEvent>

class ZTDraggable;
class TofzView;

class TofzCurve : public QGraphicsPolygonItem
{
    TofzView *view;
public:
    TofzCurve(const QPolygonF& p, TofzView *_view, QGraphicsItem *parent=0,QGraphicsScene *scene=0)
        : QGraphicsPolygonItem(p,parent,scene), view(_view)
    {
    }
    void paint (QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
//    void mousePressEvent(QGraphicsSceneMouseEvent * event);

};

class TofzView : public QGraphicsView
{
    Q_OBJECT
public:

    TofzView(PhysicalModel *m, QWidget *parent = 0);
//    virtual ~TofzView();
    void setCurve(int id,const QPolygonF&, const QPen& = QPen());
    TofzCurve *getCurve(int id) const { return curves[id]; }
    void removeCurve(int id);
public slots:
    void setViewportMapping();
    void resizePicture();
    void slot_whole_T_of_z();
    void slotZline();
    void clearAll();
    void updateScaleTZ();
signals:
    void infoMouseMovedTo(QPointF);
    void signalScaleTChanged();
protected:
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void scrollView(int dx, int dy);
    void setScaleTZ();
    void scaleView(qreal scaleFactor);
    void setScalesFromModel();
    bool Erase;
    void contextMenuEvent(QContextMenuEvent *event);
public:
    void showDialogScaleY();
    void showDialogZ();
private:
    void initDialogScaleY();
    double tMax, tMin;
    double Zmin,Zmax,hZ;
    double lineWidth;
    QGraphicsLineItem *lineh,*linev;
    ZTDraggable *lineZ;

    QLineEdit *leTmin;
    QLineEdit *leTmax;

    PhysicalModel *model;
    Zview *dialogZ;
    QGroupBox  *gbScaleXY;
    QMap<int,TofzCurve*> curves;
    friend class ZTDraggable;
    int curve_number;
};
class TofzViewWidget : public QGroupBox
{
    Q_OBJECT
public:
    TofzViewWidget(PhysicalModel *model, QWidget * parent = 0);
private:
    TofzView *tofzView;

};

