/***************************************************************************
    solutionVizArg: Visualization of Solutions of Abstract Argumentation Frameworks


    Written in Qt/C++

                             -------------------
    This software is written as a University project.
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <QtWidgets>
#include <QtGlobal>
#include <QtDebug>
#include <QPrintDialog>
#include <QKeySequence>
#include "argumentationframework.h"
#include "mainwindow.h"
#include "graphicswidget.h"
#include "argument.h"
#include "attack.h"
#include "textedit.h"
#include "aspartix.h"
#include "solution.h"
#include "solutiondialogiter.h"
#include <QObject>

bool printDebug = false;
QString currentCombo;
//debug messages
void myMessageOutput (
        QtMsgType type, const QMessageLogContext &context, const QString &msg )
{
    QByteArray localMsg = msg.toLocal8Bit();
    Q_UNUSED(context);
    if (printDebug)
        switch ( type ) {
        case QtDebugMsg:
            fprintf( stderr, "Debug: %s\n", localMsg.constData() );
            break;
        case QtWarningMsg:
            fprintf( stderr, "Warning: %s\n", localMsg.constData() );
            break;
        case QtFatalMsg:
            fprintf( stderr, "Fatal: %s\n", localMsg.constData() );
            abort();                    // deliberately core dump
        case QtCriticalMsg:
            fprintf( stderr, "Critical: %s\n", localMsg.constData() );
            abort();                    // deliberately core dump
        }
}


/**
 * MainWindow contruction method
**/
MainWindow::MainWindow() {

    qInstallMessageHandler( myMessageOutput );
    setWindowIcon (QIcon(":/images/af.png"));

    /** inits that invoke all other construction parts **/
    initActions();  //register and construct menu Actions
    initMenuBar();  //construct menu
    initToolBar();  //build the toolbar
    initStatusBar();  //and now add the status bar.
    initToolBox(); //finally, build the toolbox

    //set MW minimum size, before creating scene and canvas
    this->setMinimumSize(900,600);

    initView(); //create the canvas

    //Connect some signals to/from the canvas and the Graph
    connect( graphicsWidget, SIGNAL( selectedArgument(Argument*) ),
             this, SLOT( ArgumentInfoStatusBar(Argument*) ) 	);

    connect( graphicsWidget, SIGNAL( selectedAttack(Attack*) ),
             this, SLOT ( AttackInfoStatusBar(Attack*) )  );

    connect( graphicsWidget, SIGNAL( userDoubleClicked(int, QPointF) ),
             this, SLOT( addArgumentWithMouse(int,QPointF) ) ) ;

    connect( graphicsWidget, SIGNAL( userMiddleClicked(QString, QString,int,int, float) ),
             this, SLOT( addAttack(QString, QString,int,int, float) ) 	);

    connect( semanticCombo,SIGNAL(currentIndexChanged(QString)),
             this,SLOT(setCurrentIndex(QString)) );

    connect( argumentFramework,SIGNAL(drawNode(bool,QString)),
             this,SLOT(slotaddArgumentUpload(bool,QString)) );

    connect( argumentFramework,SIGNAL(drawEdge(QString,QString,int,int)),
             this,SLOT(slotAddAttackUpload(QString,QString,int,int)) );

    connect( graphicsWidget, SIGNAL( openArgumentMenu() ),
             this, SLOT( openArgumentContextMenu() ) ) ;

    connect( graphicsWidget, SIGNAL( openAttackMenu() ),
             this, SLOT( openAttackContextMenu() ) ) ;

    connect (graphicsWidget, &GraphicsWidget::openContextMenu,
             this, &MainWindow::openContextMenu);

    connect( graphicsWidget, SIGNAL(updateArgumentCoords(int, int, int)),
             this, SLOT( updateArgumentCoords(int, int, int) ) );

    connect( &activeGraph, SIGNAL( addGuideCircle(int, int, int) ),
             graphicsWidget, SLOT(  addGuideCircle(int, int, int) ) ) ;

    connect( &activeGraph, SIGNAL( addGuideHLine(int) ),
             graphicsWidget, SLOT(  addGuideHLine(int) ) ) ;

    connect( &activeGraph, SIGNAL( moveArgument(int, qreal, qreal) ),
             graphicsWidget, SLOT( moveArgument(int, qreal, qreal) ) ) ;

    connect( &activeGraph, SIGNAL( moveArgument(int, qreal) ),
             graphicsWidget, SLOT( moveArgument(int, qreal) ) ) ;

    connect( fileSaveAs, SIGNAL(triggered()),
             text, SLOT(saveAsFile()));

    connect( fileOpen, SIGNAL(triggered()) ,
             text, SLOT(selectFileOpened() ) );

    connect( fileSave, SIGNAL(triggered()),
             text, SLOT(saveFile()) );

    connect( rotateSpinBox, SIGNAL(valueChanged(int)),
             graphicsWidget, SLOT( rot(int) ) );

    connect( fileOpen,SIGNAL(triggered()),
             argumentFramework,SLOT(getAllArguments()) );

    connect( fileOpen,SIGNAL(triggered()),
             argumentFramework,SLOT(getAllAttacks()) );

    connect( &activeGraph,SIGNAL(drawArgument( QString, int ,int,  QString, QPointF, QString, bool)),
             graphicsWidget,SLOT(drawArgument( QString, int ,int,  QString, QPointF, QString, bool))) ;

    connect( &activeGraph, SIGNAL( eraseAttack(int, int)),
             graphicsWidget, SLOT( eraseAttack(int, int) ) );

    connect( &activeGraph, SIGNAL( graphChanged() ),
             this, SLOT( graphChanged() ) ) ;

    connect( &activeGraph,SIGNAL(drawAttack( QString,QString,int, int, float, bool, bool, QString, bool, bool)),
             graphicsWidget,SLOT(drawAttack( QString,QString,int, int,float, bool, bool, QString, bool,bool))  ) ;

    connect( &activeGraph, SIGNAL( drawAttackReciprocal(QString,QString,int, int,bool) ),
             graphicsWidget, SLOT( drawAttackReciprocal(QString,QString,int, int,bool) ) );


    connect( &activeGraph, SIGNAL( changeAttackColor(long int,long int,QString)),
             graphicsWidget, SLOT( setAttackColor(long int,long int,QString) ) );


    connect( &activeGraph, SIGNAL( statusMessage (QString) ),
             this, SLOT( statusMessage (QString) ) ) ;

    connect( &activeGraph, SIGNAL( describeDataset (QString) ),
             this, SLOT( showMessageToUser (QString) ) ) ;

    connect( &activeGraph, SIGNAL( eraseArgument(long int) ),
             graphicsWidget, SLOT(  eraseArgument(long int) ) );
    connect( extn , SIGNAL(signalsetSizeOfArgument()),this,SLOT(setSizeOfArgument()) );

    connect( &activeGraph, SIGNAL( setAttackVisibility (int, int, int, bool) ),
             graphicsWidget, SLOT(  setAttackVisibility (int, int, int, bool) ) );

    connect( &activeGraph, SIGNAL( setVertexVisibility(long int, bool)  ),
             graphicsWidget, SLOT(  setArgumentVisibility (long int ,  bool) ) );

    connect( &activeGraph, SIGNAL( setArgumentSize(long int, int)  ),
             graphicsWidget, SLOT(  setArgumentSize (long int , int) ) );

    connect( &activeGraph, SIGNAL( setArgumentShape(const long int, const QString)  ),
             graphicsWidget, SLOT(  setArgumentShape (const long int , const QString) ) );

    connect( &activeGraph, SIGNAL( setArgumentColor(long int,QString))  ,
             graphicsWidget, SLOT(  setArgumentColor(long int, QString) ) );

    //connect some signals/slots with MW widgets
    connect( addArgumentBt,SIGNAL(clicked()), this, SLOT( addArgument() ) );

    connect( addAttackBt,SIGNAL(clicked()), this, SLOT( slotAddAttack() ) );

    connect( removeArgumentBt,SIGNAL(clicked()), this, SLOT( slotRemoveArgument() ) );

    connect( removeAttackBt,SIGNAL(clicked()), this, SLOT( slotRemoveAttack() ) );

    connect( clearGuidesAct, SIGNAL(triggered()),
             graphicsWidget, SLOT(clearGuides()));
    connect(&activeGraph,SIGNAL(signalRemoveGuides()),graphicsWidget,SLOT(clearGuides()) );
    //connect zoom in and zoom out
    connect( zoomOutAct, SIGNAL(triggered()), graphicsWidget, SLOT( zoomOut() ) );
    connect( zoomInAct, SIGNAL(triggered()), graphicsWidget, SLOT( zoomIn() ) );

    // create an horizontal layout for the toolbox and the canvas.
    // This will be our MW layout.
    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->addWidget(text);
    vlayout->addWidget(toolBox);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addLayout(vlayout); 		//add them
    layout->addWidget(graphicsWidget);

    //create a dummy widget, for the above layout
    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    //now set this as central widget of MW
    setCentralWidget(widget);

    /*
        initialise default parameters
    */
    qDebug()<<"   initialise default parameters";
    initNet();


    /*
     *  DEFAULTING HERE DOES NOT CHANGE BOOL VALUE
        EVERY TIME INITNET IS CALLED
    */
    bezier=false;
    firstTime=true;

    graphicsWidget->setInitArgumentColor(initArgumentColor);
    graphicsWidget->setInitNumberDistance(numberDistance);
    graphicsWidget->setInitLabelDistance(labelDistance);
    graphicsWidget->setInitArgumentSize(initArgumentSize);
    graphicsWidget->setBackgroundBrush(QBrush(initBackgroundColor));

    dataDir= QDir::homePath() +QDir::separator() + "Aspartix-data" + QDir::separator() ;
    lastUsedDirPath = QDir::homePath();
    if (firstTime) {
        QDir ourDir(dataDir);
        if ( !ourDir.exists() ) {
            ourDir.mkdir(dataDir);
            QMessageBox::information(this, "solutionVizArg Aspartix Directory",
                                 tr("solutionVizArg can use files in the "
                                    "directory %1")
                                 .arg (ourDir.absolutePath())
                                 , QMessageBox::Ok, 0);

        }
    }

    graphicsWidget->setFocus();
    statusMessage( tr("Abstract Argumentation Framework Solution Visualizer"));
}



MainWindow::~MainWindow() {
    delete printer;
    delete scene;
    delete graphicsWidget;
    delete visSolutionIter;
}



