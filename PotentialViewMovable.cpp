#include "PotentialViewMovable.h"
#include "MainWindow.h"
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
            update();//repaint();
        }
    }
    void hoverEnterEvent ( QGraphicsSceneHoverEvent * event )
    {
        penForPainter = penHover;
        update();//repaint();
    }
    void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event )
    {
        penForPainter = pen;
        this->setSelected(false);
        update();//repaint();
    }
    double EnergyDraggableLine::getEnergyFromLine()
    {
        QPointF p = pos();
        return p.y();
    }
    QRectF boundingRect() const
    {
    QRectF vp = view->sceneRect();
/*    double lineWidth= 0.005*vp.height();
        QPoint v1(0,0);
        QPoint v2(0,5);
        QPointF fv1 = view->mapToScene(v1);
        QPointF fv2 = view->mapToScene(v2);
        double widthLine1 = fabs(fv2.y() - fv1.y());
 */
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
/*        QPoint va(0,0);
        QPoint vb(0,view->lineWidth);
        QPointF sa = view->mapToScene(va);
        QPointF sb = view->mapToScene(vb);
        double ay = fabs(sa.y() - sb.y());
        double ax = fabs(sa.x() - sb.x());
        view->widthLineE=ay;*/
        penForPainter.setWidthF(view->widthLineE);
        penForPainter.setCapStyle(Qt::FlatCap);

//        penForPainter.setWidthF(ay);
//        penForPainter.setWidthF(view->widthLineE);
        painter->setPen(penForPainter);
        qreal x2 = penForPainter.widthF();
/*        QPoint va(0,0);
        QPoint vb(0,5);
        QPointF sa = view->mapToScene(va);
        QPointF sb = view->mapToScene(vb);
        double ax = fabs(sa.x() - sb.x());
        double ay = fabs(sa.y() - sb.y());
        */
        painter->drawLine(QLineF(QPointF(),p2));
    }
    QLineF line() const { return QLineF(pos(),pos() + p2); }

    void mousePressEvent(QGraphicsSceneMouseEvent * event)
    {
        if (event->buttons() & Qt::RightButton)
        {
            QMenu m;
//            QAction *scaleX = m.addAction("Scale x");
//            QAction *scaleE = m.addAction(m.tr("Scale E"));
            QAction *what = m.exec(event->screenPos());
//            if (what == scaleX)
//            {
//                view->askScaleX();
//            }
/*            if (what == scaleE)
            {
                view->scaleE();
            }*/
               update();// repaint();
        }
    }

};
class EnergyLevels : public QGraphicsItem
{
    PotentialViewMovable *view;
    int n;
    QPointF p2;
    QPen pen;
public:
    double widthLineE;
    EnergyLevels(PotentialViewMovable *v,QGraphicsItem *parent = 0);
    void set_n(int _n) { n = _n; }
    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
    void setPen(QPen _pen) { pen = _pen; }
    void setLine(double x1,double y1,double width)
    {
        QPointF p = pos();
        if (x1 != p.x() || y1 != p.y() || width != p2.x())
        {
            prepareGeometryChange();
            p2 = QPointF(width,0);
            this->setPos(x1,y1);
             update();//repaint();
        }
    }
    QRectF boundingRect() const
    {
        return QRectF(QPointF(),p2);
    }
    void paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *)
    {
        pen.setWidthF(view->widthLineE);
        pen.setCapStyle(Qt::FlatCap);
        painter->setPen(pen);
        painter->drawLine(QLineF(QPointF(),p2));
    }
    QLineF line() const { return QLineF(pos(),pos() + p2); }

};
EnergyLevels::EnergyLevels(PotentialViewMovable *v,QGraphicsItem *parent)
: QGraphicsItem(parent), view(v)
{
    setCursor(Qt::SizeVerCursor);
    setFlag(QGraphicsItem::ItemIsMovable,false);
    setFlag(QGraphicsItem::ItemIsSelectable,false);
}

QVariant EnergyLevels::itemChange(GraphicsItemChange change, const QVariant & value)
{
    return QGraphicsItem::itemChange(change,value);
}

