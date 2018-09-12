/***************************************************************************
    solutionVizArg: Visualization of Solutions of Abstract Argumentation Frameworks


    Written in Qt/C++

                             -------------------
    This software is written as a University project.
 ***************************************************************************/
#include "solutiondialogiter.h"
#include "ui_solutiondialogiter.h"
#include "solution.h"
#include "textedit.h"
#include "graphicswidget.h"
#include "graph.h"
#include <QColorDialog>
#include <QColor>
#include <QDebug>
#include <QMessageBox>
static int i = 0;
SolutionDialogIter::SolutionDialogIter(TextEdit *tex, GraphicsWidget *gw, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SolutionDialog2)
{
    en_text = tex;
    en_view = gw;
    ui->setupUi(this);
    argExtensionColor = "blue";
    extn = new Solution(en_text,en_view);
    connect(ui->toolButton,SIGNAL(clicked(bool)),this,SLOT(colorArgInExtension()));
    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(nextgatherData()));
}

SolutionDialogIter::~SolutionDialogIter()
{
    delete ui;
}

//check empty solution. Not required. HELPME: Not used.
bool SolutionDialogIter::checkEmpty(QString sol)
{
    if (sol.contains("[]"))
        return true;
    else
        return false;
}


//choose the color of the args to be changed to.
void SolutionDialogIter::colorArgInExtension()
{
    QColor color = QColorDialog::getColor( Qt::cyan, this,
                                           "Change the color of all Arguments" );
    if (color.isValid()) {
        argExtensionColor = color.name();
        QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
        QApplication::restoreOverrideCursor();
        qDebug() << "ready...";
    }
    else {
        qDebug() << "discarded.";
    }
}

//Get the next extension
void SolutionDialogIter::nextgatherData()
{
    if (en_text->toPlainText().isEmpty()) {
        return;
    }
    qDebug () <<"getSoltionSet()";
    QString sol= en_text->toPlainText();
    if ( sol == "[[]]" ) {
        ui->textEdit->setText("{}");
        return;
    } else if (sol == "[]") {
        ui->textEdit->setText("No solution.");
        qDebug() << "No solution !!" ;
        return;
    } else if (sol.contains("[[") && sol.contains("]]") && !sol.contains("],[")) {
        en_view->setAllArgumentColor();
        sol.remove("[[");
        sol.remove("]]");
        sol = sol.simplified();
        QStringList listSol1 = sol.split(",");
        ui->textEdit->setText(listSol1.join(","));
        en_view->setSolutionColor(argExtensionColor.name(), listSol1);
        return;
    }
    sol.remove("]]");
    sol.remove("[[");
    QStringList listSol = sol.split("],[");
    qDebug() << listSol;
    if (listSol[i].isEmpty()) {
        ui->textEdit->setText("{}");
        qDebug() << "Click Previous all solution list traversed!!";
        QMessageBox::critical(this, "Tip!!",tr("No more set of Enumeration \n"
                                               ), "OK",0);

    } else if (i < listSol.length()){
        en_view->setAllArgumentColor();
        QStringList listSol1 = extn->getNextSolutionSet(listSol[i]);
        ui->textEdit->setText(listSol1.join(","));
        en_view->setSolutionColordial(argExtensionColor.name(), listSol1);
        i++;
        if (i >= listSol.length()) {
            i = 0;
            return;
        }
        qDebug() << "counter:i =" << i;
    } else {
        i=0;
        return;
    }
}


