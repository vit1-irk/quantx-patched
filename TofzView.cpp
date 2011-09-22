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
#include <QLabel>
#include "BreakStatus.h"
#include "TofzView.h"

class ZTDraggable : public QGraphicsItem
{
    TofzView *view;
    QPointF p2;
    QPen penForPainter;
    QPen penHover;
    QPen pen;
public:
//    double widthLineE;
    ZTDraggable(TofzView *v,QGraphicsItem *parent = 0);
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
    double ZTDraggable::getEnergyFromLine()
    {
        QPointF p = pos();
        return p.x();
    }
    QRectF boundingRect() const
    {
    QRectF vp = view->sceneRect();
    //double lineWidth= 0.005*vp.height();
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

        qreal ax1 = view->mapFromGlobal(QPoint(1,1)).x();
        qreal ax2 = view->mapFromScene(QPoint(1,1)).x();
        qreal ax3 = view->width();


//        double ay = fabs(sa.y() - sb.y());
//        view->widthLineE=0;

//        penForPainter.setWidthF(ax);
        penForPainter.setWidthF(view->lineWidth);
        painter->setPen(penForPainter);
        QPainter::RenderHints saved_hints = painter->renderHints();
        painter->setRenderHint(QPainter::Antialiasing, false);
        painter->drawLine(QLineF(QPointF(),p2));
        painter->setRenderHints(saved_hints);
    }
    QLineF line() const { return QLineF(pos(),pos() + p2); }

    void mousePressEvent(QGraphicsSceneMouseEvent * event)
    {
        if (event->buttons() & Qt::RightButton)
        {
            QMenu m;
/*            QAction *scaleE = m.addAction("Scale E");
            QAction *what = m.exec(event->screenPos());
            if (what == scaleE)
            {
//                view->scaleE();
            }*/
               update();// repaint();
        }
    }

};
TofzView::TofzView(PhysicalModel *m, QWidget *parent)
: QGraphicsView(parent), model(m)
{
    Erase = true; // this must initially be true
    lineWidth = 3;
    tMax = 1.1;
    tMin = -0.1;
    Zmin = -0.1;
    Zmax = 1.1;
    hZ = 0.05;

    lineh = NULL;
    linev = NULL;
    lineZ = NULL;
    gbScaleXY = NULL;
    dialogZ = NULL;
    leTmin = NULL;
    leTmax = NULL;
    initDialogScaleY();

    model = m;
    curve_number = 0;

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
    setMinimumSize(160, 100);
    connect(this,SIGNAL(signalScaleTChanged()),this,SLOT(resizePicture()));
    connect(model,SIGNAL(signalScaleZChanged()),this,SLOT(resizePicture()));
    connect(model,SIGNAL(signalEnergyChanged(double)),this,SLOT(slot_whole_T_of_z()));
    connect(model,SIGNAL(signalWidthChanged()),this,SLOT(resizePicture()));
    connect(model,SIGNAL(signalZChanged(double)),this,SLOT(slotZline()));
    resizePicture();
}
/*TofzView::~TofzView()
{
    disconnect(this, 0, 0, 0);
    for (QMap<int,TofzCurve*>::iterator i = curves.begin(); i != curves.end(); ++i)
    {
        int n = i.key();
        removeCurve(n);
    }
    if (!gbScaleXY) delete gbScaleXY;
    if (!lineZ) delete lineZ;
    if (!lineh) delete lineh;
    if (!linev) delete linev;
}
*/
void TofzView::setScalesFromModel()
{
    zParameters tp = model->getzParam();
    Zmin=tp.zmin;
    Zmax=tp.zmax;
    hZ=tp.hz;
}

void TofzView::slotZline()
{
    zParameters tp = model->getzParam();
    double z=tp.z;
    if(z>tp.zmax) z=tp.zmax;
    if(z<tp.zmin) z=tp.zmin;
    QRectF vp = scene()->sceneRect();
    if (!lineZ)
    {
        lineZ = new ZTDraggable(this);
        scene()->addItem(lineZ);
    }
    QPen p;
    p.setStyle(Qt::DashLine);
    p.setColor(Qt::blue);
    lineZ->setPen(p);

    lineZ->setLine(vp.width()*(z-Zmin)/(Zmax-Zmin), 0., vp.height());

}