//------------------
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
//    void setAcceptHoverEvents ( bool enabled );
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
             update();//repaint();
        }
    }
    void hoverEnterEvent ( QGraphicsSceneHoverEvent * event )
    {
     QGraphicsItem::GraphicsItemFlags f;
     f=this->flags();
      if(this->flags()==3)//QGraphicsItem::ItemIsSelectable)
      {
      penForPainter = penHover;
            update();//repaint();
      }
    }
    void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event )
    {
        penForPainter = pen;
        this->setSelected(false);
        update();//repaint();
    }
    QRectF boundingRect() const
    {
        QPoint va(0,0);
        QPoint vb(0,5);
        QPointF sa = view->mapToScene(va);
        QPointF sb = view->mapToScene(vb);
        double ax  = fabs(sa.x() - sb.x());
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
/*        QPoint va(0,0);
        QPoint vb(0,2);
        QPointF sa = view->mapToScene(va);
        QPointF sb = view->mapToScene(vb);
        double ax = fabs(sa.x() - sb.x());
        double ay = fabs(sa.y() - sb.y());
        view->widthLineH=ay;*/
//        view->widthLineV=ay;
//        view->widthLineE=ay;
//        penForPainter.setWidthF(ay);
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
            QAction *splitHere = m.addAction(m.tr("Split here"));
            if (fraction < 0.05 || fraction > 0.95) splitHere->setDisabled(true);
            QAction *removeSegment = m.addAction(m.tr("Remove segment"));
            QAction *what = m.exec(event->screenPos());
            if (what == splitHere)
            {
                view->model->split_d(n, fraction);
            }
            if (what == removeSegment)
            {
                view->model->remove_d(n);
                update();//repaint();
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
            update();//repaint();
        }
    }
    void hoverEnterEvent ( QGraphicsSceneHoverEvent * event )
    {
        QGraphicsItem::GraphicsItemFlags f;
        f=this->flags();
        if(this->flags()==3)//QGraphicsItem::ItemIsSelectable)
        {
            penForPainter = penHover;
            penForPainter.setWidthF(view->widthLineV);
            update();//repaint();
        }
    }
    void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event )
    {
        penForPainter = pen;
        penForPainter.setWidthF(view->widthLineV);
        this->setSelected(false);
        update();//repaint();
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
//        if(option->state & QStyle::State_Selected)
//        {
            //            painter->setPen(Qt::red);
//        }
/*        QPoint va(0,0);
        QPoint vb(2,0);
        QPointF sa = view->mapToScene(va);
        QPointF sb = view->mapToScene(vb);
        double ax = fabs(sa.x() - sb.x());*/
//        view->widthLineV=ax;
//        penForPainter.setWidthF(ax);
        penForPainter.setWidthF(view->widthLineV);
        penForPainter.setCapStyle(Qt::FlatCap);
        painter->setPen(penForPainter);
        painter->drawLine(QLineF(QPointF(),p2));

    }
    QLineF line() const { return QLineF(pos(),pos() + p2); }
};

PotentialViewMovable::PotentialViewMovable(PhysicalModel *m, QWidget *parent)
: QGraphicsView(parent), model(m),  lineEnergy(0),
Umin(-15.), Umax(10.), xmin(-1.), xmax(10.),
dialogScalesU(0),gbRangeE(0),
widthLineE(0.02),widthLineH(0.02),widthLineV(0.02),
//psiMax(4), psiMin(-1), whatToDraw(2),
Emin(-10.),Emax(20.),hE(0.01)
{
    lineWidth = 2;
    this->initDialogRangeE();
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
    setMinimumSize(200, 100);
    connect(model,SIGNAL(signalPotentialChanged()),this,SLOT(slotEnergyChanged()));
    connect(model,SIGNAL(signalPotentialChanged()),this,SLOT(slotUChanged()));
    connect(model,SIGNAL(signalEnergyChanged(double)),this,SLOT(slotEnergyChanged()));
    connect(model,SIGNAL(signalEboundChanged()),this,SLOT(slotEboundChanged()));
//    connect(this,SIGNAL(signalRangeE()),this,SLOT(slotEboundChanged()));
    connect(model,SIGNAL(signalScalesUChanged()),this,SLOT(resizePicture()));
    connect(model,SIGNAL(signalWidthChanged()),this,SLOT(resizePicture()));
    resizePicture();
//    model->set_Energy(0.1);
//    model->getTatE();
}
#define ID_PSI_ENERGY (200)

