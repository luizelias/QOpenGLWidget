#ifndef CREATEMAINWINDOW_H
#define CREATEMAINWINDOW_H

#include "MainWindow.h"

#include <QtCore/QPointer>
#include <QPushButton>
#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>



#include <vector>

#define deformerFlag              "-d"
#define deformerFlagLong          "-deformer"

namespace mush3d
{

class CreateMainWindow : public MPxCommand
{
public:
    static void*	creator()		{ return new CreateMainWindow(); }

    MStatus			doIt(const MArgList& args);

    static MSyntax newSyntax();

private:

};

}




#endif // CREATEMAINWINDOW_H
