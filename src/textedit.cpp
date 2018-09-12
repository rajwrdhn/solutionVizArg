/***************************************************************************
    solutionVizArg: Visualization of Solutions of Abstract Argumentation Frameworks


    Written in Qt/C++

                             -------------------
    This software is written as a University project.
 ***************************************************************************/
#include "textedit.h"
#include "mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QTextDocumentWriter>
#include <QTemporaryFile>


QString apxFile;
QString streamAF;

//constructor
TextEdit::TextEdit(MainWindow *parent)
    :QTextEdit(parent)
{
    setMaximumWidth(300);
}


//Destructor
TextEdit::~TextEdit()
{
    apxFile.clear();
    clear();
}

/**
 * select the apx file opened for further evaluation.
 * call from mainwindow
 */
void TextEdit::selectFileOpened()
{
    //this->clear();
    //apxFile.clear();
    apxFile=QFileDialog::getOpenFileName(this,tr("open text"),
                                         "/home",
                                         tr("text files(*.apx *.txt)"));
    QFile *file = new QFile(apxFile);
    if (file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "apx file Opened(file address and name):" << apxFile;
        while(!file->atEnd()){
            QByteArray aframework=file->readAll();
            this->insertPlainText(aframework);
        }

    file->close();
    } else {
        qDebug() << "File Not Selected!!";
    }
}


/**
 * @brief MainWindow::saveAsFile
 * save the text on text browser as apx file.
 * call from mainwindow
 *
 */
void TextEdit::saveAsFile()
{
    apxFile = QFileDialog::getSaveFileName(this,tr("save file"),
                                           "/home",
                                           tr("File (*.apx)"));
    QFile fileName;
    fileName.setFileName(apxFile);
    if (fileName.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug ( "Save As apx File!!");
    }

    QTextStream outText(&fileName);
    outText.setCodec("UTF-8");
    outText << this->toPlainText();
    fileName.close();
}


//returns file name along with address as one string.
QString TextEdit::getFileNameAddress(QString apx)
{
    apx = apxFile;
    return apx;
}


// Save data from text edit as a temporary file.
void TextEdit::saveFile()
{
    apxFile.clear();
    tempFile.open();
    apxFile = tempFile.fileName();
    QTextStream outText(&tempFile);
    outText.setCodec("UTF-8");
    outText << this->toPlainText();
    outText.flush();
}

//reload the main apx file again.
void TextEdit::reloadFile()
{
    this->clear();
    QFile file(apxFile);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        while(!file.atEnd()){
            QByteArray aframework=file.readAll();
            this->insertPlainText(aframework);
        }
        file.close();
    }
}

