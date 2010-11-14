#include "PotentialViewMovable.h"
//#include "PotentialScene.h"
//#include "DraggableLine.h"
#include <QGraphicsView>
#include <QMouseEvent>
#include <QMatrix>
#include <QGraphicsItem>
#include <QRectF>
#include <QLineF>

// HorDraggableLine places a horizontal line of length width()
// positioned by its left side at pos().
class HorDraggableLine : public QGraphicsItem
{
    PotentialViewMovable *view;
    int n;
    QPointF p2;
public:
    HorDraggableLine(PotentialViewMovable *v,QGraphicsItem *parent = 0);
    void set_n(int _n) { n = _n; }
    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
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
    QRectF boundingRect() const
    {
        return QRectF(QPointF(),p2);//.adjusted(0,-1,0,+1);
    }
    void paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *)
    {
        if(option->state & QStyle::State_Selected)
            painter->setPen(Qt::red);
        else
            painter->setPen(Qt::black);
        painter->drawLine(QLineF(QPointF(),p2));
    }
    QLineF line() const { return QLineF(pos(),pos() + p2); }
};

class VerDraggableLine : public QGraphicsItem
{
    PotentialViewMovable *view;
    int n;
    QPointF p2;
public:
    VerDraggableLine(PotentialViewMovable *v,QGraphicsItem *parent = 0);
    void set_n(int _n) { n = _n; }
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
    QRectF boundingRect() const
    {
        return QRectF(QPointF(),p2);//.adjusted(-1,0,+1,0);
    }
    void paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *)
    {
        if(option->state & QStyle::State_Selected)
            painter->setPen(Qt::red);
        else
            painter->setPen(Qt::black);
        painter->drawLine(QLineF(QPointF(),p2));
    }
    QLineF line() const { return QLineF(pos(),pos() + p2); }
};

PotentialViewMovable::PotentialViewMovable(PhysicalModel *m, QWidget *parent)
: QGraphicsView(parent), model(m)
{
    setScene(new QGraphicsScene(this));
    scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
    if (0)
    {
        setViewportUpdateMode(BoundingRectViewportUpdate);///
        setCacheMode(CacheBackground);
        setRenderHint(QPainter::Antialiasing);///
        setTransformationAnchor(AnchorUnderMouse);///
        setResizeAnchor(AnchorViewCenter);///
    }
    QRectF vp = model->getGoodViewportAtU();
    this->setSceneRect(vp);
    connect(model,SIGNAL(signalPotentialChanged()),this,SLOT(slotUChanged()));
    connect(model,SIGNAL(signalEboundChanged()),this,SLOT(slotEboundChanged()));
    slotUChanged();
    slotEboundChanged();
}
void PotentialViewMovable::setMyTransformMatrix()
{
    QRectF b = scene()->sceneRect();
    QRectF a = QRectF(this->viewport()->rect());
    qreal m11 = a.width() / b.width();
    qreal m22 = - a.height() / b.height();
    qreal dx = - m11 * a.x();
    qreal dy = - m22 * (a.y() + a.height());
    QMatrix m(m11,0,0,m22,dx,dy);
    this->setMatrix(m);
}

void PotentialViewMovable::resizeEvent(QResizeEvent*) 
{
    if (scene()) setMyTransformMatrix();
}

void PotentialViewMovable::mouseMoveEvent(QMouseEvent *e)
{
    QPointF f = mapToScene(e->pos());
    emit(infoMouseMovedTo(f));
    QGraphicsView::mouseMoveEvent(e);
}

void PotentialViewMovable::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
}
void PotentialViewMovable::scaleView(qreal scaleFactor)
{
    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}

void PotentialViewMovable::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
 /*   case Qt::Key_Up:
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
        break;
        */
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
    QRectF vp = sceneRect();

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
        if (0 < n && n < Ui.size()-1)
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
    slotEboundChanged();
    update();
}
void PotentialViewMovable::slotEboundChanged()
{
    QRectF vp = scene()->sceneRect();
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
        linesEn.push_back(line);
        scene()->addItem(line);
        addCurve(QPolygonF());
    }
    while (Ebound.size() < linesEn.size())
    {
        scene()->removeItem(linesEn.last());
        removeCurve(linesEn.size()-1);
        linesEn.pop_back();
    }
    double scalePsi=2;
    for (int n = 0; n < linesEn.size(); ++n)
    {
        double modelEn = Ebound[n];
        linesEn[n]->setLine(xmin,modelEn,xmax-xmin);
        QPolygonF psi = model->getPsiOfX(Ebound[n],scalePsi, xmin,xmax,1000);
//        psi.translate(0,n);
//        psi.translate(0,Ebound[n]);
        replaceCurve(n,psi);
    }
    update();
}


HorDraggableLine::HorDraggableLine(PotentialViewMovable *v,QGraphicsItem *parent)
: QGraphicsItem(parent), view(v)
{
    setCursor(Qt::SizeVerCursor);
    setFlag(QGraphicsItem::ItemIsMovable,true);		
    setFlag(QGraphicsItem::ItemIsSelectable,true);		
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
                left->setLine(p1.x(),p1.y(),newU-p1.y());
            }
            if (right)
            {
                QLineF r = right->line();
                QPointF p1 = r.p1();
                QPointF p2 = r.p2();
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
    setCursor(Qt::SizeHorCursor);
    setFlag(QGraphicsItem::ItemIsMovable,true);		
    setFlag(QGraphicsItem::ItemIsSelectable,true);		
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
            if (newx < left.x1())  newx = left.x1(); 
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

int PotentialViewMovable::addCurve(const QPolygonF & curve)
{
    MyGraphicsPolylineItem *c = new MyGraphicsPolylineItem(curve);
    scene()->addItem(c);
    curves.push_back(c);
    update();
    return curves.size() - 1;
}

void PotentialViewMovable::replaceCurve(int id, const QPolygonF & curve)
{
    if (id < curves.size())
    {
        if (curves[id])
        {
            curves[id]->setPolygon(curve);
        }
        else
        {
            MyGraphicsPolylineItem *c = new MyGraphicsPolylineItem(curve);
            scene()->addItem(c);
            curves[id] = c;
        }
    }
    else
    {
        curves.resize(id+1);
        MyGraphicsPolylineItem *c = new MyGraphicsPolylineItem(curve);
        scene()->addItem(c);
        curves[id] = c;
    }
    update();
}

void PotentialViewMovable::removeCurve(int id)
{
    scene()->removeItem(curves[id]);
    delete curves[id];
    curves[id] = 0;
    update();
}

void MyGraphicsPolylineItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    painter->setPen(Qt::green);
    painter->setRenderHint(QPainter::Antialiasing);
    //painter->drawPolygon(polygon().data(),polygon().size());
    painter->drawPolyline(polygon().data(),polygon().size());
}
