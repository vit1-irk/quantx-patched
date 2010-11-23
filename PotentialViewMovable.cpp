#include "PotentialViewMovable.h"
//#include "PotentialScene.h"
//#include "DraggableLine.h"
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

// HorDraggableLine places a horizontal line of length width()
// positioned by its left side at pos().
class EnergyDraggableLine : public QGraphicsItem
{
    PotentialViewMovable *view;
    QPointF p2;
    QPen penForPainter;
    QPen penHover;
    QPen pen;
public:
    double widthLineE;
    EnergyDraggableLine(PotentialViewMovable *v,QGraphicsItem *parent = 0);
    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
    void setPen(QPen _pen) { penForPainter = pen = _pen; }
    void setLine(double x1,double y1,double width)
    {
        QPointF p = pos();
        if (x1 != p.x() || y1 != p.y() || width != p2.x())
        {
            prepareGeometryChange();
            p2 = QPointF(width,0);
            this->setPos(x1,y1);
            update();
        }
    }
    void hoverEnterEvent ( QGraphicsSceneHoverEvent * event )
    {
        penForPainter = penHover;
//        penForPainter.setWidthF(widthLineE);
        update();
    }
    void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event )
    {
        penForPainter = pen;
//        penForPainter.setWidthF(widthLineE);
        this->setSelected(false);
        update();
    }
    double EnergyDraggableLine::getEnergyFromLine()
    {
        QPointF p = pos();
        return p.y(); 
    }
    QRectF boundingRect() const
    {
    QRectF vp = view->sceneRect();
    double widthLine= 0.005*vp.height();
        QPoint v1(0,0);
        QPoint v2(0,3);
        QPointF fv1 = view->mapToScene(v1);
        QPointF fv2 = view->mapToScene(v2);
        double widthLine1 = fabs(fv2.y() - fv1.y());
    
        QPoint va(0,0);
        QPoint vb(0,5);
        QPointF sa = view->mapToScene(va);
        QPointF sb = view->mapToScene(vb);
        double ax = fabs(sa.x() - sb.x());
        double ay = fabs(sa.y() - sb.y());
        QRect vpr = view->viewport()->rect();
        QPointF vpr1 = view->mapToScene(vpr.topLeft());
        QPointF vpr2 = view->mapToScene(vpr.bottomRight());
        QMatrix m = view->matrix();
        QRectF aa=QRectF(QPointF(),p2).adjusted(-ax,-ay,ax,ay);
        return aa;///QRectF(QPointF(),p2).adjusted(-ax,-ay,ax,ay);
    }
    void paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *)
    {
 //       QPen pen;
 //       pen.setStyle(Qt::DotLine);
        if(option->state & QStyle::State_Selected)
        {
//            pen.setColor(Qt::green);
            qreal x = pen.widthF();
            int y = pen.width();
 //           pen.setWidthF(x);
//            pen.setWidthF(0.1);
            int y1 = pen.width();
            qreal x1 = pen.widthF();
//            qreal x2 = pen.widthF();

        }
        else
        {
//            pen.setColor(Qt::red);
        }
        penForPainter.setWidthF(view->widthLineE);
        painter->setPen(penForPainter);
        qreal x2 = penForPainter.widthF();
        QPoint va(0,0);
        QPoint vb(0,5);
        QPointF sa = view->mapToScene(va);
        QPointF sb = view->mapToScene(vb);
        double ax = fabs(sa.x() - sb.x());
        double ay = fabs(sa.y() - sb.y());
 //           QRectF aa=QRectF(QPointF(),p2).adjusted(-ax,-ay,ax,ay);
 //           painter->drawRect(aa);

        painter->drawLine(QLineF(QPointF(),p2));
    }
    QLineF line() const { return QLineF(pos(),pos() + p2); }
};