/*PotentialViewMovable::~PotentialViewMovable()
{
    disconnect(this, 0, 0, 0);
    if (dialogScalesU) delete dialogScalesU;
//    if (gbScaleE) delete gbScaleE;
    if (lineEnergy) delete lineEnergy;
//    removeCurve(ID_PSI_ENERGY);
}
*/
void PotentialViewMovable::setScalesFromModel()
{
    QPair<double,double> umin_umax = model->getUminUmax();
    Umin = 1.2*umin_umax.first;
    Umax = umin_umax.second;
    if(Umax<=0.) Umax=-Umin;
    QPair<double,double> xmin_xmax = model->getXminXmax();
    xmin = xmin_xmax.first;
    xmax = xmin_xmax.second;
    ScalesUParameters tp;
    tp.Xmin = xmin;
    tp.Xmax = xmax;
    tp.Umin = Umin;
    tp.Umax = Umax;
    model->setScalesUParam(tp);
}
void PotentialViewMovable::resizePicture()
{
    ScalesUParameters tp = model->getScalesUParam();
    if(xmin!=tp.Xmin||xmax!=tp.Xmax||
        Umin!=tp.Umin||Umax!=tp.Umax)
    {
        xmin=tp.Xmin;
        xmax=tp.Xmax;
        Umin=tp.Umin;
        Umax=tp.Umax;
/*        if(dialogScalesU)dialogScalesU->modelChanged();
        else
        {
            dialogScalesU = new ScalesUx(this);
            dialogScalesU->setModel(model);
        }*/
    }
    setViewportMapping();
    slotUChanged();
    slotEboundChanged();
    slotEnergyChanged();
}
void PotentialViewMovable::setViewportMapping()
{
    QRectF a = QRectF(this->viewport()->rect());
    QRectF vp = QRectF(QPointF(this->xmin,this->Umin),QPointF(this->xmax,this->Umax));

    QRectF sr = scene()->sceneRect();
        scene()->setSceneRect(vp);

        QRectF b = vp; //scene()->sceneRect();
        qreal m11 = a.width() / b.width();
        qreal m22 = - a.height() / b.height();
        qreal dx = - m11 * a.x();
        qreal dy = - m22 * (a.y() + a.height());
        QMatrix m(m11,0,0,m22,dx,dy);
        this->setMatrix(m);
        scene()->update(scene()->sceneRect());
        sr = scene()->sceneRect();
    SettingParameters ts;
    ts=model->getSettingParameters();
    lineWidth=ts.lineWidth;
//    if(lineWidth==0)lineWidth=1;

    double ax=fabs(xmax-xmin)*lineWidth/a.width();
    double ay=fabs(Umax-Umin)*lineWidth/a.height();
    widthLineH= fabs(ay);
    widthLineV= fabs(ax);//0.05;//0.0025*vp.width();
    widthLineE= fabs(ay);
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
    switch (event->key())
    {
    case Qt::Key_Plus:
        scaleView(qreal(1.2));
        break;
    case Qt::Key_Minus:
        scaleView(1 / qreal(1.2));
        break;
    case Qt::Key_Left:
        scrollView(-1, 0);
        break;
    case Qt::Key_Right:
        scrollView(+1, 0);
        break;
    case Qt::Key_Down:
        scrollView(0, -1);
        break;
    case Qt::Key_Up:
        scrollView(0, +1);
        break;
    case Qt::Key_Space:
    case Qt::Key_Enter:
        break;
    default:
         QGraphicsView::keyPressEvent(event);
    }
}
void PotentialViewMovable::scrollView(int hx, int hy)
{
    ScalesUParameters tp = model->getScalesUParam();
    xmin=tp.Xmin;
    xmax=tp.Xmax;
    Umin=tp.Umin;
    Umax=tp.Umax;
    setViewportMapping();
    double stepX=(xmax-xmin)/5;//numTTicks;
    xmin +=hx*stepX;
    xmax +=hx*stepX;
    double stepY=(Umax-Umin)/5;//numETicks;
    Umin +=hy*stepY;
    Umax +=hy*stepY;
    ScalesUParameters tt;
    tt.Xmin = xmin;
    tt.Xmax = xmax;
    tt.Umin = Umin;
    tt.Umax = Umax;
    model->setScalesUParam(tt);
/*    if(dialogScalesU)dialogScalesU->modelChanged();
    else
    {
        dialogScalesU = new ScalesUx(this);
        dialogScalesU->setModel(model);
    }*/
}
void PotentialViewMovable::slotUChanged()
{
    PotentialType type = model->getPotentialType();
    if(type==PERIODIC) slotUperChanged();
    else
    {

        QVector<double> d = model->get_d();
        QVector<double> Ui = model->get_Ui();
        int Nd=d.size();
        int Nu=Ui.size();
        int Nl=linesU.size();
        int Nln=linesUmin.size();
        int Nlx=linesUmax.size();
    while (0 < linesUmin.size())
    {
        scene()->removeItem(linesUmin.last());
        linesUmin.pop_back();
    }
    int j=linesVmin.size();
    while (0 < linesVmin.size())
    {
        scene()->removeItem(linesVmin.last());
        linesVmin.pop_back();
        j--;
    }
    while (0 < linesUmax.size())
    {
        scene()->removeItem(linesUmax.last());
        linesUmax.pop_back();
    }
    while (0 < linesVmax.size())
    {
        scene()->removeItem(linesVmax.last());
        linesVmax.pop_back();
    }
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
        double xmin = vp.x();
        double xmax = vp.x() + vp.width();
        HorDraggableLine *l = linesU[1];
        QLineF ll = l->line();
        QPointF p1 = ll.p1();
        QPointF p2 = ll.p2();
        //----------------------
        double xlast = vp.x();
        int nlast=Ui.size()-1;
        for (int n = 0; n < Ui.size(); ++n)
        {
            linesU[n]->setFlag(QGraphicsItem::ItemIsMovable,true);
            linesU[n]->setFlag(QGraphicsItem::ItemIsSelectable,true);
            linesU[n]->setZValue(1000);
//            linesU[n]->setAcceptHoverEvent(true);
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

            //       if(xlast+w < xmax)
            {
                linesU[n]->setLine(xlast,Ui[n],w);
                linesU[n]->setZValue(1000);
                xlast += w;
            }
            /*       else
            {
            w = xmax-xlast;
            linesU[n]->setLine(xlast,Ui[n],w);
            nlast=n;
            break;//xlast = xmax;
            }*/
        }
        linesU.last()->setFlag(QGraphicsItem::ItemIsMovable,false);
        linesU.last()->setFlag(QGraphicsItem::ItemIsSelectable,false);
        linesU.last()->setZValue(1000);
//        linesU.last()->setAcceptHoverEvent(false);

        for (int n = 0; n < linesV.size(); ++n)
        {
            //        if(n<nlast)
            {
                linesV[n]->setCursor(n == 0 ? QCursor() : Qt::SizeHorCursor);
                linesV[n]->setFlag(QGraphicsItem::ItemIsMovable,n == 0 ? false : true  );
                linesV[n]->setFlag(QGraphicsItem::ItemIsSelectable,n == 0 ? false : true  );
//                if(n==0)linesV[n]->setAcceptHoverEvent(false);
//                else linesV[n]->setAcceptHoverEvent(true);
                linesV[n]->setZValue(1000);
                linesV[n]->set_n(n);
                QLineF left  = linesU[n]->line();
                QLineF right = linesU[n+1]->line();
                qreal y = right.y1();
                (void)y;
                double ud = right.y1()-left.y2();
                linesV[n]->setLine(left.x2(),left.y2(),ud);
            }
            //        else break;
        }
        update();
    }
}
void PotentialViewMovable::slotUperChanged()
{
    ///////////////////////////////////////////////////
    // now we have number of graphical lines and segments of physical potential algned
    ///////////////////////////////////////////////////
    QRectF vp = sceneRect();
    double xmin = vp.x();
    double xmax = vp.x() + vp.width();
    QVector<double> d = model->get_d();
    QVector<double> Ui = model->get_Ui();
    int Nd=d.size();
    int Nu=Ui.size();
    int Nl=linesU.size();
    int Nlmin=linesUmin.size();
    double  xN=0;
    for (int n = 1; n < Nd-1; n++)
    {
        xN+= d[n];
    }
    int np;
    if(xmin<=0) np = 1+abs(xmin)/xN;
    else  np = abs(xmin)/xN;
//    if(xmin<=0) np = 1+abs(xmin-0.5*xN)/xN;
//    else  np = abs(xmin+0.5*xN)/xN;
//    int npx = abs((xmax+0.5*xN)/xN);
    int npx = abs(xmax/xN)+1;
    int Nlpmin=(np-1)*(Nu-2);
    int Nlpmax=(npx-1)*(Nu-2) ;
    while (Ui.size()-2 < linesU.size())
    {
        scene()->removeItem(linesU.last());
        linesU.pop_back();
    }
    while (Ui.size()-2 < linesV.size())
    {
        scene()->removeItem(linesV.last());
        linesV.pop_back();
    }
    while ((Ui.size()-2)*np < linesUmin.size())
    {
        scene()->removeItem(linesUmin.last());
        linesUmin.pop_back();
    }
    int j=linesVmin.size();
    while ((Ui.size()-2)*np < linesVmin.size())
    {
        scene()->removeItem(linesVmin.last());
        linesVmin.pop_back();
        j--;
    }
    while ((Ui.size()-2)*(npx-1) < linesUmax.size())
    {
        scene()->removeItem(linesUmax.last());
        linesUmax.pop_back();
    }
    while ((Ui.size()-2)*(npx-1) < linesVmax.size())
    {
        scene()->removeItem(linesVmax.last());
        linesVmax.pop_back();
    }
    while(Ui.size()-2 > linesU.size())
    {
        HorDraggableLine *line = new HorDraggableLine(this);
        scene()->addItem(line);
        linesU << line;
    }
    while((Ui.size()-2)*np > linesUmin.size())
    {
        HorDraggableLine *line = new HorDraggableLine(this);
        scene()->addItem(line);
        linesUmin << line;
    }
    while((Ui.size()-2)*(npx-1) > linesUmax.size())
    {
        HorDraggableLine *line = new HorDraggableLine(this);
        scene()->addItem(line);
        linesUmax << line;
    }
    while((Ui.size()-2)*np > linesVmin.size())
    {
        VerDraggableLine *lined = new VerDraggableLine(this);
        scene()->addItem(lined);
        linesVmin << lined;
    }
    while((Ui.size()-2)*(npx-1) > linesVmax.size())
    {
        VerDraggableLine *lined = new VerDraggableLine(this);
        scene()->addItem(lined);
        linesVmax << lined;
    }
    while(Ui.size()-2 > linesV.size())
    {
        VerDraggableLine *line = new VerDraggableLine(this);
        scene()->addItem(line);
        linesV << line;
    }
    double xlast=0;
    int nlast=Ui.size()-1;
    int it=linesU.size();
    int iv=linesV.size();
    int iun=linesUmin.size();
    int iux=linesUmax.size();
    int ivn=linesVmin.size();
    int ivx=linesVmax.size();
//    for (int n = 1; n < linesU.size(); ++n)
        QPen p;
        p.setColor(Qt::darkRed);
    for (int n = 1; n < Ui.size()-1; ++n)
    {
        linesU[n-1]->setFlag(QGraphicsItem::ItemIsMovable,true);
        linesU[n-1]->setFlag(QGraphicsItem::ItemIsSelectable,true);
        linesU[n-1]->setZValue(1000);
        linesU[n-1]->set_n(n-1);
        qreal y = Ui[n];
        (void)y;
        double w=d[n];
        linesU[n-1]->setLine(xlast,Ui[n],w);
        linesU[n-1]->setZValue(1000);
        linesU[n-1]->setPen(p);
        xlast += w;
    }
    for (int n = 0; n < linesU.size(); ++n)
    {
        linesV[n]->setCursor(Qt::SizeHorCursor);
        linesV[n]->setFlag(QGraphicsItem::ItemIsMovable,n == linesU.size()-1? false : true );
        linesV[n]->setFlag(QGraphicsItem::ItemIsSelectable,n == linesU.size()-1 ? false : true  );
        linesV[n]->setZValue(1000);
        linesV[n]->set_n(n);
        if(n==linesU.size()-1)
        {
            QLineF left  = linesU[n]->line();
            QLineF right = linesU[0]->line();
            qreal y = right.y1();
            (void)y;
            double ud = right.y1()-left.y2();
            linesV[n]->setLine(left.x2(),left.y2(),ud);
        }
        else
        {
            QLineF left  = linesU[n]->line();
            QLineF right = linesU[n+1]->line();
            qreal y = right.y1();
            (void)y;
            double ud = right.y1()-left.y2();
            linesV[n]->setLine(left.x2(),left.y2(),ud);
        }
    linesV[n]->setPen(p);
    }
    xlast=-np*xN;
    if(xmin<0)
    {
        for (int j = 0; j < np; ++j)
        {
            int jj;
            for (int n = 1; n < Ui.size()-1; ++n)
            {
                qreal y = Ui[n];
                double w=d[n];
                jj=(Ui.size()-2)*j-1+n;
                linesUmin[jj]->setFlag(QGraphicsItem::ItemIsMovable,false);
                linesUmin[jj]->setFlag(QGraphicsItem::ItemIsSelectable,false);
                linesUmin[jj]->setZValue(0);
                linesUmin[jj]->setLine(xlast,Ui[n],w);
                linesUmin[jj]->setZValue(0);
//                linesUmin[jj]->setAcceptHoverEvent(false);
                xlast += w;
            }
        }
        for (int n = 0; n < linesUmin.size(); ++n)
        {
            linesVmin[n]->setCursor(n == 0 ? QCursor() : Qt::SizeHorCursor);
            linesVmin[n]->setFlag(QGraphicsItem::ItemIsMovable,false );
            linesVmin[n]->setFlag(QGraphicsItem::ItemIsSelectable,false);
            linesVmin[n]->setZValue(0);
            linesVmin[n]->set_n(n);
//            linesVmin[n]->setAcceptHoverEvent(false);
        if(n==linesUmin.size()-1)
        {
            QLineF left  = linesUmin[n]->line();
            QLineF right = linesU[0]->line();
            qreal y = right.y1();
            (void)y;
            double ud = right.y1()-left.y2();
            linesVmin[n]->setLine(left.x2(),left.y2(),ud);
        }
        else
        {
            QLineF left  = linesUmin[n]->line();
            QLineF right = linesUmin[n+1]->line();
            qreal y = right.y1();
            (void)y;
            double ud = right.y1()-left.y2();
            double xx=left.x2();
            linesVmin[n]->setLine(left.x2(),left.y2(),ud);
        }
        }
    }

    if(xmax>xN)
    {
        xlast=xN;
        int iu=linesUmax.size();
        for (int j = 0; j < npx-1; ++j)
        {
            int jj;
            for (int n = 1; n < Ui.size()-1; ++n)
            {
                qreal y = Ui[n];
                double w=d[n];
                double x=xlast+w;
                jj=(Ui.size()-2)*j-1+n;
                linesUmax[jj]->setFlag(QGraphicsItem::ItemIsMovable,false);
                linesUmax[jj]->setFlag(QGraphicsItem::ItemIsSelectable,false);
                linesUmax[jj]->setZValue(0);
//                linesUmax[jj]->setAcceptHoverEvent(false);
                linesUmax[jj]->setLine(xlast,Ui[n],w);
                linesUmax[jj]->setZValue(0);
                xlast += w;
            }
        }
        for (int n = 0; n < linesUmax.size(); ++n)
        {
            linesVmax[n]->setCursor(n == 0 ? QCursor() : Qt::SizeHorCursor);
            linesVmax[n]->setFlag(QGraphicsItem::ItemIsMovable,false );
            linesVmax[n]->setFlag(QGraphicsItem::ItemIsSelectable,false);
            linesVmax[n]->setZValue(0);
//            linesVmax[n]->setAcceptHoverEvent(false);
            linesVmax[n]->set_n(n);
        if(n==linesUmax.size()-1)
        {
            QLineF left  = linesUmax[n]->line();
            QLineF right = linesU[0]->line();
            qreal y = right.y1();
            (void)y;
            double ud = right.y1()-left.y2();
            linesVmax[n]->setLine(left.x2(),left.y2(),ud);
        }
        else
        {
            QLineF left  = linesUmax[n]->line();
            QLineF right = linesUmax[n+1]->line();
            qreal y = right.y1();
            (void)y;
            double ud = right.y1()-left.y2();
            linesVmax[n]->setLine(left.x2(),left.y2(),ud);
        }
        }
    }
    update();
}
void PotentialViewMovable::slotEboundChanged()
{
    static const QColor colorForIds[12] = {
        Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta,
//        Qt::yellow,
        Qt::black,
        Qt::darkRed, Qt::darkGreen, Qt::darkBlue, Qt::darkCyan, Qt::darkMagenta, Qt::darkYellow
    };
    const int size_colorForIds = sizeof(colorForIds)/sizeof(colorForIds[0]);
    PotentialType type = model->getPotentialType();
    QVector<double> Ebound;
    QVector<complex> Equasi;
    if(type==FINITE||type==PERIODIC)
    {
        Ebound = model->getEn();
        while (Ebound.size() > linesEn.size())
        {
            int n = linesEn.size();
            EnergyLevels *line = new EnergyLevels(this);
            double e = Ebound[n];
            line->setLine(xmin,e,xmax-xmin);
            line->setFlag(QGraphicsItem::ItemIsMovable,false);
            line->setFlag(QGraphicsItem::ItemIsSelectable,false);
            line->setCursor(QCursor());
            scene()->addItem(line);
            linesEn.push_back(line);
        }
        while (Ebound.size() < linesEn.size())
        {
            scene()->removeItem(linesEn.last());
            linesEn.pop_back();
        }
        for (int n = 0; n < linesEn.size(); ++n)
        {
            linesEn[n]->setLine(xmin,Ebound[n],xmax-xmin);
            linesEn[n]->setPen(colorForIds[n % size_colorForIds]);
        }
    }
    if(type==QUASISTATIONARY)
    {
        Equasi = model->getEquasi();
        while (Equasi.size() > linesEn.size())
        {
            int n = linesEn.size();
            EnergyLevels *line = new EnergyLevels(this);
            double e = real(Equasi[n]);
            line->setLine(xmin,e,xmax-xmin);
            line->setFlag(QGraphicsItem::ItemIsMovable,false);
            line->setFlag(QGraphicsItem::ItemIsSelectable,false);
            line->setCursor(QCursor());
            scene()->addItem(line);
            linesEn.push_back(line);
        }
        while (Equasi.size() < linesEn.size())
        {
            scene()->removeItem(linesEn.last());
            linesEn.pop_back();
        }
        for (int n = 0; n < linesEn.size(); ++n)
        {
            linesEn[n]->setLine(xmin,real(Equasi[n]),xmax-xmin);
            linesEn[n]->setPen(colorForIds[n % size_colorForIds]);
        }
    }
    update();
}

