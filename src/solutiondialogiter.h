/***************************************************************************
    solutionVizArg: Visualization of Solutions of Abstract Argumentation Frameworks


    Written in Qt/C++

                             -------------------
    This software is written as a University project.
 ***************************************************************************/
#ifndef SolutionDialogIter_H
#define SolutionDialogIter_H

//this class is for viusalizing by iteration.
using namespace std;

#include <QDialog>
#include <ui_solutiondialogiter.h>
class Solution;
class GraphicsWidget;
class TextEdit;
class Graph;
class SolutionDialogIter : public QDialog
{
    Q_OBJECT

public:
    explicit SolutionDialogIter(TextEdit*,GraphicsWidget*,QWidget *parent = 0);
    ~SolutionDialogIter();
    QColor initColor;
    QColor argExtensionColor;
    QColor solutionColor;
    bool checkEmpty(QString);
public slots:
    void nextgatherData();
    void colorArgInExtension();
private:
    Ui::SolutionDialog2 *ui;
    TextEdit *en_text;
    GraphicsWidget *en_view;
    Solution *extn;
    QStringList solList;
    QString sol;
};

#endif // SolutionDialogIter_H