class HorDraggableLine : public QGraphicsItem
{
    PotentialViewMovable *view;
    int n;
    QPointF p2;
    QPen pen,penForPainter;
    QPen penHover;
public:
    double widthLineH;
    HorDraggableLine(PotentialViewMovable *v,QGraphicsItem *parent = 0);
    void set_n(int _n) { n = _n; }
    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
    void setPen(QPen _pen) { penForPainter = pen = _pen; }
    void setLine(double x1,double y1,double width)
    {
        QPointF p = pos();
        if (x1 != p.x() || y1 != p.y() || width != p2.x())
        {
            prepareGeometryChange();
            p2 = QPointF(width,0);
            this->setPos(x1,y1);
             update();
        }
    }
    void hoverEnterEvent ( QGraphicsSceneHoverEvent * event )
    {
        penForPainter = penHover;
//        penForPainter.setWidthF(view->widthLineH);
        update();
    }
    void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event )
    {
        penForPainter = pen;
//        penForPainter.setWidthF(view->widthLineH);
        this->setSelected(false);
        update();
    }
    QRectF boundingRect() const
    {
        QPoint va(0,0);
        QPoint vb(0,5);
        QPointF sa = view->mapToScene(va);
        QPointF sb = view->mapToScene(vb);
        double ax = fabs(sa.x() - sb.x());
        double ay = fabs(sa.y() - sb.y());
        QRect vpr = view->viewport()->rect();
        QPointF vpr1 = view->mapToScene(vpr.topLeft());
        QPointF vpr2 = view->mapToScene(vpr.bottomRight());
        QMatrix m = view->matrix();
        QRectF aa=QRectF(QPointF(),p2).adjusted(-ax,-ay,ax,ay);
        return aa;//QRectF(QPointF(),p2).adjusted(-ax,-ay,ax,ay);
    }
    void paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *)
    {
        if(option->state & QStyle::State_Selected)
        {
//            pen.setStyle(Qt::DashLine);
        }
        else
        {
//            pen.setStyle(Qt::SolidLine);
        }
        penForPainter.setWidthF(view->widthLineH);
        penForPainter.setCapStyle(Qt::FlatCap);
        painter->setPen(penForPainter);
        painter->drawLine(QLineF(QPointF(),p2));
    }
    QLineF line() const { return QLineF(pos(),pos() + p2); }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent * event)
    {
        if (event->buttons() & Qt::RightButton)
        {
            QMenu m;
            QPointF p = event->pos();
            double fraction = p.x() / p2.x();
            QAction *splitHere = m.addAction("Split here");
            if (fraction < 0.05 || fraction > 0.95) splitHere->setDisabled(true);
            QAction *removeSegment = m.addAction("Remove segment");
            QAction *what = m.exec(event->screenPos());
            if (what == splitHere)
            {
                view->model->split_d(n, fraction);
            }
            if (what == removeSegment)
            {
                view->model->remove_d(n);
            }
        }
    }
};

class VerDraggableLine : public QGraphicsItem
{
    PotentialViewMovable *view;
    int n;
    QPointF p2;
    QPen pen,penForPainter;
    QPen penHover;
public:
    double widthLineV;
    VerDraggableLine(PotentialViewMovable *v,QGraphicsItem *parent = 0);
    void set_n(int _n) { n = _n; }
    void setPen(QPen _pen) { penForPainter = pen = _pen; }
    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
    void setLine(double x1,double y1,double upDown)
    {
        QPointF p = pos();
        if (x1 != p.x() || y1 != p.y() || upDown != p2.y())
        {
            prepareGeometryChange();
            p2 = QPointF(0,upDown);
            this->setPos(x1,y1);
            update();
        }
    }
    void hoverEnterEvent ( QGraphicsSceneHoverEvent * event )
    {
        penForPainter = penHover;
//        penForPainter.setWidthF(widthLineV);
        update();
    }
    void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event )
    {
        penForPainter = pen;
//        penForPainter.setWidthF(widthLineV);
        this->setSelected(false);
        update();
    }
    QRectF boundingRect() const
    {
        QPoint va(0,0);
        QPoint vb(5,0);
        QPointF sa = view->mapToScene(va);
        QPointF sb = view->mapToScene(vb);
        double ax = fabs(sa.x() - sb.x());
        double ay = fabs(sa.y() - sb.y());
    QRectF aa=QRectF(QPointF(),p2).adjusted(-ax,-ay,ax,ay);
    return aa;//QRectF(QPointF(),p2).adjusted(-0.1,-0.1,+0.1,+0.1);
//    return QRectF(QPointF(),p2).adjusted(-0.1,0,+0.1,0);
//    const qreal adj = 0.1; //todo
//    return QRectF( p1.x()-adj, p1.y()-adj, abs(p2.x() - p1.x()) + 2*adj, 2*adj );
    }
    void paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *)
    {
        if(option->state & QStyle::State_Selected)
        {
            //            painter->setPen(Qt::red);
        }
        else
        {
//            painter->setPen(Qt::black);
//        painter->setPen(pen);
        }
         penForPainter.setWidthF(view->widthLineV);
         painter->setPen(penForPainter);
         painter->drawLine(QLineF(QPointF(),p2));
    }
    QLineF line() const { return QLineF(pos(),pos() + p2); }
};

