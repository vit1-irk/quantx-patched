#include "PotentialViewMovable.h"

PotentialViewMovable::PotentialViewMovable(QWidget *parent, Qt::WindowFlags f)
: QWidget(parent,f), model(0), scene(0), view(0)
{
}

PotentialViewMovable::~PotentialViewMovable(void)
{
}

void PotentialViewMovable::setModel(PhysicalModel *_model)
{
    model = _model;
}

void PotentialViewMovable::modelChanged()
{
}

void PotentialViewMovable::updateModel()
{
}

#if 0
QPointF PotentialViewMovable::toReal(const QPoint& p)
{
}
QPoint PotentialViewMovable::fromReal(const QPointF& f)
{
}

void PotentialViewMovable::modelChanged()
{
    scene->clear();
    lines.clear();

    double model_xmin = model->getXmin();
    double model_xmax = model->getXmax();
    double model_umin = model->getUmin();
    double model_umax = model->getUmax();
    double model_deltax = model_xmax - model_xmin;
    double model_deltau = model_umax - model_umin;

    double display_xmin = model_xmin - 0.1*(model_deltax ? model_deltax : 1.0);
    double display_xmax = model_xmax + 0.1*(model_deltax ? model_deltax : 1.0);
    double display_umin = model_umin - 0.1*(model_deltau ? model_deltau : 1.0);
    double display_umax = model_umax + 0.1*(model_deltau ? model_deltau : 1.0);

    scene->setSceneRect();

    int ix1, iy1, iy2;
    int ix0=model->scalex*model->Xmin;
    int iy0= -model->scaley*model->Umax;
//    int iy0= model->scaley*model->Umin;
    setSceneRect(ix0,iy0,model->iwidth,model->iheight);
//    int iy1=-model->Ui(1)*model->scaley;
//    int ix1=model->d(1)*model->scalex;

//	first = new InfDraggableLine(model,this); first->setPos(ix0,  0); 
//    DraggableLine * line1 = new VDraggableLine( iy1,this); line1->setPos(   0,  0); 	
//    DraggableLine * line2 = new HDraggableLine( model, ix1,this); line2->setPos(   0,iy1); 	
//	DraggableLine * line3 = new VDraggableLine(-iy1,this); line3->setPos( ix1,iy1);
//   	last  = new InfDraggableLine(model, this); last ->setPos( ix1,  0);
    int inner_N = model->getN ();


    DraggableLine *linev;
    DraggableLine *lineh;

    double current_x = 0;
	QPoint left = fromReal(display_xmin, model->U[0]);
    QPoint right = fromReal(current_x, model->U[0]);
    HDraggableLine *first = new HDraggableLine(left,right,this);

        Draggabfirst = new InfDraggableLine(model,this); first->setPos(ix0,  0); 
    ix0=0;    iy0=0;
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
//            linevl = new VDraggableLine( iy1,this); linev->setPos( ix0, iy0);
            lineh = new HDraggableLine( model, id, this); lineh->setPos( ix0, iy1);
            linev->SetRight(lineh); lineh->SetLeft(linev);
            addItem(linev);
            iy2=-model->Ui(inner_N+1-i+1)*model->scaley;
            linev = new VDraggableLine( iy2-iy1,this); linev->setPos( ix1, iy1);
//            linev = new VDraggableLine( -iy1,this); linev->setPos( ix1, iy1);
            lineh->SetRight(linev); linev->SetLeft(lineh);
            addItem(lineh);
            ix0=ix1;
            iy0=iy2;
        }
//	    linev = new VDraggableLine(-iy1,this); linev->setPos( ix1,iy1);
     	last  = new InfDraggableLine(model, this); last->setPos( ix1,  iy2);
        linev->SetRight(last );	last->SetLeft(linev);
       	addItem(linev); addItem(last);
        nhlines = inner_N+2;
/*	first->SetRight(line1);
    line1->SetLeft(first); //connect first and line1
    line1->SetRight(line2);	line2->SetLeft(line1); 
    line2->SetRight(line3);	line3->SetLeft(line2); 
    line3->SetRight(last );	last ->SetLeft(line3);

	addItem(first);
	addItem(line1);
	addItem(line2);
	addItem(line3);
	addItem(last );

	nhlines = 3;
*/	realloc();
}


void PotentialViewMovable::clearPotential()
{
}

void PotentialViewMovable::addNewSegment()
{
}

void PotentialViewMovable::updateModel()
{
}
#endif
