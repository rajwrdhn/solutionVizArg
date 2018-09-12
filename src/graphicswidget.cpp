/***************************************************************************
    solutionVizArg: Visualization of Solutions of Abstract Argumentation Frameworks


    Written in Qt/C++

                             -------------------
    This software is written as a University project.
 ***************************************************************************/
#include "graphicswidget.h"
#include <QList>
#include <QGraphicsScene>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QDebug>
#include <QWheelEvent>
#include <math.h>
#include <QColor>
#include "mainwindow.h"
#include "argument.h"
#include "attack.h"
#include "textedit.h"
#include "guidecircle.h"

static qreal Pi = 3.14159265;

/** 
    Constructor method. Called when a GraphicsWidget object is created in MW
*/
GraphicsWidget::GraphicsWidget( QGraphicsScene *sc, TextEdit *text, MainWindow* par)  : QGraphicsView ( sc,par) {
    setScene(sc);
    secondDoubleClick=false;
    dynamicMovement=false;
    moving=0;
    timerId=0;
    tex = text;
    layoutType=0;
    arg_ArgumentLabel="";
    zoomIndex=5;
    arg_currentScaleFactor = 1;
    arg_currentRotationAngle = 0;
    markedArgumentExist=false; //used in findArgument()
    markedAttackExist = false; //used in selecting and Attack
    AttacksHash.reserve(1000);
    ArgumentHash.reserve(1000);
}



/**
*paint event to improve graphics view
*/
void GraphicsWidget::paintEvent ( QPaintEvent * event ){
    QPaintEvent *newEvent=new QPaintEvent(event->region().boundingRect());
    QGraphicsView::paintEvent(newEvent);
    delete newEvent;
}
/**
 * @brief GraphicsWidget::clearGuides
 */
void GraphicsWidget::clearGuides(){
    qDebug()<< "GW: clearGuides";
    this->removeAllItems(TypeGuide);
}


/**
*	Removes all items of type Guides
*/
void GraphicsWidget::removeAllItems(int type){
    qDebug()<< "GW: removeAllItems";
    QList<QGraphicsItem *> list = scene()->items();
    for (QList<QGraphicsItem *>::iterator item=list.begin();item!=list.end(); item++) {
        if ( (*item)->type() == type){
            GuideCircle *guide = qgraphicsitem_cast<GuideCircle *>  (*item);
            qDebug()<< "GW: removeAllItems - located element";
            guide->die();
            guide->deleteLater ();
            delete *item;
        }
    }
}

/** 
*Clears the scene
*/
void GraphicsWidget::clear() {
    qDebug() << " clear GW";
    ArgumentHash.clear();
    AttacksHash.clear();
    ArgFind.clear();
    tex->clear();
    scene()->clear();
    arg_curRelation=0;
    markedArgumentExist=false;
    markedAttackExist = false;
    allArgumentList.clear();
}

/**	
    Adds a new Argument onto the scene
    Called from Graph::createVertex method when:
        we load files or
        the user presses "Add Argument" button or
        the user double clicks (mouseDoubleClickEvent() calls Graph::createVertex
*/
void GraphicsWidget::drawArgument(QString argName,
                                  int num, int size, QString ArgumentColor,
                                  QPointF p,
                                  QString shape, bool upload
                                  ) {
    qDebug()<< "GW: drawArgument(): drawing new Argument at: "
            << p.x() << ", "<< p.y() ;


    size = size +3;

    Argument *jim= new Argument (
                this, argName, num, size, ArgumentColor, shape,
                arg_labelDistance, arg_numberDistance,
                p
                );

    allArgumentList << argName;

    ArgFind.insert(argName, jim); //main container of arg name and reference for coloration


    ArgumentHash.insert(num, jim);//add new Argument to a container to ease finding, Attack creation etc
    if(!upload)
        tex->insertPlainText("arg("+QString::number(num)+").");

    jim -> setPos( p.x(), p.y());	//finally, move the Argument where it belongs!

    argInitPos.insert(jim,p);
}

//sets the initial position of the AF
void GraphicsWidget::setInitPos() {
    this->clearGuides();
    foreach (Argument *arg, ArgFind) {
        arg->setX(argInitPos.value(arg).x());
        arg->setY(argInitPos.value(arg).y());
        emit moveArgument(arg->argumentNumbe(),  argInitPos.value(arg).x(),  argInitPos.value(arg).y());
    }
}

/**
 * @brief Bubble::randomColor
 * @return
 * not used
 */