void PotentialViewMovable::changeE()
{
//    double E=Emin;
    double E=model->get_E0();
    if(E+hE>Emax&&hE>0) E=Emin;
    if(E-hE<Emin&&hE<0) E=Emax;
    while(E>=Emin&&E<=Emax)
    {
        model->set_Energy(E);
        E=E+hE;
        if (getBreakStatus(0))
        {
            return;
        }

    }
}
void PotentialViewMovable::slotEnergyChanged()
{
//    double scalePsi = (Umax-Umin)/(psiMax-psiMin);
    double E=model->get_E0();
    //    model->set_Energy(E);
    if (!lineEnergy)
    {
        lineEnergy = new EnergyDraggableLine(this);
        scene()->addItem(lineEnergy);
    }
    QPen p;
    p.setJoinStyle(Qt::BevelJoin);
    p.setCapStyle(Qt::RoundCap);
    p.setWidthF(this->widthLineE);
    p.setStyle(Qt::DashLine);
    p.setColor(Qt::green);
    lineEnergy->setPen(p);
    lineEnergy->setLine(xmin,E,xmax-xmin);
    PotentialType type = model->getPotentialType();
    if(type==PERIODIC) model->getQaatE();
    if(type==FINITE) model->getTatE();
    if(type==QUASISTATIONARY) model->getTnatE();
}
EnergyDraggableLine::EnergyDraggableLine(PotentialViewMovable *v,QGraphicsItem *parent)
: QGraphicsItem(parent), view(v)
{
    setCursor(Qt::SizeHorCursor);
    penHover.setStyle(Qt::DotLine);
    penHover.setWidthF(v->widthLineE);
    penHover.setColor(Qt::green);

    pen.setStyle(Qt::DashLine);
    pen.setColor(Qt::green);
    pen.setWidthF(v->widthLineE);

    setCursor(Qt::SizeVerCursor);
    setFlag(QGraphicsItem::ItemIsMovable,true);
    setFlag(QGraphicsItem::ItemIsSelectable,true);
    setZValue(999);
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
    penHover.setStyle(Qt::DotLine);
//    penHover.setStyle(Qt::DashLine);
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
    PotentialType type = view->model->getPotentialType();
    if(type==PERIODIC) view->model->set_Ui(n+1,newU);
    else  view->model->set_Ui(n,newU);
//            view->model->set_Ui(n,newU);
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
    penHover.setStyle(Qt::DotLine);
//    penHover.setStyle(Qt::DashLine);
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

    PotentialType type = view->model->getPotentialType();
    if(type==PERIODIC) view->model->set_d(n+1,left_w,n+2,right_w);
    else
            view->model->set_d(n,left_w,n+1,right_w);

            return QVariant();
        }
    }
    return QGraphicsItem::itemChange(change,value);
}