/** initializes all QActions of the application */
void MainWindow::initActions(){
    printer = new QPrinter;

    /**
    File menu actions
    */
    fileNew = new QAction(QIcon(":/images/new.png"), tr("&New"),  this);
    fileNew->setShortcut(tr("Ctrl+N"));
    fileNew->setStatusTip(tr("Create a Argumentation Framework!!"));
    fileNew->setToolTip(tr("New AF (Ctrl+N)"));
    fileNew->setWhatsThis(tr("New\n\nCreate a new Argumentation Framework"));
    connect(fileNew, SIGNAL(triggered()), this, SLOT(slotCreateNew()));

    fileOpen = new QAction(QIcon(":/images/open.png"), tr("&Open"), this);
    fileOpen->setShortcut(tr("Ctrl+O"));
    fileOpen->setToolTip(tr("Open APX File (Ctrl+O)"));
    fileOpen->setStatusTip(tr("Open an APX File for viewing as Argumentation Framework."));
    fileOpen->setWhatsThis(tr("Open \n\n Opens an apx file for viewing"));

    fileSave = new QAction(QIcon(":/images/saved.png"), tr("&Save"),  this);
    fileSave->setShortcut(tr("Ctrl+S"));
    fileSave->setToolTip(tr("Save APX in temp.(Ctrl+S)"));
    fileSave->setStatusTip(tr("Saves the apx file in temp."));
    fileSave->setWhatsThis(tr("Save.\n\nSaves the apx File as temporary File"));

    fileSaveAs = new QAction(QIcon(":/images/saveas.png"), tr("Save &As..."),  this);
    fileSaveAs->setShortcut(tr("Ctrl+Shift+S"));
    fileSaveAs->setStatusTip(tr("Saves the apx file"));
    fileSaveAs->setWhatsThis(tr("Save As\n\nSaves the apx file"));

    exportPDF = new QAction(tr("&PDF..."), this);
    exportPDF->setStatusTip(tr("Export graph(AF) to a PDF file"));
    exportPDF->setWhatsThis(tr("Export PDF\n\n Export graph(AF) to a PDF document"));
    connect(exportPDF, SIGNAL(triggered()), this, SLOT(slotExportPDF()));

    fileClose = new QAction( tr("&Close"), this);
    fileClose->setStatusTip(tr("Closes the current Argumentation Framework"));
    fileClose->setWhatsThis(tr("Close \n\nCloses the actual Argumentation Framework"));
    connect(fileClose, SIGNAL(triggered()), this, SLOT(slotFileClose()));

    printGraph = new QAction(QIcon(":/images/print.png"), tr("&Print"), this);
    printGraph->setShortcut(tr("Ctrl+P"));
    printGraph->setStatusTip(tr("Prints whatever is on the canvas. if the canvas is not empty."));
    printGraph->setWhatsThis(tr("Printing \n\n This function prints whatever is viewable on the canvas."));
    connect(printGraph, SIGNAL(triggered()), this, SLOT(slotPrintView()));

    fileQuit = new QAction(QIcon(":/images/exit.png"), tr("E&xit"), this);
    fileQuit->setShortcut(tr("Ctrl+Q"));
    fileQuit->setStatusTip(tr("Quits the application"));
    fileQuit->setWhatsThis(tr("Exit\n\nQuits the application"));
    connect(fileQuit, SIGNAL(triggered()), this, SLOT(close()));

    /**
    Edit menu actions
    */
    selectAllAct = new QAction(tr("Select All"), this);
    selectAllAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_A));
    selectAllAct->setStatusTip(tr("Selects all Arguments"));
    selectAllAct->setWhatsThis(tr("Select All\n\nSelects all Arguments in the canvas graph."));
    connect(selectAllAct, SIGNAL(triggered()), this, SLOT(slotSelectAll()));

    selectNoneAct = new QAction(tr("Deselect all"), this);
    selectNoneAct->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_A));
    selectNoneAct->setStatusTip(tr("Deselects all Arguments"));
    selectNoneAct->setWhatsThis(tr("Deselect all\n\n Clears the Argument selection"));
    connect(selectNoneAct, SIGNAL(triggered()), this, SLOT(slotSelectNone()));

    findArgumentAct = new QAction(QIcon(":/images/find.png"), tr("Find Argument"), this);
    findArgumentAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));
    findArgumentAct->setStatusTip(tr("Finds and highlights a Argument by number. Press Ctrl+F again to undo."));
    findArgumentAct->setWhatsThis(tr("Find Argument\n\nFinds a Argument with a given number or label and doubles its size. Ctrl+F again resizes back the Argument"));
    connect(findArgumentAct, SIGNAL(triggered()), this, SLOT(slotFindArgument()) );

    addArgumentAct = new QAction(QIcon(":/images/add.png"), tr("Add Argument"), this);
    addArgumentAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_X));
    addArgumentAct->setStatusTip(tr("Adds a Argument"));
    addArgumentAct->setWhatsThis(tr("Add Argument\n\nAdds a Argument to the argumentation framework."));
    connect(addArgumentAct, SIGNAL(triggered()), this, SLOT(addArgument()));

    removeArgumentAct = new QAction(QIcon(":/images/remove.png"),tr("Remove Argument"), this);
    removeArgumentAct ->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Backspace));
    //Single key shortcuts with backspace or del do not work in Mac http://goo.gl/7hz7Dx
    removeArgumentAct->setStatusTip(tr("Removes an Argument"));
    removeArgumentAct->setWhatsThis(tr("Remove Argument\n\nRemoves an Argument from the argumentation framework."));
    connect(removeArgumentAct, SIGNAL(triggered()), this, SLOT(slotRemoveArgument()));

    addAttackAct = new QAction(tr("Add Attack"),this);
    addAttackAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
    addAttackAct->setStatusTip(tr("Adds a Attack from an Argument to another"));
    addAttackAct->setWhatsThis(tr("Add Attack\n\nAdds a directed Attack from an Argument to another"));
    connect(addAttackAct, SIGNAL(triggered()), this, SLOT(slotAddAttack()));

    getEnumerations = new QAction(tr("GetExtension"),this);
    getEnumerations->setStatusTip("get Extension in which this argument is present");
    getEnumerations->setWhatsThis(tr("Extensions in which the entered argument is present!!"));
    connect (getEnumerations,SIGNAL(triggered()), this, SLOT(slotEnumerationSetsbyArguments()));

    removeAttackAct = new QAction(tr("Remove"), this);
    //removeAttackAct ->setShortcut(QKeySequence(Qt::SHIFT+Qt::Key_Backspace));
    removeAttackAct ->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));
    removeAttackAct->setStatusTip(tr("Removes an Attack"));
    removeAttackAct->setWhatsThis(tr("Remove Attack\n\nRemoves an Attack from the argumentation framework"));
    connect(removeAttackAct, SIGNAL(triggered()), this, SLOT(slotRemoveAttack()));


    changeBackColorAct = new QAction(QIcon(":/images/color.png"),tr("Change Background Color"), this);
    changeBackColorAct->setStatusTip(tr("Click to change the background color"));
    changeBackColorAct->setWhatsThis(tr("Background\n\nChanges background color"));
    connect(changeBackColorAct, SIGNAL(triggered()), this, SLOT(slotBackgroundColor()));

    changeColorAllArgs = new QAction(QIcon(":/images/color.png"),tr("Change All Arg Color"), this);
    changeColorAllArgs->setStatusTip(tr("Click to change all arg color"));
    changeColorAllArgs->setWhatsThis(tr("All Args color changes"));
    connect(changeColorAllArgs, SIGNAL(triggered()), this, SLOT(slotAllArgumentsColor()));
    /**
    Layout menu actions
    */

    randLayoutAct = new QAction( tr("Random"),this);
    randLayoutAct -> setShortcut(Qt::CTRL+Qt::Key_0);
    randLayoutAct -> setStatusTip(tr("Repositions all Arguments in random places"));
    randLayoutAct -> setWhatsThis(tr("Random Layout\n\n Repositions all Arguments in random places"));
    connect(randLayoutAct, SIGNAL(triggered()), this, SLOT(slotLayoutRandom()));


    randCircleLayoutAct = new QAction(tr("Random Circles"),	this);
    randCircleLayoutAct -> setShortcut(Qt::CTRL+Qt::ALT+Qt::Key_0);
    randCircleLayoutAct ->setStatusTip(tr("Repositions the Arguments randomly on circles"));
    randCircleLayoutAct->
            setWhatsThis(
                tr("Random Circles Layout\n\n Repositions the Arguments randomly on circles"));
    connect(randCircleLayoutAct, SIGNAL(triggered()), this, SLOT(slotLayoutCircularRandom()));

    clearGuidesAct = new QAction(tr("Remove Layout GuideLines"), this);
    clearGuidesAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_5));
    clearGuidesAct ->setStatusTip(tr("Removes all layout guideLines from the canvas."));
    clearGuidesAct->setWhatsThis(tr("Remove GuideLines\n\n Removes any guidelines"
                                    " (circles or horizontal lines) created for the af layout."));

    springLayoutAct= new QAction(tr("Spring Embedder (Eades)"), this);
    springLayoutAct->setShortcut(tr("Alt+1"));
    springLayoutAct->setCheckable(true);
    springLayoutAct->setChecked(false);
    springLayoutAct->setStatusTip(tr("All Arguments repel each other while the connected ones are attracted as if connected by springs."));
    springLayoutAct->setWhatsThis(tr("Dynamic Layout Arrangement of Graph!!"));
    connect(springLayoutAct, SIGNAL(triggered(bool)), this, SLOT(slotLayoutSpringEmbedder(bool)));

    FRLayoutAct= new QAction( tr("Fruchterman-Reingold"),	this);
    FRLayoutAct->setShortcut(tr("Alt+2"));
    FRLayoutAct->setCheckable(true);
    FRLayoutAct->setChecked(false);
    FRLayoutAct->setStatusTip(tr("Repelling forces between all Arguments, and attracting forces between adjacent Arguments."));
    FRLayoutAct->setWhatsThis(tr("Dynamic Layout Arrangement of Graph!!"));
    connect(FRLayoutAct, SIGNAL(triggered()), this, SLOT(slotLayoutFruchterman()));


    /**
    Options menu actions
    */
    displayAttacksArrowsAct = new QAction( tr("Display Arrows"),this);
    displayAttacksArrowsAct->setStatusTip(tr("Toggles displaying of arrows on Attacks"));
    displayAttacksArrowsAct->setWhatsThis(tr("Display Arrows\n\nClick to enable or disable displaying of arrows on Attacks"));
    displayAttacksArrowsAct->setCheckable(true);
    displayAttacksArrowsAct->setChecked(true);
    connect(displayAttacksArrowsAct, SIGNAL(toggled(bool)), this, SLOT(slotDisplayAttacksArrows(bool)) );

    /**
    Options > View menu actions
    */

    antialiasingAct = new QAction(tr("Anti-Aliasing"), this);
    antialiasingAct ->setShortcut(tr("F8"));
    antialiasingAct ->setStatusTip(tr("Enables/disables anti-aliasing"));
    antialiasingAct ->setWhatsThis(tr("Enable or disable Anti-Aliasing\n\n Anti-aliasing is a technique which makes Arguments, lines and text, smoother and fancier. But it comes at the cost of speed..."));
    antialiasingAct ->setCheckable(true);
    antialiasingAct ->setChecked (true);
    connect(antialiasingAct , SIGNAL(toggled(bool)), this, SLOT(slotAntialiasing(bool)));

    printDebugAct = new QAction(tr("Debug Messages"),	this);
    printDebugAct ->setShortcut(tr("F9"));
    printDebugAct->setStatusTip(tr("Enables/disables printing debug messages to stdout"));
    printDebugAct->setWhatsThis(tr("Enables/disable Debug Messages\n\nPrinting debug messages to strerr."));
    printDebugAct->setCheckable(true);
    printDebugAct->setChecked (false);
    printDebug=false;
    connect(printDebugAct, SIGNAL(toggled(bool)), this, SLOT(slotPrintDebug(bool)));

    //zoom actions via ctrl +/-
    zoomInAct = new QAction(tr("Zoom &in"),  this);
    zoomInAct->setShortcut(Qt::CTRL + Qt::Key_Plus);
    zoomInAct->setToolTip(tr("Zoom in"));
    zoomInAct->setStatusTip(tr("Zooms inside the actual AF."));
    zoomInAct->setWhatsThis(tr("Zoom In.\n\nZooms in."));

    zoomOutAct = new QAction(tr("Zoom &out"),  this);
    zoomOutAct->setShortcut(Qt::CTRL + Qt::Key_Minus);
    zoomOutAct->setToolTip(tr("Zoom out"));
    zoomOutAct->setStatusTip(tr("Zooms out of the actual AF."));
    zoomOutAct->setWhatsThis(tr("Zoom out.\n\nZooms out."));



    viewToolBar = new QAction(tr("Toolbar"), this);
    viewToolBar->setStatusTip(tr("Enables/disables the toolbar"));
    viewToolBar->setWhatsThis(tr("Enable or disable Toolbar\n\nThe toolbar is the widget right below the menu, and carries useful icons. You can disable it if you like..."));
    viewToolBar->setCheckable(true);
    viewToolBar->setChecked(true);
    connect(viewToolBar, SIGNAL(toggled(bool)), this, SLOT(slotViewToolBar(bool)));

    callASPARTIX = new QAction(tr("Aspartix"),this);
    callASPARTIX->setEnabled(false);
    callASPARTIX->setText("Aspartix");
    callASPARTIX->setToolTip("Run Aspartix after upload apx file");
    connect( callASPARTIX,SIGNAL(triggered(bool)),
             this,SLOT(callAspartixProcess(bool)) );

    nextMapReduce = new QAction(tr("Visualize"),this);
    nextMapReduce->setEnabled(false);
    nextMapReduce->setText("Visualize");
    nextMapReduce->setToolTip("Visualization of the Solution.");
    connect( nextMapReduce,SIGNAL(triggered()),this,SLOT(MapReduce()) );

    //reload AF
    reloadArgumentationFramework = new QAction(tr("Reload AF"),this);
    reloadArgumentationFramework->setEnabled(false);
    reloadArgumentationFramework->setText("Reload AF");
    reloadArgumentationFramework->setToolTip("Reload the Full dung's argumentation framework of the current Solution Set!!");
    connect (reloadArgumentationFramework,SIGNAL(triggered()),this, SLOT(reloadAF()));

    //set initial position of arguments
    initPosArg = new QAction(tr("Init Pos"),this);
    initPosArg->setText("Initial Position");
    initPosArg->setToolTip("Put all arguments back to the initial position!!");
    connect(initPosArg,SIGNAL(triggered()),this,SLOT(slotSetInitPos()) );

    loadFromTextB = new QAction(tr("Load AF as Graph"),this);
    loadFromTextB->setText("Load AF from text ");
    loadFromTextB->setToolTip("create argumentation framework from writing text in text Box in Aspartix format!!");
    connect (loadFromTextB,SIGNAL(triggered()),this, SLOT(loadFromText()));

    /**
    Visualize Menu Methods and Properties
    */
    visualizeIter = new QAction(tr("Visualize by Iteration."),this);
    visualizeIter->setDisabled(true);
    connect (visualizeIter,SIGNAL(triggered()),this, SLOT(slotSolutionDialogIter()));

    visualizeSize = new QAction(tr("Degree of Acceptance by Size"),this);
    visualizeSize->setDisabled(true);
    connect (visualizeSize,SIGNAL(triggered()),this,SLOT(slotMapSize()));

    visualizeShape = new QAction(tr("Credulous Acceptance by Shape"),this);
    visualizeShape->setDisabled(true);
    connect (visualizeShape,SIGNAL(triggered()),this,SLOT(slotShapeRandom()));


    visualizeCircle = new QAction(tr("Circular layout"), this);
    visualizeCircle->setDisabled(true);
    connect(visualizeCircle,SIGNAL(triggered()),this,SLOT(slotLayoutOfSolutionBySize()) );

    visualizeLinear = new QAction(tr("Linear layout"), this);
    visualizeLinear->setDisabled(true);
    connect(visualizeLinear,SIGNAL(triggered()),this,SLOT(slotLayoutLinear()) );

    visualizecol = new QAction(tr("Credulous Acceptance by Color"), this);
    visualizecol->setDisabled(true);
    connect(visualizecol,SIGNAL(triggered()),this,SLOT(slotSolArgCol()) );

    resizeSolArgs = new QAction (tr("Resize to Normal"),this);
    resizeSolArgs->setDisabled(true);
    connect(resizeSolArgs,SIGNAL(triggered()),this,SLOT(slotResizeArgs()) );

    reshapeSolArgs = new QAction (tr("Reshape All args &Circle"),this);
    reshapeSolArgs->setDisabled(true);
    connect(reshapeSolArgs,SIGNAL(triggered()),this,SLOT(slotReshapeArgs()) );

    reColorInitArg = new QAction (tr("Init Color of Args."), this);
    reColorInitArg->setDisabled(true);
    connect(reColorInitArg,SIGNAL(triggered()),this,SLOT(slotRecolorArgs()) );


    scepticalSol = new QAction(tr("skeptical Acceptance by Color"),this);
    scepticalSol->setEnabled(false);
    connect(scepticalSol,SIGNAL(triggered()),this,SLOT(slotSceptical()) );
    /**
    *Help menu actions
    */
    helpAboutQt = new QAction(tr("About Qt"), this);
    helpAboutQt->setStatusTip(tr("About Qt"));
    helpAboutQt->setWhatsThis(tr("About\n\nAbout Qt"));
    connect(helpAboutQt, SIGNAL(triggered()), this, SLOT(slotAboutQt() ) );
    //not used
    //helpAboutTool = new QAction(tr("Manual"), this);
    //helpAboutTool->setStatusTip(tr("Manual"));
    //helpAboutTool->setWhatsThis(tr("Help \nManual"));
    //connect(helpAboutTool, SIGNAL(triggered()), this, SLOT(slotAboutTool() ) );
}



