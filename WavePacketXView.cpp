#include "WavePacketXView.h"
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

WavePacketXView::WavePacketXView(PhysicalModel *m, QWidget *parent)
: QGraphicsView(parent), model(m), butTime(0), dialogTime(0), gbScaleXY(0), gbWP(0), gbWPl(0), gbWPr(0), gbVPsi(0), bgRVF(0),bgR(0),lineh(0), linev(0),
xmin(-1.), xmax(10.), nmaxWP(100),nminWP(0),hnWP(1),
//time(0),htime(0.1), tmin(0), tmax(100.),
wpE_lo(5.), wpE_hi(10.), wpN(30), psiMax(.2), psiMin(-.1),viewWF(0),need_build_WavePacket(true)
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
}
void WavePacketXView::resizePicture()
{
    setViewportMapping();
    slot_WavePacket_of_t();
}
void WavePacketXView::setViewportMapping()
{
    QRectF vp = QRectF(QPointF(this->xmin,this->psiMin),QPointF(this->xmax,this->psiMax));

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
void WavePacketXView::resizeEvent(QResizeEvent *) 
{
    setViewportMapping();
}

void WavePacketXView::mouseMoveEvent(QMouseEvent *e)
{
    QPointF f = mapToScene(e->pos());
    emit(infoMouseMovedTo(f));
    QGraphicsView::mouseMoveEvent(e);
}
void WavePacketXView::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
}
void WavePacketXView::scaleView(qreal scaleFactor)
{
    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.007 || factor > 1000)
        return;

    scale(scaleFactor, scaleFactor);
}

void WavePacketXView::keyPressEvent(QKeyEvent *event)
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

void WavePacketXView::slot_rebuildWavePacket()
{
    need_build_WavePacket=true;
}
void WavePacketXView::slot_WavePacket_of_t()
{
    for ( QMap<int,CoordinateDistributionCurve*>::iterator i = curves.begin();  i != curves.end();   ++i)
    {
        int n = i.key();
        removeCurve(n);
    }
    if(!linev)
    {
        lineh = new QGraphicsLineItem();
        linev = new QGraphicsLineItem();
        linev->setLine(0., psiMin, 0., psiMax);
        lineh->setLine(xmin,0.,xmax,0.);
        scene()->addItem(lineh);
        scene()->addItem(linev);
    }
    else
    {
        linev->setLine(0., psiMin, 0., psiMax);
        lineh->setLine(xmin,0.,xmax,0.);
    }
    int npoints=501;
    QVector<double> waveFunction;
    QPolygonF psi;
    psi.resize(npoints);
    waveFunction.resize(npoints);
    double dx = (xmax-xmin)/(npoints-1);
    if(bgRVF!=0) viewWF=bgRVF->checkedId();
    else viewWF=2;
    TimeParameters tp=model->getTimeParam(); 
    this->time=tp.time;
    this->htime=tp.ht;
    this->tmin=tp.tmin;
    this->tmax=tp.tmax;
    double tt=this->time+this->htime;
    if(tt>=this->tmax) 
    { 
        tt=this->tmin;
        this->time=tt;
    }
    if(tt<this->tmin) 
    { 
        tt=this->tmax;
        this->time=tt;
    }
         for (double t=this->time; t>=tp.tmin&&t<=tp.tmax; t+=htime)
         {
            waveFunction = model->getPsiOfXT(t, xmin, xmax, npoints, viewWF);
//            tp.time=t;
//            model->setTimeParam(tp);
            for (int i=0; i < npoints; i++)
            {
                double x = xmin + dx*i;
                double y = waveFunction[i];
                psi[i]  = QPointF(x, y);
            }
            setCurve(0,psi,QPen(Qt::darkCyan));
            tp=model->getTimeParam();
            if((htime!=tp.ht)||(tmax!=tp.tmax)||(tmin!=tp.tmin))
            {
               htime=tp.ht;
               tmax=tp.tmax;
               tmin=tp.tmin;
               time=tp.time;
            }
            if (getBreakStatus(0)) 
            {
                return;
            }
    }
}

