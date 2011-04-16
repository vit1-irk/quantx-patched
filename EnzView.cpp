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

class ZDraggable : public QGraphicsItem
{
    EnzView *view;
    QPointF p2;
    QPen penForPainter;
    QPen penHover;
    QPen pen;
public:
//    double widthLineE;
    ZDraggable(EnzView *v,QGraphicsItem *parent = 0);
    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
    void setPen(QPen _pen) { penForPainter = pen = _pen; }
    void setLine(double x1,double y1,double height)
    {
        QPointF p = pos();
        if (x1 != p.x() || y1 != p.y() || height != p2.y())
        {
            prepareGeometryChange();
            p2 = QPointF(0,height);
//            p2 = QPointF(width,0);
            this->setPos(x1,y1);
//            this->setPos(x1,y1);
            update();//repaint();
        }
    }
    void hoverEnterEvent ( QGraphicsSceneHoverEvent *  )
    {
        penForPainter = penHover;
        update();//repaint();
    }
    void hoverLeaveEvent ( QGraphicsSceneHoverEvent *  )
    {
        penForPainter = pen;
        this->setSelected(false);
        update();//repaint();
    }
    double ZDraggable::getZFromLine()
    {
        QPointF p = pos();
        return p.x(); 
    }
    QRectF boundingRect() const
    {
    QRectF vp = view->sceneRect();
    //double widthLine= 0.005*vp.height();
        QPoint v1(0,0);
        QPoint v2(3,0);
//        QPoint v2(0,3);
        QPointF fv1 = view->mapToScene(v1);
        QPointF fv2 = view->mapToScene(v2);
        //double widthLine1 = fabs(fv2.x() - fv1.x());
//        double widthLine1 = fabs(fv2.y() - fv1.y());
    
        QPoint va(0,0);
        QPoint vb(5,0);
//        QPoint vb(0,5);
        QPointF sa = view->mapToScene(va);
        QPointF sb = view->mapToScene(vb);
        double ay = fabs(sa.y() - sb.y());
//        double ax = fabs(sa.x() - sb.x());
        double ax= fabs(sa.x() - sb.x());
//      double ay = fabs(sa.y() - sb.y());
        QRect vpr = view->viewport()->rect();
        QPointF vpr1 = view->mapToScene(vpr.topLeft());
        QPointF vpr2 = view->mapToScene(vpr.bottomRight());
        QMatrix m = view->matrix();
        QRectF aa=QRectF(QPointF(),p2).adjusted(-ax,-ay,ax,ay);
        return aa;///QRectF(QPointF(),p2).adjusted(-ax,-ay,ax,ay);
    }
    void paint(QPainter *painter,const QStyleOptionGraphicsItem *,QWidget *)
    {
        QPoint va(0,0);
        QPoint vb(1,0);
//        QPoint vb(0,1);
        QPointF sa = view->mapToScene(va);
        QPointF sb = view->mapToScene(vb);
        double ax = fabs(sa.x() - sb.x());
//        double ay = fabs(sa.y() - sb.y());
//        view->widthLineE=0;
        penForPainter.setWidthF(view->widthLine);
//        penForPainter.setWidthF(ax);
//        penForPainter.setWidthF(ay);
        painter->setPen(penForPainter);
        //qreal x2 = penForPainter.widthF();
        painter->drawLine(QLineF(QPointF(),p2));
    }
    QLineF line() const { return QLineF(pos(),pos() + p2); }

    void mousePressEvent(QGraphicsSceneMouseEvent * event)
    {
        if (event->buttons() & Qt::RightButton)
        {
            QMenu m;
            QAction *scaleE = m.addAction("Scale z");
            QAction *what = m.exec(event->screenPos());
            if (what == scaleE)
            {
//                view->scaleE();
            }
               update();// repaint();
        }
    }

};

ZDraggable::ZDraggable(EnzView *v,QGraphicsItem *parent)
: QGraphicsItem(parent), view(v)
{
    setCursor(Qt::SizeVerCursor);
    penHover.setStyle(Qt::DashLine);//(Qt:: DotLine);
//    penHover.setWidthF(view->widthLineE);
    penHover.setColor(Qt::blue);
//     pen.setDashPattern(dashes);
    pen.setStyle(Qt::DotLine);
//    pen.setStyle(Qt::DashLine);
    pen.setColor(Qt::blue);
//    pen.setWidthF(view->widthLineE);

    setCursor(Qt::SizeHorCursor);
    setFlag(QGraphicsItem::ItemIsMovable,true);		
    setFlag(QGraphicsItem::ItemIsSelectable,true);	
    setZValue(999);
    setAcceptHoverEvents ( true );
}

