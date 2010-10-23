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
//    connect(this,SIGNAL(draggableLineMoved()),this,SLOT(updatePhysicalModel()));
    drawU();
    redrawEn();
}
void PotentialScene::drawU()
{
   int inner_N = model->getN();
   QRectF vp = model->getGoodViewportAtU();
   setSceneRect(vp);
   setItemIndexMethod(QGraphicsScene::NoIndex);
    HDraggableLine *lasth = new HDraggableLine(QPointF(-1000,model->get_Ui(0)), 0);
    linesU.push_back(lasth);
    addItem(lasth);
//    lasth->setFlags(lasth->flags() & ~QGraphicsItem::ItemIsMovable);
     VDraggableLine *lastv=0;
    for (int i = 1; i <= inner_N; ++i)
    {
        QPointF le = lasth->rightEnd();
        double uu=model->get_Ui(i);
        lastv = new VDraggableLine( le, model->get_Ui(i));
        linesV.push_back(lastv);
        addItem(lastv);
        lasth->SetRight(lastv);
        lastv->SetLeft(lasth);

        le = lastv->lastEnd();
        double dd=model->get_d(i);
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
}

void PotentialScene::updatePhysicalModel()
{
    QVector<double> Ui;
    QVector<double> d;
    QVector<double> m;
    double Ub = model->get_Ub();
    d.push_back(0.);
    int NN = linesU.size();
    for (int i = 0; i < linesU.size(); ++i)
    {
        QPointF p1 = linesU[i]->p1;
        QPointF p2 = linesU[i]->p2;

        double _u = p1.y();
       if (0 < i && i < linesU.size() - 1)
       {
         double _d = p2.x() - p1.x();
         d.push_back(_d);
       }
        double _m = model->get_m(i);
        Ui.push_back(_u);
        m.push_back(_m);
    }
    model->set_Ui_d_m_Ub(Ui,d,m,Ub);
}

void PotentialScene::redrawU()
{
#if 1
    QList<QGraphicsItem*> hm = this->selectedItems();
    int hmsz = hm.size();
    if (hmsz) return;
#endif
   int inner_N = model->getN();
   QRectF sR = this->sceneRect();
   QRectF vp = model->getGoodViewportAtU();
   setSceneRect(vp);
   update(vp);
   double xmin = vp.left(); //model->Xmin;
   sR = this->sceneRect();
    for (int i = 0; i < linesU.size(); ++i)
    {
        linesU[i]->hide();
        removeItem(linesU[i]);
        delete linesU[i];
    }
    
    linesU.clear();

    for (int i = 0; i < linesV.size(); ++i)
    {
        linesV[i]->hide();
        removeItem(linesV[i]);
        delete linesV[i];
    }
    linesV.clear();

/*    foreach (DraggableLine *v, linesV)
    {
        v->hide();
        removeItem(v);
        delete v;
    }
    linesV.clear();
*/
    int Nh=linesU.size();
    int Nv=linesV.size();
    HDraggableLine *lasth = new HDraggableLine(QPointF(-1000,model->get_Ui(0)), 0);
    linesU.push_back(lasth);
    addItem(lasth);
     VDraggableLine *lastv=0;
    for (int i = 1; i <= inner_N; ++i)
    {
        QPointF le = lasth->rightEnd();
        double uu=model->get_Ui(i);
        lastv = new VDraggableLine( le, model->get_Ui(i));
        linesV.push_back(lastv);
        addItem(lastv);
        lasth->SetRight(lastv);
        lastv->SetLeft(lasth);

        le = lastv->lastEnd();
        double dd=model->get_d(i);
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
}



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
//    return;
    QRectF vp = model->getGoodViewportAtU();
    setSceneRect(vp);
    update ( vp);
    for (int i = 0 ; i < linesEn.size(); ++i)
    {
        linesEn[i]->hide();
        removeItem(linesEn[i]);
        delete linesEn[i];
    }
    linesEn.clear();

    QVector<double> Ebound = model->getEn();

    double xmin = -1000;
    double xmax = 1000;//vp.right(); //model->Xmax;
    int NN = Ebound.size();
 	for (int i=0; i<Ebound.size(); i++)
	{
        double U = Ebound[i];
        QGraphicsLineItem *last = new QGraphicsLineItem(xmin,U,xmax,U,0,this);
        linesEn.push_back(last);
    }
    update();
}

void PotentialScene::drawBackground(QPainter *, const QRectF &)
{
}

 