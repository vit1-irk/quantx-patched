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
public:
    void showDialogScaleY();

private:
    void initDialogScaleY();
    void updateRubberBandRegion();
    double tMax, tMin; 
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
class EofqaWidget : public QGroupBox
{
    Q_OBJECT
public:
    EofqaWidget(PhysicalModel *model, QWidget * parent = 0);
private:
    EofqaView *eofqaView;
//    QPushButton *bRunEqa;	

public slots:
    void slotRunEqa();

};