void TofzView::resizePicture()
{
    zParameters tp = model->getzParam();
    if(tp.zmax!=Zmax||tp.zmin!=Zmin||hZ!=tp.hz)
    {
        Zmin=tp.zmin;
        Zmax=tp.zmax;
        hZ=tp.hz;
/*        if(this->dialogZ) dialogZ->modelChanged();
        else
        {
            dialogZ=new Zview(this);
            dialogZ->setModel(model);
        }*/
    }
    setViewportMapping();
    slot_whole_T_of_z();
    slotZline();
}
void TofzView::setViewportMapping()
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
//---------------
/*    QRectF vp  = QRectF(QPointF(this->Zmin,this->tMin),QPointF(this->Zmax,this->tMax));
    QRectF sr  = scene()->sceneRect();
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
    update();*/
}
void TofzView::resizeEvent(QResizeEvent *)
{
    resizePicture();
}

void TofzView::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
}
void TofzView::scaleView(qreal scaleFactor)
{
    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.007 || factor > 1000)
        return;

    scale(scaleFactor, scaleFactor);
}
void TofzView::showDialogZ()
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

void TofzView::clearAll()
{
    for ( QMap<int,TofzCurve*>::iterator i = curves.begin();  i != curves.end();   ++i)
    {
        int m = i.key();
        removeCurve(m);
    }
}
void TofzView::scrollView(int dx, int dy)
{
    double stepY=(tMax-tMin)/5;//numTTicks;
    tMin +=dy*stepY;
    tMax +=dy*stepY;
    setScaleTZ();
    zParameters tp = model->getzParam();
    Zmin=tp.zmin;
    Zmax=tp.zmax;
    hZ=tp.hz;
    double stepX=(Zmax-Zmin)/5;//numETicks;
    Zmin +=dx*stepX;
    Zmax +=dx*stepX;
    if(Zmin<0) Zmin=0;//-0.2;
    if(Zmax<0) Zmax=1.;
    zParameters tz;
    tz.zmin=Zmin;
    tz.zmax=Zmax;
    tz.hz=hZ;
    model->setzParam(tz);
/*    if(this->dialogZ) dialogZ->modelChanged();
    else
    {
        dialogZ=new Zview(this);
        dialogZ->setModel(model);
    }*/
}
void TofzView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Delete:
        clearAll();
        break;
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

void TofzView::slot_whole_T_of_z()
{
    if (! isVisible()) return;
    QRectF vp = scene()->sceneRect();
    QRectF vp_old=vp;
    SettingParameters ts;
    ts=model->getSettingParameters();
    lineWidth=ts.lineWidth;
    QPen p;
    p.setWidthF(lineWidth);
    p.setJoinStyle(Qt::BevelJoin);
    p.setCapStyle(Qt::RoundCap);
    p.setColor(Qt::black);
    static const QColor colorForIds[6] = {
        Qt::red, Qt::green, Qt::black, Qt::cyan, Qt::magenta, Qt::yellow
    };
    const int size_colorForIds = sizeof(colorForIds)/sizeof(colorForIds[0]);
    zParameters tp = model->getzParam();
    double zt=tp.z;
    hZ=tp.hz;
    Zmin=tp.zmin;
    Zmax=tp.zmax;
    if(Zmin>Zmax) Zmax=1.5*Zmin;
    if(Erase) curve_number=0;
    else curve_number=curve_number+1;
    int n=curve_number;
    if(Erase)
    {
        for ( QMap<int,TofzCurve*>::iterator i = curves.begin();  i != curves.end();   ++i)
        {
            int m = i.key();
            removeCurve(m);
        }
    }
    if(!linev)
    {
        lineh = new QGraphicsLineItem();
        linev = new QGraphicsLineItem();
        linev->setPen(p);
        lineh->setPen(p);
        linev->setLine(vp.width()*(-Zmin)/(Zmax-Zmin), 0, vp.width()*(-Zmin)/(Zmax-Zmin),vp.height() );
        lineh->setLine(0,vp.height()*(-tMin)/(tMax-tMin),vp.width(),vp.height()*(-tMin)/(tMax-tMin));
//        lineh->setLine(Zmin,0.,Zmax,0);
//        linev->setLine(0, tMin, 0., tMax);
        scene()->addItem(lineh);
        scene()->addItem(linev);
    }
    else
    {
        linev->setPen(p);
        lineh->setPen(p);
        linev->setLine(vp.width()*(-Zmin)/(Zmax-Zmin), 0, vp.width()*(-Zmin)/(Zmax-Zmin),vp.height() );
        lineh->setLine(0,vp.height()*(-tMin)/(tMax-tMin),vp.width(),vp.height()*(-tMin)/(tMax-tMin));
//        lineh->setLine(Zmin,0.,Zmax,0);
//        linev->setLine(0, tMin, 0., tMax);
    }
    int npoints;//=501;
    double x0=Zmin;
    if(Zmin<0) x0=0.;//1e-7;
    QVector<double> transmission;
    QPolygonF curveTZ;
    npoints=1+(Zmax-x0)/hZ;
    curveTZ.resize(npoints);
    transmission.resize(npoints);
    double Eold=model->get_E0();
    p.setColor(colorForIds[n % size_colorForIds]);
    transmission=model->getTransmissionOfZ(x0,Zmax,npoints);
    for (int i=0; i < npoints; i++)
    {
        double x = vp.width()*(-Zmin)/(Zmax-Zmin)+(x0 + hZ*i)*vp.width()/(Zmax-Zmin);
        double y = vp.height()*(transmission[i]-tMin)/(tMax-tMin);//transmission[i];
//        double x = x0 + hZ*i;
//        double y = transmission[i];
        curveTZ[i]  = QPointF(x,y);
    }
    setCurve(n,curveTZ,p);
//    setCurve(n,curveTZ,colorForIds[n % size_colorForIds]);
}
void TofzView::setCurve(int id,const QPolygonF & curve, const QPen& pen)
{

    if (curves[id])
    {
        curves[id]->setPolygon(curve);
    }
    else
    {
        TofzCurve *c = new TofzCurve(curve,this);
        scene()->addItem(c);
        curves[id] = c;
    }
    curves[id]->setPen(pen);
    update();
}

