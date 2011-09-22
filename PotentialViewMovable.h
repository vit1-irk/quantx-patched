#pragma once
#include <QtGui>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QResizeEvent>
#include <QMouseEvent>
//#include "DraggableLine.h"
#include <QPointF>
#include <QGraphicsItem>
#include "PhysicalModel.h"
#include <QGraphicsPolygonItem>
#include <QPolygonF>
#include <QMap>
#include "myparam.h"
#include "ScalesUx.h"
#include "ScalePsin.h"

class EnergyDraggableLine;
class HorDraggableLine;
class VerDraggableLine;
class EnergyLevels;
class PotentialViewMovable;

class MyGraphicsPolylineItem : public QGraphicsPolygonItem
{
    PotentialViewMovable *view;
public:
    MyGraphicsPolylineItem(const QPolygonF& p, PotentialViewMovable *_view, QGraphicsItem *parent=0,QGraphicsScene *scene=0)
        : QGraphicsPolygonItem(p,parent,scene), view(_view)
    {
    }
    void paint (QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
//    void mousePressEvent(QGraphicsSceneMouseEvent * event);
};

class PotentialViewMovable : public QGraphicsView
{
    Q_OBJECT
public:

    PotentialViewMovable(PhysicalModel *m, QWidget *parent = 0);
//    virtual ~PotentialViewMovable();

    void setCurve(int id,const QPolygonF&, const QPen& = QPen());
    MyGraphicsPolylineItem *getCurve(int id) const { return curves[id]; }
//    void mousePressEvent(QMouseEvent * event);
    void contextMenuEvent(QContextMenuEvent *event);

    void removeCurve(int id);

public slots:

    void slotEboundChanged();
    void slotEnergyChanged();
    void slotUChanged();
    void slotUperChanged();
    void setViewportMapping();
    void resizePicture();
    void setScalesFromModel();
    void updateRangeE();
    void changeE();
//    void setRangeE();
    void modelChanged();
signals:
    void infoMouseMovedTo(QPointF);
//    void signalRangeEChanged();
protected:
    void initDialogRangeE();
    void showDialogRangeE();
    void scrollView(int dx, int dy);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void scaleView(qreal scaleFactor);
    void setRangeE();

public:
    void showDialogScaleY();
    ScalesUx *dialogScalesU;
    PhysicalModel *model;

private:
    double widthLineV;
    double widthLineH;
    double widthLineE;
    QLineEdit *leEmin;
    QLineEdit *leEmax;
    QLineEdit *leHE;
    double lineWidth;
    double Umin,Umax, xmin, xmax,Emax,Emin,hE;
//        dx, psiMax, psiMin;
    QGroupBox  *gbRangeE;

//    PhysicalModel *model;

    QVector<HorDraggableLine*> linesU;
    QVector<VerDraggableLine*> linesV;

    QVector<HorDraggableLine*> linesUmax;
    QVector<VerDraggableLine*> linesVmax;

    QVector<HorDraggableLine*> linesUmin;
    QVector<VerDraggableLine*> linesVmin;

    QVector<EnergyLevels*> linesEn;
    EnergyDraggableLine* lineEnergy;
    QMap<int,MyGraphicsPolylineItem*> curves;

    friend class HorDraggableLine;
    friend class EnergyLevels;
    friend class VerDraggableLine;
    friend class EnergyDraggableLine;

    int whatToDraw;
public slots:
    void setWhatToDraw(int);
signals:
    void whatToDrawChanged(int);
public:
    int getWhatToDraw();
};

class PotentialMovableWidget : public QGroupBox
{
    Q_OBJECT
public:
    PotentialMovableWidget(PhysicalModel *model, QWidget * parent = 0);
    PotentialViewMovable *potentialViewMovable;
public slots:
    void slotRunE();
    void updateEnergy();
    void updateLevelNumber();
    void printEnergy();
    void printLevelNumber();
//    void runHelp_ctx();
private:
    void runHelp(const char *);
    QLineEdit *lE;
    QLineEdit *lN;
    QButtonGroup *bgR;
    QToolButton *bRun;

};

