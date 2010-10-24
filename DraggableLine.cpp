#include "MainWindow.h"
#include <QStyleOption>
#include <QGraphicsScene>
#include <QMessageBox>
#include "PhysicalModel.h"
#include "PotentialScene.h"
#include "PotentialViewMovable.h"
#include "DraggableLine.h"
#include <QLineF>
#include <QPen>
#include <QGraphicsItem>

    //***************** Base Class *******************************
///DraggableLine::DraggableLine(PotentialViewMovable *pVM)///
///: PotentialViewMovable(pVM), left(0), right(0)///
DraggableLine::DraggableLine(QGraphicsItem *parent)
: QGraphicsItem(parent), left(0), right(0)
{
    setFlag(QGraphicsItem::ItemIsMovable,true);		
    setFlag(QGraphicsItem::ItemIsSelectable,true);		
    setCacheMode(DeviceCoordinateCache);///?
    setZValue(1);///?
}
void DraggableLine::paint(QPainter * painter, const QStyleOptionGraphicsItem *option,QWidget *)
{
    if(option->state & QStyle::State_Selected)
        painter->setPen(Qt::red);
    else 
        painter->setPen(Qt::black);

    painter->drawLine(QLineF( p1, p2 ));
}
//*************** Concrete Classes ****************************
HDraggableLine::HDraggableLine(QPointF at, qreal other_end_x, QGraphicsItem *parent)
: DraggableLine(parent)
{ 
    if (at.x() > other_end_x)
        qSwap(at.rx(),other_end_x);
    p1 = at;
    p2 = QPointF(other_end_x, at.y());
    setCursor(Qt::SizeVerCursor);
}

QRectF HDraggableLine::boundingRect() const
{
    const qreal adj = 0.1; //todo
    return QRectF( p1.x()-adj, p1.y()-adj, abs(p2.x() - p1.x()) + 2*adj, 2*adj );
}
QVariant HDraggableLine::itemChange(GraphicsItemChange change, const QVariant & value)
{
    switch (change)
    {
    case ItemPositionChange:
        {
            QPointF newpos = value.toPointF();
            newpos.setX( 0 );//( pos().x() );
            qreal newy = p2.y() + newpos.y();
            qreal newp1y=p1.y();
            qreal newp2y=p2.y();
            //return newpos;
            return QGraphicsItem::itemChange(change, newpos);
            break;
        }
        //#if 1
    case ItemPositionHasChanged:
        {
            QPointF pos1 = pos();
            QPointF pos2 = value.toPointF();
            QPointF pos3 = this->scenePos();
            QPointF pos4 = mapToScene(pos1);
            QPointF pos5 = this->mapFromScene(pos1);


            //prepareGeometryChange();
            p1.setY(p1.y() + pos().y());
            p2.setY(p2.y() + pos().y());
            qreal newp1y=p1.y();
            qreal newp2y=p2.y();
            qreal y20=left->p2.y();
            qreal y10=right->p1.y();
            if (left)  left->SetP2(  QPointF( left->p1.x(), p1.y() ) );
            if (right) right->SetP1( QPointF( right->p2.x(), p2.y() ) );
            qreal y2=left->p2.y();
            qreal y1=right->p1.y();
                        update();
 //           update();
//            static_cast<PotentialScene*>(scene())->updatePhysicalModel();
        }
        //#endif
    default:
        break;
    };
        return value;
//    return QGraphicsItem::itemChange(change, value);
}

VDraggableLine::VDraggableLine(QPointF at, qreal other_end_y, QGraphicsItem *parent)
: DraggableLine(parent)
{ 
    p1 = at;
    p2 = QPointF( at.x(), other_end_y );
    setCursor(Qt::SizeHorCursor);
}

#define min(a,b) ((a)<(b) ? (a) : (b))
QRectF VDraggableLine::boundingRect() const
{
    const qreal adj = 0.1;
    return QRectF( p1.x()-adj, min(p1.y(),p2.y())-adj, 2*adj, abs(p1.y() - p2.y()) +2*adj );
//       .normalized()
//        .adjusted(-extra, -extra, extra, extra);

}
QVariant VDraggableLine::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange)
    {
        QPointF newpos = value.toPointF();
        newpos.setY( pos().y() );
        qreal newx = p1.x() + newpos.x();
        qreal newp1x=p1.x();
        qreal newp2x=p2.x();

        if (newx > right->p2.x()) newx = right->p2.x(); 
        if (newx < left->p1.x()) newx = left->p1.x(); 
        newpos.setX( newx - p1.x() );

        return newpos;
    }
    if (change == ItemPositionHasChanged)
    {
        prepareGeometryChange();
        p1.setX(p1.x() + pos().x());
        p2.setX(p2.x() + pos().x());
        qreal newp1x=p1.x();
        qreal newp2x=p2.x();
        if (left)  left->SetP2(  QPointF( p1.x(), left->p1.y() ) );
        if (right) right->SetP1( QPointF( p2.x(), right->p1.y() ) );
            update();
        static_cast<PotentialScene*>(scene())->updatePhysicalModel();
    }
