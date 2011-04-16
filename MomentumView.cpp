#include "MomentumView.h"
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
#include "ScalePhin.h"

MomentumView::MomentumView(PhysicalModel *m, QWidget *parent)
: QGraphicsView(parent), model(m), dialogScalePhin(0),lineh(0),linev(0)//,
{
    widthLine = 3;;
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
    connect(model,SIGNAL(signalEboundChanged()),this,SLOT(slot_Phi_n_of_k()));
    connect(model,SIGNAL(signalScalePhinChanged()),this,SLOT(resizePicture()));
    connect(model,SIGNAL(signalLevelNumberChanged()),this,SLOT(resizePicture()));
    resizePicture();
}

MomentumView::~MomentumView()
{
    disconnect(this, 0, 0, 0);
    if (!dialogScalePhin) delete dialogScalePhin;
    for (QMap<int,MomentumDistribution*>::iterator i = curves.begin(); i != curves.end(); ++i)
    {
        int n = i.key();
        removeCurve(n);
    }
    if (!lineh) delete lineh;
    if (!linev) delete linev;
}

void MomentumView::resizePicture()
{
    ScalePhinParameters tp = model->getScalePhinParam();
    dk=tp.Hk;
    kmin=tp.Kmin;
    kmax=tp.Kmax;
    phiMax=tp.Phinmax;
    phiMin=tp.Phinmin;
    setViewportMapping();
    slot_Phi_n_of_k();
}

void MomentumView::setViewportMapping()
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
void MomentumView::resizeEvent(QResizeEvent *e) 
{
        resizePicture();
//    setViewportMapping();
}

void MomentumView::mouseMoveEvent(QMouseEvent *e)
{
    QPointF f = mapToScene(e->pos());
    emit(infoMouseMovedTo(f));
    QGraphicsView::mouseMoveEvent(e);
}
void MomentumView::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
    event->accept();
}
void MomentumView::scaleView(qreal scaleFactor)
{
    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.007 || factor > 1000)
        return;

    scale(scaleFactor, scaleFactor);
}
void MomentumView::clearAll()
{
    
        for ( QMap<int,MomentumDistribution*>::iterator i = curves.begin();  i != curves.end();   ++i)
    {
        int m = i.key();
            removeCurve(m);
    }
}
void MomentumView::keyPressEvent(QKeyEvent *event)
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
    case Qt::Key_Enter:
        break;
    }
    QGraphicsView::keyPressEvent(event);
}

