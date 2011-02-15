#include "EnzView.h"
#include <QGraphicsView>
#include <QMouseEvent>
#include <QMatrix>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QRectF>
#include <QLineF>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QWidget>
#include <QAction>
#include <QPainterPath>
#include "BreakStatus.h"

EnzView::EnzView(PhysicalModel *m, QWidget *parent)
: QGraphicsView(parent), model(m), gbScaleXY(0),lineh(0),linev(0),
Umin(-11.), Umax(0.1)
{
    setScene(new QGraphicsScene(this));
    scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
    if (1)
    {
        setViewportUpdateMode(BoundingRectViewportUpdate);///
        setCacheMode(CacheBackground);
        setRenderHint(QPainter::Antialiasing);///
        setTransformationAnchor(AnchorUnderMouse);///
        setResizeAnchor(AnchorViewCenter);///
    }
//    setScalesFromModel();
//    connect(model,SIGNAL(signalPotentialChanged()),this,SLOT(slot_En_of_z()));
//    resizePicture();
}
void EnzView::setScalesFromModel()
{
    QPair<double,double> umin_umax = model->getUminUmax();
    Umin = umin_umax.first;
    Umax = umin_umax.second;
}

void EnzView::resizePicture()
{
    setViewportMapping();
    slot_En_of_z();
}
void EnzView::setViewportMapping()
{
    zParameters tp = model->getzParam();
    double zmin=tp.zmin;
    double zmax=tp.zmax;
    QRectF vp = QRectF(QPointF(zmin,this->Umin),QPointF(zmax,this->Umax));

    QRectF sr = scene()->sceneRect();
//    if (vp != sr)
    {
        scene()->setSceneRect(vp);

        QRectF b = vp; //scene()->sceneRect();
        QRectF a = QRectF(this->viewport()->rect());
        qreal m11 = a.width() / b.width();
        qreal m22 = - a.height() / b.height();
        qreal dx = - m11 * a.x();
        qreal dy = - m22 * (a.y() + a.height());
        QMatrix m(m11,0,0,m22,dx,dy);
        this->setMatrix(m);
        scene()->update(scene()->sceneRect());
        sr = scene()->sceneRect();
    }
    update();
} 
void EnzView::resizeEvent(QResizeEvent*) 
{
    setViewportMapping();
}

void EnzView::mouseMoveEvent(QMouseEvent *e)
{
    QPointF f = mapToScene(e->pos());
    emit(infoMouseMovedTo(f));
    QGraphicsView::mouseMoveEvent(e);
}
void EnzView::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
}
void EnzView::scaleView(qreal scaleFactor)
{
    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.007 || factor > 1000)
        return;

    scale(scaleFactor, scaleFactor);
}

void EnzView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Plus:
        scaleView(qreal(1.2));
        break;
    case Qt::Key_Minus:
        scaleView(1 / qreal(1.2));
        break;
    case Qt::Key_Space:
        setBreakStatus(1);
        break;
    case Qt::Key_Enter:
        setBreakStatus(1);
        break;
    case Qt::Key_Escape:
        setBreakStatus(1);
        break;
    case Qt::Key_Pause :
        setBreakStatus(1);
        break;
    }
    QGraphicsView::keyPressEvent(event);
}
typedef  QPolygonF mycurve;
typedef  QVector<mycurve> curveSet;
//typedef  std::vector<double> mycurve;
//typedef  QVector<mycurve> curveSet;

static void updateCurves(curveSet& cs, const QVector<double>& Ebound, double z)
{
    for (int i=0; i < Ebound.size(); i++)
    {
        if (i >= cs.size())
        {
            mycurve a;
            cs.push_back(a);
        }
        cs[i].push_back(QPointF(z, Ebound[i]));
//        cs[i].push_back(z);
//        cs[i].push_back(Ebound[i]);
    }
}

static void myrepaint(const curveSet& cs)
{ 
    for(int i=0; i<cs.size(); i++)
    {
 //       setCurve(i,cs[i]);
    }
}