/*void PotentialViewMovable::setCurve(int id,const QPolygonF & curve, const QPen& pen)
{

    if (curves.contains(id))
    {
        curves[id]->setPolygon(curve);
    }
    else
    {
        MyGraphicsPolylineItem *c = new MyGraphicsPolylineItem(curve,this);
        scene()->addItem(c);
        curves[id] = c;
    }
    curves[id]->setPen(pen);
    repaint();
//    update();
}

void PotentialViewMovable::removeCurve(int id)
{
    scene()->removeItem(curves[id]);
    delete curves[id];
    curves.remove(id);
    repaint();
}

void MyGraphicsPolylineItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    painter->setPen( pen() );
    QPen p;
    p.setWidth(3);
//        p.setWidthF(this->widthLineE);
        p.setJoinStyle(Qt::BevelJoin);
        p.setCapStyle(Qt::RoundCap);
//      p.setCapStyle(Qt::FlatCap);
        p.setColor(Qt::darkRed);
    painter->setRenderHint(QPainter::Antialiasing);
//    painter->setPen(QPen(Qt::red, 0.1, Qt::DotLine, Qt::RoundCap, Qt::BevelJoin));
    painter->drawPolyline(polygon().data(),polygon().size());
}
*/
void PotentialViewMovable::showDialogScaleY()
{
    if (!dialogScalesU)
    {
        dialogScalesU = new ScalesUx(this);
        dialogScalesU->setModel(model);
    }
    dialogScalesU->show();
    dialogScalesU->activateWindow();
    dialogScalesU->setFocus();
}
//---------
void PotentialViewMovable::initDialogRangeE()
{
    gbRangeE = new QGroupBox(this);
    gbRangeE->setWindowTitle("Energy range");
    gbRangeE->setWindowFlags(Qt::Window);
    gbRangeE->setFont(QFont("Serif", 12, QFont::Bold ));

    QVBoxLayout *vl = new QVBoxLayout;
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel("hE",this));
        h->addWidget(this->leHE = new QLineEdit(this));
        this->leHE->setToolTip("E-increment");
        QString x;
        x.sprintf("%lg",this->hE);
        this->leHE->setText(x);
        connect(this->leHE,SIGNAL(editingFinished()),this,SLOT(updateRangeE()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel("Emin",this));
        h->addWidget(this->leEmin= new QLineEdit(this));
        QString x;
        x.sprintf("%lg",this->Emin);
        this->leEmin->setText(x);
        this->leEmin->setToolTip("Lower value of E-interval");
        connect(this->leEmin,SIGNAL(editingFinished()),this,SLOT(updateRangeE()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel("Emax",this));
        h->addWidget(this->leEmax= new QLineEdit(this));
        QString x;
        x.sprintf("%lg",this->Emax);
        this->leEmax->setText(x);
        this->leEmax->setToolTip("High value of E-interval");
        connect(this->leEmax,SIGNAL(editingFinished()),this,SLOT(updateRangeE()));
        vl->addWidget(line);
    }
    gbRangeE->setLayout(vl);
}