/**
  Creates and populates the MenuBar
*/
void MainWindow::initMenuBar() {


    /** menuBar for File Menu*/
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu -> addAction(fileNew);
    fileMenu -> addAction(fileOpen);
    fileMenu -> addSeparator();

    fileMenu  -> addAction(fileSave);
    fileMenu  -> addAction(fileSaveAs);
    fileMenu  -> addSeparator();

    exportSubMenu = fileMenu  -> addMenu(tr("Export..."));
    exportSubMenu -> addAction (exportPDF);
    exportSubMenu -> addSeparator();

    fileMenu  -> addAction(printGraph);
    fileMenu  -> addSeparator();

    fileMenu  -> addAction(fileClose);
    fileMenu  -> addAction(fileQuit);

    /** menuBar entry editMenu */
    editMenu = menuBar()->addMenu(tr("&Edit"));
    editArgumentMenu = new QMenu(tr("Argument..."));
    editArgumentMenu -> setIcon(QIcon(":/images/Argument.png"));
    editMenu -> addMenu ( editArgumentMenu );
    editArgumentMenu -> addAction (selectAllAct);
    editArgumentMenu -> addAction (selectNoneAct);
    editArgumentMenu -> addSeparator();
    editArgumentMenu -> addAction (findArgumentAct);
    editArgumentMenu -> addAction (addArgumentAct);
    editArgumentMenu -> addAction (removeArgumentAct);
    editArgumentMenu -> addSeparator();


    editAttackMenu = new QMenu(tr("Attack..."));
    editAttackMenu -> setIcon(QIcon(":/images/line.png"));
    editMenu-> addMenu (editAttackMenu);
    editAttackMenu -> addAction(addAttackAct);
    editAttackMenu -> addAction(removeAttackAct);
    editAttackMenu  ->addSeparator();
    editMenu ->addSeparator();


    colorChange = new QMenu ( tr("Color change"));
    editMenu ->addMenu(colorChange);
    colorChange -> addAction(changeBackColorAct );
    colorChange -> addAction(changeColorAllArgs);

    editMenu -> addAction(zoomInAct);

    editMenu -> addAction(zoomOutAct);
    /** menuBar entry layoutMenu  */

    layoutMenu = menuBar()->addMenu(tr("&GraphLayout"));
    randomLayoutMenu = new QMenu(tr("Random..."));
    layoutMenu -> addMenu (randomLayoutMenu );
    randomLayoutMenu ->  addAction(randLayoutAct);
    randomLayoutMenu ->  addAction( randCircleLayoutAct );
    layoutMenu->addSeparator();
    physicalLayoutMenu = new QMenu (tr("Force/Spring..."));
    layoutMenu -> addMenu (physicalLayoutMenu);
    physicalLayoutMenu -> addAction (springLayoutAct);
    physicalLayoutMenu -> addAction (FRLayoutAct);

    layoutMenu->addSeparator();
    layoutMenu -> addAction (clearGuidesAct);

    /** menuBar entry optionsMenu  */
    optionsMenu = menuBar()->addMenu(tr("&Options"));
    viewOptionsMenu = new QMenu (tr("&View..."));
    optionsMenu -> addMenu (viewOptionsMenu);
    viewOptionsMenu-> addAction (antialiasingAct);
    viewOptionsMenu-> addAction (printDebugAct);
    viewOptionsMenu-> addAction (viewToolBar);

    /** menu bar visualize */

    visualizeMenu = menuBar()->addMenu(tr("&Visualize"));

    sizeShapeSolMenu = new QMenu (tr("Credulous Acceptance"));
    visualizeMenu->addMenu(sizeShapeSolMenu);
    sizeShapeSolMenu->addAction(visualizeShape);
    sizeShapeSolMenu->addAction(visualizecol);
    visualizeMenu->addSeparator();
    visualizeMenu->addAction(scepticalSol);
    visualizeMenu->addSeparator();
    layoutSolMenu = new QMenu (tr("Degree of Acceptance.."));
    visualizeMenu->addMenu(layoutSolMenu);
    layoutSolMenu->addAction(visualizeSize);
    layoutSolMenu->addAction(visualizeLinear);
    layoutSolMenu->addAction(visualizeCircle);
    visualizeMenu->addSeparator();
    visualizeMenu->addAction(visualizeIter);
    visualizeMenu->addSeparator();
    reSizeShapeSolMenu = new QMenu (tr("Resize...Reshape...Recolor"));
    visualizeMenu->addMenu(reSizeShapeSolMenu);
    reSizeShapeSolMenu->addAction(resizeSolArgs);
    reSizeShapeSolMenu->addAction(reshapeSolArgs);
    reSizeShapeSolMenu->addAction(reColorInitArg);

    visualizeMenu->addAction(clearGuidesAct);

    /**  menuBar entry helpMenu */
    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu -> addSeparator();
    helpMenu-> addAction (helpAboutQt);
    //helpMenu->addSeparator();
    //helpMenu->addAction(helpAboutTool);
}

/**
    Initializes the toolbar
*/
void MainWindow::initToolBar(){
    toolBar = addToolBar("operations");

    toolBar -> addAction (fileNew);
    toolBar -> addAction (fileOpen);
    toolBar -> addAction (fileSave);
    toolBar -> addAction (printGraph);
    toolBar -> addSeparator();

    //Create relation select widget
    QLabel *labelsemanticSelect= new QLabel;
    labelsemanticSelect ->setText(tr("Select Semantics"));
    toolBar -> addWidget (labelsemanticSelect);

    //Add the semantics for the aspartix . In order to add a new semantics add It to the semantics string list.
    semanticCombo = new QComboBox;
    semantics << "ST" << "PR" << "CO" << "GR";
    semanticCombo->addItems(semantics);
    toolBar -> addWidget(semanticCombo);

    toolBar -> addSeparator();


    toolBar->addAction(callASPARTIX);
    toolBar->addSeparator();

    toolBar->addAction(nextMapReduce);
    toolBar->addSeparator();

    toolBar->addAction(reloadArgumentationFramework);
    toolBar->addSeparator();
    toolBar->addAction(initPosArg);
    toolBar->addSeparator();
    toolBar->addAction(loadFromTextB);
    toolBar->addSeparator();
    QLabel *labelRotateSpinBox= new QLabel;
    labelRotateSpinBox ->setText(tr("Rotate Scene:"));

    rotateSpinBox = new QSpinBox;
    rotateSpinBox ->setRange(-360, 360);
    rotateSpinBox->setSingleStep(1);
    rotateSpinBox->setValue(0);

    toolBar -> addWidget(labelRotateSpinBox);
    toolBar -> addWidget(rotateSpinBox);

    toolBar -> addSeparator();
}

