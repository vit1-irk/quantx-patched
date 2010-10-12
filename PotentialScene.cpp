#include <QMessageBox>
#include "PotentialScene.h"
#include "DraggableLine.h"
#include "PhysicalModel.h"
#include <QRectF>
#include <QReadWriteLock>


PotentialScene::PotentialScene(PhysicalModel *_model, QObject * parent)
    : QGraphicsScene(parent), model(_model)
{
    connect(model,SIGNAL(signalPotentialChanged()),this,SLOT(redrawU()));
    connect(model,SIGNAL(signalEboundChanged()),this,SLOT(redrawEn()));
    //connect(this,SIGNAL(draggableLineMoved()),this,SLOT(updatePhysicalModel()));
    redrawU();
    redrawEn();
}

void PotentialScene::redrawU()
{
#if 0
    QList<QGraphicsItem*> hm = this->selectedItems();
    int hmsz = hm.size();
    if (hmsz) return;
#endif

    model->build_U();
    model->XUscales();
    QRectF newSceneRect(
        model->Xmin,
        model->Umin,
        model->Xmax - model->Xmin,
        model->Umax - model->Umin);
    setSceneRect(newSceneRect);

    for (int i = 0; i < linesU.size(); ++i)
    {
        linesU[i]->hide();
        removeItem(linesU[i]);
        delete linesU[i];
    }
    linesU.clear();

    foreach (DraggableLine *v, linesV)
    {
        v->hide();
        removeItem(v);
        delete v;
    }
    linesV.clear();

    int inner_N = model->getN();

    HDraggableLine *lasth = new HDraggableLine(QPointF(-1000,model->get_Ui(0)), 0);
    linesU.push_back(lasth);
    addItem(lasth);
    VDraggableLine *lastv = 0;
    for (int i = 1; i <= inner_N; ++i)
    {
        lastv = new VDraggableLine( lasth->rightEnd(), model->get_Ui(i));
        linesV.push_back(lastv);
        addItem(lastv);
        lasth->SetRight(lastv);
        lastv->SetLeft(lasth);

        QPointF le = lastv->lastEnd();
        lasth = new HDraggableLine( le, le.x() + model->get_d(i));
        linesU.push_back(lasth);
        addItem(lasth);
        lastv->SetRight(lasth);
        lasth->SetLeft(lastv);
    }
    lastv = new VDraggableLine( lasth->rightEnd(), model->get_Ui(inner_N+1));
    linesV.push_back(lastv);
    addItem(lastv);
    lasth->SetRight(lastv);
    lastv->SetLeft(lasth);

    QPointF le = lastv->lastEnd();
    lasth = new HDraggableLine( le, le.x()+1000 );
    linesU.push_back(lasth);
    addItem(lasth);
    lasth->setFlags(lasth->flags() & ~QGraphicsItem::ItemIsMovable);
    lastv->SetRight(lasth);
    lasth->SetLeft(lastv);
//    model->slotPotentialChanged();
}

void PotentialScene::updatePhysicalModel()
{
    static int mycounter;
    mycounter++;

    if (model->getN() != linesU.size() - 2)
        model->set_N(linesU.size() - 2);
    QPointF p=linesU[0]->scenePos();
    double u = p.y();
    double x;
    model->set_Ui(0, u);
    for (int i = 0; i < linesU.size(); ++i)
    {
        DraggableLine *l = linesU[i];
        p = l->scenePos();
        u = p.y();
        double ddd = l->p1.y()+u;
        double dde = l->p2.y();
        model->set_Ui(i, ddd);
        if (model->get_m(i) == 0)
            model->set_m(i, 0.5);
        if (0 < i && i < linesU.size() - 1)
        {
            model->set_d(i, l->p2.x() - l->p1.x());
//            model->d(i) = linesU[i]->p2.x() - linesU[i]->p1.x();
        }
    }
    model->slotPotentialChanged();
}