void PotentialViewMovable::showDialogRangeE()
{
    gbRangeE->show();
    gbRangeE->raise();
    gbRangeE->setFocus();
}
void PotentialViewMovable::setRangeE()
{
    QString x;
    x.sprintf("%lg",this->hE);
    this->leHE->setText(x);
    x.sprintf("%lg",this->Emin);
    this->leEmin->setText(x);
    x.sprintf("%lg",this->Emax);
    this->leEmax->setText(x);
}
void PotentialViewMovable::updateRangeE()
{
    double hENew = this->leHE->text().toDouble();
    double EminNew = this->leEmin->text().toDouble();
    double EmaxNew = this->leEmax->text().toDouble();
    bool changed = false;
    if (hE != hENew)
    {
        hE=hENew;
        changed = true;
    }
    if (Emin != EminNew)
    {
        changed = true;
        Emin=EminNew;
    }
    if (Emax != EmaxNew)
    {
        changed = true;
        Emax=EmaxNew;
    }
    if(changed)
    {
        EParameters sE;
        sE.hE= this->hE;
        sE.Emin = this->Emin;
        sE.Emax = this->Emax;
        model->setEParameters(sE);
    }
}

//-------
void PotentialViewMovable::setWhatToDraw(int w)
{
    if (whatToDraw != w)
    {
        whatToDraw = w;
        this->slotEnergyChanged();
        emit( whatToDrawChanged(w) );
    }
}

