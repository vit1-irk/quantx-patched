#include "MomentumView.h"
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

MomentumView::MomentumView(PhysicalModel *m, QWidget *parent)
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
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(model,SIGNAL(signalEboundChanged()),this,SLOT(slot_Phi_n_of_k()));
    resizePicture();
}
void MomentumView::resizePicture()
{
    setViewportMapping();
    slot_Phi_n_of_k();
}

void MomentumView::setViewportMapping()
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
void MomentumView::resizeEvent(QResizeEvent *e) 
{
    setViewportMapping();
}

void MomentumView::mouseMoveEvent(QMouseEvent *e)
{
    QPointF f = mapToScene(e->pos());
    emit(infoMouseMovedTo(f));
    QGraphicsView::mouseMoveEvent(e);
}
void MomentumView::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
    event->accept();
}
void MomentumView::scaleView(qreal scaleFactor)
{
    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.007 || factor > 1000)
        return;

    scale(scaleFactor, scaleFactor);
}

void MomentumView::keyPressEvent(QKeyEvent *event)
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
    case Qt::Key_Enter:
        break;
    }
    QGraphicsView::keyPressEvent(event);
}

void MomentumView::slot_Phi_n_of_k()
{
    static const QColor colorForIds[6] = {
        Qt::red, Qt::green, Qt::black, Qt::cyan, Qt::magenta, Qt::yellow
    };
    const int size_colorForIds = sizeof(colorForIds)/sizeof(colorForIds[0]);

    QVector<double> Ebound = model->getEn();
    int number_of_levels = Ebound.size();
    if(number_of_levels==0) return;
    int nmin=model->get_LevelNmin();
    int nmax=model->get_LevelNmax();
/*    if(nmax>=number_of_levels) 
    {
        nmax=number_of_levels-1;
    }*/
    for ( QMap<int,MomentumDistribution*>::iterator i = curves.begin();  i != curves.end();   ++i)
    {
        int n = i.key();
        if (n >= number_of_levels)
        {
            removeCurve(n);
        }
        if(n>nmax&&nmax<number_of_levels-1) removeCurve(n);
        if(n<nmin&&nmin<number_of_levels-1) removeCurve(n);
    }

/*    for (int n = 0; n < number_of_levels; ++n)
    {
        if (! curves[n])
        {
            curves[n] = new MomentumDistribution(curves[n],this);
            scene()->addItem(curves[n]);
        }
    }
*/
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

    int npoints=500;
    QVector<double> waveFunction;
    QPolygonF phi;
    phi.resize(npoints);
    waveFunction.resize(npoints);
    double dk = (kmax-kmin)/(npoints-1);
//    int nmin=model->get_LevelNmin();
//    int nmax=model->get_LevelNmax();
//    if(nmax>=number_of_levels) 
//    {
//        nmax=number_of_levels-1;
//    }
//    if(nmin>nmax) return;
    for (int n = nmin; n <= nmax; ++n)
//    for (int n = 0; n < number_of_levels; ++n)
    {
            if(n>number_of_levels-1) break;    
            waveFunction = model->getPhiOfk(Ebound[n],kmin,kmax,npoints);
            for (int i=0; i < npoints; i++)
            {
                double x = kmin + dk*i;
                double y = waveFunction[i];
                phi[i]  = QPointF(x, y);
            }
            setCurve(n,phi,colorForIds[n % size_colorForIds]);
    }
    update();
}

void MomentumView::setCurve(int id,const QPolygonF & curve, const QPen& pen)
{

    if (curves[id])
    {
        curves[id]->setPolygon(curve);
    }
    else
    {
        MomentumDistribution *c = new MomentumDistribution(curve,this);
        scene()->addItem(c);
        curves[id] = c;
    }
    curves[id]->setPen(pen);
//    repaint();
    update();
}

void MomentumView::removeCurve(int id)
{
    scene()->removeItem(curves[id]);
    delete curves[id];
    curves[id] = 0; //this is dangerous: curves.remove(id);
    update();
//    repaint();
}

void MomentumDistribution::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    painter->setPen( pen() );
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPolyline(polygon().data(),polygon().size());
}

void MomentumView::showDialogScaleY()
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
void MomentumView::contextMenuEvent(QContextMenuEvent *event)
{
        QMenu m;
        QAction *scalePsi = m.addAction("Set scales");
        QAction *what = m.exec(event->globalPos());
        if (what == scalePsi)
        {
            this->showDialogScaleY();
            update();
        }
}
/*void MomentumDistribution::mousePressEvent(QGraphicsSceneMouseEvent * event)
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
*/