#if 0
 void PotentialScene__ clearPotential()
{
	clear();
	lines.clear();
    model->XUscales();

    double x1, y1, y2;
//    int ix1, iy1, iy2;
    double x0=model->scalex*model->Xmin;
    double y0= -model->scaley*model->Umax;
    setSceneRect(x0,y0,model->width,model->height);

//    int ix0=model->scalex*model->Xmin;
//    int iy0= -model->scaley*model->Umax;
//    setSceneRect(ix0,iy0,model->iwidth,model->iheight);
    int inner_N = model->getN ();
    DraggableLine *linev;
    DraggableLine *lineh;
    y0= -model->scaley*model->Ui(0);
    double dd=abs(model->Xmin*model->scalex);
//    iy0= -int(model->scaley*model->Ui(0));
//	first = new HDraggableLine(model,abs(int(model->Xmin*model->scalex)),this); first->setPos(ix0, iy0);
	first = new HDraggableLine(model,dd,this); first->setPos(x0, y0);
//    ix0=0;    iy0=-model->Ui(0)*model->scaley;//0
    x0=0;    y0=-model->Ui(0)*model->scaley;//0
    y1=-model->Ui(1)*model->scaley;
    x1=model->d(1)*model->scalex;
//    iy1=-model->Ui(1)*model->scaley;
//    ix1=model->d(1)*model->scalex;
    linev = new VDraggableLine( y1-y0,this); linev->setPos( x0, y0);
//    linev = new VDraggableLine( iy1-iy0,this); linev->setPos( ix0, iy0);
    first->SetRight(linev);     linev->SetLeft(first);
    addItem(first);
    double x=0;
        for(int i = inner_N; i>=1; i--)
        {
            x+=model->d(inner_N+1-i);   
            x1=x*model->scalex;
//            ix1=x*model->scalex;
//            int id=model->d(inner_N+1-i)*model->scalex;
            dd=model->d(inner_N+1-i)*model->scalex;
            y1=-model->Ui(inner_N+1-i)*model->scaley;
//            iy1=-model->Ui(inner_N+1-i)*model->scaley;
//            lineh = new HDraggableLine( model, id, this); lineh->setPos( ix0, iy1);
            lineh = new HDraggableLine( model, dd, this); lineh->setPos( x0, y1);
            linev->SetRight(lineh); lineh->SetLeft(linev);
            addItem(linev);
//            iy2=-model->Ui(inner_N+1-i+1)*model->scaley;
            y2=-model->Ui(inner_N+1-i+1)*model->scaley;
            linev = new VDraggableLine( y2-y1,this); linev->setPos( x1, y1);
//            linev = new VDraggableLine( iy2-iy1,this); linev->setPos( ix1, iy1);
            lineh->SetRight(linev); linev->SetLeft(lineh);
            addItem(lineh);
            x0=x1;
            y0=y2;
 //           ix0=ix1;
 //           iy0=iy2;
        }
     	last  = new InfDraggableLine(model, this); last->setPos( x1,  y2);
//     	last  = new InfDraggableLine(model, this); last->setPos( ix1,  iy2);
        linev->SetRight(last );	last->SetLeft(linev);
       	addItem(linev); addItem(last);
        nhlines = inner_N+2;
//	realloc();
}
#endif

void PotentialScene::addNewSegment()
{
#if 0
	DraggableLine * splt = last->GetLeft()->GetLeft();
	splt->SetLength(splt->GetLength()/2);
	DraggableLine * lineH = new HDraggableLine(model, splt->GetLength(),this); 
	lineH->setPos(splt->pos()+QPointF(splt->GetLength(),0)); 	
	DraggableLine * lineV = new VDraggableLine(0,this); 
	lineV->setPos(splt->pos()+QPointF(splt->GetLength(),0)); 	

	//Connecting 
	lineV->SetLeft(splt); splt->SetRight(lineV);
	lineV->SetRight(lineH); lineH->SetLeft(lineV);
	lineH->SetRight(last->GetLeft());last->GetLeft()->SetLeft(lineH);

	nhlines = nhlines + 1;
    model->set_N ( nhlines-2 );
    addItem(lineH);
	addItem(lineV);
#endif
}

void PotentialScene::redrawEn()
{
    model->findBoundStates();
//    model->XUscales();
    QRectF newSceneRect(
        model->Xmin,
        model->Umin,
        model->Xmax - model->Xmin,
        model->Umax - model->Umin);
    setSceneRect(newSceneRect);

    for (int i = 0 ; i < linesEn.size(); ++i)
    {
        linesEn[i]->hide();
        removeItem(linesEn[i]);
        delete linesEn[i];
    }
    linesEn.clear();

    double xmin = model->Xmin;
    double xmax = model->Xmax;
 	for (int i=0; i<model->Ebound.size(); i++)
	{
        double U = model->Ebound[i];
        QGraphicsLineItem *last = new QGraphicsLineItem(xmin,U,xmax,U,0,this);
        linesEn.push_back(last);
    }
    update();
}

void PotentialScene::drawBackground(QPainter *, const QRectF &)
{
}

 