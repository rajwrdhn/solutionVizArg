/***************************************************************************
    solutionVizArg: Visualization of Solutions of Abstract Argumentation Frameworks


    Written in Qt/C++

                             -------------------
    This software is written as a University project.
 ***************************************************************************/
#ifndef SOLUTION
#define SOLUTION
//this class deals with the solution for visualization.
//MapReduce
using namespace std;

#include <QObject>
#include <QStringList>
#include <QHash>
#include <iostream>

using namespace std;
typedef QList<QString> setList;
typedef QMultiHash<QString,QString> argInExtension;


class TextEdit;
class GraphicsWidget;


class Solution:public QObject
{
    Q_OBJECT
public:
    Solution(TextEdit *, GraphicsWidget *);
    ~Solution();
    QStringList listSol;
    QStringList getNextSolutionSet(QString);
    bool getPresentSolutionset(QString, QString );
    QStringList getExtension (int i, QString solString);
    argInExtension theMap;
    int numberOfExtensions(QString);
    void clear();
    void normalizeSize();
    int sizeOfArgInSolution(QString arg);
    void setScepticalDataCol();
public slots:
    void calculateMapReduce(QString afSolutionstr);
    void calculateShapeRandom();
    void calculateSize();
    void setSizeOfArgument();
    void calculateColor();
signals:
    void signalsetSizeOfArgument();
private:
    TextEdit *en_text;
    GraphicsWidget *en_view;
    QString allSolutions;
    bool present;
    long int i_extension_number;
    QStringList listSol1;
 };

#endif // SOLUTION

