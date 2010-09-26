#include <QMessageBox>
#include "PotentialScene.h"
#include "DraggableLine.h"
#include "model.h"

PotentialScene::PotentialScene(Model *_model, QObject * parent)
    : QGraphicsScene(parent), pdata(new PotData),eld(new Elevdata), model(_model)
{
	eld->energies = new double[20];//??
	eld->bufsize = 20;
	eld->a = 0;
	eld->b = 0;
	pdata->e2find = -1;
	pdata->u=0;
	pdata->l=0;
	clearPotential();
	
}


void PotentialScene::setModel(Model *_model)
{
    model = _model;
}

void PotentialScene::modelChanged()
{

}

void PotentialScene::realloc()
{
	if(eld->a != 0) delete[] eld->a; eld->a = new double[nhlines];
	if(eld->b != 0) delete[] eld->b; eld->b = new double[nhlines];
	if(pdata->u!=0) delete[] pdata->u;pdata->u = new double[nhlines-2];
	if(pdata->l!=0) delete[] pdata->l;pdata->l = new double[nhlines-2];
	pdata->size = nhlines-2;
}

void PotentialScene::clearPotential()
{
	clear();
	lines.clear();
    model->XUscales();
    int ix1, iy1, iy2;
    int ix0=model->scalex*model->Xmin;
    int iy0= -model->scaley*model->Umax;
    setSceneRect(ix0,iy0,model->iwidth,model->iheight);
    int inner_N = model->getN ();
    DraggableLine *linev;
    DraggableLine *lineh;
	first = new InfDraggableLine(model,this); first->setPos(ix0, 0);//first->setPos(ix0,  iy0); //!!!!
//    ix0=0;    iy0=-model->Ui(1)*model->scaley;//0!!!!
    ix0=0;    iy0=-model->Ui(0)*model->scaley;//0
    iy1=-model->Ui(1)*model->scaley;
    ix1=model->d(1)*model->scalex;
    linev = new VDraggableLine( iy1-iy0,this); linev->setPos( ix0, iy0);
    first->SetRight(linev);     linev->SetLeft(first);
    addItem(first);
    double x=0;
        for(int i = inner_N; i>=1; i--)
        {
            x+=model->d(inner_N+1-i);   
            ix1=x*model->scalex;
            int id=model->d(inner_N+1-i)*model->scalex;
            iy1=-model->Ui(inner_N+1-i)*model->scaley;
            lineh = new HDraggableLine( model, id, this); lineh->setPos( ix0, iy1);
            linev->SetRight(lineh); lineh->SetLeft(linev);
            addItem(linev);
            iy2=-model->Ui(inner_N+1-i+1)*model->scaley;
            linev = new VDraggableLine( iy2-iy1,this); linev->setPos( ix1, iy1);
            lineh->SetRight(linev); linev->SetLeft(lineh);
            addItem(lineh);
            ix0=ix1;
            iy0=iy2;
        }
     	last  = new InfDraggableLine(model, this); last->setPos( ix1,  iy2);
        linev->SetRight(last );	last->SetLeft(linev);
       	addItem(linev); addItem(last);
        nhlines = inner_N+2;
	realloc();
}

void PotentialScene::addNewSegment()
{
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
	realloc();
	addItem(lineH);
	addItem(lineV);
}

static void my_compute(PotData *p,Elevdata *e, Model *model)
{
    int inner_N = p->size;
    model->set_N ( inner_N );
    model->Ui(0) = 0;
    model->d(0) = 0;
    model->m(0) = 0.5;
    model->Ui( inner_N + 1 ) = 0;
    model->m( inner_N + 1 ) = 0.5;
    for(int i=1; i <= inner_N; i++)
    {
        model->Ui(inner_N+1-i) = p->u[i-1];
        model->d(inner_N+1-i) = p->l[i-1];
        model->m(inner_N+1-i) = 0.5;
    }
    model->build_U();
    model->findBoundStates();
    e->nlevels = model->Ebound.size();
    for(int n=0; n<e->nlevels; n++)
    {
        e->energies[n] = model->Ebound[n];
    }
}

//#define SCALE 1.
//#define SCALE 20
void PotentialScene::calculateEnergies()
{

	//static FILE * pFile = NULL;
	//if(pFile == NULL) pFile=fopen("dbga.txt","w");

    // Fill pdata from visual appearance of potential
	DraggableLine * line = last->GetLeft()->GetLeft();
	int i =0;
//    model->XUscales(); 
	while(line != first)
	{
        pdata->u[i] = (first->pos().y() - line->pos().y())/model->scaley;
//        pdata->u[i] = (first->pos().y() - line->pos().y())/model->scaley;
		pdata->l[i] = line->GetLength()/model->scalex;
//		pdata->u[i] = (first->pos().y() - line->pos().y())/SCALE;
//		pdata->l[i] = line->GetLength()/SCALE;
		i++;
		line = line->GetLeft()->GetLeft();
	}
#if COMPUTING_AND_DRAWING_ELEVLES_VIA_OCAML
	ML_Interface::GetInstance()->CallEnergies(pdata,eld);
#else
    my_compute(pdata,eld,model);
#endif    
	//Drawing levels
    double y, y1;
    int ixmin=model->scalex*model->Xmin;
    int ixmax=model->scalex*model->Xmax;
	for(i=0;i<eld->nlevels;i++)
	{
		y1 = eld->energies[i];
		y = first->pos().y();
        y=y-y1*model->scaley;
//        y=y-y1*model->scaley;
        
//        y=y-y1*SCALE;
        if(i >= lines.size()) lines.push_back(addLine(ixmin,y,ixmax,y));
//		if(i >= lines.size()) lines.push_back(addLine(-100,y,200,y));
		else {lines[i]->setLine(ixmin,y,ixmax,y); lines[i]->setVisible(true);}
//		else {lines[i]->setLine(-100,y,200,y); lines[i]->setVisible(true);}
	}
	for(i=eld->nlevels;i<lines.size();i++) lines[i]->setVisible(false);
}

void PotentialScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) { QGraphicsScene::mousePressEvent(mouseEvent); }
void PotentialScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) { QGraphicsScene::mouseMoveEvent(mouseEvent); }
void PotentialScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) { QGraphicsScene::mouseReleaseEvent(mouseEvent); }

void PotentialScene::drawBackground(QPainter * painter, const QRectF & rect) { }

 