/***************************************************************************
    solutionVizArg: Visualization of Solutions of Abstract Argumentation Frameworks


    Written in Qt/C++

                             -------------------
    This software is written as a University project.
 ***************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QPrinter>
#include <QTextEdit>
#include <math.h>
#include "graphicswidget.h"
#include "graph.h"
#include <QLCDNumber>
#include <QThread>
/**
  * This Class is the base class (contains all mainwindow widgets). It sets up the main
  * window and provides a menubar, toolbar and statusbar.
  * For the main view, an instance of class GraphicsWidget is
  * created which creates a graphics widget and an instance of textedit is also created
  * for the text.
  */

using namespace std;

class QMenu;
class QAction;
class QCheckBox;
class QProgressDialog;
class QSpinBox;
class Attack;
class Argument;
class Solution;
class SolutionDialogIter;
class QPushButton;
class QComboBox;	
class QGroupBox;
class QTabWidget;
class TextEdit;
class ArgumentationFramework;
class Aspartix;


typedef QHash <QString, bool> checkArgPresence;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();
    QStringList semantics;
    void initActions();
    void initMenuBar();
    void initToolBar();
    void initToolBox();
    void initStatusBar();
    void initNet();
    void initView();
    QString getCurrentComboString(QString);
    void setLastPath(QString filePath);
    QString getLastPath();

    void openContextMenu(const QPointF & mPos);
    void argCountergw();
    //progress bar for events timing
    void createProgressBar();
    void destroyProgressBar();
    int clickedargumentNumber; //it is public because we need to be visible from activegraph.
protected:
    QStringList getSoltionSetforArg(QString);
public slots:
    //FILE MENU
    void slotCreateNew();
    void slotFileClose();
    void slotPrintView();
    bool slotExportPDF();

    //EDIT MENU
    void slotSelectAll();
    void slotSelectNone();
    void slotFindArgument();
    void slotAddAttack();
    void slotAddAttackUpload(QString, QString , int , int );
    void slotRemoveArgument();
    void slotRemoveAttack();


    //solutions
    void MapReduce();
    void slotMapSize();
    void slotShapeRandom();
    void slotEnumerationSetsbyArguments();

    // LAYOUT MENU
    void slotLayoutRandom();
    void slotLayoutCircularRandom();
    void slotLayoutSpringEmbedder(bool);
    void slotLayoutFruchterman();
    void layoutFruchterman(int);
    void slotLayoutOfSolutionBySize();
    void slotLayoutLinear();
    void slotResizeArgs();
    void slotReshapeArgs();
    void slotRecolorArgs();
    void slotSetInitPos();
    void slotSolArgCol();

    //OPTIONS MENU
    void slotDisplayAttacks(bool toggle);
    void slotDisplayAttacksArrows(bool toggle);

    void slotBackgroundColor ();
    void slotAllArgumentsColor();

    //VIEW MENU
    void slotAntialiasing(bool );
    void slotPrintDebug(bool toggle);
    void slotViewToolBar(bool toggle);

    //visualize Menu
    void slotSolutionDialogIter();



    //HELP MENU
    void slotAboutQt();


    //PUBLICLY AVAILABLE SLOTS. CALLED FROM GRAPHICSVIEW
    void ArgumentInfoStatusBar(Argument*);
    void AttackInfoStatusBar (Attack*);
    void openArgumentContextMenu();
    void openAttackContextMenu() ;
    void windowInfoStatusBar(int, int);
    void graphChanged();

    //Called by graphicswidget to update Argument coords in Graph
    void updateArgumentCoords(int no, int x, int y);

    //called when upload a file
    void slotaddArgumentUpload(bool stringName_a , QString nameArg);

    //Called when user pushes the New Argument button on the MW
    void addArgument();

    //Called by graphicswidget when the user middle-clicks
    void addAttack (QString str1, QString str2, int v1, int v2, float weight);

    //Called by graphicswidget when the user double-clicks
    void addArgumentWithMouse(int, QPointF);

    //Called by Graph to display some message to the user
    void statusMessage(const QString);
    void showMessageToUser(const QString);

    //Called from MW
    void toolBoxLayoutByIndexButtonPressed();

    QList<QGraphicsItem *> selectedArguments();

    //current index for semantic combo box
    void setCurrentIndex(QString);
    void callAspartixProcess(bool);

    //slot for reload of text from initial upload
    void reloadAF();
    void loadFromText();

    void slotLayoutLinearRandom();
    void setSizeOfArgument();
    int activeAttacks();
    //void slotAboutTool();
    void slotSceptical();
