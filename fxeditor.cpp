#include "fxeditor.h"
#include "ui_fxeditor.h"

FXEditor::FXEditor(QWidget *parent) :
    QDockWidget(parent),
    _ui(new Ui::FXEditor)
{
    _ui->setupUi(this);
    _ui->knob2->setScale(0.0f, 2.0f, 0.4f);
    _ui->knob2->setRange(0.0f, 2.0f);
    //_ui->knob2->setMass(10.0f);
}

FXEditor::~FXEditor()
{
    delete _ui;
}

void FXEditor::connectWithProcessor(const CompressorProcessor& cp)
{
    connect(_ui->knob1, SIGNAL(valueChanged(double)), &cp, SLOT(setThreshold(double)));
    connect(_ui->knob2, SIGNAL(valueChanged(double)), &cp, SLOT(setGain(double)));
}
