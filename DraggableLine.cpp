#include <QStyleOption>
#include <QGraphicsScene>
#include <QMessageBox>
#include "PhysicalModel.h"
#include "PotentialScene.h"
#include "DraggableLine.h"
#include <QLineF>
#include <QPen>
#include <QGraphicsLineItem>

const int adj = 5;

//***************** Base Class *******************************
DraggableLine::DraggableLine(QGraphicsScene *scene)
: QGraphicsLineItem(0,scene)
{
    setFlag(QGraphicsItem::ItemIsMovable,true);		
    setFlag(QGraphicsItem::ItemIsSelectable,true);		
}
void DraggableLine::paint(QPainter * painter, const QStyleOptionGraphicsItem *option,QWidget *)
{
    if(option->state & QStyle::State_Selected)
        painter->setPen(Qt::red);
    else 
        painter->setPen(Qt::black);

    painter->drawLine(line());
}

#if 0
QVariant DraggableLine::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && isSelected() ) 
    {
        QRectF sceneRect = scene()->sceneRect();
        QPointF posf = value.toPointF();
        if(!sceneRect.contains(posf)) return pos();
        return RestrictMotion(posf);
    }
    return QGraphicsItem::itemChange(change, value);
}
#endif

//*************** Concrete Classes ****************************
HDraggableLine::HDraggableLine(QPointF at, qreal other_end_x, QGraphicsScene *scene)
: DraggableLine(scene)
{ 
    if (at.x() > other_end_x)
        swap(at.rx(),other_end_x);
    this->setLine(at.x(), at.y(), other_end_x, at.y());
    setCursor(Qt::SizeVerCursor);
}
QPointF HDraggableLine::rightEnd() const
{
    return line().p2();
}

QVariant HDraggableLine::itemChange(GraphicsItemChange change, const QVariant & value)
{
    if (change == ItemPositionChange)
    {
        QPointF newpos = value.toPointF();
        newpos.setX( pos().x() );

        if (left)
        {
            QLineF l = left->line();
            QPointF p1 = l.p1();
            QPointF p2 = mapFromScene(l.p2());
            p2.setY( p2.y() + newpos.y() );
            p2 = mapToScene( p2 );
            left->setLine( QLineF( p1, p2 ) );
        }
        if (right)
        {
            QLineF l = right->line();
            right->setLine( QLineF( l.x1(), l.y1() + newpos.y(), l.x2(), l.y2()) );
        }

        return newpos;
    }
    return value;
}

VDraggableLine::VDraggableLine(QPointF at, qreal other_end_y, QGraphicsScene *scene)
: DraggableLine(scene)
{ 
    this->setLine(at.x(), at.y(), at.x(), other_end_y);
    setCursor(Qt::SizeHorCursor);
}
QPointF VDraggableLine::lastEnd() const
{
    return line().p2();
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