void PotentialViewMovable::contextMenuEvent(QContextMenuEvent *event)
{
    {
        QMenu m;
        QFont font( "Serif", 10, QFont::DemiBold );
        m.setFont(font);
        QAction *scalePsi = m.addAction(tr("Scales"));
        QAction *rangeE = m.addAction(tr("Energy range"));
        QAction *U1Action = m.addAction(tr("Set U(x) as initial"));
        QAction *U2Action = m.addAction(tr("Set U(x) as final"));
        QPoint poscur = event->globalPos();
        QAction *what = m.exec(event->globalPos());
        if (what == scalePsi)
        {
            this->showDialogScaleY();
            update();
        }
        if (what == rangeE)
        {
            this->showDialogRangeE();
            update();
        }
        if (what == U1Action)
        {
            model->slotU1();
            update();
        }
        if (what == U2Action)
        {
            model->slotU2();
            update();
        }
        event->accept();
    }
}
class MyLabel : public QLabel
{
public:
    MyLabel(const QString s, QWidget *parent) : QLabel(s,parent) {}
    void resizeEvent( QResizeEvent *e )
    {
        QLabel::resizeEvent(e);
        setMinimumWidth(width());
    }
};

/*void PotentialMovableWidget::runHelp(const char *s)
{
    QStringList args;
    args.push_back("/A");
    QString dest;
    dest.sprintf("nameddest=%s",s);
    args.push_back(dest);
    args.push_back("kvant.pdf");
    int i = QProcess::startDetached("C:\\Program Files\\Adobe\\Reader 10.0\\Reader\\AcroRd32.exe",args);
}
void PotentialMovableWidget::runHelp_ctx()
{
    runHelp("ctx");
}*/