void GraphicsWidget::setArgColorSol( )
{
    //QString col = "blue";
    foreach (Argument *arg, ArgumentHash) {
        //int red = int(205 + 50.0*qrand()/(RAND_MAX+1.0));
        //int green = int(205 + 50.0*qrand()/(RAND_MAX+1.0));
        //int blue = int(205 + 50.0*qrand()/(RAND_MAX+1.0));
        //int alpha = int(91 + 100.0*qrand()/(RAND_MAX+1.0));
        QColor col = "blue";
        arg->setColor(col);
    }
}

/**
 * @brief GraphicsWidget::addGuideCircle
 * @param x0
 * @param y0
 * @param radius
 */
void GraphicsWidget::addGuideCircle( int x0, int y0, int radius){
    GuideCircle *circ=new GuideCircle (this, x0, y0, radius);
    circ->show();
}

void GraphicsWidget::addGuideHLine( int y0){
    GuideCircle *line = new GuideCircle (this, y0, this->width());
    line->show();
}


/** Draws an Attack from source to target Argument.
    This is used when we do not have references to Arguments - only ArgumentNumbers or names:
    a) when we load a apx file (check = FALSE)
    b) when the user clicks on the AddLink button on the MW.
*/
void GraphicsWidget::drawAttack(QString argName_i,QString argName_j,int i, int j,
                                float weight, bool reciprocal, bool drawArrows, QString color, bool bezier, bool upload){

    QString AttackName = "att("+argName_i+","+argName_j+").";
    qDebug()<<"GW: drawAttack "<< AttackName << "weight "<<weight
           << " - ArgumentHash reports "<< ArgumentHash.size()<<" Arguments.";
    if (i == j ) {
        bezier = true;
    }

    Attack *att=new Attack (this, ArgFind.value(argName_i), ArgFind.value(argName_j),
                         Qt::SolidLine,
                         weight,
                         arg_ArgumentSize, color, reciprocal, drawArrows, bezier);
    att -> setZValue(253);		//Attacks have lower z than Arguments. Arguments always appear above Attacks.
    // Keep it here so that it doesnt interfere with dashed lines.
    att -> setBoundingRegionGranularity(0.05);	// Slows down the universe...Keep it 0.05...

    if (!upload)
        tex->insertPlainText("att("+argName_i +","+ argName_j +").");

    AttacksHash.insert(AttackName, att);
}




/**
    Called from Graph to make an existing a symmetric (reciprocal)
*/
void GraphicsWidget::drawAttackReciprocal(QString source, QString target, int, int, bool upload){
    qDebug("GW: drawAttackReciprocal ()");
    QString AttackName = "att("+source+","+target+").";
    //    qDebug("GW: making existing Attack between %i and %i reciprocal. Name: "+AttackName.toUtf8(), source, target );
    if (!upload)
        tex->insertPlainText("att("+target+","+source+").");
    AttacksHash.value(AttackName)->makeReciprocal();
}


/**
    Called from Graph to unmake an existing symmetric (reciprocal) Attack to one-directed only.
*/
void GraphicsWidget::unmakeAttackReciprocal(int source, int target){
    qDebug("GW: unmakeAttackReciprocal ()");

    QString AttackName =  "att("+ArgumentHash.value(source)->argumentName()+","+ArgumentHash.value(target)->argumentName()+").";
    QString str = tex->toPlainText().remove("att("+ArgumentHash.value(source)->argumentName()+","+ArgumentHash.value(target)->argumentName()+").");
    tex->clear();
    tex->insertPlainText(str);
    AttacksHash.value(AttackName)->unmakeReciprocal();
}



/** 	
    Called when the user middle-clicks on two Arguments consecutively. .
    It saves the source & target Arguments that were clicked
    On the second middle-click, it emits the userMiddleClicked() signal to MW,
    which will notify activeGraph, which in turn will signal back to drawAttack()...
*/
void GraphicsWidget::startAttack(Argument *argument){
    if (secondDoubleClick){
        qDebug("GW: startAttack(): this is the second double click. Emitting userMiddleClicked() to create Attack");
        secondArgument=argument;
        emit userMiddleClicked(firstArgument->argumentName(), secondArgument->argumentName(), firstArgument->argumentNumbe(), secondArgument->argumentNumbe(),1.0);
        ( (MainWindow*)parent() )->setCursor(Qt::ArrowCursor);
        secondDoubleClick=false;
    }
    else{
        qDebug("GW: startAttack(): this is the first double click.");
        firstArgument=argument;
        secondDoubleClick=true;
        ( (MainWindow*)parent() )->setCursor( Qt::PointingHandCursor);
    }
}



