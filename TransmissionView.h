#pragma once
#include <QtGui>
//#include "MainWindow.h"
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
//    TransmissionView(PhysicalModel *m, MainWindow *mw, QWidget *parent = 0);
    void setCurve(int id,const QPolygonF&, const QPen& = QPen());
    TransmissionCurve *getCurve(int id) const { return curves[id]; }
    void removeCurve(int id);
//    void setPlotSettings(const PlotSettings &settings);
public slots:
    void setViewportMapping();
    void resizePicture();
    void slot_T_of_E();
    void slot_whole_T_of_E();
    void slotEline();
signals:
    void infoMouseMovedTo(QPointF);

protected:
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void scaleView(qreal scaleFactor);
    void setScalesFromModel();
    void clearAll();
    bool Erase;
    void contextMenuEvent(QContextMenuEvent *event);

/*    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void enterEvent(QEvent *event);
*/
public:
    void showDialogScaleY();

private:
    void updateRubberBandRegion();
    double widthLine;
    MyParamD tMax, tMin; 
    MyParamD Emin,Emax;
    QGraphicsLineItem *lineh,*linev,*lineE;
    
    PhysicalModel *model;
    QGroupBox  *gbScaleXY;
    QMap<int,TransmissionCurve*> curves;
    bool rubberBandIsShown;
//    bool savePlot();
//    bool savePlotAs();
    QRect rubberBandRect;
    QPixmap pixmap;
    int curve_number;
};

