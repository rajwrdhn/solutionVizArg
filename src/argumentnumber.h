/***************************************************************************
    solutionVizArg: Visualization of Solutions in Abstract Argumentation Frameworks


    Written in Qt/C++

                             -------------------
    This software is written as a University project.
 ***************************************************************************/
#ifndef ARGUMENTNUMBER_H
#define ARGUMENTNUMBER_H

using namespace std;

#include <QGraphicsTextItem>

class Argument;


static const int TypeNumber=QGraphicsItem::UserType+3;
/**
 * @brief The ArgumentNumber class
 * basically implemented as number for each argument
 * we do not use this
 * can be later used as a labeling
 * argument labeling
 */
class ArgumentNumber:public QGraphicsTextItem {

public:
    ArgumentNumber(Argument * , int, QString);
    void removeRefs();
    enum { Type = UserType + 3 };
    int type() const { return Type; }
    Argument* argument() { return source; }
    ~ArgumentNumber();
private:
    Argument *source;
};

#endif // ARGUMENTNUMBER_H