//Creates a dock widget for instant menu access
void MainWindow::initToolBox(){
    toolBox = new QTabWidget;
    toolBox->setMaximumHeight(200);


    // create buttons for the Edit GroupBox
    addArgumentBt= new QPushButton(QIcon(":/images/add.png"),tr("&Add Argument"));
    addArgumentBt->setFocusPolicy(Qt::NoFocus);
    addArgumentBt->setToolTip(
                tr("Add a new argument to the AF (Ctrl+X. \n\n "
                   "Alternately, you can create a new argument \n"
                   "in a specific position by double-clicking \n"
                   "on that spot of the canvas.")
                );

    removeArgumentBt= new QPushButton(QIcon(":/images/remove.png"),tr("&Remove Argument"));
    removeArgumentBt->setFocusPolicy(Qt::NoFocus);
    removeArgumentBt->setToolTip(
                tr("Remove an argument from the AF. \n\n "
                   "Alternately, you can remove an argument \n"
                   "by right-clicking on it.")
                );

    addAttackBt= new QPushButton(QIcon(":/images/connect.png"),tr("Add &Attack"));
    addAttackBt->setFocusPolicy(Qt::NoFocus);
    addAttackBt->setToolTip(
                tr("Add a new Attack from a argument to another (Ctrl+E.\n\n "
                   "Alternately, you can create a new attack between two \n"
                   "arguments by middle-clicking on them consecutively.")
                );

    removeAttackBt= new QPushButton(QIcon(":/images/disconnect.png"),tr("Remove Attack"));
    removeAttackBt->setFocusPolicy(Qt::NoFocus);
    removeAttackBt->setToolTip(
                tr("Remove an Attack from the AF  \n\n "
                   "Alternately, you can remove an Attack \n"
                   "by right-clicking on it."
                   )
                );

    // create widgets for the "Force-Directed Models" groupBox
    layoutEadesBx = new QCheckBox(tr("Spring Layout") );
    layoutEadesBx->setEnabled(true);
    layoutEadesBx->setChecked(false);
    layoutEadesBx->setToolTip(tr("Dynamic Layout Arrangement of Graph!! slow with |Arg| > 100"));

    layoutFruchtermanBx = new QCheckBox(tr("Fruchterman-Reingold") );
    layoutFruchtermanBx->setEnabled(true);
    layoutFruchtermanBx->setChecked(false);
    layoutFruchtermanBx->setToolTip(tr("Dynamic Layout Arrangement of Graph!!"));

    //create layout for dynamic visualisation
    QGridLayout *layoutForceDirectedGrid = new QGridLayout();
    layoutForceDirectedGrid -> addWidget(layoutEadesBx, 0,0);
    layoutForceDirectedGrid -> addWidget(layoutFruchtermanBx, 1,0);
    //layoutForceDirectedGrid->setSpacing(1);
    layoutForceDirectedGrid->setMargin(0);

    //create a box for dynamic layout options
    QGroupBox *layoutDynamicBox= new QGroupBox(tr("Force-Directed Model"));
    layoutDynamicBox->setMaximumWidth(300);
    layoutDynamicBox->setLayout (layoutForceDirectedGrid );


    //Parent box with vertical layout for all layout/visualization boxes
    QVBoxLayout *visualizationBoxLayout = new QVBoxLayout;
    visualizationBoxLayout -> addWidget(layoutDynamicBox);

    QGroupBox *visualizationBox= new QGroupBox(tr("Visualize"));
    visualizationBox->setMaximumWidth(300);
    visualizationBox->setLayout (visualizationBoxLayout );

    //Parent box with vertical layout for all boxes of Controls
    QVBoxLayout *controlTabVerticalLayout = new QVBoxLayout;
    controlTabVerticalLayout -> addWidget(visualizationBox);
    controlTabVerticalLayout->setSpacing(0);
    controlTabVerticalLayout->setMargin(0);

    QGroupBox *controlGroupBox = new QGroupBox;
    controlGroupBox->setLayout(controlTabVerticalLayout);
    controlGroupBox->setMaximumWidth(300);
    controlGroupBox->setContentsMargins(0,0,0,0);
    toolBox->addTab(controlGroupBox, tr("Force Layout"));


    connect(layoutEadesBx, SIGNAL(clicked(bool)),
            this, SLOT(slotLayoutSpringEmbedder(bool)));
    connect(layoutFruchtermanBx, SIGNAL(stateChanged(int)),
            this, SLOT(layoutFruchterman(int)));

    QLabel *labelNodesLCD = new QLabel;
    labelNodesLCD->setText(tr("Total Arguments"));
    QLabel *labelEdgesLCD = new QLabel;
    labelEdgesLCD->setText(tr("Total Attacks"));


    nodesLCD=new QLCDNumber(7);
    nodesLCD->setSegmentStyle(QLCDNumber::Flat);
    nodesLCD->setToolTip(tr("Counts how many arguments in the AF!!"));

    edgesLCD=new QLCDNumber(7);
    edgesLCD->setSegmentStyle(QLCDNumber::Flat);
    edgesLCD->setToolTip(tr("Counts how many attacks in the AF!!"));



    //create a grid layout
    QGridLayout *propertiesGrid = new QGridLayout();

    propertiesGrid -> setColumnMinimumWidth(0, 10);
    propertiesGrid -> setColumnMinimumWidth(1, 10);

    propertiesGrid -> addWidget(labelNodesLCD, 0,0);
    propertiesGrid -> addWidget(labelEdgesLCD, 0,1);


    propertiesGrid -> addWidget(nodesLCD,1,0);
    propertiesGrid -> addWidget(edgesLCD,1,1);

    //create a box with title
    QGroupBox *afPropertiesGroup = new QGroupBox;
    afPropertiesGroup -> setLayout (propertiesGrid);

    toolBox->addTab( afPropertiesGroup, tr("Statistics") );
    toolBox->setFixedWidth(300);

    QLabel *labelNumberOfExtension = new QLabel;
    labelNumberOfExtension->setText("Total Extensions");

    extensionNumP=new QLCDNumber(7);
    extensionNumP->setSegmentStyle(QLCDNumber::Flat);
    extensionNumP->setToolTip(tr("Counts how many extensions \n in the AF semantics!!"));

    QGridLayout *extensionGrid = new QGridLayout();

    extensionGrid -> setColumnMinimumWidth(0,10);
    extensionGrid -> addWidget(labelNumberOfExtension,0,0);
    extensionGrid -> addWidget(extensionNumP,1,0);

    QGroupBox *extensionProperties = new QGroupBox;
    extensionProperties -> setLayout (extensionGrid);

    toolBox->addTab( extensionProperties, tr("Extensions") );
    toolBox->setFixedWidth(300);
}


/**
 * @brief MainWindow::toolBoxLayoutByIndexButtonPressed
 * selection of layout type circular or random.
 */
void MainWindow::toolBoxLayoutByIndexButtonPressed(){
    qDebug()<<"MW::toolBoxLayoutByIndexButtonPressed()";
    int selectedIndex = toolBoxLayoutByIndexSelect->currentIndex();
    QString selectedIndexText = toolBoxLayoutByIndexSelect -> currentText();
    int selectedLayoutType = toolBoxLayoutByIndexTypeSelect ->currentIndex();
    qDebug() << " selected index is " << selectedIndexText << " : " << selectedIndex
             << " selected layout type is " << selectedLayoutType;
    switch(selectedIndex) {
    case 0:
        break;
    case 1:
        if (selectedLayoutType==0)
            slotLayoutCircularRandom();
        else if (selectedLayoutType==1)
            slotLayoutRandom();
        break;
    default:
        break;
    }
}


//FIXME this is a bug: Graph calls GraphicsWidget which calls this to call Graph!
void MainWindow::updateArgumentCoords(int ArgumentNumber, int x, int y){
    //	qDebug("MW: updateArgumentCoords() for %i with x %i and y %i", ArgumentNumber, x, y);
    activeGraph.updateVertCoords(ArgumentNumber, x, y);
}




/**
    Initializes the status bar
*/
void MainWindow::initStatusBar() {
    statusBarDuration=4000;
    statusMessage( tr("Ready."));
}


/**
    Initializes the scene and its graphicsWidget, the main widget of SocNetV
*/
void MainWindow::initView() {
    qDebug ("MW initView()");
    text = new TextEdit(this);
    text->setEnabled(true);
    //create a scene
    scene=new QGraphicsScene();

    //create a view widget for this scene
    graphicsWidget=new GraphicsWidget(scene, text, this);
    argumentFramework = new ArgumentationFramework(graphicsWidget,text);
    extn = new Solution(text, graphicsWidget);
    graphicsWidget->setViewportUpdateMode( QGraphicsView::BoundingRectViewportUpdate );
    //  FullViewportUpdate  // MinimalViewportUpdate //SmartViewportUpdate  //BoundingRectViewportUpdate
    //QGraphicsView can cache pre-rendered content in a QPixmap, which is then drawn onto the viewport.
    graphicsWidget->setCacheMode(QGraphicsView::CacheNone);  //CacheBackground | CacheNone

    graphicsWidget->setRenderHint(QPainter::Antialiasing, true);
    graphicsWidget->setRenderHint(QPainter::TextAntialiasing, true);
    graphicsWidget->setRenderHint(QPainter::SmoothPixmapTransform, true);
    //Optimization flags:
    //if items do restore their state, it's not needed for graphicsWidget to do the same...
    graphicsWidget->setOptimizationFlag(QGraphicsView::DontSavePainterState, false);
    //Disables QGraphicsView's antialiasing auto-adjustment of exposed areas.
    graphicsWidget->setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, false);
    //"QGraphicsScene applies an indexing algorithm to the scene, to speed up item discovery functions like items() and itemAt().
    // Indexing is most efficient for static scenes (i.e., where items don't move around).
    // For dynamic scenes, or scenes with many animated items, the index bookkeeping can outweight the fast lookup speeds." So...
    scene->setItemIndexMethod(QGraphicsScene::BspTreeIndex); //NoIndex (for anime) | BspTreeIndex

    graphicsWidget->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    graphicsWidget->setResizeAnchor(QGraphicsView::AnchorViewCenter);

    // sets dragging the mouse over the scene while the left mouse button is pressed.
    graphicsWidget->setDragMode(QGraphicsView::RubberBandDrag);
    graphicsWidget->setFocusPolicy(Qt::StrongFocus);
    graphicsWidget->setFocus();

    this->resize(1280,800);

    //set minimum size of canvas

    graphicsWidget->setMinimumSize( (qreal)  ( this->width()-text->sizeHint().width() -40 ) , (qreal) ( this->height()-statusBar()->sizeHint().height() -toolBar->sizeHint().height() -menuBar()->sizeHint().height() -100 ) );
    qDebug ("MW initView(): now window size %i, %i, graphicsWidget size %i, %i, scene %f,%f",this->width(),this->height(), graphicsWidget->width(),graphicsWidget->height(), graphicsWidget->scene()->width(), graphicsWidget->scene()->height());

}




/**
    Resizes the scene when the window is resized.
*/
void MainWindow::resizeEvent( QResizeEvent * ){
    qDebug ("MW resizeEvent():INITIAL window size %i, %i, graphicsWidget size %i, %i, scene %f,%f",this->width(),this->height(), graphicsWidget->width(),graphicsWidget->height(), graphicsWidget->scene()->width(), graphicsWidget->scene()->height());

    //the area of the scene displayed by the CanvasView
    scene->setSceneRect(0, 0, (qreal) ( graphicsWidget->width() -5 ), (qreal) (graphicsWidget->height() -5 ) );
    qDebug ( "MW resizeEvent(): now window size %i, %i, graphicsWidget size %i, %i, scene %f,%f",this->width(),this->height(),
             graphicsWidget->width(),graphicsWidget->height(),
             graphicsWidget->scene()->width(), graphicsWidget->scene()->height() );
}