protected:
    void resizeEvent( QResizeEvent * );
    void closeEvent( QCloseEvent* );
signals:
    void comboIndex(QString);
    void toMaptheSolutions(QString);
    void setColorwithSize();
    void signalMapReduce();
private:
    TextEdit *text;
    Solution *extn;
    ArgumentationFramework *argumentFramework;
    GraphicsWidget *graphicsWidget;
    QGraphicsScene *scene;
    Graph activeGraph;
    SolutionDialogIter *visSolutionIter;
    QPrinter *printer;
    QToolBar *toolBar;
    QComboBox  *semanticCombo;
    QTabWidget *toolBox;
    QProgressDialog *progressDialog;
    Argument *clickedargument;
    Attack *clickedAttack;
    QSpinBox *rotateSpinBox;
    //menu
    QMenu *visualizeMenu, *reSizeShapeSolMenu, *layoutSolMenu, *sizeShapeSolMenu;
    QMenu *exportSubMenu, *editMenu, *helpMenu;
    QMenu *optionsMenu,*viewOptionsMenu;
    QMenu *editArgumentMenu, *editAttackMenu, *layoutMenu;
    QMenu *fileMenu, *colorChange;
    QMenu *randomLayoutMenu, *circleLayoutMenu, *physicalLayoutMenu;
    //For spring and force layout
    QCheckBox *layoutEadesBx, *layoutFruchtermanBx;
    //For Layout of Random and Circular
    QComboBox *toolBoxLayoutByIndexSelect, *toolBoxLayoutByIndexTypeSelect;
    //arguments
    QPushButton *addArgumentBt, *addAttackBt, *removeArgumentBt, *removeAttackBt;
    //file options
    QAction *fileNew, *fileOpen, *fileSave, *fileSaveAs,*fileClose, *printGraph,*fileQuit;
    QAction *exportPDF;
    //visualize
    QAction *visualizeIter,*visualizeSize,*visualizeShape, *visualizeCircle, *visualizeLinear, *visualizecol;
    QAction *resizeSolArgs, *reshapeSolArgs , *reColorInitArg;
    //for qhash and used in get extensions for a single argument
    checkArgPresence ArgPresence;

    QAction *selectNoneAct, *selectAllAct;
    QAction *findArgumentAct,*addArgumentAct, *addAttackAct, *removeArgumentAct, *removeAttackAct;

    QAction *displayAttacksArrowsAct;
    QAction *viewToolBar, *helpAboutQt;//*helpAboutTool;
    QAction *antialiasingAct;
    QAction *printDebugAct;
    QAction *getEnumerations;
    QAction *createGraphfromText, *reloadArgumentationFramework,*loadFromTextB, *initPosArg;
    QAction *randLayoutAct, *randCircleLayoutAct,*clearGuidesAct;
    QAction *springLayoutAct, *FRLayoutAct;
    QAction *zoomInAct, *zoomOutAct ;
    QAction *nextMapReduce;
    QAction *callASPARTIX;
    QAction *changeBackColorAct,*changeColorAllArgs;
    QAction *calculationData;
    QAction *scepticalSol;


    QString fileName, previous_fileName;
    QString dataDir, lastUsedDirPath;
    QStringList fileNameNoPath, rgbValues;
    QStringList tempFileNameNoPath;
    int statusBarDuration,  minDuration, progressCounter;
    int maxArguments;
    int initArgumentSize, labelDistance, numberDistance,initNumberSize, initLabelSize;
    bool fileLoaded, checkSelectFileType;
    bool afModified;
    bool bezier,  AttackClicked, ArgumentClicked, markedArgumentsExist, firstTime;
    QString initAttackColor, initNumberColor,  initArgumentShape, initLabelColor;
    QColor initBackgroundColor;
    QPointF cursorPosGW;	//Carries the position of the cursor in graphicsWidget coordinates
    QLCDNumber *nodesLCD, *edgesLCD,*extensionNumP;
    Aspartix *process;
    bool present;
    QString initArgumentColor;

    bool aspartixClicked;
};
#endif 