void TofzView::removeCurve(int id)
{
    QGraphicsItem *item = curves[id];
    if (item)
    {
    scene()->removeItem(curves[id]);
    delete curves[id];
    curves[id] = 0;
    }//this is dangerous: curves.remove(id);
    update();
//    repaint();
}

void TofzCurve::paint(QPainter * painter, const QStyleOptionGraphicsItem * , QWidget * )
{
    painter->setPen( pen() );
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPolyline(polygon().data(),polygon().size());

}

void TofzView::initDialogScaleY()
{
    gbScaleXY = new QGroupBox(this);
    gbScaleXY->setWindowTitle(tr("Scales for T(z)"));
    gbScaleXY->setWindowFlags(Qt::Window);
    gbScaleXY->setFont(QFont("Serif", 12, QFont::Bold ));

    QVBoxLayout *vl = new QVBoxLayout;
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("Tmin"),this));
        h->addWidget(this->leTmin= new QLineEdit(this));
        QString x;        x.sprintf("%lg",this->tMin);
        this->leTmin->setText(x);
        connect(this->leTmin,SIGNAL(editingFinished()),this,SLOT(updateScaleTZ()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("Tmax"),this));
        h->addWidget(this->leTmax= new QLineEdit(this));
        QString x;
        x.sprintf("%lg",this->tMax);
        this->leTmax->setText(x);
        //        this->leTmax->setToolTip("High value of E-interval");
        connect(this->leTmax,SIGNAL(editingFinished()),this,SLOT(updateScaleTZ()));
        vl->addWidget(line);
    }
    gbScaleXY->setLayout(vl);
}

void TofzView::showDialogScaleY()
{
    gbScaleXY->show();
    gbScaleXY->raise();
    gbScaleXY->setFocus();
}
void TofzView::setScaleTZ()
{
    QString x;
    x.sprintf("%lg",this->tMax);
    this->leTmax->setText(x);
    x.sprintf("%lg",this->tMin);
    this->leTmin->setText(x);
}
void TofzView::updateScaleTZ()
{
    double TminNew = this->leTmin->text().toDouble();
    double TmaxNew = this->leTmax->text().toDouble();
    if (tMin != TminNew||tMax != TmaxNew)
    {
        tMin=TminNew;
        tMax=TmaxNew;
        emit(signalScaleTChanged());
    }
}
void TofzView::contextMenuEvent(QContextMenuEvent *event)
{
        QMenu m;
        QFont font( "Serif", 10, QFont::DemiBold );
        m.setFont(font);
        QAction *scaleT = m.addAction(tr("Y-scale"));
        QAction *scaleZ = m.addAction(tr("Z-scale"));
        QAction *what = m.exec(event->globalPos());
        if (what == scaleZ)
        {
            this->showDialogZ();
            update();
        }
        if (what == scaleT)
        {
            this->showDialogScaleY();
            update();
        }
}
ZTDraggable::ZTDraggable(TofzView *v,QGraphicsItem *parent)
: QGraphicsItem(parent), view(v)
{
    setCursor(Qt::SizeVerCursor);
    penHover.setStyle(Qt::DashLine);
    penHover.setWidth(v->lineWidth);
    penHover.setColor(Qt::blue);
    pen.setStyle(Qt::DotLine);
    pen.setColor(Qt::blue);
    pen.setWidth(v->lineWidth);

    setCursor(Qt::SizeHorCursor);
    setFlag(QGraphicsItem::ItemIsMovable,true);
    setFlag(QGraphicsItem::ItemIsSelectable,true);
    setZValue(999);
    setAcceptHoverEvents ( true );
}

