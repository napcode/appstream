#ifndef FXEDITOR_H
#define FXEDITOR_H

#include <QDockWidget>
#include "compressorprocessor.h"
namespace Ui {
class FXEditor;
}

class FXEditor : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit FXEditor(QWidget *parent = 0);
    ~FXEditor();
    void connectWithProcessor(const CompressorProcessor& cp);
private:
    Ui::FXEditor *_ui;
};

#endif // FXEDITOR_H