void WavePacketXView::setCurve(int id,const QPolygonF & curve, const QPen& pen)
{
 
    if (curves[id])
    {
        curves[id]->setPolygon(curve);
    }
    else
    {
        CoordinateDistributionCurve *c = new CoordinateDistributionCurve(curve,this);
        scene()->addItem(c);
        curves[id] = c;
    }
    curves[id]->setPen(pen);
    update();
}

void WavePacketXView::removeCurve(int id)
{
    scene()->removeItem(curves[id]);
    delete curves[id];
    curves[id] = 0; //this is dangerous: curves.remove(id);
    update();
}

void CoordinateDistributionCurve::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget *)
{
    (void)option;
    painter->setPen( pen() );
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPolyline(polygon().data(),polygon().size());
}
void WavePacketXView::showDialogViewPsiX()
{
    if (!gbVPsi) 
    {

        gbVPsi = new QGroupBox(this);//"Wavepacket definition:");
        gbVPsi->setFont(QFont("Serif", 12, QFont::Bold )); 
        gbVPsi->setWindowFlags(Qt::Window);
        gbVPsi->setWindowTitle("View of psi(x)");
        QVBoxLayout *vl=new QVBoxLayout;

        QRadioButton *rad1= new QRadioButton("real(psi(x))");
        QRadioButton *rad2= new QRadioButton("imag(psi(x))");
        QRadioButton *rad3= new QRadioButton("|psi(x)|^2");
        vl->addWidget(rad1);
        vl->addWidget(rad2);
        vl->addWidget(rad3);

        if (!bgRVF)
        {
            bgRVF= new QButtonGroup(gbVPsi);
            bgRVF->setExclusive(true);
            bgRVF->addButton(rad1,0);
            bgRVF->addButton(rad2,1);
            bgRVF->addButton(rad3,2);
            bgRVF->button(2)->setChecked(true);
            gbVPsi->setLayout(vl);
        }

    }
    gbVPsi->show(); 
    gbVPsi->raise();//activateWindow();
    gbVPsi->setFocus();

}


void WavePacketXView::showDialogScaleY()
{   
    if (!gbScaleXY) 
    {
        gbScaleXY = new QGroupBox(this);
        gbScaleXY->setWindowTitle("Scales for plots Psi(x)");
        gbScaleXY->setWindowFlags(Qt::Window);
        gbScaleXY->setFont(QFont("Serif", 12, QFont::Bold )); 
        QVBoxLayout *vl = new QVBoxLayout;
        psiMin.setDisplay(("PsiMin"),("Y-scale for wave function plots"),vl);
        psiMax.setDisplay(("PsiMax"),("Y-scale for wave function plots"),vl);
        this->xmin.setDisplay(("xmin"),("lower bond of x-interval"),vl);
        this->xmax.setDisplay(("xmax"),("upper bond of x-interval"),vl);
        gbScaleXY->setLayout(vl);
    }
    gbScaleXY->show(); 
    gbScaleXY->raise();
    gbScaleXY->setFocus();
}
/*void  WavePacketXView::slotSetTime()
{
    if (!dialogTime)
    {
        dialogTime = new TimeView(this);
        dialogTime->setModel(model);
    }
    dialogTime->show(); 
    dialogTime->activateWindow();
    dialogTime->setFocus();
}
*/
/*void WavePacketXView::showDialogWavePacket()
{
    if (!butTime)
    {
        butTime = new QPushButton(tr("Time")); 
        QVBoxLayout *vl=new QVBoxLayout(this);
        connect(butTime, SIGNAL(clicked()), this, SLOT(slotSetTime()));
        butTime->setLayout(vl);
    }
    butTime->show(); 
    butTime->raise();//activateWindow();
    butTime->setFocus();
        /*
    if (!gbWP) 
    {

        gbWP = new QGroupBox(this);//"Wavepacket definition:");
        gbWP->setFont(QFont("Serif", 12, QFont::Bold )); 
        gbWP->setWindowFlags(Qt::Window);
        gbWP->setWindowTitle("Time and wavepacket definition");
        QVBoxLayout *vl=new QVBoxLayout;
//        {
            this->time.setDisplay(("time"), ("time"),vl);
            this->htime.setDisplay(("delta_t"),("time increment"),vl);
            this->tmin.setDisplay(("tmin"),("lower bound of the time interval"),vl);
            this->tmax.setDisplay(("tmax"),("upper bound of the time interval"),vl);
 //       }
 //       if(!grrb)
 //       {
        grrb=new QGroupBox("Scattering or Bound States?");
        QRadioButton *rad1= new QRadioButton("E>0");
        QRadioButton *rad2= new QRadioButton("E<0");

        bgR= new QButtonGroup(gbWP);
        bgR->setExclusive(true);
        bgR->addButton(rad1,1);
        bgR->addButton(rad2,2);
        bgR->button(2)->setChecked(true);

        QHBoxLayout *hlr=new QHBoxLayout;
        hlr->addWidget(rad1);
        hlr->addWidget(rad2);
        grrb->setLayout(hlr);
        vl->addWidget(grrb);
 //       }
        QPushButton *butWP= new QPushButton( "Wavepacket definition");
        QMenu *menuWP =new QMenu(this);
        menuWP->setFont(QFont("Serif", 12, QFont::Bold )); 
        QAction *wpAction = menuWP->addAction(tr("&Wavepacket E>0"));
        QAction *spAction = menuWP->addAction(tr("&Superposition E<0"));
        connect(wpAction, SIGNAL(triggered()), this, SLOT(defWP()));
        connect(spAction, SIGNAL(triggered()), this, SLOT(defSP()));
        butWP->setMenu(menuWP);
        vl->addWidget(butWP);

        gbWP->setLayout(vl);

    }
    gbWP->show(); 
    gbWP->raise();//activateWindow();
    gbWP->setFocus();
*/
//}

