#include <QMessageBox>
#include "PotentialScene.h"
#include "DraggableLine.h"
#include "PhysicalModel.h"
#include <QRectF>

PotentialScene::PotentialScene(PhysicalModel *_model, QObject * parent)
    : QGraphicsScene(parent), model(_model)
{
    modelChanged();
}

void PotentialScene::modelChanged()
{
    model->XUscales();
    QRectF newSceneRect(
        model->Xmin,
        model->Umin,
        model->Xmax - model->Xmin,
        model->Umax - model->Umin);
    setSceneRect(newSceneRect);

    clear();
    lines.clear();

    int inner_N = model->getN();

    HDraggableLine *lasth = new HDraggableLine(QPointF(-1000,model->Ui(0)), 0, this);
    addItem(lasth);
    VDraggableLine *lastv = 0;
    for (int i = 1; i <= inner_N; ++i)
    {
        lastv = new VDraggableLine( lasth->rightEnd(), model->Ui(i),this);
        QLineF lv = lastv->line();
        addItem(lastv);
        lasth->SetRight(lastv);
        lastv->SetLeft(lasth);

        QPointF le = lastv->lastEnd();
        lasth = new HDraggableLine( le, le.x() + model->d(i), this);
        QLineF lh = lasth->line();
        addItem(lasth);
        lastv->SetRight(lasth);
        lasth->SetLeft(lastv);
    }
    lastv = new VDraggableLine( lasth->rightEnd(), model->Ui(inner_N+1),this);
    QLineF lv = lastv->line();
    addItem(lastv);
    lasth->SetRight(lastv);
    lastv->SetLeft(lasth);

    QPointF le = lastv->lastEnd();
    lasth = new HDraggableLine( le, le.x()+1000, this);
    QLineF lh = lasth->line();
    addItem(lasth);
    lasth->setFlags(lasth->flags() & ~QGraphicsItem::ItemIsMovable);
    lastv->SetRight(lasth);
    lasth->SetLeft(lastv);
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

static void my_compute(PhysicalModel *model)
{
    int inner_N= model->getN ();
    model->d(0) = 0;
    model->m(0) = 0.5;
    model->Ui( inner_N + 1 ) = 0.;
    model->m( inner_N + 1 ) = 0.5;
    for(int i=1; i <= inner_N; i++)
    {
        model->m(inner_N+1-i) = 0.5;
    }
    model->build_U();
    model->findBoundStates();
}

void PotentialScene::calculateEnergies()
{
#if 0
    // Fill pdata from visual appearance of potential
	DraggableLine * line = last->GetLeft()->GetLeft();
	int i = 1;
    int inner_N= model->getN ();
	while(line != first)
	{
        model->Ui(inner_N+1-i) = (last->pos().y() - line->pos().y())/model->scaley;
//        pdata->u[i] = (last->pos().y() - line->pos().y())/model->scaley;//Added 30.09
//        pdata->u[i] = (first->pos().y() - line->pos().y())/model->scaley;
		model->d(inner_N+1-i)= line->GetLength()/model->scalex; //pdata->l[i] = line->GetLength()/model->scalex;
		i++;
		line = line->GetLeft()->GetLeft();
	}
       model->Ui(0) = (last->pos().y() - line->pos().y())/model->scaley;//pdata->Uwleft = (last->pos().y() - line->pos().y())/model->scaley;//Added 30.09

    my_compute(model);

	//Drawing levels
    double y, y1;
    double xmin=model->scalex*model->Xmin;
    double xmax=model->scalex*model->Xmax;
//    int ixmin=model->scalex*model->Xmin;
//    int ixmax=model->scalex*model->Xmax;
	for(i=0; i<model->Ebound.size(); i++)
	{
		y1 = model->Ebound[i];//eld->energies[i];
		y = last->pos().y();//Added 30.09
//		y = first->pos().y();
        y=y-y1*model->scaley;
        if(i >= lines.size()) lines.push_back(addLine(xmin,y,xmax,y));
		else {lines[i]->setLine(xmin,y,xmax,y); lines[i]->setVisible(true);}
//        if(i >= lines.size()) lines.push_back(addLine(ixmin,y,ixmax,y));
//		else {lines[i]->setLine(ixmin,y,ixmax,y); lines[i]->setVisible(true);}
	}
	for(i=model->Ebound.size();i<lines.size();i++) lines[i]->setVisible(false);
//	for(i=eld->nlevels;i<lines.size();i++) lines[i]->setVisible(false);
#endif
}

void PotentialScene::drawBackground(QPainter *, const QRectF &)
{
}

 