QVariant ZTDraggable::itemChange(GraphicsItemChange change, const QVariant & value)
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
            newZ=view->Zmin+(view->Zmax-view->Zmin)*newZ/vp.width();
            view->model->set_z(newZ);
            return QVariant();
        }
    }
    return QGraphicsItem::itemChange(change,value);
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

TZWidget::TZWidget(PhysicalModel *model, QWidget *parent)
: QGroupBox(parent)
{
    setTitle(tr("Transmission T(z) at energy E"));
    QVBoxLayout *vl = new QVBoxLayout();
    tofzView = new TofzView(model,this);
    vl->addWidget(tofzView);

    QHBoxLayout *hl = new QHBoxLayout();
    QToolButton *reset = new QToolButton(this);
    reset->setIcon(QIcon(":/images/player_play.png"));
    reset->adjustSize();
//    QPushButton *reset = new QPushButton(tr("&Resize"));
    connect(reset,SIGNAL(clicked()),tofzView,SLOT(resizePicture()));

    QLabel *lTtext= new QLabel(this);
    lTtext->setText(tr("T,N="));
    QLabel *lT= new MyLabel("",this);
    lT->setTextFormat(Qt::AutoText);
    connect(model, SIGNAL(signalTransmissionChanged(double)), lT, SLOT(setNum(double)));

    QLabel *lEtext= new QLabel(this);
    lEtext->setText(tr("E="));

    QLabel *lE = new MyLabel("",this);

    hl->addWidget(reset);
    hl->addWidget(lEtext);
    hl->addWidget(lE);
    hl->addWidget(lTtext);
    hl->addWidget(lT);

    connect(model, SIGNAL(signalEnergyChanged(double)), lE, SLOT(setNum(double)));

//    QPushButton *buttonClose = new QPushButton(tr("Close"));
    QToolButton *buttonClose = new QToolButton(this);
    buttonClose->setIcon(QIcon(":/images/erase.png"));
    buttonClose->adjustSize();//QPushButton(tr("Close"));
    hl->addStretch();
    hl->addWidget(buttonClose);

    connect(buttonClose,SIGNAL(clicked()),this,SLOT(hide()),Qt::QueuedConnection); //???
    vl->addLayout(hl);
    setLayout(vl);
}
void TZWidget::readFromXml(QXmlStreamReader *r)
{
    Q_ASSERT(this);
    Q_ASSERT(r->isStartElement() && r->name() == "TZ");
    double Emin = 0, Emax = 0, he=0;
    double qamin = 0, qamax = 0;
    while (!r->atEnd())
    {
        r->readNext();
        if (r->isEndElement())
            break;
        if (!r->isStartElement())
            continue;
/*        if (r->name() == "Emin")
        {
            QString s = r->readElementText();
            transmissionView->Emin = s.toDouble();
        }
        else if (r->name() == "Emax")
        {
            QString s = r->readElementText();
            transmissionView->Emax = s.toDouble();
        }
        else if (r->name() == "hE")
        {
            QString s = r->readElementText();
            transmissionView->hE = s.toDouble();
        }*/
        else if (r->name() == "Tmax")
        {
            QString s = r->readElementText();
            tofzView->tMax = s.toDouble();
        }
        else if (r->name() == "Tmin")
        {
            QString s = r->readElementText();
            tofzView->tMin = s.toDouble();
        }
        else
            skipUnknownElement(r);
    }
    tofzView->setScaleTZ();
}


void TZWidget::writeToXml(QXmlStreamWriter *w)
{
    w->writeStartElement("TZ");
    {
        QString s;
/*        s.sprintf("%lg",transmissionView->Zmin);
        w->writeTextElement("Zmin",s);
        s.sprintf("%lg",transmissionView->Zmax);
        w->writeTextElement("Zmax",s);
        s.sprintf("%lg",transmissionView->hZ);
        w->writeTextElement("hZ",s);*/
        s.sprintf("%lg",tofzView->tMin);
        w->writeTextElement("Tmin",s);
        s.sprintf("%lg",tofzView->tMax);
        w->writeTextElement("Tmax",s);
    }
    w->writeEndElement();
}
