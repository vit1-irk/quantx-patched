#include "WavePacketKView.h"
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


WavePacketKView::WavePacketKView(PhysicalModel *m, QWidget *parent)
: QGraphicsView(parent), model(m),lineh(0),linev(0),
dialogScaleWPK(0)
{
    lineWidth = 3;

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
    setMinimumSize(160, 220);
    connect(model,SIGNAL(signalScaleWPKChanged()),this,SLOT(resizePicture()));
    connect(model,SIGNAL(signalTimeChanged(double)),this,SLOT(slot_WavePacket_of_t()));
    connect(model,SIGNAL(signalWidthChanged()),this,SLOT(resizePicture()));

    resizePicture();
}
/*WavePacketKView::~WavePacketKView()
{
    disconnect(this, 0, 0, 0);
    if (!dialogScaleWPK) delete dialogScaleWPK;
    for (QMap<int,MomentumDistributionCurve*>::iterator i = curves.begin(); i != curves.end(); ++i)
    {
        int n = i.key();
        removeCurve(n);
    }
    if (!lineh) delete lineh;
    if (!linev) delete linev;
}
*/
void WavePacketKView::resizePicture()
{
    ScaleWPKParameters tp = model->getScaleWPKParam();
    dk=tp.Hk;
    kmin=tp.Kmin;
    kmax=tp.Kmax;
    phiMax=tp.WPKmax;
    phiMin=tp.WPKmin;
    setViewportMapping();
    slot_WavePacket_of_t();
}

void WavePacketKView::setViewportMapping()
{
    QRectF a = QRectF(this->viewport()->rect());
    double rxmin=0;
    double rxmax=a.width();//npoints-1;
    double rpsiMin=0;
    double rpsiMax=a.height();//a.height();
    QRectF b = QRectF(QPointF(rxmin,rpsiMin),QPointF(rxmax,rpsiMax));
    scene()->setSceneRect(b);
        qreal m11 = a.width() / b.width();
        qreal m22 = - a.height() / b.height(); 
        qreal dx = - m11 * a.x();
        qreal dy = - m22 * (a.y() + a.height());
        QMatrix m(m11,0,0,m22,dx,dy);
        this->setMatrix(m);
        scene()->update(scene()->sceneRect());
     QRectF   sr = scene()->sceneRect();
    update();
/*
    QRectF vp = QRectF(QPointF(this->kmin,this->phiMin),QPointF(this->kmax,this->phiMax));

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
    update();*/
} 
void WavePacketKView::resizeEvent(QResizeEvent*) 
{
    this->resizePicture();
//    setViewportMapping();
}

void WavePacketKView::mouseMoveEvent(QMouseEvent *e)
{
    QPointF f = mapToScene(e->pos());
    emit(infoMouseMovedTo(f));
    QGraphicsView::mouseMoveEvent(e);
}
void WavePacketKView::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
}
void WavePacketKView::scaleView(qreal scaleFactor)
{
    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.007 || factor > 1000)
        return;

    scale(scaleFactor, scaleFactor);
}

void WavePacketKView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
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

void WavePacketKView::slot_WavePacket_of_t()
{
    if (! isVisible()) return;
    QRectF vp = scene()->sceneRect();
    QRect a = QRect(this->viewport()->rect());
    QPen p;
    SettingParameters ts;  
    ts=model->getSettingParameters();
    lineWidth=ts.lineWidth;
    p.setWidthF(lineWidth);
    p.setJoinStyle(Qt::BevelJoin);
    p.setCapStyle(Qt::RoundCap);
    p.setColor(Qt::black);

    QVector<double> Ebound = model->getEn();
    int number_of_levels = Ebound.size();
    if(number_of_levels==0) return;    
    int iwp=model->get_type_of_WP();
    if(iwp==1) return;
    for ( QMap<int,MomentumDistributionCurve*>::iterator i = curves.begin();  i != curves.end();   ++i)
    {
        int n = i.key();
        removeCurve(n);
    }
    if(!linev)
    {
        lineh = new QGraphicsLineItem();
        linev = new QGraphicsLineItem();
        linev->setPen(p);
        lineh->setPen(p);
        linev->setLine(vp.width()*(-kmin)/(kmax-kmin), 0, vp.width()*(-kmin)/(kmax-kmin),vp.height() );
        lineh->setLine(0,vp.height()*(-phiMin)/(phiMax-phiMin),vp.width(),vp.height()*(-phiMin)/(phiMax-phiMin));
/*        linev->setLine(0., phiMin, 0., phiMax);
        lineh->setLine(kmin,0.,kmax,0.);*/
        scene()->addItem(lineh);
        scene()->addItem(linev);
    }
    else
    {
        linev->setPen(p);
        lineh->setPen(p);
        linev->setLine(vp.width()*(-kmin)/(kmax-kmin), 0, vp.width()*(-kmin)/(kmax-kmin),vp.height() );
        lineh->setLine(0,vp.height()*(-phiMin)/(phiMax-phiMin),vp.width(),vp.height()*(-phiMin)/(phiMax-phiMin));
//        linev->setLine(0., phiMin, 0., phiMax);
//        lineh->setLine(kmin,0.,kmax,0.);
    }
    int npoints;//=501;
    p.setColor(Qt::darkCyan);
    QVector<double> waveFunction;
    QPolygonF psi;
    npoints=1+(kmax-kmin)/this->dk;
    psi.resize(npoints);
    waveFunction.resize(npoints);
    {
            waveFunction = model->getPsiOfKT(kmin, kmax, npoints);
            for (int i=0; i < npoints; i++)
            {
//                double kk = kmin + dk*i;
//                double y = waveFunction[i];
                double x = (i*vp.width())/(npoints-1);
                double y =vp.height()*(waveFunction[i]-phiMin)/(phiMax-phiMin);
                psi[i]  = QPointF(x, y);
//                psi[i]  = QPointF(kk, y);
            }
//            setCurve(0,psi,QPen(Qt::darkCyan));
        setCurve(0, psi, p);
            //dima qApp->processEvents(QEventLoop::AllEvents);
    }
}


