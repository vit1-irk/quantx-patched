#include "TransmissionView.h"
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

//TransmissionView::TransmissionView(PhysicalModel *m, MainWindow *mw, QWidget *parent)
TransmissionView::TransmissionView(PhysicalModel *m, QWidget *parent)
//: QGraphicsView(parent), model(m), mainWindow(mw), gbScaleXY(0),lineh(0),linev(0),
: QGraphicsView(parent), model(m), gbScaleXY(0),lineh(0),linev(0),lineE(0),
Emin(-1.), Emax(20.), Erase(true),
tMax(1.1), tMin(-0.1),curve_number(0)//,
//Eline(0.)
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
    rubberBandIsShown = false;

//    connect(model,SIGNAL(signalEboundChanged()),this,SLOT(slot_T_of_E));
    setScalesFromModel();
    connect(model,SIGNAL(signalPotentialChanged()),this,SLOT(slot_whole_T_of_E()));
    connect(model,SIGNAL(signalEnergyChanged(double)),this,SLOT(slotEline()));
//    resizePicture();
}
void TransmissionView::setScalesFromModel()
{
    QPair<double,double> umin_umax = model->getUminUmax();
    Emin = umin_umax.first;
    Emax = umin_umax.second;
    if(Emax<=0.) Emax=-Emin;
}
void TransmissionView::slotEline()
{
    double E=model->get_E0(); 
    if(!lineE)
    {
        lineE = new QGraphicsLineItem();
        lineE->setLine(tMin, E, tMax, E);
        scene()->addItem(lineE);
    }
    else
    {
        lineE->setLine(tMin, E, tMax, E);
    }
}
void TransmissionView::resizePicture()
{
    setViewportMapping();
    slot_T_of_E();
}
void TransmissionView::setViewportMapping()
{
    QRectF vp  = QRectF(QPointF(this->tMin,this->Emin),QPointF(this->tMax,this->Emax));
    QRectF sr  = scene()->sceneRect();
 //   if (vp != sr)
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
void TransmissionView::resizeEvent(QResizeEvent *e)
{
//    QSize s = e->size();
//    QSize o = e->oldSize();
    setViewportMapping();
    //QWidget::resizeEvent(e);
}

void TransmissionView::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
}
void TransmissionView::scaleView(qreal scaleFactor)
{
    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.007 || factor > 1000)
        return;

    scale(scaleFactor, scaleFactor);
}
void TransmissionView::clearAll()
{
    
        for ( QMap<int,TransmissionCurve*>::iterator i = curves.begin();  i != curves.end();   ++i)
    {
        int m = i.key();
            removeCurve(m);
    }
//curveMap.erase(curveMap.begin(),curveMap.end());
//    this->refreshPixmap();
}
void TransmissionView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Delete:
    case Qt::Key_Clear:
        clearAll();
        break;
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

void TransmissionView::slot_whole_T_of_E()
{
    static const QColor colorForIds[6] = {
        Qt::red, Qt::green, Qt::black, Qt::cyan, Qt::magenta, Qt::yellow
    };
    const int size_colorForIds = sizeof(colorForIds)/sizeof(colorForIds[0]);
//    bool Erase=false;
    if(Erase) curve_number=0;
    else curve_number=curve_number+1;
    int n=curve_number;
    if(Erase)
    {
    for ( QMap<int,TransmissionCurve*>::iterator i = curves.begin();  i != curves.end();   ++i)
    {
        int m = i.key();
            removeCurve(m);
    }
    }
    if(!linev)
    {
        lineh = new QGraphicsLineItem();
        linev = new QGraphicsLineItem();
        lineh->setLine(tMin, 0., tMax, 0);
        linev->setLine(0.,Emin,0.,Emax);
        scene()->addItem(lineh);
        scene()->addItem(linev);
    }
    else
    {
        lineh->setLine(tMin, 0., tMax, 0);
        linev->setLine(0.,Emin,0.,Emax);
    }
    int npoints=501;
    QVector<double> transmission;
    QPolygonF curveTE;
    curveTE.resize(npoints);
    transmission.resize(npoints);
    double dE = (Emax-Emin)/(npoints-1);
   transmission=model->getTransmissionOfE(Emin,Emax,npoints);
            for (int i=0; i < npoints; i++)
            {
                double x = Emin + dE*i;
                double y = transmission[i];
                curveTE[i]  = QPointF(y,x);
            }
            setCurve(n,curveTE,colorForIds[n % size_colorForIds]);
//    update();
}
void TransmissionView::slot_T_of_E()
{
    static const QColor colorForIds[6] = {
        Qt::red, Qt::green, Qt::black, Qt::cyan, Qt::magenta, Qt::yellow
    };
    const int size_colorForIds = sizeof(colorForIds)/sizeof(colorForIds[0]);
    int n=0; 
    bool Erase=false;
    if(Erase) n=0;
    else n=n+1;
    if(Erase)
    {
    for ( QMap<int,TransmissionCurve*>::iterator i = curves.begin();  i != curves.end();   ++i)
    {
        int m = i.key();
            removeCurve(m);
    }
    }
    if(!linev)
    {
        lineh = new QGraphicsLineItem();
        linev = new QGraphicsLineItem();
        lineh->setLine(tMin, 0., tMax, 0);
        linev->setLine(0.,Emin,0.,Emax);
        scene()->addItem(lineh);
        scene()->addItem(linev); 
    }
    else
    {
        lineh->setLine(tMin, 0., tMax, 0);
        linev->setLine(0.,Emin,0.,Emax);
    }
    int npoints=501;
    QVector<double> transmission;
    QPolygonF curveTE;
    curveTE.resize(npoints);
    transmission.resize(npoints);
    double dE = (Emax-Emin)/(npoints-1);
    n=0;
            for (int i=0; i < npoints; i++)
            {
                double x = Emin + dE*i;
                double T=model->getTatE(x);
                curveTE[i]  = QPointF(T,x);
                    setCurve(n,curveTE,colorForIds[n % size_colorForIds]);
            }
            update();
}