/**
    Initializes the default AF parameters.
    Also used when erasing an AF to start a new one
*/
void MainWindow::initNet(){
    qDebug()<<"MW: initNet() START INITIALISATION";
    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );

    // Init basic variables

    initArgumentSize=10;
    initArgumentColor="cyan";
    initAttackColor="Gray";
    initLabelColor="darkblue";
    initLabelSize=7;
    initNumberSize=5;
    initNumberColor="black";
    initArgumentShape="circle";
    initBackgroundColor="white";

    minDuration=3000; //dialogue duration - obsolete
    maxArguments=5000;		//Max Arguments for randomness FIXME::AM I used?
    labelDistance=8;
    numberDistance=5;

    previous_fileName=fileName;
    fileName="";
    fileLoaded=false;
    afModified=false;
    fileSave->setIcon(QIcon(":/images/saved.png"));
    fileSave->setEnabled(true);

    markedArgumentsExist=false;	//used by slotFindArgument()

    cursorPosGW=QPointF(-1,-1);
    clickedargumentNumber=-1;
    AttackClicked=false;
    ArgumentClicked=false;
    aspartixClicked = false;
    /** Clear previous data */
    activeGraph.clear();
    activeGraph.setInitVertexShape(initArgumentShape);
    activeGraph.setInitVertexSize(initArgumentSize);
    activeGraph.setInitVertexColor(initArgumentColor);

    activeGraph.setInitVertexNumberSize(initNumberSize);
    activeGraph.setInitVertexNumberColor(initNumberColor);

    activeGraph.setInitVertexLabelColor(initLabelColor);
    activeGraph.setInitVertexLabelSize(initLabelSize);

    activeGraph.setInitAttackColor(initAttackColor);

    /** Clear scene **/
    graphicsWidget->clear();


    /** Clear toolbox and menu checkboxes **/
    layoutEadesBx->setChecked(false);
    springLayoutAct->setChecked(false);
    FRLayoutAct->setChecked(false);

    /** set window title **/
    setWindowTitle(tr("Abstract Argumentation Framework Solution Visualizer "));

    QApplication::restoreOverrideCursor();
    statusMessage( tr("Ready"));
    qDebug("MW: initNet() INITIALISATION END");

}




/**
 * Slot called by Graph::statusMessage to display some message to the user
 */
void MainWindow::statusMessage(const QString message){
    statusBar()->showMessage( message, statusBarDuration );
}


void MainWindow::showMessageToUser(const QString message) {
    QMessageBox::information(this, tr("Info"),
                             message,
                             QMessageBox::Ok, 0);
}


/**
*	Displays a message	on the status bar when you resize the window.
*/
void MainWindow::windowInfoStatusBar(int w, int h){
    statusMessage(  QString(tr("Window resized to (%1, %2) pixels.")).arg(w).arg(h) );
}



/**
    Closes the application.
    Asks to save changed argumentation framework.
*/
void MainWindow::closeEvent( QCloseEvent* ce ) {
    if ( !afModified )       {
        ce->accept();
        return;
    }
    switch( QMessageBox::information( this, "Save file",
                                      tr("Do you want to save the changes") +
                                      tr(" to the Argumentation Framework?"),
                                      tr("Yes"), tr("No"), tr("Cancel"),
                                      0, 1 ) )
    {
    case 0:
        text->saveAsFile();
        ce->accept();
        break;
    case 1:
        ce->accept();
        break;
    case 2:
    default: // default
        ce->ignore();
        break;
    }
}




/**
    Creates a New Window for argumentation framework
*/
void MainWindow::slotCreateNew() {
    fileOpen->setEnabled(true);
    slotFileClose();
}


/**
 * @brief MainWindow::getLastPath
 * returns the last path used by user to open/save something
 * Not Used (can be used!!)
 */
QString MainWindow::getLastPath() {
    if ( lastUsedDirPath == "solution Visualization Args") {
        lastUsedDirPath = QDir::homePath();
    }
    qDebug() << lastUsedDirPath ;
    return lastUsedDirPath ;
}


/**
 * @brief MainWindow::setLastPath
 * sets the last path used by user to open/save something
 * @param filePath
 */
void MainWindow::setLastPath(QString filePath) {
    lastUsedDirPath = filePath.left( filePath.lastIndexOf("/"));
    qDebug() << lastUsedDirPath;
}


/**
    Closes the current argumentation framework. Saves it as apx file if necessary.
    Used by createNew and file close.
*/
void MainWindow::slotFileClose() {
    statusMessage( tr("Closing file..."));
    QMessageBox *msgBox = new QMessageBox;
    qDebug()<<"slotFileClose()";
    if (afModified) {
        switch ( msgBox->information (this,
                                      "New canvas for argumentation Framework...",
                                      tr("argumentation framework has not been saved. \n"
                                         " Do you want to save as apx file before closing it?"),
                                      "Yes", "No",0,1))
        {
        case 0:
            text->saveAsFile();
            break;
        case 1:
            text->clear();
            graphicsWidget->clear();
            activeGraph.clear();
            extn->clear();
            argumentFramework->clear();
            nodesLCD->display(0);
            edgesLCD->display(0);
            extensionNumP->display(0);
            break;
        case 2:
            msgBox->close();
            break;
        default: //should never reach
            msgBox->close();
            break;
        }
    }
    statusMessage( tr("Erasing old data..."));
    initNet();
    statusMessage( tr("Ready."));
}



/**
    Prints whatever is viewable on the Graphics widget
*/
void MainWindow::slotPrintView() {
    statusMessage( tr("Printing..."));
    QPrintDialog dialog(printer, this);
    if ( dialog.exec() )   {
        QPainter painter(printer);
        graphicsWidget->render(&painter);
    };
    statusMessage( tr("Ready."));
}


/**
    Calls Graph::createVertex method to add a new RANDOM Argument into the activeGraph.
    Called when "Create Argument" button is clicked on the Main Window.
*/
void MainWindow::addArgument() {
    callASPARTIX->setEnabled(true);
    if (aspartixClicked != true) {
        qDebug() << "MW::addArgument() ";
        // minus a  screen Attack offset...
        activeGraph.createVertex (
                    -1, graphicsWidget->width()-10,  graphicsWidget->height()-10);
        statusMessage( tr("New Argument a(numbered %1) added.")
                       .arg(activeGraph.lastVertexNumber())  );
    }
}

/**
 * @brief MainWindow::slotaddArgumentUpload
 * @param upload apx file and create arguments
 * @param nameArg
 */
void MainWindow::slotaddArgumentUpload(bool upload, QString nameArg)
{
    callASPARTIX->setEnabled(true);
    qDebug() << "MW::slotaddArgumentUpload()";
    activeGraph.createVertexUpload( upload, nameArg,
                                    -1, graphicsWidget->width()-10,  graphicsWidget->height()-10 );
    statusMessage(tr("uploading argumentation framework!!"));
}

/**
    Calls Graph::createVertex method to add a new Argument into the activeGraph.
    Called on double clicking
*/
void MainWindow::addArgumentWithMouse(int num, QPointF p) {
    qDebug () << "MW::addArgumentUpload()";
    callASPARTIX->setEnabled(true);
    if (aspartixClicked != true) {
        qDebug("MW: addArgumentWithMouse(). Calling activeGraph::createVertex() for a vertex named %i", num);
        activeGraph.createVertex(num, p);
        statusMessage( tr("New Argument (numbered %1) added.").arg(activeGraph.lastVertexNumber())  );
    }
}



/**
    Exports the canvas graph to a PDF Document
    Best Quality
*/
bool MainWindow::slotExportPDF(){
    qDebug(	"slotExportPDF()");
    if (!fileLoaded && !afModified )  {
        QMessageBox::critical(this, "Error",tr("The canvas is empty!\n create a new argumentation framework first."), "OK",0);
        statusMessage( tr("Cannot export PDF.")  );
        return false;
    }

    QString arg_fileName = QFileDialog::getSaveFileName(
                this, tr("Export to PDF"), getLastPath(),
                tr("Portable Document Format files (*.pdf)"));
    if (arg_fileName.isEmpty())  {
        statusMessage( tr("Saving aborted"));
        return false;
    }
    else {
        if (QFileInfo(arg_fileName).suffix().isEmpty())
            arg_fileName.append(".pdf");

        QPrinter printer2(QPrinter::HighResolution);
        printer2.setOutputFormat(QPrinter::PdfFormat);
        printer2.setOutputFileName(arg_fileName);
        QPainter painter(&printer2);
        graphicsWidget->render(&painter);
    }
    qDebug()<< "Exporting PDF to "<< arg_fileName;
    tempFileNameNoPath=arg_fileName.split ("/");
    setLastPath(arg_fileName);
    QMessageBox::information(this, tr("Export to PDF..."),tr("File saved as: ")+tempFileNameNoPath.last() , "OK",0);
    statusMessage(  tr("Exporting completed") );
    return true;
}


/**
     Calls GW: findArgument() to find a Argument by its name. The Argument is then marked .
*/
void MainWindow::slotFindArgument(){
    qDebug ("MW: slotFindArgument()");
    if (!fileLoaded && !afModified  ) {
        QMessageBox::critical( this, tr("Find Argument"),
                               tr("No Arguments present! \nLoad a apx file first or create some Arguments..."),
                               tr("OK"),0 );
        statusMessage(  QString(tr("Nothing to find!"))  );
        return;
    }

    if ( markedArgumentsExist ) {				// if a Argument has been already marked
        graphicsWidget->setMarkedArgument(""); 	// call setMarkedArgument to just unmark it.
        markedArgumentsExist=false;
        statusMessage( tr("Argument unmarked.") );
        return;								// and return to MW
    }

    bool ok=false;
    QString ArgumentText = QInputDialog::getText(this, tr("Find Argument"),
                                                 tr("Enter Argument Name:"),
                                                 QLineEdit::Normal,QString::null, &ok );
    if (!ok) {
        statusMessage( tr("Find Argument operation cancelled.") );
        return;
    }

    else {
        if	( graphicsWidget->setMarkedArgument(ArgumentText) ) {
            markedArgumentsExist=true;
            statusMessage( tr("Argument has been found and marked. Press Ctrl+F again to unmark...") );
        }
        else {
            QMessageBox::information(this, tr("Find Argument"),
                                     tr("Sorry. There is no such Argument in the argumentation framework. \n Try again."), "OK",0);
        }
    }
}





/**
*	A slot activated when something has been changed in the graph.
    Makes the fileSave icon active and refreshes any LCD values.
    Also called from graphicsWidget.
*/
void MainWindow::graphChanged(){
    qDebug("MW: graphChanged");
    afModified=true;
    fileSave->setIcon(QIcon(":/images/save.png"));
    fileSave->setEnabled(true);
    nodesLCD->display(activeGraph.vertices());
    edgesLCD->display(activeGraph.enabledAttacks());
}

/**
 * @brief MainWindow::slotSelectAll Arguments
 */
void MainWindow::slotSelectAll(){
    qDebug() << "MainWindow::slotSelectAll()";
    graphicsWidget->selectAll();
    statusMessage( QString(tr("Selected Arguments: %1") )
                   .arg( selectedArguments().count() ) );

}

/**
 * @brief MainWindow::slotSelectNone -deselect all argument
 */
void MainWindow::slotSelectNone(){
    qDebug() << "MainWindow::slotSelectNone()";
    graphicsWidget->selectNone();
    statusMessage( QString(tr("Selection cleared") ) );
}


/**
     Popups a context menu with some options when the user right-clicks on a Argument
*/
void MainWindow::openArgumentContextMenu() {
    clickedargumentNumber=clickedargument->argumentNumbe();
    qDebug("MW: openArgumentContextMenu() for Argument %i at %i, %i",
           clickedargumentNumber, QCursor::pos().x(), QCursor::pos().y());

    QMenu *ArgumentContextMenu = new QMenu(QString::number(clickedargumentNumber), this);
    Q_CHECK_PTR( ArgumentContextMenu );  //displays "out of memory" if needed
    if ( selectedArguments().count() == 1 ) {
        ArgumentContextMenu -> addAction( tr("## Argument ") + clickedargument->argumentName() + tr("##present in ") + QString::number (extn->sizeOfArgInSolution( clickedargument->argumentName()) ) + "  extensions.##  ");
        //ArgumentContextMenu -> addAction();
    }
    else {
        ArgumentContextMenu -> addAction( tr("## Argument ") + QString::number(clickedargumentNumber) + " ##  " + tr(" (selected Arguments: ") + QString::number (selectedArguments().count() ) + ")");
    }
    ArgumentContextMenu -> addSeparator();
    if ( aspartixClicked == false ) {

        ArgumentContextMenu -> addAction(addAttackAct);
        ArgumentContextMenu -> addAction(removeArgumentAct );
    } else {
        ArgumentContextMenu -> addAction(getEnumerations);
        fileLoaded = true;
        afModified = true;
        markedArgumentsExist = false;
    }
    ArgumentContextMenu -> exec(QCursor::pos() );
    delete  ArgumentContextMenu;
    clickedargumentNumber=-1;    //undo Argument selection
}



