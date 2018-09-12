/***************************************************************************
    solutionVizArg: Visualization of Solutions in Abstract Argumentation Frameworks


    Written in Qt/C++

                             -------------------
    This software is written as a University project.
 ***************************************************************************/
#ifndef ARGUMENTWEIGHTEXTENSION_H
#define ARGUMENTWEIGHTEXTENSION_H
using namespace std;

#include <QGraphicsTextItem>

class Argument;


static const int TypeNumber=QGraphicsItem::UserType+3;
/**
 * @brief The ArgumentWeightExtension is a class
 * basically implemented as number of extension for each argument
 */
class ArgumentWeightExtension:public QGraphicsTextItem {

public:
    ArgumentWeightExtension(Argument * , int, QString);
    void removeRefs();
    enum { Type = UserType + 3 };
    int type() const { return Type; }
    Argument* argument() { return source; }
    ~ArgumentWeightExtension();
private:
    Argument *source;
};
#endif // ARGUMENTWEIGHTEXTENSION_H