/** 
    This is called from each Argument when the user clicks on it.
    It emits the selectedArgument signal to MW which is used to
    - display Argument info on the status bar
    - notify context menus for the clicked Argument.
*/
void GraphicsWidget::ArgumentClicked(Argument *arg){
    qDebug ("GW: Emitting selectedArgument()");
    emit selectedArgument(arg);
}

/** 
    This is called from each Attack when the user clicks on it.
    It emits the selectedAttack signal to MW which is used to
    - display Attack info on the status bar
    - notify context menus for the clicked Attack.
    Also, it makes source and target Arguments to stand out of other Arguments.
*/
void GraphicsWidget::AttackClicked(Attack *att){
    qDebug ("GW: Emitting selectedAttack()");
    if (markedAttackExist) {
        //unselect them, restore their color
        markedAttackSource->setSelected(false);
        markedAttackTarget->setSelected(false);
        //restore their size
        markedAttackSource->setSize(markedAttackSourceOrigSize);
        markedAttackTarget->setSize(markedAttackTargetOrigSize);
        markedAttackExist=false;
        return;
    } else {
        markedAttackSource=att->sourceArgument();
        markedAttackTarget=att->targetArgument();
        markedAttackExist=true;
        // select Arguments to change their color
        markedAttackSource->setSelected(true);
        markedAttackTarget->setSelected(true);
        // save their original size
        markedAttackSourceOrigSize=markedAttackSource->size();
        markedAttackTargetOrigSize=markedAttackTarget->size();
        //now, make them larger
        markedAttackSource->setSize(2*markedAttackSourceOrigSize-1);
        markedAttackTarget->setSize(2*markedAttackTargetOrigSize-1);
        emit selectedAttack(att);
    }
}

/**
    On the event of a right-click on a Argument, the Argument calls this function
    to emit a signal to MW to open a context menu at the mouse position.
    Argument is already passed with selectedArgument(Argument *) signal
    The position of the menu is determined by QMouse:pos()...
*/
void GraphicsWidget::openArgumentContextMenu(){
    qDebug("GW: emitting openArgumentMenu()");
    emit openArgumentMenu();
}

/**
    On the event of a right-click on an Attack, the Attack calls this function
    to emit a signal to MW to open a context menu at the mouse position.
    Attack is already passed with selectedAttack(Argument *) signal
    The position of the menu is determined by QMouse:pos()...
*/
void GraphicsWidget::openAttackContextMenu(){
    qDebug("GW: emitting openAttackMenu()");
    emit openAttackMenu();
}

/** 
    Called from each Argument when they move.
    Updates
    - Argument coordinates in activeGraph (via updateArgumentCoords() signal)

*/
void GraphicsWidget::ArgumentMoved(int number, int x, int y){
    emit updateArgumentCoords(number, x, y);
}



/** 
    Called from activeGraph to update Argument coordinates
    on the canvas
*/
void GraphicsWidget::moveArgument(int number, qreal x, qreal y){
    qDebug() << "   GW: moveArgument() " << number << ": " << x << y;
    ArgumentHash.value(number)->setPos(x,y);
}

void GraphicsWidget::moveArgumentX(int number, qreal x){
    qDebug() << "   GW: moveArgumentX() " << number << ": " << x ;
    ArgumentHash.value(number)->setX(x);
}


/**
    Called from Graph signal eraseArgument(int)
*/
void GraphicsWidget::eraseArgument(long int doomedJim){
    qDebug() << "GW: Deleting Argument "<< doomedJim;
    QString str;
    QList<QGraphicsItem *> list=scene()->items();
    qDebug("GW: Scene items= %i - View items : %i",scene()->items().size(), items().size());
    for (QList<QGraphicsItem *>::iterator it=list.begin(); it!=list.end(); it++) {
        if ( (*it)->type()==TypeArgument) {
            Argument *jim=(Argument*) (*it);
            if ( jim->argumentNumbe()==doomedJim)	{
                qDebug() << "GW: found ARGUMENT " <<  jim->argumentName() << " Demanding Argument->die() :)" ;
                delete *it;

                str = tex->toPlainText().remove("arg("+jim->argumentName()+").");
                ArgFind.remove(jim->argumentName());
                tex->clear();
                //tex->toPlainText().remove("arg("+jim->ArgumentName()+").");
                tex->insertPlainText(str);
                //ArgFind.remove(jim->ArgumentName());
                break;
            }
        }
    }
    markedArgumentExist = false;
    qDebug("GW: Scene items now= %i - View items now= %i ", scene()->items().size(), items().size() );
}



