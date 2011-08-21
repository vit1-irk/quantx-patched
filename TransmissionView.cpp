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
#include "TransmissionView.h"

class EnergyDraggable : public QGraphicsItem
{
    TransmissionView *view;
    QPointF p2;
    QPen penForPainter;
    QPen penHover;
    QPen pen;
public:
//    double widthLineE;
    EnergyDraggable(TransmissionView *v,QGraphicsItem *parent = 0);
    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
    void setPen(QPen _pen) { penForPainter = pen = _pen; }
    void setLine(double x1,double y1,double height)
    {
        QPointF p = pos();
        QRectF vp = view->sceneRect();//
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
    double EnergyDraggable::getEnergyFromLine()
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
        //double ax = fabs(sa.x() - sb.x());

        //qreal ax1 = view->mapFromGlobal(QPoint(1,1)).x();
        //qreal ax2 = view->mapFromScene(QPoint(1,1)).x();
        //qreal ax3 = view->width();
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
               update();// repaint();
        }
    }

};

TransmissionView::TransmissionView(PhysicalModel *m, QWidget *parent)
: QGraphicsView(parent), model(m)
{
    Erase = true; // this must initially be true
    lineWidth = 3;//10;
    tMax = 1.1;
    tMin = -0.1;
    Emin = -15;
    Emax = 20;
    hE = 0.05;

    lineh = NULL;
    linev = NULL;
    lineE = NULL;

    leTmin = NULL;
    leTmax = NULL;
    leEmin = NULL;
    leEmax = NULL;
    leHE   = NULL;
    initDialogScaleY();

    model = m;
    curve_number = -1;

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
    setMinimumSize(300, 150);
    model->set_EmaxEmin(Emax,Emin);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setViewportMapping();

//    connect(this,SIGNAL(signalScaleTEChanged()),this,SLOT(resizePicture()));
    connect(model,SIGNAL(signalLevelNumberChanged(int)),this,SLOT(slot_whole_T_of_E()));
    connect(model,SIGNAL(signalPotentialChanged()),this,SLOT(resizePicture()));
    connect(model,SIGNAL(signalEnergyChanged(double)),this,SLOT(slotEline()));
    connect(model,SIGNAL(signalWidthChanged()),this,SLOT(reDraw()));
//    connect(model,SIGNAL(signalWidthChanged()),this,SLOT(resizePicture()));
    connect(this,SIGNAL(signalScaleTEChanged()),this,SLOT(reDraw()));
//    resizePicture();
}
/*TransmissionView::~TransmissionView()
{
    disconnect(this, 0, 0, 0);
    for (QMap<int,TransmissionCurve*>::iterator i = curves.begin(); i != curves.end(); ++i)
    {
        int n = i.key();
        removeCurve(n);
    }
    if (!gbScaleXY) delete gbScaleXY;
    if (!lineE) delete lineE;
    if (!lineh) delete lineh;
    if (!linev) delete linev;
}
*/
void TransmissionView::setScalesFromModel()
{
    if(Emax<=0.) Emax=20;
}
void TransmissionView::slotEline()
{
    QRectF vp = scene()->sceneRect();
    PotentialType type = model->getPotentialType();
    double E=model->get_E0();
    if (!lineE)
    {
        lineE = new EnergyDraggable(this);
        //        lineE->setLine(vp.width()*(E-Emin)/(Emax-Emin), 0., vp.height());
        scene()->addItem(lineE);
    }
    //    else
    QPen p;
    p.setStyle(Qt::DashLine);
    p.setColor(Qt::green);
    lineE->setPen(p);
    lineE->setLine(vp.width()*(E-Emin)/(Emax-Emin), 0., vp.height());
}
void TransmissionView::resizePicture()
{
    setViewportMapping();
    slot_whole_T_of_E();
    slotEline();
}
void TransmissionView::setViewportMapping()
{
    QRectF a = QRectF(this->viewport()->rect());
    double rEmin=0;
    double rEmax=a.width();//npoints-1;
    double rTMin=0;
    double rTMax=a.height();//a.height();
    QRectF b = QRectF(QPointF(rEmin,rTMin),QPointF(rEmax,rTMax));
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
}
void TransmissionView::resizeEvent(QResizeEvent *)
{
    setViewportMapping();
    this->reDraw();
    slotEline();
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
    physCurves.resize(0);
    colorOfCurves.resize(0);
    curve_number=-1;
}
void TransmissionView::scrollView(int dx, int dy)
{
    double stepY=(tMax-tMin)/5;//numTTicks;
    tMin +=dy*stepY;
    tMax +=dy*stepY;
    double stepX=(Emax-Emin)/5;//numETicks;
    Emin +=dx*stepX;
    Emax +=dx*stepX;
    if(Emin<0) Emin=-0.2;
    if(Emax<0) Emax=1.;
    setScaleTE();
    emit(signalScaleTEChanged());
}