PotentialViewMovable::PotentialViewMovable(PhysicalModel *m, QWidget *parent)
: QGraphicsView(parent), model(m), lineEnergy(0)
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
    connect(model,SIGNAL(signalPotentialChanged()),this,SLOT(slotEnergyChanged()));
    connect(model,SIGNAL(signalPotentialChanged()),this,SLOT(slotUChanged()));
    connect(model,SIGNAL(signalEnergyChanged()),this,SLOT(slotEnergyChanged()));
    connect(model,SIGNAL(signalEboundChanged()),this,SLOT(slotEboundChanged()));
    resizePicture();
//    setViewportMapping();
//    slotUChanged();
//    slotEnergyChanged();
//    slotEboundChanged();
}
void PotentialViewMovable::resizePicture()
{
    setViewportMapping();
    slotUChanged();
    slotEboundChanged();
    slotEnergyChanged();
}
void PotentialViewMovable::setViewportMapping()
{
    QRectF vp = model->getGoodViewportAtU();
    QRectF sr = scene()->sceneRect();
    if (vp != sr)
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
//        scene()->invalidate(scene()->sceneRect()); 
        sr = scene()->sceneRect();
    }
//    QRectF vp = view->sceneRect();
    widthLineV= 0.0025*vp.width();
    widthLineH= 0.0025*vp.height();
    widthLineE= 0.005*vp.height();
    update();
} 
void PotentialViewMovable::resizeEvent(QResizeEvent*) 
{
    setViewportMapping();
}

void PotentialViewMovable::mouseMoveEvent(QMouseEvent *e)
{
    QPointF f = mapToScene(e->pos());
    emit(infoMouseMovedTo(f));
    QGraphicsView::mouseMoveEvent(e);
}
/*void PotentialViewMovable::mouseReleaseEvent( QMouseEvent * event)
{
    QRectF vp = model->getGoodViewportAtU();//????????
    this->setSceneRect(vp);//????????????
    if (scene()) setMyTransformMatrix();
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}
void PotentialViewMovable::mousePressEvent(QMouseEvent *event)
{
    update();
    QGraphicsItem::mousePressEvent(event);
}
*/
void PotentialViewMovable::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
}
void PotentialViewMovable::scaleView(qreal scaleFactor)
{
    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.007 || factor > 1000)
        return;

    scale(scaleFactor, scaleFactor);
}

void PotentialViewMovable::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
/*    case Qt::Key_Up:
        centerNode->moveBy(0, -20);
        break;
    case Qt::Key_Down:
        centerNode->moveBy(0, 20);
        break;
    case Qt::Key_Left:
        centerNode->moveBy(-20, 0);
        break;
    case Qt::Key_Right:
        centerNode->moveBy(20, 0);
        break; */
    case Qt::Key_Plus:
        scaleView(qreal(1.2));
        break;
    case Qt::Key_Minus:
        scaleView(1 / qreal(1.2));
        break;
    case Qt::Key_Space:
    case Qt::Key_Enter:
/*
        foreach (QGraphicsItem *item, scene()->items()) {
            if (qgraphicsitem_cast<Node *>(item))
                item->setPos(-150 + qrand() % 300, -150 + qrand() % 300);
        }*/
        break;
    default:
         QGraphicsView::keyPressEvent(event);
    }
}

