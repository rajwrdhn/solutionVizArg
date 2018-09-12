/***************************************************************************
    solutionVizArg: Visualization of Solutions of Abstract Argumentation Frameworks


    Written in Qt/C++

                             -------------------
    This software is written as a University project.
 ***************************************************************************/
#ifndef ARGUMENTATIONFRAMEWORK_H
#define ARGUMENTATIONFRAMEWORK_H

using namespace std;

#include <QObject>
#include <QPointF>
#include <QHash>

class MainWindow;
class TextEdit;
class Attack;
class GraphicsWidget;

typedef QHash<QString , int>mapArg; //stores the args.

/**
 * @brief The ArgumentationFramework class
 * used by MW.
 * reads the file uploaded as arguments and attacks,
 * for drawing on the canvas.
 */
class ArgumentationFramework:public QObject
{
    Q_OBJECT
public:
    ArgumentationFramework(GraphicsWidget *view, TextEdit *text);
    ~ArgumentationFramework();

    void clear();
signals:
    void drawNode(bool,QString);
    void drawEdge( QString , QString , int , int );
public slots:
    void getAllArguments();
    void getAllAttacks();

private:
    mapArg mapArgument;
    TextEdit *aftext;
    GraphicsWidget *gview;
    QString str;
    QString str1;
    int n;
    bool frameworkDraw;
};

#endif // ARGUMENTATIONFRAMEWORK_H