/**
    Called from MainWindow when erasing Attacks using argument names names
*/
void GraphicsWidget::eraseAttack(int sourceArgument, int targetArgument){
    qDebug("GW: Scene items= %i - View items : %i",scene()->items().size(), items().size());
    QString str;
    QList<QGraphicsItem *>  list=scene()->items();
    for (QList<QGraphicsItem *>::iterator it=list.begin(); it!= list.end() ; it++){
        if ( (*it)->type()==TypeAttack ) {
            Attack *att=(Attack*) (*it);
            if ( att->sourceArgumentNumber()==sourceArgument && att->targetArgumentNumber()==targetArgument ) {
                removeItem(att);
                AttacksHash.value( "att(" + att->eFrom + "," + att->eTo + ")" )->deleteLater();
                break;
            }
        }
    }
    qDebug("GW: Scene items now= %i - View items now= %i ", scene()->items().size(), items().size() );
}


/** 
    Called from Argument::die() to removeItem from ArgumentHash...
    FIXME : Do we use this ??
*/
void GraphicsWidget::removeItem( Argument *arg){
    long int i=arg->argumentNumbe();
    foreach ( Argument *candidate, ArgumentHash) {
        if ( candidate->argumentNumbe() == i )
            ArgumentHash.remove( i );
    }
    arg->deleteLater ();
    qDebug() << "GW items now:  " << items().size();
}


//to be called from solution for setting the size for arrangement.
void GraphicsWidget::insertIntoargSolSize(QString arg, int argsized)
{
    argSolSize.insert(arg,argsized);
    arrangeSizeOfArg();
}

//not used
void GraphicsWidget:: arrangeSizeOfArg () {
    QList<int> vals = argSolSize.values();
    qSort( vals );
    foreach( int val, vals )
    {
        qDebug() << val << ":" << argSolSize.key( val );
    }
}

/** Called from Argument::die() to remove an attack ... */
void GraphicsWidget::removeItem( Attack * att){
    qDebug() << "remove Item form Attacks Hash:::" << att->eFrom << att->eTo << "Delete This!!";

    if (att->eFrom == "" || att->eTo == "") {
        return;
    }
    QString str = tex->toPlainText().remove("att("+att->eFrom+","+att->eTo+").");
    tex->clear();
    //tex->toPlainText().remove("arg("+jim->ArgumentName()+").");
    tex->insertPlainText(str);
    QString str1 = tex->toPlainText().remove("att("+att->eTo+","+att->eFrom+").");
    tex->clear();
    //tex->toPlainText().remove("arg("+jim->ArgumentName()+").");
    tex->insertPlainText(str1);
    AttacksHash.value( "att(" + att->eFrom + "," + att->eTo + ")" )->deleteLater();
    delete (att);
}


/** 
    Accepts initial Argument color from MW.
    It is called from MW on startup and when user changes it.
    FIXME:: do we use this???
*/
void GraphicsWidget::setInitArgumentColor(QString color){
    qDebug("GW setting initArgumentColor");
    arg_ArgumentColor=color;
}



/** 
    Sets initial Attack color.
    Called from MW on startup and when user changes it.
*/
void GraphicsWidget::setInitLinkColor(QString color){
    qDebug("GW setting initLinkColor");
    arg_linkColor=color;
}



/** 
    Sets the color of an Argument.
    Called from MW when the user changes the color of a Argument (right-clicking).
*/
bool GraphicsWidget::setArgumentColor(long int ArgumentNumber, QString color){
    qDebug() << "GraphicsWidget::setArgumentColor() : " << color;
    ArgumentHash.value(ArgumentNumber) -> setColor(color);
    return true;

}

//called from iterator ui dialog
void GraphicsWidget::setSolutionColor(QString color, QStringList list)
{
    qDebug() << "GraphicsWidget::setSolutionColor: " <<   color ;

    foreach (QString var, list) {
        var.remove("]");
        ArgFind.value(var)->setColor(color);
    }
}

//called from dialog to set color
void GraphicsWidget::setSolutionColordial(QColor color, QStringList list)
{
    qDebug() << "GraphicsWidget::setSolutionColor: " <<   color ;

    foreach (QString var, list) {
        ArgFind.value(var)->setColor(color);
    }
}


//set shape randomly. NOT USED!!
QString GraphicsWidget::randomSelectShapeArg()
{
    QStringList shapeList;
    shapeList << "triangle" << "box" << "ellipse" << "diamond" << "rectangle" ;
    QString shape;
    shape = shapeList.at(rand() % shapeList.size());
    return shape;
}

//sets shape randomly
void GraphicsWidget::setArgumentShape(QString arg)
{
    QString shape;
    shape = "box";
    ArgFind.value(arg)->setShape(shape);
}

