#include "WaveFunctionView.h"
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
//#include "ScalesUx.h"
#include "ScalePsin.h"

WaveFunctionView::WaveFunctionView(PhysicalModel *m, QWidget *parent)
: QGraphicsView(parent), model(m), gbScaleXY(0), 
gbLevNum(0), lineh(0), linev(0),
xmin(-1.), xmax(10.),dx(0.01), 
nMax(4),nMin(0),hn(1),
psiMax(1.), psiMin(-1.), gbVPsi(0),bgR(0),dialogScalePsin(0)
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

    //connect(model,SIGNAL(signalPotentialChanged()),this,SLOT(slotUChanged()));
    connect(model,SIGNAL(signalEboundChanged()),this,SLOT(slot_Psi_n_of_x()));
//    connect(model,SIGNAL(signalScalesUChanged()),this,SLOT(resizePicture()));
    connect(model,SIGNAL(signalScalePsinChanged()),this,SLOT(resizePicture()));
    setScalesFromModel();
    resizePicture();
}
void WaveFunctionView::resizePicture()
{       
    ScalePsinParameters tp = model->getScalePsinParam();
    dx=tp.Hx;
    xmin=tp.Xmin;
    xmax=tp.Xmax;
    psiMax=tp.Psinmax;
    psiMin=tp.Psinmin;
    setViewportMapping();
    slot_Psi_n_of_x();
}

void WaveFunctionView::setScalesFromModel()
{
    QPair<double,double> xmin_xmax = model->getXminXmax();
    xmin = xmin_xmax.first;
    xmax = xmin_xmax.second;
    ScalePsinParameters tp;
    tp.Hx = this->dx;
    tp.Xmin = xmin;
    tp.Xmax = xmax;
    tp.Psinmin = psiMin;
    tp.Psinmax = psiMax;
    model->setScalePsinParam(tp);
}

void WaveFunctionView::setViewportMapping()
{
    QRectF vp = QRectF(QPointF(this->xmin,this->psiMin),QPointF(this->xmax,this->psiMax));

    QRectF sr = scene()->sceneRect();
    
    QRectF a = QRectF(this->viewport()->rect());
    if (vp != sr||a != a_old)
    {
        scene()->setSceneRect(vp);

        QRectF b = vp; //scene()->sceneRect();
 //       QRectF a = QRectF(this->viewport()->rect());
        qreal m11 = a.width() / b.width();
        qreal m22 = - a.height() / b.height();
        qreal dx = - m11 * a.x();
        qreal dy = - m22 * (a.y() + a.height());
        QMatrix m(m11,0,0,m22,dx,dy);
        this->setMatrix(m);
        scene()->update(scene()->sceneRect());
        sr = scene()->sceneRect();
        a_old=a; 
    }
    update();
} 
void WaveFunctionView::resizeEvent(QResizeEvent*) 
{
    setViewportMapping();
}

void WaveFunctionView::mouseMoveEvent(QMouseEvent *e)
{
    QPointF f = mapToScene(e->pos());
    emit(infoMouseMovedTo(f));
    QGraphicsView::mouseMoveEvent(e);
}
void WaveFunctionView::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
}
void WaveFunctionView::scaleView(qreal scaleFactor)
{
    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.007 || factor > 1000)
        return;

    scale(scaleFactor, scaleFactor);
}

void WaveFunctionView::keyPressEvent(QKeyEvent *event)
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

void WaveFunctionView::slot_Psi_n_of_x()
{
    static const QColor colorForIds[6] = {
        Qt::red, Qt::green, Qt::black, Qt::cyan, Qt::magenta, Qt::yellow
    };
    const int size_colorForIds = sizeof(colorForIds)/sizeof(colorForIds[0]);

    QVector<double> Ebound = model->getEn();
    int number_of_levels = Ebound.size();
        model->set_LevelNmin(this->nMin);
        model->set_LevelNmax(this->nMax);
    int nmin=model->get_LevelNmin();
    int nmax=model->get_LevelNmax();
    for ( QMap<int,CoordinateDistribution*>::iterator i = curves.begin();  i != curves.end();   ++i)
    {
        int n = i.key();
        if (n >= number_of_levels)
        {
            removeCurve(n);

        }
        if(n>nmax&&nmax<number_of_levels-1) removeCurve(n);
        if(n<nmin&&nmin<number_of_levels-1) removeCurve(n);
    }

/*    for (int n = 0; n < number_of_levels; ++n)
    {
        if (! curves[n])
        {
            curves[n] = new CoordinateDistribution(curves[n],this);
            scene()->addItem(curves[n]);
        }
    }
*/
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
    int npoints;//=501;
    QVector<double> waveFunction;
    QPolygonF psi;
    npoints=1+(xmax-xmin)/this->dx;
    psi.resize(npoints);
    waveFunction.resize(npoints);
//    double dx = (xmax-xmin)/(npoints-1);
/*    if(nmin!=this->nMin||nmax!=this->nMax)//||hnM!=this->hn) 
    {
        model->set_LevelNmin(this->nMin);
        model->set_LevelNmax(this->nMax);
//        model->set_LevelHn(this->hn);
    }
    else
    {
    nmin=model->get_LevelNmin();
    nmax=model->get_LevelNmax();
    if(nmax>number_of_levels-1) nmax=number_of_levels-1;
    if(nmin>nmax) return;
    }
    */
//    for (int n = nmin; n <= nmax; n+= this->hn)
    if(bgR!=0) viewWF=bgR->checkedId();
    else viewWF=0;

    for (int n = this->nMin; n <= this->nMax; n+= this->hn)
//    for (int n = 0; n < number_of_levels; ++n)
    {
        if(n>number_of_levels-1) break;    
        waveFunction = model->getPsiOfX(Ebound[n],xmin,xmax,npoints,viewWF);
            for (int i=0; i < npoints; i++)
            {
                double x = xmin + dx*i;
                double y = waveFunction[i];
                psi[i]  = QPointF(x, y);
            }
//            curves[n]=psi;
            setCurve(n,psi,colorForIds[n % size_colorForIds]);
    }
    update();
}
void WaveFunctionView::setCurve(int id,const QPolygonF & curve, const QPen& pen)
{

    if (curves[id])
    {
        curves[id]->setPolygon(curve);
    }
    else
    {
        CoordinateDistribution *c = new CoordinateDistribution(curve,this);
        scene()->addItem(c);
        curves[id] = c;
    }
    curves[id]->setPen(pen);
//    repaint();
    update();
}

