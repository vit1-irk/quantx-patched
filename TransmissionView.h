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
public slots:
    void setViewportMapping();
    void resizePicture();
//    void slot_T_of_E();
    void slot_whole_T_of_E();
    void slotEline();
    void clearAll();
    void updateScaleTE();
signals:
    void infoMouseMovedTo(QPointF);
//    void signalScalesChanged();
    void signalScaleTEChanged();    
protected:
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void scrollView(int dx, int dy);
    void setScaleTE();
    void scaleView(qreal scaleFactor);
    void setScalesFromModel();
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
    void initDialogScaleY();
    void updateRubberBandRegion();
    double tMax, tMin; 
    double Emin,Emax,hE;
    QGraphicsLineItem *lineh,*linev;
    EnergyDraggable *lineE;

    QLineEdit *leTmin;
    QLineEdit *leTmax;
    QLineEdit *leEmin;
    QLineEdit *leEmax;
    QLineEdit *leHE;
    double widthLine;
    
    PhysicalModel *model;
    QGroupBox  *gbScaleXY;
    QMap<int,TransmissionCurve*> curves;
//    EnergyDraggable* lineEnergy;
    friend class EnergyDraggable;

//    bool rubberBandIsShown;
//    bool savePlot();
//    bool savePlotAs();
//    QRect rubberBandRect;
//    QPixmap pixmap;
    int curve_number;
//    std::vector zoomStack;
//    std::vector<ViewSettings> zoomStack;
//    int curZoom,yScale,xScale;

};
class TransmissionWidget : public QGroupBox
{
    Q_OBJECT
public:
    TransmissionWidget(PhysicalModel *model, QWidget * parent = 0);
private:
    TransmissionView *transmissionView;
    QPushButton *bRunTE;	

public slots:
    void slotRunTE();

};
/*class ViewSettings
{ 
public:
    ViewSettings();
    void scrollview(int dx, int dy);
//    void adjust();
    double spanX() const {return tMax-tMin;}
    double spanY() const {return Emax-Emin;}
    double tMin;
    double tMax;
    int numTTicks;
    double Emin;
    double Emax;
    int numETicks;
};
*/

