#include "BoundaryCondition.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>

BoundaryCondition::BoundaryCondition(QWidget *parent, Qt::WindowFlags f)
: QDialog(parent,f), model(0)
{
    this->setWindowTitle(tr("Boundaru conditions"));
    this->setFont(QFont("Serif", 12, QFont::Bold ));

    QVBoxLayout *vl = new QVBoxLayout(this);
    {
        QRadioButton *rad1= new QRadioButton("Finite");
        QRadioButton *rad2= new QRadioButton("Periodic");
        vl->addWidget(rad1);
        vl->addWidget(rad2);
        bgR = new QButtonGroup(this);
        bgR->setExclusive(true);
        bgR->addButton(rad1,0);
        bgR->addButton(rad2,1);
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
    if(u==SEMIPERIODIC) index=2;
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
    if (u != last)
    {
        model->setPotentialType(u);
        last = u;
    }
}