//sets color to blue of arguments present in the solution
void GraphicsWidget::setArgumentColor(QString arg)
{
    QString col;
    col = "blue";
    ArgFind.value(arg)->setColor(col);
}
/**
 * @brief GraphicsWidget::setAllArgumentColor
 * the init color for all arguments
 */
void GraphicsWidget::setAllArgumentColor()
{
    qDebug() << "GraphicsWidget::setAllColor: " <<   "cyan" ;
    QString cyn = "cyan";
    QHash <long int, Argument*>::iterator i;
    for (i = ArgumentHash.begin(); i != ArgumentHash.end(); ++i) {
        i.value()->setColor(cyn);
    }
}


/** 
    Changes/Sets the color of an Attack.
    Called from MW when the user changes the color of an Attack (right-clicking).
*/

void GraphicsWidget::setAttackColor(long int source, long int target, QString color){

    QString AttackName =  QString::number(arg_curRelation) + QString(":") +
            QString::number( source ) + QString(">")+ QString::number( target );

    qDebug()<<"GW::setAttackColor() -" << AttackName << " to "  << color;;

    //    if  ( AttacksHash.contains (AttackName) ) { // VERY SLOW
    AttacksHash.value(AttackName) -> setColor(color);

}

/** 
    Sets initial Argument size from MW.
    It is Called from MW on startup and when user changes it.
*/
void GraphicsWidget::setInitArgumentSize(int size){
    qDebug("GW setting initArgumentSize");
    arg_ArgumentSize=size;
}




/** 
    Sets initial number distance from Argument
    Called from MW on startup. Not Used!!
*/
void GraphicsWidget::setInitNumberDistance(int numberDistance){
    qDebug("GW setting initNumberDistance");
    arg_numberDistance=numberDistance;
}


/** 
    Passes initial label distance from Argument
    It is called from MW on startup and when user changes it.
    Not Used!!
*/
void GraphicsWidget::setInitLabelDistance(int labelDistance){
    qDebug("GW setting initLabelDistance");
    arg_labelDistance=labelDistance;
}






/**
*	Changes the visibility of an GraphicsView Attack (number, label, Attack, etc)
*/
void GraphicsWidget::setAttackVisibility(int relation, int source, int target, bool toggle){
    QString AttackName =  QString::number(relation) + QString(":") +
            QString::number( source ) + QString(">")+ QString::number( target );

    if  ( AttacksHash.contains (AttackName) ) {
        qDebug()<<"GW: setAttackVisibility(). relation " << relation
               << " : " << source  << " ->  "<< target << " to " << toggle;
        AttacksHash.value(AttackName) -> setVisible(toggle);
        AttacksHash.value(AttackName) -> setEnabled(toggle);
        return;
    }
    qDebug()<<"GW: setAttackVisibility(). Cannot find Attack " << relation
           << " : " << source  << " ->  "<< target ;

}



/**
*	Changes the visibility of a  Argument
*   Not Used!!
*/
void GraphicsWidget::setArgumentVisibility(long int number, bool toggle){
    if  ( ArgumentHash.contains (number) ) {
        qDebug() << "GW: setArgumentVisibility(): for  "
                 << number << " to " << toggle;
        ArgumentHash.value(number) -> setVisible(toggle);
        ArgumentHash.value(number) -> setEnabled(toggle);
        return;
    }
    qDebug() << "GW: setArgumentVisibility(): cannot find Argument " << number;
}


/**
 * @brief GraphicsWidget::setArgumentSize
 * @param number
 * @param size
 * @return
 */
bool GraphicsWidget::setArgumentSize(long int number, int size ){
    qDebug () << " GraphicsWidget::setArgumentSize() Argument: "<< number
              << " new size "<< size;
    if  ( ArgumentHash.contains (number) ) {
        if (size>0){
            qDebug() << "GW: setArgumentSize(): for "<< number << " to " << size ;
            ArgumentHash.value(number) -> setSize(size);
            return true;

        }
        else {
            qDebug() << "GW: setArgumentSize(): for "<< number
                     << " to initial size" << arg_ArgumentSize;
            ArgumentHash.value(number) -> setSize(arg_ArgumentSize);
            return true;

        }
    }
    qDebug() << "GW: setArgumentSize(): cannot find Argument " << number;
    return false;
}

/**
 * @brief GraphicsWidget::setAllArgumentSize
 * @param size
 * @return
 */
void GraphicsWidget::setAllArgumentSize( ){
    int size =13;
    qDebug() << "GW: setAllArgumentSize():: " << size ;
    foreach ( Argument *arg_Argument, ArgumentHash ) {
        if (size>0){
            arg_Argument -> setSize(size);
        }
        else {
            arg_Argument -> setSize(arg_ArgumentSize);
        }
    }
}