void TransmissionView::setCurve(int id,const QPolygonF & curve, const QPen& pen)
{

    if (curves[id])
    {
        curves[id]->setPolygon(curve);
    }
    else
    {
        TransmissionCurve *c = new TransmissionCurve(curve,this);
        scene()->addItem(c);
        curves[id] = c;
    }
    curves[id]->setPen(pen);
    update();
}

void TransmissionView::removeCurve(int id)
{
    scene()->removeItem(curves[id]);
    delete curves[id];
    curves[id] = 0; //this is dangerous: curves.remove(id);
    update();
//    repaint();
}

void TransmissionCurve::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    painter->setPen( pen() );
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPolyline(polygon().data(),polygon().size());

}


void TransmissionView::showDialogScaleY()
{   
    if (!gbScaleXY) 
    {
        gbScaleXY = new QGroupBox(this);
        gbScaleXY->setWindowTitle("Scales for T(E)");
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
void TransmissionView::contextMenuEvent(QContextMenuEvent *event)
{
        QMenu m;
        QAction *scaleT = m.addAction("Set scales");
        QAction *what = m.exec(event->globalPos());
        if (what == scaleT)
        {
            this->showDialogScaleY();
            update();
        }
}

/*void TransmissionCurve::mousePressEvent(QGraphicsSceneMouseEvent * event)
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
/*void TransmissionView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        rubberBandIsShown = false;
        updateRubberBandRegion();
        unsetCursor();
        QRect rect = rubberBandRect.normalized();
        if(rect.width()<4||rect.height()<4)
            return;
        rect.translate(-1, -1);
    }
}
void TransmissionView::updateRubberBandRegion()
{
    QRect rect=rubberBandRect.normalized();
    update(rect.left(), rect.top(), rect.width(), 1);
    update(rect.left(), rect.top(), 1, rect.height());
    update(rect.left(), rect.bottom(), rect.width(), 1);
    update(rect.right(), rect.top(), 1, rect.height());
}
void TransmissionView::mousePressEvent(QMouseEvent *event)
{
    if (event->button()== Qt::LeftButton)
    {
        rubberBandIsShown = true;
        rubberBandRect.setTopLeft(event->pos());
        rubberBandRect.setBottomRight(event->pos());
        updateRubberBandRegion();
        setCursor(Qt::CrossCursor);
    }
}
void TransmissionView::mouseMoveEvent(QMouseEvent *event)
{
    QPointF f = mapToScene(event->pos());
    emit(infoMouseMovedTo(f));
    QGraphicsView::mouseMoveEvent(event);
    if (rubberBandIsShown)
    {
        updateRubberBandRegion();
        rubberBandRect.setBottomRight(event->pos());
        updateRubberBandRegion();
    }

}
void TransmissionView::enterEvent ( QEvent * event )
{
    setFocus();
    QWidget::enterEvent(event);
}
void TransmissionView::paintEvent(QPaintEvent *event)
{
    QVector<QRect> rects = event->region().rects();
    QPainter painter(this);
   for(int i=0; i<(int)rects.size();++i)
        painter.drawImage(rects[i].topLeft(), pixmap.toImage(), rects[i]);
    if(rubberBandIsShown)
    {
        painter.setPen(palette().light().color());//was 2!!!!
        painter.drawRect(rubberBandRect.normalized().adjusted(0,0,-1,-1));
    }
    if (hasFocus()) 
    {
        QStyleOptionFocusRect option;
        option.initFrom(this);
        style()->drawPrimitive(QStyle::PE_FrameFocusRect, &option, &painter, this);
    }
}
*/