void TransmissionView::keyPressEvent(QKeyEvent *event)
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

void TransmissionView::reDraw()
{
    if (! isVisible()) return;
    QRectF vp = scene()->sceneRect();
    QPen p;
    SettingParameters ts;
    ts=model->getSettingParameters();
    lineWidth=ts.lineWidth;
    p.setWidthF(lineWidth);
    p.setJoinStyle(Qt::BevelJoin);
    p.setCapStyle(Qt::RoundCap);
    p.setColor(Qt::black);
    if(!linev)
    {
        lineh = new QGraphicsLineItem();
        linev = new QGraphicsLineItem();
        linev->setPen(p);
        lineh->setPen(p);
        linev->setLine(vp.width()*(-Emin)/(Emax-Emin), 0, vp.width()*(-Emin)/(Emax-Emin),vp.height() );
        lineh->setLine(0,vp.height()*(-tMin)/(tMax-tMin),vp.width(),vp.height()*(-tMin)/(tMax-tMin));
        scene()->addItem(lineh);
        scene()->addItem(linev);
    }
    else
    {
        linev->setPen(p);
        lineh->setPen(p);
        linev->setLine(vp.width()*(-Emin)/(Emax-Emin), 0, vp.width()*(-Emin)/(Emax-Emin),vp.height() );
        lineh->setLine(0,vp.height()*(-tMin)/(tMax-tMin),vp.width(),vp.height()*(-tMin)/(tMax-tMin));
    }
    if(physCurves.size()==0) return;
    int j=physCurves.size();
        for ( QMap<int,TransmissionCurve*>::iterator i = curves.begin();  i != curves.end();   ++i)
        {
            int m = i.key();
            removeCurve(m);
        }
    for (int ic = 0; ic < physCurves.size();    ++ic)
    {
         QPolygonF ipg;// = curves[ic]->polygon();
         QPolygonF ppg = physCurves[ic];
        for (int k = 0; k < ppg.size(); ++k)
        {
            qreal newx = vp.width()*(-Emin)/(Emax-Emin) + ppg[k].x()*vp.width()/(Emax-Emin);
            qreal newy = vp.height()*(ppg[k].y()-tMin)/(tMax-tMin);
            ipg.push_back(QPointF(newx,newy));
//            ipg[k] = QPointF(newx,newy);
        }
        QColor q=colorOfCurves[ic];
        p.setColor(colorOfCurves[ic]);
        if(ipg.size()>=1)setCurve(ic,ipg,p);
    }
    update();
}