/**
 * @brief GraphicsWidget::setAllArgumentSizeAndShape
 * @param size
 * @return
 */
void GraphicsWidget::setAllArgumentSizeAndShape(int size, QString shape ){
    qDebug() << "GW: setAllArgumentSize() ";
    foreach ( Argument *n_Argument, ArgumentHash ) {
        if (size > 0){
            n_Argument -> setSize(size);
            n_Argument -> setShape(shape);
        }
        else {
            n_Argument -> setSize(13);
            n_Argument -> setShape("circle");
        }
    }
}

//set normal shape to circle
void GraphicsWidget::setAllArgumentsShape() {
    foreach ( Argument *n_Argument, ArgumentHash ) {
            n_Argument -> setShape("circle");
    }
}

/**
 * @brief GraphicsWidget::setArgumentSizeForSolutions
 */
void GraphicsWidget::setArgumentSizeForSolutions(QString arg,double num)
{
    ArgFind.value(arg)->setSize(num);
}



/**
     Marks (by double-sizing and highlighting) or unmarks a Argument, given its name.
     Called by MW:slotFindArgument()
*/
bool GraphicsWidget::setMarkedArgument(QString ArgumentText){
    qDebug ("GW: setMarkedArgument()");
    if (markedArgumentExist) {
        markedArgument1->setSelected(false);		//unselect it, so that it restores its color
        markedArgument1->setSize(markedArgumentOrigSize);	//restore its size
        markedArgumentExist=false;
        return true;
    }

    markedArgument1 = hasArgument (ArgumentText);
    if (!markedArgumentExist)
        return false;

    markedArgument1->setSelected(true);		//select it, so that it changes color
    markedArgumentOrigSize=markedArgument1->size(); // save its original size
    markedArgument1->setSize(markedArgumentOrigSize+3);	//now, make it larger
    markedArgument1->setShape("triangle");
    return true;
}



/**
*	Changes the visibility of all items of certain type (i.e. number, label, Attack, etc)
*/
void GraphicsWidget::setAllItemsVisibility(int type, bool visible){
    QList<QGraphicsItem *> list = scene()->items();
    for (QList<QGraphicsItem *>::iterator item=list.begin();item!=list.end(); item++) {
        qDebug()<< "GW::setAllItemsVisibility. item type is " << (*item)->type();
        if ( (*item)->type() == type){
            if (visible)	(*item)->show();
            else	(*item)->hide();
        }
    }
}




/* Called from MW */
void GraphicsWidget::selectAll()
{
    qDebug() << "GraphicsWidget::selectAll()";
    QPainterPath path;
    path.addRect(0,0, this->scene()->width() , this->scene()->height());
    this->scene()->setSelectionArea(path);
    qDebug() << "selected items now: "
             << selectedItems().count();
}


/* Called from MW */
void GraphicsWidget::selectNone()
{
    qDebug() << "GraphicsWidget::selectNone()";
    this->scene()->clearSelection();
}

/* Called from MW */
QList<QGraphicsItem *> GraphicsWidget::selectedItems(){
     return this->scene()->selectedItems();

}

/** 	
    Starts a new Argument when the user double-clicks somewhere
    Emits userDoubleClicked to MW slot addArgumentWithMouse() which
        - displays Argument info on MW status bar and
        - calls Graph::createVertex(), which in turn calls this->drawArgument()...
        Yes, we make a full circle! :)
*/
void GraphicsWidget::mouseDoubleClickEvent ( QMouseEvent * e ) {

    if ( QGraphicsItem *item= itemAt(e->pos() ) ) {
        if (Argument *arg = qgraphicsitem_cast<Argument *>(item)) {
            Q_UNUSED(arg);
            qDebug() << "GW: mouseDoubleClickEvent() double click on a Argument detected!"
                     << " Cant create new one!";
            return;
        }
    }

    QPointF p = mapToScene(e->pos());
    qDebug()<< "GW::mouseDoubleClickEvent()"
            << " double click on empty space. "
            << " Signaling MW to create a new vertex in graph. e->pos() "
            << e->pos().x() << ","<< e->pos().y() << ", "<< p.x() << "," <<p.y();
    emit userDoubleClicked(-1, p);
    qDebug() << "GW::mouseDoubleClickEvent() "
             << "Scene items: "<< scene()->items().size()
             << " GW items: " << items().size();
}