/*void  WavePacketXView::defSP()
{   
    if (!gbWPl) 
    {
            gbWPl=new QGroupBox();
            gbWPl->setWindowTitle("Superposition of wavefunctions E<0");
            this->setFont(QFont("Serif", 12, QFont::Bold ));
            gbWPl->setWindowFlags(Qt::Window);
            QVBoxLayout *vl=new QVBoxLayout;
            this->nminWP.setDisplay(("nmin"),("lower bond of the level number interval>=0"),vl);
            this->nmaxWP.setDisplay(("nmax"),("upper bond of the level number interval<N"),vl);
            this->hnWP.setDisplay(("hn"),("number level increment"),vl);
            gbWPl->setLayout(vl);
    }
    gbWPl->show(); 
    gbWPl->raise();//activateWindow();
    gbWPl->setFocus();
}
void  WavePacketXView::defWP()
{   
    if (!gbWPr) 
    {
          gbWPr=new QGroupBox(this);
        gbWPr->setFont(QFont("Serif", 12, QFont::Bold )); 
        gbWPr->setWindowTitle("Wavepacket definition E>0");
        gbWPr->setWindowFlags(Qt::Window);
        QVBoxLayout *vl=new QVBoxLayout;
        this->wpE_lo.setDisplay(("E_low"),("lower bound of the wp energy, meV"),vl);
        this->wpE_hi.setDisplay(("E_high"),("upper bound of the wp energy, meV"),vl);
        this->wpN.setDisplay(("Nwp"),("the number of wavepacket modes"),vl);
        gbWPr->setLayout(vl);
    }
    gbWPr->show(); 
    gbWPr->raise();//activateWindow();
    gbWPr->setFocus();
}
*/
void CoordinateDistributionCurve::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    if (event->buttons() & Qt::RightButton)
    {
        QMenu m;
        QAction *scalePsi = m.addAction("Scales");
        QAction *wavepacketViewAction = m.addAction("Real, imag or |psi|^2?");
//        QAction *wavepacketAction = m.addAction("Wavepacket");
        QAction *what = m.exec(event->screenPos());
        if (what == scalePsi)
        {
            view->showDialogScaleY();
            update();
        }
//        if (what == wavepacketAction)
//        {
//            view->showDialogWavePacket();
//            update();
//        }
        if (what == wavepacketViewAction)
        {
            view->showDialogViewPsiX();
            update();
        }
        event->accept();
    }
}