void WavePacketKView::setCurve(int id,const QPolygonF & curve, const QPen& pen)
{

    if (curves[id])
    {
        curves[id]->setPolygon(curve);
    }
    else
    {
        MomentumDistributionCurve *c = new MomentumDistributionCurve(curve,this);
        scene()->addItem(c);
        curves[id] = c;
    }
    curves[id]->setPen(pen);
//    repaint();
    update();
}

void WavePacketKView::removeCurve(int id)
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

void MomentumDistributionCurve::paint(QPainter * painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setPen( pen() );
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPolyline(polygon().data(),polygon().size());
}
void WavePacketKView::showDialogScaleY()
{   
    if (!dialogScaleWPK) 
    {
        dialogScaleWPK = new ScaleWPK(this);
        dialogScaleWPK->setModel(model);
    }
    dialogScaleWPK->show(); 
    dialogScaleWPK->activateWindow();
    dialogScaleWPK->setFocus();
}
void WavePacketKView::contextMenuEvent(QContextMenuEvent *event)
{
        QMenu m;
        QFont font( "Serif", 10, QFont::DemiBold );
        m.setFont(font);
        QAction *scalePsi = m.addAction(m.tr("Scales"));
        QAction *what = m.exec(event->globalPos());
        if (what == scalePsi)
        {
            this->showDialogScaleY();
            update();
        }
        event->accept();
}
WavePacketKWidget::WavePacketKWidget(PhysicalModel *model, QWidget *parent)
: QGroupBox(parent)
{
    setTitle(tr("Momentum distribution: time development"));
    QVBoxLayout *vl = new QVBoxLayout();
    wavePacketKView = new WavePacketKView(model,this);
    vl->addWidget(wavePacketKView);

    QHBoxLayout *hl = new QHBoxLayout();
//    bRunPsiKT = new QPushButton(tr("Run "));	
//    connect(bRunPsiKT,SIGNAL(clicked()),this,SLOT(slotRunWP()));
    QString Phi=QChar(0x03A6);
    QLabel *ltext= new QLabel(this);
    ltext->setText(tr("time:"));
    QLabel *ltime= new QLabel(this);
    connect(model, SIGNAL(signalTimeChanged(double)), ltime, SLOT(setNum(double)));
    hl->addWidget(ltext);
    hl->addWidget(ltime);

    QToolButton *buttonClose = new QToolButton(this);
//     QPushButton *buttonClose = new QPushButton(tr("Close"));
    buttonClose->setIcon(QIcon("images/erase.png"));
    buttonClose->adjustSize();//QPushButton(tr("Close"));
    connect(buttonClose,SIGNAL(clicked()),this,SLOT(hide()),Qt::QueuedConnection); //???

    hl->addStretch();
    hl->addWidget(buttonClose);

    vl->addLayout(hl);
    setLayout(vl);
}
/*void WavePacketKWidget::slotRunWP()
{
    bRunPsiKT->setText(tr("STOP"));
    disconnect(bRunPsiKT, SIGNAL(clicked()), this, SLOT(slotRunWP()));
    breakStatus.onButton(bRunPsiKT);
    wavePacketKView->resizePicture();
    breakStatus.noButton(bRunPsiKT);
    bRunPsiKT->setText(tr("RUN "));
    connect(bRunPsiKT, SIGNAL(clicked()), this, SLOT(slotRunWP()));
}
*/
