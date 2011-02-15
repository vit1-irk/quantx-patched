#include "WavePacketKView.h"
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

WavePacketKView::WavePacketKView(PhysicalModel *m, QWidget *parent)
: QGraphicsView(parent), model(m), gbScaleXY(0),lineh(0),linev(0),
kmin(-5.), kmax(5.), 
phiMax(10), phiMin(-0.1)
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
    connect(model,SIGNAL(signalTimeChanged(double)),this,SLOT(slot_WavePacket_of_t()));
    resizePicture();
}
void WavePacketKView::resizePicture()
{
    setViewportMapping();
    slot_WavePacket_of_t();
}

void WavePacketKView::setViewportMapping()
{
    QRectF vp = QRectF(QPointF(this->kmin,this->phiMin),QPointF(this->kmax,this->phiMax));

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
void WavePacketKView::resizeEvent(QResizeEvent*) 
{
    setViewportMapping();
}

void WavePacketKView::mouseMoveEvent(QMouseEvent *e)
{
    QPointF f = mapToScene(e->pos());
    emit(infoMouseMovedTo(f));
    QGraphicsView::mouseMoveEvent(e);
}
void WavePacketKView::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
}
void WavePacketKView::scaleView(qreal scaleFactor)
{
    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.007 || factor > 1000)
        return;

    scale(scaleFactor, scaleFactor);
}

void WavePacketKView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Plus:
        scaleView(qreal(1.2));
        break;
    case Qt::Key_Minus:
        scaleView(1 / qreal(1.2));
        break;
    case Qt::Key_Space:
    case Qt::Key_Enter:
        break;
    default:
         QGraphicsView::keyPressEvent(event);
    }
}

void WavePacketKView::slot_WavePacket_of_t()
{
/*    static const QColor colorForIds[6] = {
        Qt::red, Qt::green, Qt::black, Qt::cyan, Qt::magenta, Qt::yellow
    };
    const int size_colorForIds = sizeof(colorForIds)/sizeof(colorForIds[0]);

    QVector<double> Ebound = model->getEn();
    int number_of_levels = Ebound.size();
    */
    QVector<double> Ebound = model->getEn();
    int number_of_levels = Ebound.size();
    if(number_of_levels==0) return;    
    int iwp=model->get_type_of_WP();
    if(iwp==1) return;
    for ( QMap<int,MomentumDistributionCurve*>::iterator i = curves.begin();  i != curves.end();   ++i)
    {
        int n = i.key();
        removeCurve(n);
    }
    if(!linev)
    {
        lineh = new QGraphicsLineItem();
        linev = new QGraphicsLineItem();
        linev->setLine(0., phiMin, 0., phiMax);
        lineh->setLine(kmin,0.,kmax,0.);
        scene()->addItem(lineh);
        scene()->addItem(linev);
    }
    else
    {
        linev->setLine(0., phiMin, 0., phiMax);
        lineh->setLine(kmin,0.,kmax,0.);
    }
//    model->buildWPmE(nmaxWP, nminWP, hnWP);
//    int nmin=model->get_nminWP();
//    int nmax=model->get_nmaxWP();
//    int hn=model->get_hnWP();
//     if(nmin!=this->nminWP||nmax!=this->nmaxWP||hn!=this->hnWP) 
//    {
//        need_build_WavePacket=true;
//        model->set_nminWP(this->nminWP);
//        model->set_nmaxWP(this->nmaxWP);
//        model->set_hnWP(this->hnWP);
//    }
//    else
//    {
//       need_build_WavePacket=false;
//    }
    int npoints=501;
    QVector<double> waveFunction;
    QPolygonF psi;
    psi.resize(npoints);
    waveFunction.resize(npoints);
    double dk = (kmax-kmin)/(npoints-1);
    {
            waveFunction = model->getPsiOfKT(kmin, kmax, npoints);
            for (int i=0; i < npoints; i++)
            {
                double kk = kmin + dk*i;
                double y = waveFunction[i];
                psi[i]  = QPointF(kk, y);
            }
            setCurve(0,psi,QPen(Qt::darkCyan));
            //dima qApp->processEvents(QEventLoop::AllEvents);
    }
}


void WavePacketKView::setCurve(int id,const QPolygonF & curve, const QPen& pen)
{

    if (curves[id])
    {
        curves[id]->setPolygon(curve);
    }
    else
    {
        MomentumDistributionCurve *c = new MomentumDistributionCurve(curve,this);
        scene()->addItem(c);
        curves[id] = c;
    }
    curves[id]->setPen(pen);
//    repaint();
    update();
}

void WavePacketKView::removeCurve(int id)
{
    scene()->removeItem(curves[id]);
    delete curves[id];
    curves[id] = 0; //this is dangerous: curves.remove(id);
    update();
//    repaint();
}

void MomentumDistributionCurve::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    painter->setPen( pen() );
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPolyline(polygon().data(),polygon().size());
}

void WavePacketKView::showDialogScaleY()
{   
    if (!gbScaleXY) 
    {
        gbScaleXY = new QGroupBox(this);
        gbScaleXY->setWindowTitle("Scales for plots Phi(k)");
        gbScaleXY->setWindowFlags(Qt::Window);
        gbScaleXY->setFont(QFont("Serif", 12, QFont::Bold )); 
        QVBoxLayout *vl = new QVBoxLayout;
        phiMin.setDisplay(("PhiMin"),("Y-scale for momentum distribution"),vl);
        phiMax.setDisplay(("PhiMax"),("Y-scale for momentum distribution"),vl);
        this->kmin.setDisplay(("kmin"),("lower bond of k-interval"),vl);
        this->kmax.setDisplay(("kmax"),("upper bond of k-interval"),vl);
        gbScaleXY->setLayout(vl);
    }
    gbScaleXY->show(); 
    gbScaleXY->raise();
    gbScaleXY->setFocus();
}

void MomentumDistributionCurve::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    if (event->buttons() & Qt::RightButton)
    {
        QMenu m;
//        QAction *scaleX = m.addAction("Set something");
        QAction *scalePsi = m.addAction("Set scales");
        QAction *what = m.exec(event->screenPos());
        if (what == scalePsi)
        {
            view->showDialogScaleY();
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
