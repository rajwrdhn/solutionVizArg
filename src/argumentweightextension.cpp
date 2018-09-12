/***************************************************************************
    solutionVizArg: Visualization of Solutions in Abstract Argumentation Frameworks


    Written in Qt/C++

                             -------------------
    This software is written as a University project.
 ***************************************************************************/
#include "argumentweightextension.h"
#include "argument.h"
#include <QFont>

ArgumentWeightExtension::ArgumentWeightExtension(Argument *arg , int size, QString argWeight) :QGraphicsTextItem(0) {
    source=arg;
    setPlainText( argWeight );
    setFont( QFont ("Times", size, QFont::Black, false) );
    setZValue(254);
}

void ArgumentWeightExtension::removeRefs(){
    source->deleteNumber();
}

ArgumentWeightExtension::~ArgumentWeightExtension(){

}