void PotentialViewMovable::slotUChanged()
{
    QVector<double> d = model->get_d();
    QVector<double> Ui = model->get_Ui();
  
    while (Ui.size() < linesU.size())
    {
        scene()->removeItem(linesU.last()); 
        linesU.pop_back();
    }
    while (Ui.size()-1 < linesV.size())
    {
        scene()->removeItem(linesV.last());
        linesV.pop_back();
    }
    while(Ui.size() > linesU.size())
    {
        HorDraggableLine *line = new HorDraggableLine(this);
        scene()->addItem(line);
        linesU << line;
    }
    while(Ui.size()-1 > linesV.size())
    {
        VerDraggableLine *line = new VerDraggableLine(this);
        scene()->addItem(line);
        linesV << line;
    }
    ///////////////////////////////////////////////////
    // now we have number of graphical lines and segments of physical potential algned
    ///////////////////////////////////////////////////
//    QRectF vp = model->getGoodViewportAtU();
//    scene()->setSceneRect(vp);
    QRectF vp = sceneRect();
//    QRectF vp = scene()->sceneRect();
    double xmin = vp.x();
    double xmax = vp.x() + vp.width();
//    update();
//---------------
    HorDraggableLine *l = linesU[1];
    QLineF ll = l->line();
    QPointF p1 = ll.p1();
    QPointF p2 = ll.p2();
    //if(p1.y()==Ui[1]) return; 
//----------------------
    double xlast = vp.x();
    for (int n = 0; n < Ui.size(); ++n)
    {
        linesU[n]->setFlag(QGraphicsItem::ItemIsMovable,true);
        linesU[n]->setFlag(QGraphicsItem::ItemIsSelectable,true);
        linesU[n]->set_n(n);
        qreal y = Ui[n];
        (void)y;
        double w;
        if (0 < n && n < Ui.size()-1)//!!! =
            w = d[n];
        else if (n==0)
            w = 0 - vp.x();
        else
            w = vp.x() + vp.width() - xlast;
        linesU[n]->setLine(xlast,Ui[n],w);
        xlast += w;
    }
    linesU.last()->setFlag(QGraphicsItem::ItemIsMovable,false);
    linesU.last()->setFlag(QGraphicsItem::ItemIsSelectable,false);

    for (int n = 0; n < linesV.size(); ++n)
    {
        linesV[n]->setCursor(n == 0 ? QCursor() : Qt::SizeHorCursor);
        linesV[n]->setFlag(QGraphicsItem::ItemIsMovable,n == 0 ? false : true );
        linesV[n]->setFlag(QGraphicsItem::ItemIsSelectable,n == 0 ? false : true );
        linesV[n]->set_n(n);
        QLineF left  = linesU[n]->line();
        QLineF right = linesU[n+1]->line();
        qreal y = right.y1();
        (void)y;
        double ud = right.y1()-left.y2();
        linesV[n]->setLine(left.x2(),left.y2(),ud);
    }
    update();
}
void PotentialViewMovable::slotEboundChanged()
{
    static const QColor colorForIds[6] = {
        Qt::red, Qt::green, Qt::black, Qt::cyan, Qt::magenta, Qt::yellow
    };
    const int size_colorForIds = sizeof(colorForIds)/sizeof(colorForIds[0]);

    QRectF vp = scene()->sceneRect();
//    QRectF vp = model->getGoodViewportAtU();
//    scene()->setSceneRect(vp);

    double xmin = vp.x();
    double xmax = vp.x() + vp.width();
    QVector<double> Ebound = model->getEn();
    while (Ebound.size() > linesEn.size())
    {
        int n = linesEn.size();
        HorDraggableLine *line = new HorDraggableLine(this);
        double e = Ebound[n];
        line->setLine(xmin,e,xmax-xmin);
        line->setFlag(QGraphicsItem::ItemIsMovable,false);
        line->setFlag(QGraphicsItem::ItemIsSelectable,false);
        line->setCursor(QCursor());
        scene()->addItem(line);
        linesEn.push_back(line);
        setCurve(n,QPolygonF());
    }
    while (Ebound.size() < linesEn.size())
    {
        scene()->removeItem(linesEn.last());
        linesEn.pop_back();
        removeCurve(linesEn.size());
    }
    double scalePsi=3;
    for (int n = 0; n < linesEn.size(); ++n)
    {
        double modelEn = Ebound[n];
        linesEn[n]->setLine(xmin,modelEn,xmax-xmin);
        linesEn[n]->setPen(colorForIds[n % size_colorForIds]);
        QPolygonF psi = model->getPsiOfX(Ebound[n],scalePsi, xmin,xmax,500);
        setCurve(n,psi,colorForIds[n % size_colorForIds]);
    }
    update();
}