PotentialMovableWidget::PotentialMovableWidget(PhysicalModel *model, QWidget *parent)
: QGroupBox(parent)
{
    setTitle(tr("Graphic definition of potential"));

    QVBoxLayout *vl = new QVBoxLayout();
    potentialViewMovable = new PotentialViewMovable(model,this);
    potentialViewMovable->setWhatsThis(tr("¬ этом окне вы можете редактировать потенциал и видеть уровни энергии. "));
    vl->addWidget(potentialViewMovable);
/*    QPushButton *help = new QPushButton("Help");
    vl->addWidget(help);
    connect(help,SIGNAL(clicked()),this,SLOT(runHelp_ctx()));
*/

    QHBoxLayout *hl = new QHBoxLayout();
    /*
//-------------------
    QString psi=QChar(0x03C8);
    //               QString Psi=QChar(0x03A8);
    QString to2=QChar(0x00B2);
    QString psiofx=psi+"(x)";
    QString mod_psiofx="|"+psiofx+"|"+to2;//+QChar(0x2082);
    //            QGroupBox *gb3 = new QGroupBox(tr("Wave function ")+psiofx);
//-------------------
    QRadioButton *rad1= new QRadioButton("Re "+psi);
    QRadioButton *rad2= new QRadioButton("Im "+psi);
    QRadioButton *rad3= new QRadioButton(mod_psiofx);
    hl->addWidget(rad1);
    hl->addWidget(rad2);
    hl->addWidget(rad3);
    bgR = new QButtonGroup(this);
    bgR->setExclusive(true);
    bgR->addButton(rad1,0);
    bgR->addButton(rad2,1);
    bgR->addButton(rad3,2);
    bgR->button(2)->setChecked(true);
    connect(bgR,SIGNAL(buttonClicked(int)),potentialViewMovable,SLOT(setWhatToDraw(int)));
*/
    QLabel *lNtext= new QLabel(this);
    lNtext->setText(tr("n="));
    lN = new QLineEdit(this);
    QString x;
    x.sprintf("%i",model->get_LevelNumber());
    lN->setText(x);
    lN->setMaximumWidth(50);
//    lN->setEnabled(true);
    lN->setAlignment(Qt::AlignLeft);
    connect(lN,SIGNAL(editingFinished()),this,SLOT(updateLevelNumber()));
    connect(model, SIGNAL(signalLevelNumberChanged(int)), this,SLOT(printLevelNumber()));

    QLabel *lEtext= new QLabel(this);
    lEtext->setText(tr("E="));
    lE = new QLineEdit(this);
    x.sprintf("%lg",model->get_E0());
    lE->setText(x);
    lE->setMaximumWidth(150);
//    lE->setEnabled(true);
    lE->setAlignment(Qt::AlignLeft);
    //    QLabel *lE = new MyLabel("",this);
    connect(lE,SIGNAL(editingFinished()),this,SLOT(updateEnergy()));
//    connect(model, SIGNAL(signalEnergyChanged(double)), lE, SLOT(setNum(double)));
    connect(model, SIGNAL(signalEnergyChanged(double)), this,SLOT(printEnergy()));


    QLabel *lTtext= new QLabel(this);
    lTtext->setText(tr("T/N(E)="));
    QLabel *lT= new MyLabel("",this);
    lT->setTextFormat(Qt::AutoText);
    connect(model, SIGNAL(signalTransmissionChanged(double)), lT, SLOT(setNum(double)));
    hl->addWidget(lNtext);
    hl->addWidget(lN);
    hl->addWidget(lEtext);
    hl->addWidget(lE);
    hl->addWidget(lTtext);
    hl->addWidget(lT);

//    hl->addStretch();

    bRun = new QToolButton(this);
    bRun->setIcon(QIcon(":/images/player_play.png"));
    bRun->adjustSize();
    hl->addWidget(bRun);
    connect(bRun,SIGNAL(clicked()),this,SLOT(slotRunE()));

    hl->addStretch();
    QToolButton *buttonClose = new QToolButton(this);
    buttonClose->setIcon(QIcon(":/images/erase.png"));
    buttonClose->adjustSize();//QPushButton(tr("Close"));
    hl->addWidget(buttonClose);

    connect(buttonClose,SIGNAL(clicked()),this,SLOT(hide()),Qt::QueuedConnection); //???
    vl->addLayout(hl);
    setLayout(vl);
}
void PotentialMovableWidget::slotRunE()
{
    bRun->setIcon(QIcon(":/images/player_pause.png"));
    bRun->adjustSize();
    disconnect(bRun, SIGNAL(clicked()), this, SLOT(slotRunE()));
    breakStatus.onButton(bRun);
    potentialViewMovable->changeE();
    breakStatus.noButton(bRun);
    bRun->setIcon(QIcon(":/images/player_play.png"));
    bRun->adjustSize();
    connect(bRun, SIGNAL(clicked()), this, SLOT(slotRunE()));
}

void PotentialMovableWidget::updateLevelNumber()
{
    int levelNumber = this->lN->text().toDouble();
    int N=this->potentialViewMovable->model->get_LevelNumber();
    if ( levelNumber!= N)
    {
        this->potentialViewMovable->model->set_LevelNumber(levelNumber);
    }
}
void PotentialMovableWidget::updateEnergy()
{
    double energy = this->lE->text().toDouble();
    double EE=this->potentialViewMovable->model->get_E0();
    if (energy != EE)
    {
        this->potentialViewMovable->model->set_Energy(energy);
    }
}
void PotentialMovableWidget::printEnergy()
{
    QString buf;
    buf.sprintf("%lg",this->potentialViewMovable->model->get_E0());
    this->lE->setText(buf);
}
void PotentialMovableWidget::printLevelNumber()
{
    QString buf;
    buf.sprintf("%i",this->potentialViewMovable->model->get_LevelNumber());
    this->lN->setText(buf);
}
