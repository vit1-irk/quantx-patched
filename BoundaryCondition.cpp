#include "BoundaryCondition.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>

BoundaryCondition::BoundaryCondition(QWidget *parent, Qt::WindowFlags f)
: QDialog(parent,f), model(0)
{
    this->setWindowTitle(tr("Boundary conditions"));
    this->setFont(QFont("Serif", 12, QFont::Bold ));
    int index;
    QVBoxLayout *vl = new QVBoxLayout(this);
    {
        QRadioButton *rad1= new QRadioButton("Bound States/Scattering");
        QRadioButton *rad2= new QRadioButton("Periodic");
        QRadioButton *rad3= new QRadioButton("Quasistationary");
        vl->addWidget(rad1);
        vl->addWidget(rad2);
        vl->addWidget(rad3);
        bgR = new QButtonGroup(this);
        bgR->setExclusive(true);
        bgR->addButton(rad1,0);
        bgR->addButton(rad2,1);
        bgR->addButton(rad3,2);
        bgR->button(0)->setChecked(true);
        connect(bgR,SIGNAL(buttonClicked(int)),this,SLOT(updateModel()));
    }
    this->setLayout(vl);
    this->modelChanged();
}

BoundaryCondition::~BoundaryCondition(void)
{
}

void BoundaryCondition::setModel(PhysicalModel *_model)
{
    if (model != _model)
    {
        disconnect(model,0,this,0);
        model = _model;
        modelChanged();
    }
        connect(model,SIGNAL(signalTypeOfUChanged()),this,SLOT(modelChanged()));

}

void BoundaryCondition::modelChanged()
{
    if (!model)
    {
        return;
    }
    int index;
    PotentialType u = model->getPotentialType();
    if(u==FINITE) index=0;
    if(u==PERIODIC) index=1;
    if(u==QUASISTATIONARY) index=2;
    if(u==SEMIPERIODIC) index=3;
    bgR->button(index)->setChecked(true);
//    bgR->setId(bgR->button(index),index);
}

//void BoundaryCondition::newModel()
void BoundaryCondition::updateModel()
{
    if (!model) return;
    int index = bgR->checkedId();

    PotentialType u;
    if(index==0) u=FINITE;
    if(index==1) u=PERIODIC;
    if(index==2) u=QUASISTATIONARY;
    if (u != last)
    {
        model->setPotentialType(u);
        last = u;
    }
}
/*void BoundaryCondition::readFromXml(QXmlStreamReader *r)
{
    Q_ASSERT(this);
    Q_ASSERT(r->isStartElement() && r->name() == "BoundaryCondition");
    int index;
    while (!r->atEnd())
    {
        r->readNext();
        if (r->isEndElement())
            break;
        if (!r->isStartElement())
            continue;
        if (r->name() == "typeOfPotential")
        {
            QString s = r->readElementText();
            if(s=="FINITE") index=0;
            if(s=="PERIODIC") index=1;
            if(s=="QUASISTATIONARY") index=2;
            if(s=="SEMIPERIODIC") index=3;
            PotentialType u;
            if(index==0) u=FINITE;
            if(index==1) u=PERIODIC;
            if(index==2) u=QUASISTATIONARY;
            if(index==3) u=SEMIPERIODIC;
            model->setPotentialType(u);
            bgR->setId(bgR->button(index),index);
        }
        else
            skipUnknownElement(r);
    }
}
*/
/*void BoundaryCondition::writeToXml(QXmlStreamWriter *w)
{
    w->writeStartElement("BoundaryCondition");
    {
        int index = bgR->checkedId();

        QString s;
        if(index==0) s="FINITE";
        if(index==1) s="PERIODIC";
        if(index==2) s="QUASISTATIONARY";
        if(index==3) s="SEMIPERIODIC";
        w->writeTextElement("typeOfPotential",s);
    }
    w->writeEndElement();
}
*/