#define ID_PSI_ENERGY (100)

void PotentialViewMovable::slotEnergyChanged()
{
    QRectF vp = this->sceneRect();
    QRectF vp1 = this->scene()->sceneRect();
//    QRectF vp = model->getGoodViewportAtU();
//    scene()->setSceneRect(vp);
    double scalePsi=3;
    double xmin = vp.x();
    double xmax = vp.x() + vp.width();
    double E=model->get_E0(); 
    if (lineEnergy) 
    {
        double Eold=lineEnergy->getEnergyFromLine();
        if(Eold!=E) model->set_E0(Eold);
        E=Eold;
/*        if(Eold!=E)
        { 
            scene()->removeItem(lineEnergy);
            removeCurve(ID_PSI_ENERGY);
            return;
        }
*/
        }
        /*    double xT, scaleT=2;
    if(E>0) xT = scaleT*model->TT;
    else xT=0;
    if(!lineTofE) 
    {
        lineTofE = new CurveTofE(this);
        setCurve(ID_TofE,QPolygonF());
    }
*/
    if (!lineEnergy) 
    {
        lineEnergy = new EnergyDraggableLine(this);
//        model->set_E0(E);
        lineEnergy->setLine(xmin,E,xmax-xmin);
        scene()->addItem(lineEnergy);
    }
    else     lineEnergy->setLine(xmin,E,xmax-xmin);
    QPolygonF psi = model->getPsiOfX(E, scalePsi, xmin,xmax,500);
    setCurve(ID_PSI_ENERGY, psi, QPen(Qt::darkCyan));
    update();
}


EnergyDraggableLine::EnergyDraggableLine(PotentialViewMovable *v,QGraphicsItem *parent)
: QGraphicsItem(parent), view(v)
{
    setCursor(Qt::SizeHorCursor);
    penHover.setStyle(Qt::DotLine);
    penHover.setWidthF(view->widthLineE);
    penHover.setColor(Qt::green);

    pen.setStyle(Qt::DashLine);
    pen.setColor(Qt::green);
    pen.setWidthF(view->widthLineE);

    setCursor(Qt::SizeVerCursor);
    setFlag(QGraphicsItem::ItemIsMovable,true);		
    setFlag(QGraphicsItem::ItemIsSelectable,true);		
    setAcceptHoverEvents ( true );
}

QVariant EnergyDraggableLine::itemChange(GraphicsItemChange change, const QVariant & value)
{
    switch (change)
    {
    case ItemPositionChange:
        if (isSelected())
        {
            QPointF newpos = value.toPointF();
            QPointF oldpos = pos();
            newpos.setX( oldpos.x() );
            return newpos;
        }
    case ItemPositionHasChanged:
        if (isSelected())
        {
            double newE = pos().y();
            double newx = pos().x();
            view->model->set_Energy(newE);
            return QVariant();
        }
    }
    return QGraphicsItem::itemChange(change,value);
}
HorDraggableLine::HorDraggableLine(PotentialViewMovable *v,QGraphicsItem *parent)
: QGraphicsItem(parent), view(v)
{
    penHover.setStyle(Qt::DashLine);
    penHover.setWidthF(view->widthLineH);
    penHover.setColor(Qt::blue);

    setCursor(Qt::SizeVerCursor);
    setFlag(QGraphicsItem::ItemIsMovable,true);		
    setFlag(QGraphicsItem::ItemIsSelectable,true);	
    setAcceptHoverEvents ( true );
}