void MomentumView::slot_Phi_n_of_k()
{
    QRectF vp = scene()->sceneRect();
    QRect a = QRect(this->viewport()->rect());
    QPen p;
    p.setWidthF(widthLine);
    p.setJoinStyle(Qt::BevelJoin);
    p.setCapStyle(Qt::RoundCap);
    p.setColor(Qt::black);
    static const QColor colorForIds[12] = {
            Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta, Qt::yellow,
        Qt::darkRed, Qt::darkGreen, Qt::darkBlue, Qt::darkCyan, Qt::darkMagenta, Qt::darkYellow
    };

    const int size_colorForIds = sizeof(colorForIds)/sizeof(colorForIds[0]);

    QVector<double> Ebound = model->getEn();
    int number_of_levels = Ebound.size();
    if(number_of_levels==0) return;
    LevelNumberParameters wp = model->getLevelNumberParameters();
    int nMin=wp.nmin;
    int nMax=wp.nmax;
    int hn=wp.hn;  
    for ( QMap<int,MomentumDistribution*>::iterator i = curves.begin();  i != curves.end();   ++i)
    {
        int n = i.key();
        if (n >= number_of_levels)
        {
            removeCurve(n);
        }
        if(n>nMax&&nMax<number_of_levels-1) removeCurve(n);
        if(n<nMin&&nMin<number_of_levels-1) removeCurve(n);
    }

    if(!linev)
    {
        lineh = new QGraphicsLineItem();
        linev = new QGraphicsLineItem();
        linev->setPen(p);
        lineh->setPen(p);
        linev->setLine(vp.width()*(-kmin)/(kmax-kmin), 0, vp.width()*(-kmin)/(kmax-kmin),vp.height() );
        lineh->setLine(0,vp.height()*(-phiMin)/(phiMax-phiMin),vp.width(),vp.height()*(-phiMin)/(phiMax-phiMin));
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

    int npoints;//=500;
    QVector<double> waveFunction;
    QPolygonF phi;
    npoints=1+(kmax-kmin)/this->dk;
    phi.resize(npoints);
    waveFunction.resize(npoints);
    p.setWidthF(widthLine);
    for (int n = nMin; n <= nMax; n+= hn)
    {
            if(n>number_of_levels-1) break;    
            waveFunction = model->getPhiOfk(Ebound[n],kmin,kmax,npoints);
            for (int i=0; i < npoints; i++)
            {
//                double x = kmin + dk*i;
//                double y = waveFunction[i];
//                phi[i]  = QPointF(x, y);
            double x = (i*vp.width())/(npoints-1);
            double y =vp.height()*(waveFunction[i]-phiMin)/(phiMax-phiMin);
            phi[i]  = QPointF(x, y);
            }
        p.setColor(colorForIds[n % size_colorForIds]);
        setCurve(n, phi, p);
//            setCurve(n,phi,colorForIds[n % size_colorForIds]);
    }
    update();
}

void MomentumView::setCurve(int id,const QPolygonF & curve, const QPen& pen)
{

    if (curves[id])
    {
        curves[id]->setPolygon(curve);
    }
    else
    {
        MomentumDistribution *c = new MomentumDistribution(curve,this);
        scene()->addItem(c);
        curves[id] = c;
    }
    curves[id]->setPen(pen);
//    repaint();
    update();
}

void MomentumView::removeCurve(int id)
{
    scene()->removeItem(curves[id]);
    delete curves[id];
    curves[id] = 0; //this is dangerous: curves.remove(id);
    update();
//    repaint();
}

void MomentumDistribution::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    painter->setPen( pen() );
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPolyline(polygon().data(),polygon().size());
}
void MomentumView::showDialogScale()
{   
    if (!dialogScalePhin) 
    {
        dialogScalePhin = new ScalePhin(this);
        dialogScalePhin->setModel(model);
    }
    dialogScalePhin->show(); 
    dialogScalePhin->activateWindow();
    dialogScalePhin->setFocus();
}

void MomentumView::contextMenuEvent(QContextMenuEvent *event)
{
        QMenu m;
        QFont font( "Serif", 10, QFont::DemiBold );
        m.setFont(font);
        QAction *scalePsi = m.addAction("Set scales");
        QAction *what = m.exec(event->globalPos());
        if (what == scalePsi)
        {
            this->showDialogScale();
            update();
        }
}

MomentumViewWidget::MomentumViewWidget(PhysicalModel *model, QWidget *parent)
: QGroupBox(parent)
{
    setTitle(tr("Momentum distribution: bound states"));

    QVBoxLayout *vl = new QVBoxLayout();
    MomentumView *momentumView = new MomentumView(model);
    vl->addWidget(momentumView);

    QHBoxLayout *hl = new QHBoxLayout();
//    QPushButton * reset = new QPushButton("&Resize");	
//    QPushButton *buttonClose = new QPushButton(tr("Close"));
    QToolButton *buttonClose = new QToolButton(this);
    buttonClose->setIcon(QIcon("images/erase.png"));
    buttonClose->adjustSize();//QPushButton(tr("Close"));
    hl->addStretch();
    hl->addWidget(buttonClose);

//    hl->addWidget(reset);		

//    connect(reset,SIGNAL(clicked()),momentumView,SLOT(resizePicture()));
    connect(buttonClose,SIGNAL(clicked()),this,SLOT(hide()),Qt::QueuedConnection); //???
    vl->addLayout(hl);

    setLayout(vl);
}