void WaveFunctionView::removeCurve(int id)
{
    scene()->removeItem(curves[id]);
    delete curves[id];
    curves[id] = 0; //this is dangerous: curves.remove(id);
    update();
//    repaint();
}

void CoordinateDistribution::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    painter->setPen( pen() );
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPolyline(polygon().data(),polygon().size());
}

void WaveFunctionView::showDialogLevNum()
{   
    if (!gbLevNum) 
    {
        gbLevNum = new QGroupBox(this);
        gbLevNum->setWindowTitle("Scales for plots Psi(x)");
        gbLevNum->setWindowFlags(Qt::Window);
        gbLevNum->setFont(QFont("Serif", 12, QFont::Bold )); 
        QVBoxLayout *vl = new QVBoxLayout;
        nMin.setDisplay(("nmin"),("lower level number"),vl);
        nMax.setDisplay(("nmax"),("upper level number"),vl);
        hn.setDisplay(("hn"),("step between the level numbers"),vl);
        gbLevNum->setLayout(vl);
    }
    gbLevNum->show(); 
    gbLevNum->raise();
    gbLevNum->setFocus();
}
/*void WaveFunctionView::showDialogScaleY()
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
*/
void WaveFunctionView::showDialogScaleY()
{   
    if (!dialogScalePsin) 
    {
        dialogScalePsin = new ScalePsin(this);
        dialogScalePsin->setModel(model);
    }
    dialogScalePsin->show(); 
    dialogScalePsin->activateWindow();
    dialogScalePsin->setFocus();
}

void WaveFunctionView::showDialogViewPsiX()
{
    if (!gbVPsi) 
    {

        gbVPsi = new QGroupBox(this);//"Wavepacket definition:");
        gbVPsi->setFont(QFont("Serif", 12, QFont::Bold )); 
        gbVPsi->setWindowFlags(Qt::Window);
        gbVPsi->setWindowTitle("View of psi(x)");
        QVBoxLayout *vl=new QVBoxLayout;

        rad1= new QRadioButton("real(psi(x))");
        rad2= new QRadioButton("imag(psi(x))");
        rad3= new QRadioButton("|psi(x)|^2");
        vl->addWidget(rad1);
        vl->addWidget(rad2);
        vl->addWidget(rad3);
        if (!bgR)
        {
            bgR= new QButtonGroup(gbVPsi);
            bgR->setExclusive(true);
            bgR->addButton(rad1,0);
            bgR->addButton(rad2,1);
            bgR->addButton(rad3,2);
            bgR->button(0)->setChecked(true);
        }
        gbVPsi->setLayout(vl);

    }
    gbVPsi->show(); 
    gbVPsi->raise();//activateWindow();
    gbVPsi->setFocus();

}
void WaveFunctionView::contextMenuEvent(QContextMenuEvent *event)
{
        QMenu m;
        QAction *levNum = m.addAction("Level numbers");
        QAction *viewPsiX = m.addAction("Real, imag or |psi(x)|^2?");
        QAction *scalePsi = m.addAction("Scales");
        QAction *what = m.exec(event->globalPos());
        if (what == levNum)
        {
            this->showDialogLevNum();
            update();
        }
        if (what == scalePsi)
        {
            this->showDialogScaleY();
            update();
        }
        if (what == viewPsiX)
        {
            this->showDialogViewPsiX();
            update();
        }
        event->accept();
}
/*void CoordinateDistribution::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    if (event->buttons() & Qt::RightButton)
    {
        QMenu m;
        QAction *levNum = m.addAction("Numbers of levels");
        QAction *viewPsiX = m.addAction("Real, imag or |psi(x)|^2?");
        QAction *scalePsi = m.addAction("Scales");
//        QAction *wavepacketAction = m.addAction("Set wavepacket");
        QAction *what = m.exec(event->screenPos());
        if (what == levNum)
        {
            view->showDialogLevNum();
            update();
        }
        if (what == scalePsi)
        {
            view->showDialogScaleY();
            update();
        }
        if (what == viewPsiX)
        {
            view->showDialogViewPsiX();
            update();
        }
        
        event->accept();
#if 0
        QPainterPath p = shape();
        QPointF v = event->pos();
        QRectF r = QRectF(v,v).adjusted(-0.1,-0.1,0.1,0.1);

        bool i = p.intersects(r);
        if (i)
        {
            QMenu m;
            QAction *scaleX = m.addAction("Set something");
            QAction *scaleY = m.addAction("Set scales");
            QAction *what = m.exec(event->screenPos());
            if (what == scaleY)
            {
                view->scaleY();
            }
               update();// repaint();
            event->accept();
            return;
        }
#endif
    }
}
*/