//    return value;
    return QGraphicsItem::itemChange(change, value);

}

#if 0
QRectF  HDraggableLine::boundingRect() const 
{
    return QRectF(-adj,-adj,length+2*adj,2*adj);
}
QPointF HDraggableLine::RestrictMotion(QPointF point) 
{ 
    point.setX(pos().x()); 
    if(left) if(!left->CheckEnd(point)) return pos();
    if(right) if(!right->CheckStart(point+QPointF(length,0))) return pos();
    if(left) left->SetEnd(point);
    if(right) right->SetStart(point+QPointF(length,0));
    parent->calculateEnergies();
    return point;
}
void HDraggableLine::paint(QPainter * painter, const QStyleOptionGraphicsItem * option,QWidget * )
{
    if(option->state & QStyle::State_Selected)painter->setPen(Qt::red); else painter->setPen(Qt::black);
    painter->drawLine(QPointF(0,0),QPointF(length,0));
    //    painter->drawLine(0,0,length,0);
}

bool HDraggableLine::CheckStart(QPointF point)
{
    if(length + pos().x() - point.x() < 0) return false;
    return true;
}
bool HDraggableLine::CheckEnd  (QPointF point) { if(point.x() - pos().x() < 0) return false; return true; }

void HDraggableLine::SetStart(QPointF point) { SetLength(length + pos().x() - point.x()); setPos(point); }
void HDraggableLine::SetEnd(QPointF point) { SetLength(point.x()-pos().x()); }
#endif
#if 0
// **************** Vertical Line ************************
VDraggableLine::VDraggableLine( qreal len,PotentialScene * p)
:DraggableLine(model, len,p)
{
    //setCursor(Qt::SizeHorCursor); 
}
QRectF  VDraggableLine::boundingRect() const { return QRectF(-adj,(length>0?-adj:length-adj),2*adj,qAbs(length)+2*adj); }	
QPointF VDraggableLine::RestrictMotion(QPointF point) 
{ 
    point.setY(pos().y()); 
    if(right) if(!right->CheckStart(point+QPointF(0,length))) return pos();
    if(left) if(!left->CheckEnd(point)) return pos();
    if(right) right->SetStart(point+QPointF(0,length));
    if(left)  left ->SetEnd(point);
    parent->calculateEnergies();
    return point;
}
void VDraggableLine::paint(QPainter * painter, const QStyleOptionGraphicsItem * option,QWidget * )
{
    //    QPen p;
    if(option->state & QStyle::State_Selected)
    {
        painter->setPen(Qt::red);
    }
    else 
    {
        painter->setPen(Qt::black);
    }
    //    p.setWidth(2);
    //    painter->setPen(p);
    painter->drawLine(QPointF(0,0),QPointF(0,length));
    //    painter->drawLine(0,0,0,length);
}

void VDraggableLine::SetStart(QPointF point) { SetLength( length + pos().y() - point.y()); setPos(point); }
void VDraggableLine::SetEnd(QPointF point) { SetLength( point.y()-pos().y()); }

//***************** Infinite Line ********************************************
InfDraggableLine::InfDraggableLine(PhysicalModel *_model, PotentialScene * p) : model(_model), HDraggableLine(_model,100, p) // было 100
{
    setFlag(QGraphicsItem::ItemIsMovable,false);		
    setFlag(QGraphicsItem::ItemIsSelectable,false);		
}

bool InfDraggableLine::CheckStart(QPointF ) {return true;}
bool InfDraggableLine::CheckEnd  (QPointF ) {return true;}

void InfDraggableLine::SetStart(QPointF point) { setPos(point); }
void InfDraggableLine::SetEnd(QPointF point)   {setPos(point+QPointF(-length,0));}
#endif

void DraggableLine::mouseReleaseEvent( QGraphicsSceneMouseEvent * event)
{
    setSelected(false);
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}
void DraggableLine::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mousePressEvent(event);
}

