/***************************************************************************
    solutionVizArg: Visualization of Solutions of Abstract Argumentation Frameworks


    Written in Qt/C++

                             -------------------
    This software is written as a University project.
 ***************************************************************************/
#ifndef TEXTEDIT_H
#define TEXTEDIT_H

//this class deals with file open, save, saveas and reload.
//and fills the text edit space.
using namespace std;

#include <QTextEdit>
#include <QTemporaryFile>
class MainWindow;
class TextEdit : public QTextEdit
{
    Q_OBJECT
public:
    TextEdit(MainWindow *parent);
    ~TextEdit();
    QString getFileNameAddress (QString apx);
public slots:
    void selectFileOpened();
    void saveAsFile();
    void saveFile();
    void reloadFile();
protected:
private:
    QTemporaryFile tempFile;
};

#endif // TEXTEDIT_H
