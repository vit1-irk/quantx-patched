#include "Zview.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>


Zview::Zview(QWidget *parent, Qt::WindowFlags f)
: QDialog(parent,f), model(0)
{
    this->setWindowTitle("z parameters:");
        this->setFont(QFont("Serif", 12, QFont::Bold ));
        QVBoxLayout *vl = new QVBoxLayout(this);

    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel("z",this));
        h->addWidget(this->leZ = new QLineEdit(this));
        this->leZ->setEnabled(true);
        this->leZ->setToolTip("Current value of z");
        connect(this->leZ,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel("zmin",this));
        h->addWidget(this->leZmin= new QLineEdit(this));
        this->leZmin->setToolTip("Lower bound of z interval");
        connect(this->leZmin,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel("zmax",this));
        h->addWidget(this->leZmax= new QLineEdit(this));
        this->leZmax->setToolTip("High bound of z interval");
        connect(this->leZmax,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel("hz",this));
        h->addWidget(this->leHz= new QLineEdit(this));
        this->leHz->setToolTip("z increment");
        connect(this->leHz,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    this->setLayout(vl);
    this->modelChanged();
}

Zview::~Zview(void)
{
}
//void Zview::invalidateModel()
//{
//    reset();
//}

void Zview::setModel(PhysicalModel *_model)
{
    model = _model;
//    connect(model,SIGNAL(signalZChanged(double)),this,SLOT(updateModel()));
    modelChanged();
}

void Zview::modelChanged()
{
    if (!model)
    {
        this->leZ->setDisabled(true);
        this->leZmin->setDisabled(true);
        this->leZmax->setDisabled(true);
        this->leHz->setDisabled(true);
        return;
    }
    else
    {
        this->leZ->setEnabled(true);
        this->leZmin->setEnabled(true);
        this->leZmax->setEnabled(true);
        this->leHz->setEnabled(true);
    }

    zParameters tp = model->getzParam();
    last.z = -1000;

    QString buf;
    buf.sprintf("%lg",tp.z);
    this->leZ->setText(buf);

    buf.sprintf("%lg",tp.zmin);
    this->leZmin->setText(buf);

    buf.sprintf("%lg",tp.zmax);
    this->leZmax->setText(buf);

    buf.sprintf("%lg",tp.hz);
    this->leHz->setText(buf);
}

void Zview::updateModel()
{
    if (!model) return;
    zParameters tp = { 0 };

    tp.z = this->leZ->text().toDouble();
    tp.zmin = this->leZmin->text().toDouble();
    tp.zmax = this->leZmax->text().toDouble();
    tp.hz = this->leHz->text().toDouble();
    if (tp != last)
    {
        model->setzParam(tp);
        last = tp;
    }
}