/**
     Popups a context menu with some options when the user right-clicks on an Attack
*/
void MainWindow::openAttackContextMenu() {
    QString source=clickedAttack->sourceArgumentName();
    QString target=clickedAttack->targetArgumentName();
    if (!aspartixClicked) {
        QString AttackName= source + QString("->")+ target ;
        //make the menu
        QMenu *AttackContextMenu = new QMenu(AttackName, this);
        AttackContextMenu -> addAction( "## Attack " + AttackName + " ##  ");
        AttackContextMenu -> addSeparator();
        AttackContextMenu -> addAction( removeAttackAct );
        AttackContextMenu -> exec(QCursor::pos() );
        delete  AttackContextMenu;
    }
}

/**
     Popups a context menu with some options when the user right-clicks on the scene
*/
void MainWindow::openContextMenu( const QPointF &mPos) {
    cursorPosGW=mPos;

    QMenu *contextMenu = new QMenu(" Menu",this);
    Q_CHECK_PTR( contextMenu );  //displays "out of memory" if needed


    contextMenu -> addAction( "## Selected Arguments: "
                              + QString::number(  selectedArguments().count() ) + " ##  ");
    if (!aspartixClicked) {
        contextMenu -> addSeparator();

        contextMenu -> addAction( addArgumentAct );

        contextMenu -> addAction( addAttackAct );
    }
    contextMenu -> exec(QCursor::pos() );

    delete  contextMenu;
    cursorPosGW=QPoint(-1,-1);
}


/**
 * @brief MainWindow::selectedArguments
 * @returns the list selected items from the canvas.
 */
QList<QGraphicsItem *> MainWindow::selectedArguments() {

    return graphicsWidget->selectedItems();

}



/**
*	When the user clicks on an Attack,
*   displays information about it on the status bar.
*
*/
void MainWindow::AttackInfoStatusBar (Attack* attack) {
    clickedAttack=attack;
    AttackClicked=true;
    ArgumentClicked=false;

    if (attack->isReciprocal()) {
        float outbound = activeGraph.hasArc
                (attack->sourceArgumentNumber(), attack->targetArgumentNumber());
        float inbound = activeGraph.hasArc
                (attack->targetArgumentNumber(), attack->sourceArgumentNumber());
        if (outbound==inbound)
            statusMessage(  QString
                            (tr("Symmetric Attack has been selected. "
                                "Click again to unselect it.")+ attack->sourceArgumentName() + " <--> "
                             + attack->targetArgumentName())) ;
        else
            statusMessage(  QString
                            (tr("Arc %1 --> %2 of weight %3 "
                                " and Arc %4 --> %5 of weight %6"
                                " have been selected. "
                                "Click again to unselect them."))
                            .arg(attack->sourceArgumentNumber() )
                            .arg(attack->targetArgumentNumber())
                            .arg(outbound)
                            .arg( attack->targetArgumentNumber() )
                            .arg(attack->sourceArgumentNumber())
                            .arg(inbound) ) ;

    }
    else {
        statusMessage(  QString
                        (tr("Attack has been selected. "
                            "Click again to unselect it.")+ attack->sourceArgumentName() + " --> "
                         + attack->targetArgumentName())) ;
    }
}



/**
*	When the user clicks on a Argument, displays argument name on the status bar.
*/
void MainWindow::ArgumentInfoStatusBar ( Argument *arg) {
    qDebug ("MW: ArgumentInfoStatusBar()");
    AttackClicked = false;
    ArgumentClicked = true;
    clickedargument = arg;
    clickedargumentNumber = clickedargument->argumentNumbe();

    statusMessage(QString("Argument Name ::"+arg->argumentName()));
}




/**
* 	Deletes an Argument and the attached objects (Attacks, etc).
*	It deletes clickedargument (signal from GraphicsView or set by another function)
*	or else asks for a ArgumentNumber to remove. The ArgumentNumber is doomedJim.
    Called from ArgumentContextMenu
*/
void MainWindow::slotRemoveArgument() {
    qDebug() << "MW: slotRemoveArgument()";
    if (!activeGraph.vertices())  {
        QMessageBox::critical(
                    this,
                    "Error",
                    tr("Nothing to do! \n"
                       "Load an argumentation framework or add some Arguments."), "OK",0);
        statusMessage( tr("Nothing to remove.")  );
        return;
    }

    int doomedJim=-1, min=-1, max=-1;
    bool ok=false;

    min = activeGraph.firstVertexNumber();
    max = activeGraph.lastVertexNumber();
    QString argName;

    qDebug("MW: min is %i and max is %i", min, max);
    if (min==-1 || max==-1 ) {
        qDebug("ERROR in finding Argument. Abort");
        return;
    }
    else if (ArgumentClicked && clickedargumentNumber >= 0 && clickedargumentNumber<= max ) {
        doomedJim=clickedargumentNumber ;
    }
    else if (!ArgumentClicked ) {
        argName =  QInputDialog::getText(this,tr("Argument delete"),tr("Enter Argument Name:"),
                                         QLineEdit::Normal, QString::null,&ok);
        if (!graphicsWidget->ArgFind.value(argName)) {
            QMessageBox::critical(this, "Error",tr("No Argument named ") + argName + tr(" exist."), "OK",0);
            return;
        }
        if (!ok) {
            statusMessage( "Remove Argument operation cancelled." );
            return;
        }
        if (graphicsWidget->ArgFind.value(argName)->argumentNumbe()) {
            doomedJim = graphicsWidget->ArgFind.value(argName)->argumentNumbe();
            qDebug() << "remove the argument completely.";
        } else {
            QMessageBox::critical(this, "Remove argument",tr("There is no such argument."), "OK",0);
            statusMessage( tr("There is no such Argument yet...")  );
            return;
        }
    }
    qDebug ("MW: removing vertex with number %i from Graph", doomedJim);
    activeGraph.removeVertex(doomedJim);
    clickedargumentNumber=-1;
    markedArgumentsExist = false;
    ArgumentClicked=false;
    graphChanged();
    qDebug("MW: removeArgument() completed. Argument %i removed completely.",doomedJim);
    statusMessage( tr("Argument removed completely. Ready. ") );
}



/**
*	Adds a new Attack between two Arguments specified by the user.
    Called when user clicks on the MW button "Add Attack".
*/
void MainWindow::slotAddAttack(){
    qDebug ("MW: slotAddAttack()");
    if ( (!fileLoaded && !afModified ) && !activeGraph.vertices() )  {
        QMessageBox::critical(this, "Error",tr("No Arguments!! \n Create some Arguments first."), "OK",0);
        statusMessage( tr("There are no Arguments...")  );
        return;
    }

    if (activeGraph.vertices() == 1)
        return;
    QString argFirst,argLast;
    float weight = 1.0;
    bool ok=false;

    argFirst = QInputDialog::getText(this, tr("Source Argument"),
                                     tr("Enter source Argument Name:"),
                                     QLineEdit::Normal,QString::null, &ok );
    if (!graphicsWidget->ArgFind.value(argFirst)) {
        QMessageBox::critical(this, "Error",tr("No Argument ") + argFirst + tr(" source exist."), "OK",0);
        return;
    }
    if (!ok) {
        statusMessage( tr("Source Argument Not Entered!!") );
        return;
    }
    if (graphicsWidget->hasArgument(argFirst)) {
        argLast = QInputDialog::getText(this,tr("Traget Argument"),tr("enter target argument:"),
                                        QLineEdit::Normal, QString::null,&ok);
        if (!graphicsWidget->ArgFind.value(argLast)) {
            QMessageBox::critical(this, "Error",tr("No Argument ") + argLast + tr(" target exist."), "OK",0);
            return;
        }
        if (!ok) {
            statusMessage( tr("Target Argument Not Entered!!") );
            return;
        }

        if (graphicsWidget->hasArgument(argLast)) {
            addAttack(argFirst,argLast,
                      graphicsWidget->hasArgument(argFirst)->argumentNumbe(),
                      graphicsWidget->hasArgument(argLast)->argumentNumbe(), weight);

        } else {
            showMessageToUser("Argument " + argLast + " does not exist!!");
        }
    } else {
        showMessageToUser("Argument " + argFirst + " does not exist!!");;
    }

    graphChanged();
    statusMessage( tr("Ready. ")  );
}


/**
 * @brief MainWindow::slotAddAttackoad
 * @param v  adds attack by uploading the apx FIle.
 * @param x
 * @param r
 * @param z
 */
void MainWindow::slotAddAttackUpload(QString v, QString x, int r, int z)
{
    qDebug("MW: addAttack() - setting user preferences and calling Graph::createAttack(...)");
    bool drawArrows=displayAttacksArrowsAct->isChecked();
    int weight = 1;
    int reciprocal=0;
    bool bezier = false;
    activeGraph.createAttackUpload(v,x,r, z, weight, reciprocal, drawArrows, bezier);
}



/** 	
    helper to slotAddAttack() above
    Also called from GW::userMiddleClicked() signal when user creates Attacks with middle-clicks
    Calls Graph::createAttack method to add the new Attack to the active Graph
*/
void MainWindow::addAttack (QString str1, QString str2, int v1, int v2, float weight) {
    qDebug("MW: addAttack() - setting user preferences and calling Graph::createAttack(...)");
    bool drawArrows=displayAttacksArrowsAct->isChecked();
    int reciprocal=0;
    bool bezier = false;
    activeGraph.createAttack(str1,str2,v1, v2, weight, reciprocal, drawArrows, bezier);
}


