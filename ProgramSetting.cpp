#include "ProgramSetting.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>


ProgramSetting::ProgramSetting(QWidget *parent, Qt::WindowFlags f)
: QDialog(parent,f), model(0)
{
        this->setWindowTitle(tr("Setting"));
        this->setFont(QFont("Serif", 12, QFont::Bold ));
        QVBoxLayout *vl = new QVBoxLayout(this);
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("width of lines"),this));
        h->addWidget(this->leLineWidth = new QLineEdit(this));
        this->leLineWidth->setEnabled(true);
        this->leLineWidth->setToolTip(tr("width of lines in pixels"));
        connect(this->leLineWidth,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    this->setLayout(vl);
    this->modelChanged();
}

ProgramSetting::~ProgramSetting(void)
{
}

void ProgramSetting::setModel(PhysicalModel *_model)
{
    model = _model;
    modelChanged();
}

void ProgramSetting::modelChanged()
{
    if (!model)
    {
        this->leLineWidth->setDisabled(true);
        return;
    }
    else
    {
        this->leLineWidth->setEnabled(true);
    }

    SettingParameters wp=model->getSettingParameters();
    last.lineWidth = -1;

    QString buf;
    buf.sprintf("%i",wp.lineWidth);
    this->leLineWidth->setText(buf);
}

void ProgramSetting::updateModel()
{
    if (!model) return;
    SettingParameters wp= {0};
    wp.lineWidth = this->leLineWidth->text().toInt();
    if (wp != last)
    {
        model->setSettingParameters(wp);
        last = wp;
    }
}