void GraphicsWidget::mousePressEvent( QMouseEvent * e ) {

    QPointF p = mapToScene(e->pos());

    bool ctrlKey = (e->modifiers() == Qt::ControlModifier);


    qDebug() << "GW::mousePressEvent() click at "
                << e->pos().x() << ","<< e->pos().y()
                << " or "<<  p.x() << ","<< p.y()
                << " selectedItems " << selectedItems().count();

  //  emit selectedItems(arg_selectedItems);

    if ( QGraphicsItem *item= itemAt(e->pos() ) ) {

        qDebug() << "GW::mousePressEvent() click on an item" ;

        if (Argument *arg = qgraphicsitem_cast<Argument *>(item)) {
            Q_UNUSED(arg);
            qDebug() << "GW::mousePressEvent() single click on a Argument " ;
            QGraphicsView::mousePressEvent(e);
            return;
        }
        if (Attack *att= qgraphicsitem_cast<Attack *>(item)) {
            Q_UNUSED(att);
            qDebug() << "GW::mousePressEvent() single click on an Attack ";
            QGraphicsView::mousePressEvent(e);
            return;
        }
    }
    else{
        qDebug() << "GW::mousePressEvent()  click on empty space. ";

        if ( selectedItems().count() > 0 && ctrlKey ) {
            qDebug() << " opening selection context menu ";
            emit openContextMenu(p);
        }

        else if ( e->button()==Qt::RightButton   ) {
            qDebug() << "GW::mousePressEvent() Right-click on empty space ";
            emit openContextMenu(p);
        }
        QGraphicsView::mousePressEvent(e);
    }

}


/**
 * @brief GraphicsWidget::mouseReleaseEvent
 * @param e Release Event
 *
 */
void GraphicsWidget::mouseReleaseEvent( QMouseEvent * e ) {
    QPointF p = mapToScene(e->pos());
    qDebug() << "GW::mouseReleaseEvent() at "
             << e->pos().x() << ","<< e->pos().y()
             << " or "<<  p.x() << ","<<p.y();

    if ( QGraphicsItem *item= itemAt(e->pos() ) ) {
        if (Argument *arg = qgraphicsitem_cast<Argument *>(item)) {
            qDebug() << "GW::mouseReleaseEvent() on a Argument ";
            Q_UNUSED(arg);
            QGraphicsView::mouseReleaseEvent(e);
        }
        if (Attack *att = qgraphicsitem_cast<Attack *>(item)) {
            qDebug() << "GW::mouseReleaseEvent() on a Argument ";
            Q_UNUSED(att);
            QGraphicsView::mouseReleaseEvent(e);
        }
    }
    else{
        qDebug() << "GW::mouseReleaseEvent() on empty space. ";

    }

    qDebug() << "GW::mouseReleaseEvent() - selected items now: "
             << selectedItems().count();

}


/** 
    Calls the scaleView() when the user spins the mouse wheel
    It passes delta as new arg_scale

void GraphicsWidget::wheelEvent(QWheelEvent *e) {
    qDebug("GW: Mouse wheel event");
    qDebug() << "GW: delta = " << e->delta();
    float arg_scale = e->delta() / qreal(800);
    qDebug("GW: arg_scale = %f", arg_scale);
    if ( arg_scale > 0)
        zoomIn();
    else  if ( arg_scale < 0)
        zoomOut();
    else arg_scale=1;
}
*/


#ifndef QT_NO_WHEELEVENT
//! [5]
void GraphicsWidget::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
}
//! [5]
#endif

void GraphicsWidget::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}

/** 
*Called from MW magnifier widgets

void GraphicsWidget::zoomOut (){
    if (zoomIndex > 0) {
        zoomIndex--;
        changeZoom(zoomIndex);
    }
    qDebug("GW: ZoomOut() index %i", zoomIndex);
}	

*/
/** 
    Called from MW magnifier widgets

void GraphicsWidget::zoomIn(){
    qDebug("GW: ZoomIn()");
    if (zoomIndex < 6) {
        zoomIndex++;
        changeZoom(zoomIndex);
    }
    qDebug("GW: ZoomIn() index %i", zoomIndex);
}
*/
void GraphicsWidget::zoomIn()
{
    scaleView(qreal(1.2));
}

void GraphicsWidget::zoomOut()
{
    scaleView(1 / qreal(1.2));
}
/**
      Initiated from MW zoomCombo widget to zoom in or out.
*/
void GraphicsWidget::changeZoom(int value) {
    double scaleFactor = 0.25;
    scaleFactor *= (value + 1);
    arg_currentScaleFactor = scaleFactor;
    resetMatrix();
    this->scale(scaleFactor, scaleFactor);
}