void EnzView::slot_En_of_z()
{
    static const QColor colorForIds[6] = {
        Qt::red, Qt::green, Qt::black, Qt::cyan, Qt::magenta, Qt::yellow
    };
    const int size_colorForIds = sizeof(colorForIds)/sizeof(colorForIds[0]);
    zParameters tp = model->getzParam();
    double zt=tp.z;
    double hz=tp.hz;
    double zmin=tp.zmin;
    double zmax=tp.zmax;
    double zz=zt+hz;
    if(zz>=zmax) 
    { 
        zt=zmin;
    }
    if(zz<zmin) 
    { 
        zt=zmin;
    }
    if(!linev)
    {
        lineh = new QGraphicsLineItem();
        linev = new QGraphicsLineItem();
        lineh->setLine(-0.1*zmax, 0., 1.1*zmax, 0);
        linev->setLine(0.,1.1*Umin,0.,-.1*Umin);
        scene()->addItem(lineh);
        scene()->addItem(linev);
    }
    else
    {
        lineh->setLine(-0.1*zmax, 0., 1.1*zmax, 0);
        linev->setLine(0.,1.1*Umin,0.,-0.1*Umin);
    }
    int n=0; 
        curveSet cs;
        int nmxold=-1;
//    int npoints=301;
//    double dz = (zmax-zmin)/(npoints-1);
         for (double z=zt; z>=tp.zmin&&z<=tp.zmax; z+=tp.hz)
//        for(double z=this->zmin;z<=this->zmax; z+=dz)
        { 
//            if (getBreakStatus(0)) 
//                return;
//            model->set_Uxz(z);
            tp.z=z;
            model->setzParam(tp);
            QVector<double> Ebound = model->getEn();
            int nmx=Ebound.size()-1;//this->nmaxLevel;
            int nmn=0;//this->nminLevel;
            if(nmxold==-1) 
            {
                nmxold=nmx;
            }
            updateCurves(cs, Ebound, z);
            for(int i=0; i<cs.size(); i++)
    {
            setCurve(i,cs[i],colorForIds[i % size_colorForIds]);
    }
//            myrepaint(cs);
            nmxold=nmx;
            if (getBreakStatus(0)) 
            {
                return;
            }
        }
    update();
}

void EnzView::setCurve(int id,const QPolygonF & curve, const QPen& pen)
{

    if (curves[id])
    {
        curves[id]->setPolygon(curve);
    }
    else
    {
        EnzCurve *c = new EnzCurve(curve,this);
        scene()->addItem(c);
        curves[id] = c;
    }
    curves[id]->setPen(pen);
//    repaint();
    update();
}

void EnzView::removeCurve(int id)
{
    scene()->removeItem(curves[id]);
    delete curves[id];
    curves[id] = 0; //this is dangerous: curves.remove(id);
    update();
//    repaint();
}

void EnzCurve::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    painter->setPen( pen() );
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPolyline(polygon().data(),polygon().size());
}

/*void EnzView::showDialogScaleY()
{   
    if (!gbScaleXY) 
    {
        gbScaleXY = new QGroupBox(this);
        gbScaleXY->setWindowTitle("Scales for plots Psi(x) and U(x)");
        gbScaleXY->setWindowFlags(Qt::Window);
        gbScaleXY->setFont(QFont("Serif", 12, QFont::Bold )); 
        QVBoxLayout *vl = new QVBoxLayout;
        tMin.setDisplay(("Tmin"),("Y-scale for transmission plot"),vl);
        tMax.setDisplay(("Tmax"),("Y-scale for transmission plot"),vl);
        this->Emin.setDisplay(("Emin"),("lower bond of E-interval"),vl);
        this->Emax.setDisplay(("Emax"),("upper bond of E-interval"),vl);
        gbScaleXY->setLayout(vl);
    }
    gbScaleXY->show(); 
    gbScaleXY->raise();
    gbScaleXY->setFocus();
}
*/

void EnzCurve::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    if (event->buttons() & Qt::RightButton)
    {
        QMenu m;
//        QAction *scaleX = m.addAction("Set something");
        QAction *scalePsi = m.addAction("Set scales");
        QAction *what = m.exec(event->screenPos());
        if (what == scalePsi)
        {
//            view->showDialogScaleY();
            update();
        }
        event->accept();
#if 0
        QPainterPath p = shape();
        QPointF v = event->pos();
        QRectF r = QRectF(v,v).adjusted(-0.1,-0.1,0.1,0.1);

        bool i = p.intersects(r);
        if (i)
        {
            QMenu m;
            QAction *scaleX = m.addAction("Set something");
            QAction *scaleY = m.addAction("Set scales");
            QAction *what = m.exec(event->screenPos());
            if (what == scaleY)
            {
                view->scaleY();
            }
               update();// repaint();
            event->accept();
            return;
        }
#endif
    }
}