QVariant HorDraggableLine::itemChange(GraphicsItemChange change, const QVariant & value)
{
    switch (change)
    {
    case ItemPositionChange:
        if (isSelected())
        {
            QPointF newpos = value.toPointF();
            QPointF oldpos = pos();
            newpos.setX( oldpos.x() );
            return newpos;
        }
    case ItemPositionHasChanged:
        if (isSelected())
        {
            double newU = pos().y();
            VerDraggableLine *left  = n-1 >= 0 ? view->linesV[n-1] : NULL;
            VerDraggableLine *right = n < view->linesV.size() ? view->linesV[n] : NULL;
            if (left)
            {
                QLineF l = left->line();
                QPointF p1 = l.p1();
                QPointF p2 = l.p2();
//                left->setLine(p1.x(),p1.y(),newU);
                left->setLine(p1.x(),p1.y(),newU-p1.y());
            }
            if (right)
            {
                QLineF r = right->line();
                QPointF p1 = r.p1();
                QPointF p2 = r.p2();
//                right->setLine(p1.x(),newU,p2.y());
                right->setLine(p1.x(),newU,p2.y()-newU);
            }
            view->model->set_Ui(n,newU);
            return QVariant();
        }
    }
    return QGraphicsItem::itemChange(change,value);
}
VerDraggableLine::VerDraggableLine(PotentialViewMovable *v,QGraphicsItem *parent)
: QGraphicsItem(parent), view(v)
{
//    QRectF vp = view->sceneRect();
    setCursor(Qt::SizeHorCursor);
    penHover.setStyle(Qt::DashLine);
    penHover.setWidthF(view->widthLineV);
    penHover.setColor(Qt::blue);

    pen.setWidthF(view->widthLineV);
    pen.setColor(Qt::black);

    setFlag(QGraphicsItem::ItemIsMovable,true);		
    setFlag(QGraphicsItem::ItemIsSelectable,true);		
    setAcceptHoverEvents ( true );
}
QVariant VerDraggableLine::itemChange(GraphicsItemChange change, const QVariant & value)
{
    switch (change)
    {
    case ItemPositionChange:
        if (isSelected())
        {
            QPointF newpos = value.toPointF();
            QPointF oldpos = pos();
            double newx = newpos.x();
            QLineF left  = view->linesU[ n ]->line();
            QLineF right = view->linesU[n+1]->line();
            if (newx > right.x2()) newx = right.x2(); 
            if (newx < left.x1())   newx = left.x1(); 
            newpos.setX(newx);
            newpos.setY(oldpos.y());
            return newpos;
        }
    case ItemPositionHasChanged:
       if (isSelected())
        {
            QLineF self  = line();
            QLineF left  = view->linesU[ n ]->line();
            QLineF right = view->linesU[n+1]->line();
    
            double x = self.x1();
            double left_w = x - left.x1();
            double right_w = right.x2() - x;

            view->linesU[ n ]->setLine(left.x1(),left.y1(),left_w);
            view->linesU[n+1]->setLine(x,right.y1(),right_w);
            
            view->model->set_d(n,left_w,n+1,right_w);

            return QVariant();
        }
    }
    return QGraphicsItem::itemChange(change,value);
}

void PotentialViewMovable::setCurve(int id,const QPolygonF & curve, const QPen& pen)
{

    if (curves.contains(id))
    {
        curves[id]->setPolygon(curve);
    }
    else
    {
        MyGraphicsPolylineItem *c = new MyGraphicsPolylineItem(curve);
        scene()->addItem(c);
        curves[id] = c;
    }
    curves[id]->setPen(pen);
    update();
}

void PotentialViewMovable::removeCurve(int id)
{
    scene()->removeItem(curves[id]);
    delete curves[id];
    curves.remove(id);
    update();
}

void MyGraphicsPolylineItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{


    painter->setPen( pen() );
//    QRect s0=painter->viewport();
//    QRect s1=painter->window();
    painter->setRenderHint(QPainter::Antialiasing);
    //painter->drawPolygon(polygon().data(),polygon().size());
    painter->drawPolyline(polygon().data(),polygon().size());
}