/**
 * @brief GraphicsWidget::rot
 * @param angle
 * used by rotaion
 */
void GraphicsWidget::rot(int angle){
    qDebug("rotating");
    arg_currentRotationAngle = angle;
    resetMatrix();
    scale(arg_currentScaleFactor, arg_currentScaleFactor);
    rotate(angle);

}

/**
 * Used by findArgument.
 * Returns, if found, the Argument with 'text'
 */
Argument* GraphicsWidget::hasArgument( QString text ){

    foreach ( Argument *candidate, ArgFind) {
        if (candidate->argumentName() == text) {
            qDebug() << "GW: hasArgument(): Argument " << text << " found!";
            markedArgumentExist=true;
            return candidate;
            break;
        }

    }
    return markedArgument1;	//dummy return. We check markedArgumentExist flag first...
}

/**
 * @brief GraphicsWidget::hasArgument
 * @param text
 * @return
 */
Argument* GraphicsWidget::hasArgumentNumber( int n ){

    foreach ( Argument *candidate, ArgumentHash) {
        if (candidate->argumentNumbe() == n) {
            qDebug() << "GW: hasArgumentNumber(): Argument " << n << " found!";
            markedArgumentExist=true;
            return candidate;
            break;
        }

    }
    return markedArgument1;	//dummy return. We check markedArgumentExist flag first...
}


void GraphicsWidget::layoutCircularForSolution(double x0, double y0, double maxRadius){
    qDebug() << "Graph::layoutCircularForSolution - ";
    double rad=0, new_radius=0, new_x=0, new_y=0;
    double i = 0;
    int allargumentNumber = allArgumentList.length();
    foreach (Argument *arg, ArgumentHash){

        new_radius=((maxRadius - 2*(allargumentNumber)) + (4*argSolSize.value(arg->argumentName())));
        qDebug () << "Vertice " << arg->argumentName()
                  << " at x=" << arg->x()
                  << ", y= "<< arg->y()
                  << ", maxradius " <<  maxRadius
                  << " new radius " << new_radius;

        //Calculate new position
        rad= (2.0* Pi/ allargumentNumber );
        new_x=x0 + new_radius * cos(i*rad);
        new_y=y0 + new_radius * sin(i*rad);
        arg->setX( new_x );
        arg->setY( new_y );
        i++;
        qDebug("Vertice will move to x=%f and y=%f ", new_x, new_y);
        //Move Argument to new position
        emit moveArgument(arg->argumentNumbe(),  new_x,  new_y);

        emit addGuideCircle (
                    static_cast<int> (x0),
                    static_cast<int> (y0),
                    static_cast<int> (new_radius)
                    );
    }

}

void GraphicsWidget::layoutLinearPosition(double x0, double y0)
{
    qDebug() << "Graph::layoutLinearPosition - ";
    double new_x=-400, new_y= -400;

    foreach (Argument *arg, ArgumentHash) {

        //Calculate new position
        if (argSolSize.value(arg->argumentName()) > 0) {
            new_x = x0 * 10.0 + 400.0 + (argSolSize.value(arg->argumentName())*3) * qrand()%400;
            new_y = y0 * 10.0 + (argSolSize.value(arg->argumentName())*3);
        } else {
            new_x = 10 + 400 + 3* qrand()%400;
            new_y = 0;
        }

        arg->setX( new_x );
        arg->setY( new_y );
        qDebug("Vertice will move to x=%f and y=%f ", new_x, new_y);
        //Move Argument to new position
        emit moveArgument(arg->argumentNumbe(),  new_x,  new_y);
        emit addGuideHLine (static_cast<int> ( new_y ));
    }
    //layoutLinearPositionRand(double x0,double y0);
}


/**
 * @brief GraphicsWidget::layoutLinearPositionRand
 * @param x0
 * @param y0
 */
void GraphicsWidget::layoutLinearPositionRand(double x0, double y0)
{
    Q_UNUSED(x0);
    Q_UNUSED(y0);
    qDebug() << "Graph::layoutLinearPositionRand - ";
    double new_x =0;
    int j = 1;
    foreach (Argument *arg, ArgumentHash) {

        //Calculate new position
        foreach (Argument *argNew, ArgumentHash) {
            if (argSolSize.value(arg->argumentName()) == argSolSize.value(argNew->argumentName())) {
                new_x = 400 + j* qrand()%400;
                break;
            }
        }
        j=j+5;
        arg->setX(new_x);
        //Move Argument to new position
        emit moveArgumentX(arg->argumentNumbe(),  new_x);

    }
}

/** 
*Destructor.
*/
GraphicsWidget::~GraphicsWidget(){
}