QVariant ZDraggable::itemChange(GraphicsItemChange change, const QVariant & value)
{
    switch (change)
    {
    case ItemPositionChange: 
        if (isSelected())
        {
            QPointF newpos = value.toPointF();
            QPointF oldpos = pos();
            newpos.setY( oldpos.y() );
            return newpos;
        }
    case ItemPositionHasChanged:
        if (isSelected())
        {
            double newZ = pos().x();
            QRectF vp = view->sceneRect();//
            newZ=view->zmin+(view->zmax-view->zmin)*newZ/vp.width();
            view->model->set_z(newZ);
            return QVariant();

//            double newZ = pos().x();
//            view->model->set_z(newZ);
//            return QVariant();
        }
    }
    return QGraphicsItem::itemChange(change,value);
}
EnzView::EnzView(PhysicalModel *m, QWidget *parent)
: QGraphicsView(parent), model(m),lineh(0),linev(0),lineZ(0),
Enzmin(-21.), Enzmax(0.1), dialogZ(0)
{
    widthLine = 3;
    leEnzmin = NULL;
    leEnzmax = NULL;
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
    setMinimumSize(260, 220);
    setScalesFromModel();
    initDialogScaleY();
//    connect(model,SIGNAL(signalZChanged(double)),this,SLOT(slotZline()));
    connect(this,SIGNAL(signalScaleEnzChanged()),this,SLOT(resizePicture()));
}
EnzView::~EnzView()
{
    disconnect(this, 0, 0, 0);
    if (!dialogZ) delete dialogZ;
    if (!gbScaleXY) delete gbScaleXY;
    for (QMap<int,EnzCurve*>::iterator i = curves.begin(); i != curves.end(); ++i)
    {
        int n = i.key();
        removeCurve(n);
    }
    if (!lineh) delete lineh;
    if (!linev) delete linev;
}

void EnzView::slotZline()
{
    zParameters tp = model->getzParam();
    double z=tp.z;
    if(z>tp.zmax) z=tp.zmax;
    if(z<tp.zmin) z=tp.zmin;
    QRectF vp = scene()->sceneRect();
    if (!lineZ) 
    {
        lineZ = new ZDraggable(this);
//        lineZ = new ZTDraggable(this);
        lineZ->setLine(vp.width()*(z-zmin)/(zmax-zmin), 0., vp.height());
//        lineZ->setLine(z,Enzmin,Enzmax-Enzmin);
        scene()->addItem(lineZ);
    }
    else     
        lineZ->setLine(vp.width()*(z-zmin)/(zmax-zmin), 0., vp.height());
//        lineZ->setLine(z,Enzmin,Enzmax-Enzmin);
}

void EnzView::setScalesFromModel()
{
    QPair<double,double> umin_umax = model->getUminUmax();
    Enzmin = umin_umax.first;
    Enzmax = umin_umax.second;
}

void EnzView::resizePicture()
{
    zParameters tp = model->getzParam();
    if(tp.zmax!=zmax||tp.zmin!=zmin||hz!=tp.hz)
    {
        zmin=tp.zmin;
        zmax=tp.zmax;
        hz=tp.hz;
        if(this->dialogZ) dialogZ->modelChanged();
        else
        {
            dialogZ=new Zview(this);
            dialogZ->setModel(model);
        }
    }
    setViewportMapping();
    slot_En_of_z();
    slotZline();
}
void EnzView::setViewportMapping()
{
    QRectF a = QRectF(this->viewport()->rect());
    double rZmin=0;
    double rZmax=a.width();//npoints-1;
    double rTMin=0;
    double rTMax=a.height();//a.height();
    QRectF b = QRectF(QPointF(rZmin,rTMin),QPointF(rZmax,rTMax));
    scene()->setSceneRect(b);
    {
        qreal m11 = a.width() / b.width();
        qreal m22 = - a.height() / b.height();
        qreal dx = - m11 * a.x();
        qreal dy = - m22 * (a.y() + a.height());
        QMatrix m(m11,0,0,m22,dx,dy);
        this->setMatrix(m);
        scene()->update(scene()->sceneRect());
    }
    update();
//-------------
/*    zParameters tp = model->getzParam();
    double zmin=tp.zmin;
    double zmax=tp.zmax;
    QRectF vp = QRectF(QPointF(zmin,this->Enzmin),QPointF(zmax,this->Enzmax));

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
*/
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
void EnzView::clearAll()
{
    
        for ( QMap<int,EnzCurve*>::iterator i = curves.begin();  i != curves.end();   ++i)
    {
        int m = i.key();
            removeCurve(m);
    }
}

void EnzView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Delete:
        clearAll();
        break;
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
    }
}

static void myrepaint(const curveSet& cs)
{ 
    for(int i=0; i<cs.size(); i++)
    {
//        setCurve(i,cs[i]);
    }
}