/**
*	Erases the clicked Attack. Otherwise asks the user to specify one Attack.
*	First deletes arc reference from object Argument Vector
*	then deletes attack item from the scene...
**/
void MainWindow::slotRemoveAttack(){
    if ( (!fileLoaded && !afModified) || !activeAttacks())  {
        QMessageBox::critical(this, "Error",tr("There are no Attacks! \nLoad a af file or create a new argumentation framework first."), "OK",0);
        statusMessage( tr("No Attacks to remove - sorry.")  );
        return;
    }

    QString argFirst, argLast;
    bool ok=false;

    if (!AttackClicked) {
        argFirst = QInputDialog::getText(this, tr("Source Argument"),
                                         tr("Enter source Argument Name:"),
                                         QLineEdit::Normal,QString::null, &ok );
        if (!ok) {
            statusMessage( "Remove Attack operation cancelled." );
            return;
        } argLast = QInputDialog::getText(this,tr("Traget Argument"),tr("enter target argument:"),
                                          QLineEdit::Normal, QString::null,&ok);
        if (!ok) {
            statusMessage( "Remove Attack operation cancelled." );
            return;
        }
        if ( activeGraph.hasArc(graphicsWidget->hasArgument(argFirst)->argumentNumbe(), graphicsWidget->hasArgument(argLast)->argumentNumbe())!=0 ) {
            if (activeGraph.symmetricAttack(graphicsWidget->hasArgument(argFirst)->argumentNumbe(), graphicsWidget->hasArgument(argLast)->argumentNumbe() ) ) {
                graphicsWidget->unmakeAttackReciprocal(graphicsWidget->hasArgument(argLast)->argumentNumbe(), graphicsWidget->hasArgument(argFirst)->argumentNumbe());
            }
            graphicsWidget->eraseAttack(graphicsWidget->hasArgument(argFirst)->argumentNumbe(), graphicsWidget->hasArgument(argLast)->argumentNumbe());
            activeGraph.removeAttack(graphicsWidget->hasArgument(argFirst)->argumentNumbe(), graphicsWidget->hasArgument(argLast)->argumentNumbe());
        }
        else {
            QMessageBox::critical(this, "Remove Attack",tr("There is no such Attack."), "OK",0);
            statusMessage( tr("There are no Arguments yet...")  );
            return;
        }

    } else {
        int sourceArgument = clickedAttack->sourceArgumentNumber();
        int targetArgument = clickedAttack->targetArgumentNumber();
        if (activeGraph.symmetricAttack(sourceArgument, targetArgument) ) {
            QString s=clickedAttack->sourceArgumentName();
            QString t=clickedAttack->targetArgumentName();
            switch (QMessageBox::information( this, tr("Remove Attack"),
                                              tr("This Attack is directed. \n") +
                                              tr("Select what Direction to delete or Both..."),
                                              s+" -> "+ t, t+" -> "+s, tr("Both"), 0, 1 ))

            {
            case 0:
                graphicsWidget->removeItem(clickedAttack);
                activeGraph.removeAttack(sourceArgument, targetArgument);
                //make new Attack
                graphicsWidget->drawAttack(t,s,targetArgument,sourceArgument,1.0,
                                           false,true,initAttackColor,false,false);
                break;
            case 1:
                clickedAttack->unmakeReciprocal();
                activeGraph.removeAttack(targetArgument, sourceArgument);
                break;
            case 2:
                graphicsWidget->removeItem(clickedAttack);
                activeGraph.removeAttack(sourceArgument, targetArgument);
                activeGraph.removeAttack(targetArgument, sourceArgument);
            }


        }
        else {
            graphicsWidget->removeItem(clickedAttack);
            activeGraph.removeAttack(sourceArgument, targetArgument);


        }


    }
    graphChanged();
    qDebug("MW: View items now: %i ", graphicsWidget->items().size());
    qDebug("MW: Scene items now: %i ", scene->items().size());
}

/**
 * @brief MainWindow::MapReduce
 * calls solution mapreduce
 */
void MainWindow::MapReduce()
{
    createProgressBar();
    visualizeIter->setEnabled(true);
    visualizeSize->setEnabled(true);
    visualizeShape->setEnabled(true);
    visualizecol->setEnabled(true);
    resizeSolArgs->setEnabled(true);
    reshapeSolArgs->setEnabled(true);
    reColorInitArg->setEnabled(true);
    visualizeLinear->setEnabled(true);
    visualizeCircle->setEnabled(true);
    scepticalSol->setEnabled(true);
    extn->normalizeSize();
    graphicsWidget->clearGuides();
    extn->calculateMapReduce(text->toPlainText());
    extensionNumP->display(extn->numberOfExtensions(text->toPlainText()));
    destroyProgressBar();
    statusMessage(tr("Calculation of Data Finished...Now use the visualization techniqies"));
}

/**
*  Changes the color of all Arguments
*/
void MainWindow::slotAllArgumentsColor(){
    QColor color = QColorDialog::getColor( Qt::cyan, this,
                                           "Change the color of all Arguments" );
    if (color.isValid()) {
        initArgumentColor=color.name();
        QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
        qDebug() << "MainWindow::slotAllArgumentsColor() : " << initArgumentColor;
        activeGraph.setAllVerticesColor(initArgumentColor);
        QApplication::restoreOverrideCursor();
        statusMessage( tr("Ready. ")  );
    }
    else {
        // user pressed Cancel
        statusMessage( tr("Arguments color change aborted. ") );
    }
}

//for sceptical
void MainWindow::slotSceptical()
{
    this->slotRecolorArgs();
    this->slotReshapeArgs();
    this->slotReshapeArgs();
    if (text->toPlainText().contains("[]") && !text->toPlainText().contains("[[]]") && !text->toPlainText().contains("],[") && semanticCombo->currentText() == "ST"){
        this->slotAllArgumentsColor();

    } else {
        extn->setScepticalDataCol();
    }
}

/**
 * @brief MainWindow::slotLayoutRandom
 * to reposition all Arguments on a circular layout randomly
 */
void MainWindow::slotLayoutRandom(){
    if (!fileLoaded && !afModified  )  {
        QMessageBox::critical(
                    this, "Error",
                    tr("Sorry!"
                       "\n Load a af file or create a new argumentation framework first. \n"
                       "Then layouts!"), "OK",0);
        statusMessage(  QString(tr("Nothing to layout!"))  );
        return;
    }
    double maxWidth=graphicsWidget->width();
    double maxHeight=graphicsWidget->height();
    statusMessage(  QString(tr("Randomizing Arguments positions. Please wait...")) );

    activeGraph.layoutRandom(maxWidth, maxHeight);

    statusMessage( tr("Argument positions are now randomized.") );
}



/**
 * @brief MainWindow::slotLayoutCircularRandom
 */
void MainWindow::slotLayoutCircularRandom(){
    qDebug() << "MainWindow::slotLayoutCircularRandom()";
    if (!fileLoaded && !afModified  )  {
        QMessageBox::critical(
                    this, "Error",
                    tr("Sorry!"
                       "\n Load a af file or create a new argumentation framework first. \n"
                       "Then we can talk about layouts!"), "OK",0);
        statusMessage(  QString(tr("Nothing to layout!"))  );
        return;
    }

    double x0=scene->width()/2.0;
    double y0=scene->height()/2.0;
    double maxRadius=(graphicsWidget->height())-50;          //pixels
    statusMessage(  QString(tr("Calculating new Arguments positions. Please wait...")) );

    activeGraph.layoutCircularRandom(x0, y0, maxRadius);
    statusMessage( tr("Arguments in random circles.") );
}


/**
 * @brief MainWindow::slotLayoutOfSolutionBySize
 */
void MainWindow::slotLayoutOfSolutionBySize(){
    qDebug() << "MainWindow::slotLayoutSolutionBySize()";
    if (!fileLoaded && !afModified && !aspartixClicked)  {
        QMessageBox::critical( this, "Error",
                        tr("Sorry!"
                       "\n only after visualize is clicked!"), "OK",0);
        statusMessage(  QString(tr("Nothing to layout!"))  );
        return;
    }

    double x0=scene->width()/2.0;
    double y0=scene->height()/2.0;
    double maxRadius=(graphicsWidget->height())-50;          //pixels

    statusMessage(  QString(tr("Calculating new Arguments positions. Please wait...")) );

    graphicsWidget-> layoutCircularForSolution(x0, y0, maxRadius);
    statusMessage( tr("Arguments in from solution set arranged!!") );
}

/**
 * @brief MainWindow::slotLayoutLinear
 * calls graphics widget visualize linear layout
 */
void MainWindow::slotLayoutLinear()
{
    qDebug() << "MainWindow::slotLayoutLinear()";
    if (!fileLoaded && !afModified && !aspartixClicked)  {
        QMessageBox::critical( this, "Error",
                        tr("Sorry!"
                       "\n only after visualize is clicked!"), "OK",0);
        statusMessage(  QString(tr("Nothing to layout!"))  );
        return;
    }

    double x0 = 0;
    double y0 = 0;

    statusMessage(  QString(tr("Calculating new Arguments positions. Please wait...")) );

    graphicsWidget-> layoutLinearPosition(x0, y0);
    statusMessage( tr("Arguments in from solution set arranged!!") );
}

/**
 * @brief MainWindow::slotResizeArgs
 * calls graphicswidget set size to normal
 */
void MainWindow::slotResizeArgs()
{
    graphicsWidget->setAllArgumentSize();
}

/**
 * @brief MainWindow::slotReshapeArgs
 * calls graphics widget set shape to normal
 */
void MainWindow::slotReshapeArgs()
{
    graphicsWidget->setAllArgumentsShape();
}

/**
 * @brief MainWindow::slotRecolorArgs
 */
void MainWindow::slotRecolorArgs()
{
    graphicsWidget->setAllArgumentColor();
}

/**
 * @brief MainWindow::slotSetInitPos
 */
void MainWindow::slotSetInitPos()
{
    graphicsWidget->setInitPos();
}

/**
 * @brief MainWindow::slotSolArgCol
 */
void MainWindow::slotSolArgCol()
{
    this->slotRecolorArgs();
    this->slotReshapeArgs();
    this->slotReshapeArgs();
    extn->calculateColor();
}

/**
 * @brief MainWindow::slotLayoutLinearRandom
 * calls graphics widget random x-axis
 */
void MainWindow::slotLayoutLinearRandom()
{
    qDebug() << "MainWindow::slotLayoutLinearRandom()";
    if (!fileLoaded && !afModified && !aspartixClicked)  {
        QMessageBox::critical( this, "Error",
                        tr("Sorry!"
                       "\n only after visualize is clicked!"), "OK",0);
        statusMessage(  QString(tr("Nothing to layout!"))  );
        return;
    }

    double x0 = 0;
    double y0 = 0;

    statusMessage(  QString(tr("Calculating new Arguments positions. Please wait...")) );

    graphicsWidget-> layoutLinearPositionRand(x0, y0);
    statusMessage( tr("Arguments in from solution set arranged!!") );
}


/**
*slotLayoutSpringEmbedder called from menu or toolbox checkbox
*/
void MainWindow::slotLayoutSpringEmbedder(bool state ){
    qDebug()<< "MW:slotLayoutSpringEmbedder";
    if (!fileLoaded && !afModified  )  {
        QMessageBox::critical(this, "Error",tr("There are no Arguments yet! \n Load a af file or create a new argumentation framework first."), "OK",0);
        statusMessage( tr("sorry. You must really load a file first... ")  );
        layoutEadesBx->setCheckState(Qt::Unchecked);
        return;
    }

    //Stop any other layout running
    layoutFruchtermanBx->setCheckState(Qt::Unchecked);
    activeGraph.ArgumentMovement(!state, 2, graphicsWidget->width(), graphicsWidget->height());

    scene->setItemIndexMethod (QGraphicsScene::NoIndex); //best when moving items

    if (state){
        statusMessage( tr("Embedding a spring-gravitational model on the graph... ")  );
        layoutEadesBx->setCheckState(Qt::Checked);
        activeGraph.ArgumentMovement(state, 1, graphicsWidget->width(), graphicsWidget->height());
        statusMessage( tr("Click on the checkbox \"Spring-Embedder\" to stop movement!") );
    }
    else {
        layoutEadesBx->setCheckState(Qt::Unchecked);
        activeGraph.ArgumentMovement(state, 1, graphicsWidget->width(), graphicsWidget->height());
        statusMessage( tr("Movement stopped!") );
    }
    scene->setItemIndexMethod (QGraphicsScene::BspTreeIndex); //best when not moving items
}




/**
*slotLayoutFruchterman called from menu
*/
void MainWindow::slotLayoutFruchterman() {
    if (!fileLoaded && !afModified  )  {
        QMessageBox::critical(this, "Error",tr("There are no Arguments yet!\n create a new argumentation framework first. \nThen we can talk about layouts!"), "OK",0);
        statusMessage( tr("I am really sorry. You must really load a file first... ")  );
        return;
    }
    if (layoutFruchtermanBx->checkState() == Qt::Unchecked){
        statusMessage( tr("Embedding a repelling-attracting forces model on the graph.... ")  );
        layoutFruchtermanBx->setCheckState(Qt::Checked);
        statusMessage( tr("Click on the checkbox \"Fruchterman-Reingold\" to stop movement!") );
    }
    else {
        layoutFruchtermanBx->setCheckState(Qt::Unchecked);
        statusMessage( tr("Movement stopped!") );
    }

}


/** 
    Called when user presses button.
    Calls Graph::startArgumentMovement to embed a repelling-attracting forces layout...
*/
void MainWindow::layoutFruchterman (int state){
    qDebug("MW: layoutFruchterman ()");
    layoutEadesBx->setChecked(false);
    scene->setItemIndexMethod (QGraphicsScene::NoIndex); //best when moving items
    activeGraph.ArgumentMovement(state, 2, graphicsWidget->width(), graphicsWidget->height());
    scene->setItemIndexMethod (QGraphicsScene::BspTreeIndex); //best when not moving items
}