void TransmissionView::slot_whole_T_of_E()
{

    if (! isVisible()) return;
    PotentialType type = model->getPotentialType();
    if(type==PERIODIC) return;
    QRectF vp = scene()->sceneRect();
    QRectF vp_old=vp;
    QPen p;
    SettingParameters ts;
    ts=model->getSettingParameters();
    lineWidth=ts.lineWidth;
    p.setWidthF(lineWidth);
    p.setJoinStyle(Qt::BevelJoin);
    p.setCapStyle(Qt::RoundCap);
    p.setColor(Qt::black);
    if(Emin>Emax) Emax=1.5*Emin;
//    Erase = false;
    if(Erase)
    {
        curve_number=0;
        physCurves.resize(0);
        colorOfCurves.resize(0);

    }
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
        linev->setPen(p);
        lineh->setPen(p);
        linev->setLine(vp.width()*(-Emin)/(Emax-Emin), 0, vp.width()*(-Emin)/(Emax-Emin),vp.height() );
        lineh->setLine(0,vp.height()*(-tMin)/(tMax-tMin),vp.width(),vp.height()*(-tMin)/(tMax-tMin));
        scene()->addItem(lineh);
        scene()->addItem(linev);
    }
    else
    {
        linev->setPen(p);
        lineh->setPen(p);
        linev->setLine(vp.width()*(-Emin)/(Emax-Emin), 0, vp.width()*(-Emin)/(Emax-Emin),vp.height() );
        lineh->setLine(0,vp.height()*(-tMin)/(tMax-tMin),vp.width(),vp.height()*(-tMin)/(tMax-tMin));
    }
    static const QColor colorForIds[12] = {
        Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta,
        Qt::black,
        Qt::darkRed, Qt::darkGreen, Qt::darkBlue, Qt::darkCyan, Qt::darkMagenta, Qt::darkYellow
    };
    const int size_colorForIds = sizeof(colorForIds)/sizeof(colorForIds[0]);
    {
        int npoints;//=501;
        QColor color_n;
        if(type==FINITE)
        {
            color_n=colorForIds[n % size_colorForIds];
        }
        if(type==QUASISTATIONARY)
        {
            color_n=colorForIds[model->get_LevelNumber() % size_colorForIds];
        }
        p.setColor(color_n);
        double x0=Emin;
        //    if(Emin<0) x0=1e-7;
        QVector<double> transmission;
        QPolygonF curveTE;
        npoints=1+(Emax-x0)/hE;
        curveTE.resize(npoints);
        transmission.resize(npoints);
        transmission=model->getTransmissionOfE(x0,Emax,npoints);
        if (curve_number >= physCurves.size())
        {
            physCurves.push_back( QPolygonF() );
            colorOfCurves.push_back( QColor());
        }
        colorOfCurves[curve_number]=color_n;
        int kk=physCurves.size();
        for (int i=0; i < npoints; i++)
        {
            double x = vp.width()*(-Emin)/(Emax-Emin)+(x0 + hE*i)*vp.width()/(Emax-Emin);//x0 + hE*i;
            double y = vp.height()*(transmission[i]-tMin)/(tMax-tMin);//transmission[i];
            curveTE[i]  = QPointF(x,y);
            physCurves[curve_number].push_back(QPointF(x0+hE*i, transmission[i]));

        }
        setCurve(curve_number,curveTE,p);
    }
}
/*
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
        lineh->setLine(Emin,0.,Emax,0);
        linev->setLine(0, tMin, 0., tMax);
//        lineh->setLine(tMin, 0., tMax, 0);
//        linev->setLine(0.,Emin,0.,Emax);
        scene()->addItem(lineh);
        scene()->addItem(linev);
    }
    else
    {
        lineh->setLine(Emin,0.,Emax,0);
        linev->setLine(0, tMin, 0., tMax);
//        lineh->setLine(tMin, 0., tMax, 0);
//        linev->setLine(0.,Emin,0.,Emax);
    }
    int npoints;//=501;
    double x0=Emin;
    if(Emin<0) x0=1e-7;
    QVector<double> transmission;
    QPolygonF curveTE;
    npoints=1+(Emax-x0)/hE;
    curveTE.resize(npoints);
    transmission.resize(npoints);
//    double dE = (Emax-Emin)/(npoints-1);
    n=0;
    transmission=model->getTransmissionOfE(x0,Emax,npoints);
    for (int i=0; i < npoints; i++)
    {
        double x = x0 + hE*i;
        double T=model->getTatE(x);
        curveTE[i]  = QPointF(x,T);
        setCurve(n,curveTE,colorForIds[n % size_colorForIds]);
    }
    update();
}
*/



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