void EnzView::slot_En_of_z()
{
    QRectF vp = scene()->sceneRect();
    QRectF vp_old=vp;
    QPen p;
    p.setWidthF(widthLine);
    p.setJoinStyle(Qt::BevelJoin);
    p.setCapStyle(Qt::RoundCap);
    p.setColor(Qt::black);
//-------
    static const QColor colorForIds[12] = {
        Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta, Qt::yellow,
        Qt::darkRed, Qt::darkGreen, Qt::darkBlue, Qt::darkCyan, Qt::darkMagenta, Qt::darkYellow
    };
    const int size_colorForIds = sizeof(colorForIds)/sizeof(colorForIds[0]);
    zParameters tp = model->getzParam();
    double zt=tp.z;
    double hz=tp.hz;
    double zmin=tp.zmin;
    double zmax=tp.zmax;
    if(zmin>zmax) 
    {
        zmax=1;
        zmin=0.;
    }
    double zz=zt+hz;
    if(zz>=zmax) 
    { 
        zt=zmin;
    }
    if(zz<zmin) 
    { 
        zt=zmax;
    }
    if(!linev)
    {
        lineh = new QGraphicsLineItem();
        linev = new QGraphicsLineItem();
        linev->setPen(p);
        lineh->setPen(p);
        linev->setLine(vp.width()*(-zmin)/(zmax-zmin), 0, vp.width()*(-zmin)/(zmax-zmin),vp.height() );
        lineh->setLine(0,vp.height()*(-Enzmin)/(Enzmax-Enzmin),vp.width(),vp.height()*(-Enzmin)/(Enzmax-Enzmin));
//        lineh->setLine(-0.1*zmax, 0., 1.1*zmax, 0);
//        linev->setLine(0.,1.2*Enzmin,0.,-.3*Enzmin);
        scene()->addItem(lineh);
        scene()->addItem(linev);
    }
    else
    {
        linev->setPen(p);
        lineh->setPen(p);
        linev->setLine(vp.width()*(-zmin)/(zmax-zmin), 0, vp.width()*(-zmin)/(zmax-zmin),vp.height() );
        lineh->setLine(0,vp.height()*(-Enzmin)/(Enzmax-Enzmin),vp.width(),vp.height()*(-Enzmin)/(Enzmax-Enzmin));
//        lineh->setLine(-0.1*zmax, 0., 1.1*zmax, 0);
//        linev->setLine(0.,1.1*Enzmin,0.,-0.1*Enzmin);
    }
    int n=0; 
    double z0=zmin;
    if(zmin<0) z0=0.;//1e-7;
    int npoints=1+(zmax-z0)/hz;
        curveSet cs;
        int nmxold=-1;
        double Eold=model->get_E0();
        for (int i=0; i < npoints; i++)
        {
            //         for (double z=zmin; z>=tp.zmin&&z<=tp.zmax; z+=hz)
            //        { 
            double zz=z0 + hz*i;
            tp.z=zz;
            model->setzParam(tp);
            QVector<double> Ebound = model->getEn();
            int nmx=Ebound.size()-1;//this->nmaxLevel;
            int nmn=0;//this->nminLevel;
            if(nmxold==-1) 
            {
                nmxold=nmx;
            }
            for(int m=0; m<=nmx; m++)
            {
                Ebound[m]=vp.height()*(Ebound[m]-Enzmin)/(Enzmax-Enzmin);    
            }
            double zi = vp.width()*(-zmin)/(zmax-zmin)+zz*vp.width()/(zmax-zmin);
            updateCurves(cs, Ebound, zi);
            for(int j=0; j<cs.size(); j++)
            {
            p.setColor(colorForIds[j % size_colorForIds]);

            setCurve(j,cs[j],p);
            }
            //        setCurve(i,cs[i],colorForIds[i % size_colorForIds]);
            nmxold=nmx;
            tp=model->getzParam();
            if(hz!=tp.hz||zmax!=tp.zmax||zmin!=tp.zmin)
            {
               hz=tp.hz;
               zmax=tp.zmax;
               zmin=tp.zmin;
               zt=tp.z;
            }
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
/*        QPushButton *bInit = new QPushButton(tr("U_1(x,z=0) = U(x)")); 
//        connect(bInit, SIGNAL(clicked()), this, SLOT(slot1()));
        connect(bInit, SIGNAL(clicked()), model, SLOT(slotU1()));
        hl->addWidget(bInit);

        QPushButton *bFin = new QPushButton(tr("U_2(x,z=1) = U(x)")); 
        connect(bFin, SIGNAL(clicked()),model, SLOT(slotU2()));
        hl->addWidget(bFin);
*/
void EnzView::contextMenuEvent(QContextMenuEvent *event)
{
        QMenu m;
        QFont font( "Serif", 10, QFont::DemiBold );
        m.setFont(font);
        QAction *scaleZ = m.addAction(tr("Z-scale"));
        QAction *scaleY = m.addAction(tr("Y-scale"));
        QAction *what = m.exec(event->globalPos());
        if (what == scaleZ)
        {
            this->showDialogZ();
            update();
        }
       if (what == scaleY)
        {
            this->showDialogScaleY();
            update();
        }
}
void EnzView::showDialogZ()
{   
    if (!dialogZ) 
    {
        dialogZ = new Zview(this);
        dialogZ->setModel(model);
    }
    dialogZ->show(); 
    dialogZ->activateWindow();
    dialogZ->setFocus();
}

EnzWidget::EnzWidget(PhysicalModel *model, QWidget *parent)
: QGroupBox(parent)
{
    setTitle(tr("En(z):U(z)=(1-z)*U1+z*U2"));

    QVBoxLayout *vl = new QVBoxLayout();
    enzView = new EnzView(model,this);
    vl->addWidget(enzView);
    QHBoxLayout *hl = new QHBoxLayout();

    //    bRunEnz = new QPushButton(tr("&Run"));
    bRunEnz = new QToolButton(this);
    bRunEnz->setIcon(QIcon("images/player_play.png"));
    bRunEnz->adjustSize();
    hl->addWidget(bRunEnz);		

    QLabel *ltext= new QLabel(this);
    ltext->setText(tr("z="));
    hl->addWidget(ltext);		
    QLabel *lz= new QLabel(this);
    hl->addWidget(lz);		
    connect(model, SIGNAL(signalZChanged(double)), lz, SLOT(setNum(double)));

    QToolButton *buttonClose = new QToolButton(this);
    buttonClose->setIcon(QIcon("images/erase.png"));
    buttonClose->adjustSize();//QPushButton(tr("Close"));
    hl->addStretch();
    hl->addWidget(buttonClose);
    connect(buttonClose,SIGNAL(clicked()),this,SLOT(hide()),Qt::QueuedConnection); //???

//    QPushButton *buttonClose = new QPushButton(tr("Close"));

    connect(bRunEnz,SIGNAL(clicked()),this,SLOT(slotRunEnz()));
    vl->addLayout(hl);

    setLayout(vl);

}
void EnzWidget::slotRunEnz()
{
    bRunEnz->setIcon(QIcon("images/player_stop.png"));//->setText(tr("STOP"));
    bRunEnz->adjustSize();
    disconnect(bRunEnz, SIGNAL(clicked()), this, SLOT(slotRunEnz()));
    breakStatus.onButton(bRunEnz);
    enzView->resizePicture();
    breakStatus.noButton(bRunEnz);
//    breakStatus.disconnect();
    bRunEnz->setIcon(QIcon("images/player_play.png"));//->setText(tr("RUN "));
    bRunEnz->adjustSize();
    connect(bRunEnz, SIGNAL(clicked()), this, SLOT(slotRunEnz()));
}

void EnzView::initDialogScaleY()
{   
    gbScaleXY = new QGroupBox(this);
    gbScaleXY->setWindowTitle(tr("Scales for En(z)"));
    gbScaleXY->setWindowFlags(Qt::Window);
    gbScaleXY->setFont(QFont("Serif", 12, QFont::Bold )); 

    QVBoxLayout *vl = new QVBoxLayout;
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("Enz_min"),this));
        h->addWidget(this->leEnzmin= new QLineEdit(this));
        QString x;
        x.sprintf("%lg",this->Enzmin);
        this->leEnzmin->setText(x);
        connect(this->leEnzmin,SIGNAL(editingFinished()),this,SLOT(updateScaleEnz()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("Enz_max"),this));
        h->addWidget(this->leEnzmax= new QLineEdit(this));
        QString x;
        x.sprintf("%lg",this->Enzmax);
        this->leEnzmax->setText(x);
        //        this->leTmax->setToolTip("High value of E-interval");
        connect(this->leEnzmax,SIGNAL(editingFinished()),this,SLOT(updateScaleEnz()));
        vl->addWidget(line);
    }
    gbScaleXY->setLayout(vl);
}

void EnzView::showDialogScaleY()
{   
    gbScaleXY->show(); 
    gbScaleXY->raise();
    gbScaleXY->setFocus();
}
void EnzView::setScaleEnz()
{
    QString x;
    this->leEnzmax->setText(x);
    x.sprintf("%lg",this->Enzmax);
    this->leEnzmax->setText(x);
    x.sprintf("%lg",this->Enzmin);
    this->leEnzmin->setText(x);
}
void EnzView::updateScaleEnz()
{
    double EnzminNew = this->leEnzmin->text().toDouble();
    double EnzmaxNew = this->leEnzmax->text().toDouble();
    if (Enzmin != EnzminNew||Enzmax != EnzmaxNew)
    {
        Enzmin=EnzminNew;
        Enzmax=EnzmaxNew;
//        emit(signalScaleEnzChanged());
    }
}