/**
 * @brief MainWindow::slotEnumerationSetsbyArguments
 */
void MainWindow::slotEnumerationSetsbyArguments()
{
    qDebug ("MW: slotEnumerationSetsbyArguments()");
    if (!fileLoaded && !afModified  ) {
        QMessageBox::critical( this, tr("Find Argument"),
                               tr("No Arguments present! \n Load an AD file first or create some Arguments..."),
                               tr("OK"),0 );
        statusMessage(  QString(tr("Nothing to find!"))  );
        return;
    }

    if ( markedArgumentsExist ) {				// if a Argument has been already marked
        graphicsWidget->setMarkedArgument(""); 	// call setMarkedArgument to just unmark it.
        markedArgumentsExist=false;
        statusMessage( tr("Argument unmarked.") );
        return;								// and return to MW
    }

    bool ok=false;
    QString ArgumentText = QInputDialog::getText(this, tr("Find Argument"),
                                                 tr("Enter Argument Name:"),
                                                 QLineEdit::Normal,QString::null, &ok );
    if (!ok) {
        statusMessage( tr("Find Argument operation cancelled.") );
        return;
    }

    else {
        QStringList strList;
        if	( graphicsWidget->setMarkedArgument(ArgumentText) ) {
            markedArgumentsExist=true;
            QString sol = text->toPlainText();
            sol.chop(1);
            QStringList listSol = sol.split("]");
            int j =0;
            QStringList s;
            foreach (QString var, listSol) {
                var.remove("[");
                present = extn->getPresentSolutionset( var , ArgumentText );
                if (present) {
                    j+=1;
                    ArgPresence.insertMulti(var,present);
                    strList << var;
                    s = strList;
                }
            }
            bool ok;
            if (!strList.isEmpty()) {
                int itemNo = QInputDialog::getInt
                        (this, "All solutions with presence of ::"+ ArgumentText +"::",
                         tr("Solution. \n Extension."), 0 , 0, j-1,1, &ok);
                if (ok) {
                    activeGraph.setAllVerticesColor("cyan");
                    QStringList str = s.value(itemNo).split(",",QString::SkipEmptyParts);
                    graphicsWidget->setSolutionColor("blue",str);
                }
                statusMessage( tr("Argument found and displayed."));
            } else {
                QMessageBox::information(this, tr("Argument Not in Solution!!"),
                                         tr("Sorry. There is no such Argument in The solution Set. \n Try again."), "OK",0);
            }
        }

        else {
            QMessageBox::information(this, tr("Find Argument"),
                                     tr("Sorry. There is no such Argument in this argumentation Framework. \n Try again."), "OK",0);
        }
    }
}



/**
* Turns on/off displaying Attacks
* Button Not Used
* FIXME: Am I Used?
*/
void MainWindow::slotDisplayAttacks(bool toggle){
    if (!fileLoaded && ! afModified) {
        QMessageBox::critical(this, "Error",tr("There are no Arguments nor Attacks! \nLoad an AF file or create a new AF first!"), "OK",0);

        statusMessage( tr("No Attacks found...") );
        return;
    }
    statusMessage( tr("Toggle Attacks Arrows. Please wait...") );

    if (!toggle) 	{
        graphicsWidget->setAllItemsVisibility(TypeAttack, false);
        statusMessage( tr("Attacks are invisible now. Click again the same menu to display them.") );
        return;
    }
    else{
        graphicsWidget->setAllItemsVisibility(TypeAttack, true);
        statusMessage( tr("Attacks visible again...") );
    }

}



/**
*Turns on/off the arrows of Attacks
* Butoon Not Used
*/
void MainWindow::slotDisplayAttacksArrows(bool toggle){
    if (!fileLoaded && ! afModified) {
        QMessageBox::critical(this, "Error",tr("There are no Attacks! \nLoad an AF file or create a new AF first!"), "OK",0);

        statusMessage( tr("No Attacks found...") );
        return;
    }
    statusMessage( tr("Toggle Attacks Arrows. Please wait...") );

    if (!toggle) 	{
        QList<QGraphicsItem *> list = scene->items();
        for (QList<QGraphicsItem *>::iterator item=list.begin();item!=list.end(); item++) {
            if ( (*item)->type() ==TypeAttack){
                Attack *att = (Attack*) (*item);
                att->showArrows(false);
            }
        }
        return;
    }
    else{
        QList<QGraphicsItem *> list = scene->items();
        for (QList<QGraphicsItem *>::iterator item=list.begin();item!=list.end(); item++)
            if ( (*item)->type() ==TypeAttack){
                Attack *att = (Attack*) (*item);
                att->showArrows(true);
            }
    }
    statusMessage( tr("Ready."));
}

/**
 * @brief MainWindow::setCurrentIndex
 * @param index
 */
void MainWindow::setCurrentIndex(QString index)
{
    currentCombo = index;
    QString combo = getCurrentComboString(index);
    emit comboIndex(combo);
}

//returns the current index of the combo
QString MainWindow::getCurrentComboString(QString index)
{
    qDebug() << "Get the Current string of the Combo Box!!" << index;
    return index;
}



/**
 * @brief MainWindow::callAspartixProcess
 * calls the aspartix tool.
 */
void MainWindow::callAspartixProcess(bool)
{   
    QString apxfile;
    this->slotRecolorArgs();
    this->slotReshapeArgs();
    this->slotResizeArgs();
    extensionNumP->display(0);
    //Enable and Disable buttons
    fileOpen->setEnabled(false);
    nextMapReduce->setEnabled(true);
    aspartixClicked = true;
    fileSave->setEnabled(false);
    visualizeIter->setEnabled(false);
    visualizeSize->setEnabled(false);
    visualizeShape->setEnabled(false);
    visualizecol->setEnabled(false);
    resizeSolArgs->setEnabled(false);
    reshapeSolArgs->setEnabled(false);
    reColorInitArg->setEnabled(false);
    visualizeLinear->setEnabled(false);
    visualizeCircle->setEnabled(false);
    scepticalSol->setEnabled(false);
    /**
     * @brief program aspartix variables.
     */
    QString program = "./aspartix.sh";
    QStringList binaryArguments;
    binaryArguments.push_back("-f");
    binaryArguments.push_back(text->getFileNameAddress(apxfile));
    binaryArguments.push_back("-fo");
    binaryArguments.push_back("apx");
    binaryArguments.push_back("-p");
    binaryArguments.push_back("EE-"+semanticCombo->currentText());

    qDebug() << binaryArguments;

    process = new Aspartix(dataDir,program, binaryArguments,text);

    //for reloading the AF enable it here.
    reloadArgumentationFramework->setEnabled(true);    
}

/**
 * @brief MainWindow::reloadAF
 * reloads the AF in the text place.
 */
void MainWindow::reloadAF()
{
    fileSave->setEnabled(true);
    nextMapReduce->setEnabled(false);
    text->reloadFile();
    setEnabled(true);
    aspartixClicked = false;
}

/**
 * @brief MainWindow::loadFromText
 * loads after writing in text from text box
 */
void MainWindow::loadFromText()
{
    createProgressBar();
    argumentFramework->getAllArguments();
    argumentFramework->getAllAttacks();
    statusMessage("Ready...Done...Loadinng...");
    destroyProgressBar();
}

void MainWindow::slotMapSize()
{
    extn->calculateSize();
    emit setColorwithSize();
    statusMessage("Ready... continue...Sized...");
}


void MainWindow::slotShapeRandom()
{
    this->slotRecolorArgs();
    this->slotReshapeArgs();
    this->slotReshapeArgs();
    extn->calculateShapeRandom();
    statusMessage("Ready... continue...Shape...Size...Done...");
}

void MainWindow::setSizeOfArgument() {
    this->slotRecolorArgs();
    this->slotReshapeArgs();
    this->slotReshapeArgs();
    extn->setSizeOfArgument();
}

/**
*  Changes the background color of the scene
*/
void MainWindow::slotBackgroundColor () {
    qDebug("MW: slotBackgroundColor ");
    QColor backgrColor = QColorDialog::getColor( initBackgroundColor, this );
    graphicsWidget ->setBackgroundBrush(QBrush(backgrColor));
    statusMessage( tr("Ready. ") );
}

/**
*  turns antialiasing on or off
*/
void MainWindow::slotAntialiasing(bool toggle) {
    statusMessage( tr("Toggle anti-aliasing. This will take some time if the number of arguments is large (>500)...") );
    //Inform graphicsWidget about the change
    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
    graphicsWidget->setRenderHint(QPainter::Antialiasing, toggle);
    graphicsWidget->setRenderHint(QPainter::TextAntialiasing, toggle);
    graphicsWidget->setRenderHint(QPainter::SmoothPixmapTransform, toggle);
    QApplication::restoreOverrideCursor();
    if (!toggle)
        statusMessage( tr("Anti-aliasing off.") );
    else
        statusMessage( tr("Anti-aliasing on.") );

}


/**
*  Turns debugging messages on or off
*/
void MainWindow::slotPrintDebug(bool toggle){
    if (!toggle)   {
        printDebug=false;
        statusMessage( tr("Debug messages off.") );
    }
    else  {
        printDebug=true;
        statusMessage( tr("Debug messages on.") );
    }
}





/**
*  Turns Toolbar on or off
*/
void MainWindow::slotViewToolBar(bool toggle) {
    statusMessage( tr("Toggle toolbar..."));
    if (toggle== false)   {
        toolBar->hide();
        statusMessage( tr("Toolbar off.") );
    }
    else  {
        toolBar->show();
        statusMessage( tr("Toolbar on.") );
    }
}


void MainWindow::slotSolutionDialogIter()
{
    statusMessage( "Creating a Solution Iteration... ");

    visSolutionIter = new SolutionDialogIter(text,graphicsWidget,this);

    visSolutionIter->exec();
}


void MainWindow::createProgressBar(){


    progressDialog= new QProgressDialog("Please wait....", "Cancel", 0, activeGraph.vertices(), this);
    progressDialog -> setWindowModality(Qt::WindowModal);
    connect( &activeGraph, SIGNAL( updateProgressDialog(int) ), progressDialog, SLOT(setValue(int) ) ) ;
    progressDialog->setMinimumDuration(0);


    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );

}

/**
*	Returns the amount of enabled/active attacks on the scene.
*/
int MainWindow::activeAttacks(){
    qDebug () << "MW::activeAttacks()";
    return activeGraph.enabledAttacks();
}

void MainWindow::destroyProgressBar(){
    QApplication::restoreOverrideCursor();
    progressDialog->deleteLater();
}

/**
* Displays a short message about Qt.
*/
void MainWindow::slotAboutQt(){
    QMessageBox::aboutQt(this, "About Qt - solutionVizArg");
}


/**
* Displays the Tool guide in Pdf
* slot not used

void MainWindow::slotAboutTool(){
    QString helpPath;
    QDir d( QCoreApplication::applicationDirPath() );
    qDebug()<< QCoreApplication::applicationDirPath().toLatin1();

    if ( d.exists("manual.pdf") ) {
        helpPath=d.filePath("manual.pdf");
    }
    else {
        if (d.dirName()=="bin") {
            d.cdUp();
        }
        if (d.cd("./manual") ) {
            if ( d.exists("manual.pdf") ) {
                helpPath=d.filePath("manual.pdf");
            }
            else 	{
                qDebug()<< "help file does not exist here.";
            }
        }
    }

    if (d.cd("../manual") ) {         // for Mac
        if ( d.exists("manual.pdf") ) {
            helpPath=d.filePath("manual.pdf");

        }
        else 	{
            qDebug()<< "help file does not exist here.";

        }
    }

    QDesktopServices::openUrl(QUrl::fromLocalFile(helpPath));

}
*/
