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
    model = _model;
    modelChanged();
}

void BoundaryCondition::modelChanged()
{
    if (!model)
    {
//        this->bgR->setDisabled(true);
        return;
    }
    else
    {
//        this->bgR->setEnabled(true);
    }
    int index;
    PotentialType u = model->getPotentialType();
    if(u==FINITE) index=0;
    if(u==PERIODIC) index=1;
    if(u==QUASISTATIONARY) index=2;
    if(u==SEMIPERIODIC) index=3;
    bgR->setId(bgR->button(index),index);
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