void TransmissionCurve::paint(QPainter * painter, const QStyleOptionGraphicsItem * , QWidget * )
{
    painter->setPen( pen() );
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPolyline(polygon().data(),polygon().size());

}

void TransmissionView::initDialogScaleY()
{
    gbScaleXY = new QGroupBox(this);
    gbScaleXY->setWindowTitle("Scales for T(E)");
    gbScaleXY->setWindowFlags(Qt::Window);
    gbScaleXY->setFont(QFont("Serif", 12, QFont::Bold ));

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
        connect(this->leHE,SIGNAL(editingFinished()),this,SLOT(updateScaleTE()));
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
        connect(this->leEmin,SIGNAL(editingFinished()),this,SLOT(updateScaleTE()));
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
        connect(this->leEmax,SIGNAL(editingFinished()),this,SLOT(updateScaleTE()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel("Tmin",this));
        h->addWidget(this->leTmin= new QLineEdit(this));
        QString x;
        x.sprintf("%lg",this->tMin);
        this->leTmin->setText(x);
        //        this->leTmin->setToolTip("Lower value of E-interval");
        connect(this->leTmin,SIGNAL(editingFinished()),this,SLOT(updateScaleTE()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel("Tmax",this));
        h->addWidget(this->leTmax= new QLineEdit(this));
        QString x;
        x.sprintf("%lg",this->tMax);
        this->leTmax->setText(x);
        //        this->leTmax->setToolTip("High value of E-interval");
        connect(this->leTmax,SIGNAL(editingFinished()),this,SLOT(updateScaleTE()));
        vl->addWidget(line);
    }
    gbScaleXY->setLayout(vl);
}

void TransmissionView::showDialogScaleY()
{
    gbScaleXY->show();
    gbScaleXY->raise();
    gbScaleXY->setFocus();
}
void TransmissionView::setScaleTE()
{
    QString x;
    x.sprintf("%lg",this->hE);
    this->leHE->setText(x);
    x.sprintf("%lg",this->Emin);
    this->leEmin->setText(x);
    x.sprintf("%lg",this->Emax);
    this->leEmax->setText(x);
    x.sprintf("%lg",this->tMax);
    this->leTmax->setText(x);
    x.sprintf("%lg",this->tMin);
    this->leTmin->setText(x);
}
void TransmissionView::updateScaleTE()
{
    double hENew = this->leHE->text().toDouble();
    double EminNew = this->leEmin->text().toDouble();
    double EmaxNew = this->leEmax->text().toDouble();
    double TminNew = this->leTmin->text().toDouble();
    double TmaxNew = this->leTmax->text().toDouble();
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
    if (tMin != TminNew)
    {
        changed = true;
        tMin=TminNew;
    }
    if (tMax != TmaxNew)
    {
        changed = true;
        tMax=TmaxNew;
    }
    if(changed)
    {
        model->set_EmaxEmin(Emax,Emin);
        emit(signalScaleTEChanged());
    }
}
void TransmissionView::contextMenuEvent(QContextMenuEvent *event)
{
        QMenu m;
        QFont font( "Serif", 10, QFont::DemiBold );
        m.setFont(font);
        QAction *scaleT = m.addAction("Set scales");
        QAction *what = m.exec(event->globalPos());
        if (what == scaleT)
        {
            this->showDialogScaleY();
            update();
        }
}

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
EnergyDraggable::EnergyDraggable(TransmissionView *v,QGraphicsItem *parent)
: QGraphicsItem(parent), view(v)
{
    //TODO: how do I control the width of this draggable line?
    setCursor(Qt::SizeVerCursor);
    penHover.setStyle(Qt::DashLine);
    penHover.setWidth(v->lineWidth);
//    penHover.setStyle(Qt::DotLine);
    penHover.setColor(Qt::green);
//    pen.setStyle(Qt::DashLine);
    pen.setStyle(Qt::DotLine);
    pen.setColor(Qt::green);
    penForPainter.setColor(Qt::green);
    pen.setWidth(v->lineWidth);

    setCursor(Qt::SizeHorCursor);
    setFlag(QGraphicsItem::ItemIsMovable,true);
    setFlag(QGraphicsItem::ItemIsSelectable,true);
    setZValue(999);
    setAcceptHoverEvents ( true );
}

QVariant EnergyDraggable::itemChange(GraphicsItemChange change, const QVariant & value)
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
            double newE = pos().x();
            QRectF vp = view->sceneRect();//
            newE=view->Emin+(view->Emax-view->Emin)*newE/vp.width();
            view->model->set_Energy(newE);
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

TransmissionWidget::TransmissionWidget(PhysicalModel *model, QWidget *parent)
: QGroupBox(parent)
{
    setTitle(tr("Transmission T(E)"));
    QVBoxLayout *vl = new QVBoxLayout();
    transmissionView = new TransmissionView(model,this);
    vl->addWidget(transmissionView);

    QHBoxLayout *hl = new QHBoxLayout();
    QToolButton *reset = new QToolButton(this);
    reset->setIcon(QIcon(":/images/player_play.png"));
    reset->adjustSize();//QPushButton(tr("Close"));
//    QPushButton *reset = new QPushButton(tr("&Resize"));
    connect(reset,SIGNAL(clicked()),transmissionView,SLOT(resizePicture()));

//    bRunTE = new QPushButton(tr("Run "));
//    connect(bRunTE,SIGNAL(clicked()),this,SLOT(slotRunWP()));

    QLabel *lTtext= new QLabel(this);
    lTtext->setText(tr("T="));
    QLabel *lT= new MyLabel("",this);
    lT->setTextFormat(Qt::AutoText);
    connect(model, SIGNAL(signalTransmissionChanged(double)), lT, SLOT(setNum(double)));

    QLabel *lEtext= new QLabel(this);
    lEtext->setText(tr("E="));

    QLabel *lE = new MyLabel("",this);

//    hl->addWidget(bRunTE);
    hl->addWidget(reset);
    hl->addWidget(lEtext);
    hl->addWidget(lE);
    hl->addWidget(lTtext);
    hl->addWidget(lT);
    connect(model, SIGNAL(signalEnergyChanged(double)), lE, SLOT(setNum(double)));
    hl->addStretch();

    QCheckBox *ch=new QCheckBox(tr("Erase"),this);
    ch->setCheckState(Qt::Checked);//Qt::Unchecked);
    connect(ch, SIGNAL(stateChanged(int)), this, SLOT(slotErase(int))); //???
    hl->addWidget(ch);

    QToolButton *buttonClose = new QToolButton(this);
    buttonClose->setIcon(QIcon(":/images/erase.png"));
    buttonClose->adjustSize();//QPushButton(tr("Close"));
//    hl->addStretch();
    hl->addWidget(buttonClose);
    connect(buttonClose,SIGNAL(clicked()),this,SLOT(hide()),Qt::QueuedConnection); //???

    vl->addLayout(hl);
    setLayout(vl);
}
void TransmissionWidget::slotErase(int j)
{
    if(transmissionView->Erase==true)
        transmissionView->Erase=false;
    else transmissionView->Erase=true;
}
void TransmissionWidget::slotRunTE()
{
 //   bRunTE->setText("STOP");
 //   disconnect(bRunTE, SIGNAL(clicked()), this, SLOT(slotRunTE()));
 //   breakStatus.onButton(bRunTE);
    transmissionView->resizePicture();
 //   breakStatus.noButton(bRunTE);
 //   bRunTE->setText("RUN ");
 //   connect(bRunTE, SIGNAL(clicked()), this, SLOT(slotRunTE()));
}
