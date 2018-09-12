#include "argumentnumber.h"
#include "argument.h"
#include <QFont>

ArgumentNumber::ArgumentNumber(Argument *arg , int size, QString labelText) :QGraphicsTextItem(0) {
    source=arg;
    //arg -> addNumber(this);
    //setParentItem(arg); //auto disables child items like this, when node is disabled.
    setPlainText( labelText );
    setFont( QFont ("Times", size, QFont::Black, false) );
    setZValue(254);
}

void ArgumentNumber::removeRefs(){
    source->deleteNumber();
}

ArgumentNumber::~ArgumentNumber(){

}
