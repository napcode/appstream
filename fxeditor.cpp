#include "fxeditor.h"
#include "ui_fxeditor.h"

FXEditor::FXEditor(QWidget* parent)
: QDockWidget(parent),
  _ui(new Ui::FXEditor)
{
    _ui->setupUi(this);
}

FXEditor::~FXEditor()
{
    delete _ui;
}

void FXEditor::connectWithProcessor(const CompressorProcessor& cp)
{
}
