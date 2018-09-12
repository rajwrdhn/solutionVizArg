/***************************************************************************
    solutionVizArg: Visualization of Solutions of Abstract Argumentation Frameworks


    Written in Qt/C++

                             -------------------
    This software is written as a University project.
 ***************************************************************************/
#include "argument.h"
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOption>
#include <QPainter>
#include <QtGlobal>  //for QT_VERSION
#include <QDebug>
#include "graphicswidget.h"
#include "attack.h"
#include <math.h> //sqrt


/**
 * @brief Argument::Argument Constructor
 * @param gw
 * @param num
 * @param size
 * @param col
 * @param shape
 * @param numIn
 * @param ldist
 * @param ndist
 * @param p
 */
Argument::Argument( GraphicsWidget* gw, QString argName, int num, int size,
                    QString col, QString shape,
                    int ldist, int ndist, QPointF p
                    ) : graphicsWidget (gw)
{
    Q_UNUSED(p);
    graphicsWidget->scene()->addItem(this); //Without this Arguments don't appear on the screen...

    setFlags(ItemSendsGeometryChanges | ItemIsSelectable | ItemIsMovable);
    setCacheMode(QGraphicsItem::ItemCoordinateCache);
    setAcceptHoverEvents(true);
    arg_name = argName;
    arg_num=num;
    arg_size=size;
    arg_shape=shape;
    arg_col_str=col;
    arg_col=QColor(col);
    arg_nd=ndist;
    arg_ld=ldist;
    arg_poly_t=new QPolygon(3);
    arg_poly_d=new QPolygon(4);
    qDebug()<< "Argument: constructor: initial position at: "
            << this->x()<<", "<<this->y()
            << " Will move at: "<< p.x()<<", "<<p.y();
} 



/** 
* Used by MW::slotChangeArgumentColor
* Used by GW::setAllArgumentColor()
*/
void Argument::setColor(QString str) {
    prepareGeometryChange();
    arg_col=QColor(str);
    update();
}

/** 
* Used by SolutionDialog1,SolutionDialog2 and SolutionDialog3.
* Used when QColor is passed as argument and
* not color as string name.
*/
void Argument::setColor(QColor color){
    prepareGeometryChange();
    arg_col=color;
    arg_col_str = arg_col.name();
    update();
}

//returns arg color
QString Argument::color() {
    return arg_col_str;
}


/** Sets the size of the Argument */
void Argument::setSize(double size){
    qDebug("Argument: setSize()");
    prepareGeometryChange();
    arg_size=size;
    foreach (Attack *Attack, inAttackList) {
        qDebug("Argument: updating Attacks in inAttackList");
        Attack->setEndOffset(size);
    }
    foreach (Attack *Attack, outAttackList) {
        qDebug("Argument: updating Attacks in outAttackList");
        Attack->setStartOffset(size);
    }
    setShape(arg_shape);
}



/**  Used by MainWindow::findArgument() and Attack::Attack()  */
int Argument::size() {
    qDebug("size()");
    return arg_size;
}


/**  Called every time the user needs to change the shape of an Argument. */
void Argument::setShape(const QString shape) {
    qDebug("Argument: setShape()");
    prepareGeometryChange();
    arg_shape=shape;
    qDebug ("Argument: setShape(): Argument is at x=%f and y=%f", x(), y());
    update();
}



/**
*	Returns the shape of the Argument as a path (an accurate outline of the item's shape)
*	Used by the collision algorithm in collidesWithItem() 
*/
QPainterPath Argument::shape() const {
    return (*arg_path);
}


/**
 *  Returns the bounding rectangle of the Argument
 *  That is the rectangle where all painting will take place.
 */
QRectF Argument::boundingRect() const {
    qreal adjust = 6;
    return QRectF(-arg_size -adjust , -arg_size-adjust , 2*arg_size+adjust , 2*arg_size +adjust);
}


/** 
*	Does the actual painting.
*	Called by GraphicsView in every update()
*/
void Argument::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *) {


    //if selected darken the Argument_arg
    if (option->state & QStyle::State_Selected) {
        painter->setBrush(arg_col.dark(150));
    }
    else if (option->state & QStyle::State_MouseOver) {
        painter->setBrush(arg_col.dark(150));
        setZValue(255);
    }
    else { //no, just paint it with the usual color.
        setZValue(254);
        painter->setBrush(arg_col);
    }
    painter->setPen(QPen(Qt::black, 0));
    //all shapes possible for the argument.
    arg_path = new QPainterPath;
    if ( arg_shape == "circle") {
        arg_path->addEllipse (-arg_size, -arg_size, 2*arg_size, 2*arg_size);
    }
    else if ( arg_shape == "ellipse") {
        arg_path->addEllipse(-arg_size, -arg_size, 2*arg_size, 1.5* arg_size);
    }
    else if ( arg_shape == "box" || arg_shape == "rectangle"  ) {
        arg_path->addRect (-arg_size , -arg_size , 1.8*arg_size , 1.8*arg_size );
    }
    else if ( arg_shape == "triangle") {
        arg_poly_t -> setPoints (3,  0,-arg_size,  -arg_size,arg_size, arg_size,+arg_size);
        arg_path->addPolygon(*arg_poly_t);
        arg_path->closeSubpath();
    }
    else if ( arg_shape == "diamond"){
        arg_poly_d -> setPoints (4, 0,-arg_size,  -arg_size,0,       0,+arg_size,     +arg_size,0);
        arg_path->addPolygon(*arg_poly_d);
        arg_path->closeSubpath();
    }

    painter->drawPath (*arg_path);
    painter->setFont(QFont("Arial", arg_size/2));
    painter->drawText(boundingRect(), Qt::AlignCenter, arg_name);
}


/** 
 *	Propagates the changes to connected elements, i.e. Attacks, numbers, etc.
 *  Checks if the Argument is inside the scene.
 */
QVariant Argument::itemChange(GraphicsItemChange change, const QVariant &value) {
    QPointF newPos = value.toPointF();

    switch (change) {
    case ItemPositionHasChanged :
    {
        foreach (Attack *att, inAttackList)  //Move each inAttack of this Argument
            att->adjust();
        foreach (Attack *att, outAttackList) //Move each outAttack of this Argument
            att->adjust();

        if ( newPos.x() !=0 && newPos.y() != 0 ){
            graphicsWidget->ArgumentMoved(argumentNumbe(), (int) newPos.x(), (int) newPos.y());
        }
        else qDebug()<<  "Argument: ItemChange():  Not emitting ArgumentMoved. Argument "
                      << argumentNumbe()<<" is at 0,0";
        break;
    }
    case ItemEnabledHasChanged:{
        if (ItemEnabledHasChanged) {
            return 1;
        }
        else{
            return 0;
        }
    }
    case ItemVisibleHasChanged:
    {
        if (ItemVisibleHasChanged){
            return 1;
        }
        else{
            return 0;
        }
    }
    default:
    {
        break;
    }
    };
    return QGraphicsItem::itemChange(change, value);
}



/** handles the events of a click on a Argument */
void Argument::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    qDebug() << "Argument::mousePressEvent() "
             << " set selected and emitting ArgumentClicked";
    this->setSelected(true);
    graphicsWidget->ArgumentClicked(this);
    if ( event->button()==Qt::RightButton ) {
        qDebug("Argument: Right-click on Argument, at %i, %i", event->screenPos().x(), event->screenPos().y());
        graphicsWidget->openArgumentContextMenu();
        /** Update commented out - caused segmentation fault when removing Argument */
    }
    if ( event->button()==Qt::MidButton) {
        qDebug("Argument:: Middle-Click on a Argument. Calling GraphicsWidget startAttack()");
        graphicsWidget->startAttack(this);
    }
}

/**
 * @brief Argument::mouseReleaseEvent
 * @param event
 * releasing the select mode of the argument.
 */
void Argument::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

/**
 * @brief Argument::addInLink
 * @param att
 * storing the in links to an argument.
 */
void Argument::addInLink( Attack *att ) {
    qDebug() << "Argument:  addInLink() for "<<  arg_num;
    inAttackList.push_back( att );
}

/**
 * @brief Argument::deleteInLink
 * @param link
 * removing the in links when arg deleted or removed.
 */
void Argument::deleteInLink( Attack *link ){
    qDebug () << "Argument:  deleteInLink for "<< arg_num;
    inAttackList.remove( link);
}


/**
 * @brief Argument::addOutLink
 * @param att
 * storing outlinks attacks from an argument.
 */
void Argument::addOutLink( Attack *att ) {
    qDebug() << "Argument: addOutLink()" << arg_num;
    outAttackList.push_back( att );
}

/**
 * @brief Argument::deleteOutLink
 * @param link
 * removing the outlinks when the argument is deleted.
 */
void Argument::deleteOutLink(Attack *link){
    qDebug () << "Argument: deleteOutLink() from " <<  arg_num;
    outAttackList.remove( link);
}
/**
 * @brief Argument::~Argument
 * Destructor
 */
Argument::~Argument(){
    qDebug() << "\n\n\n *** ~Argument() "<< argumentNumbe();
    foreach (Attack *att, inAttackList) {
        qDebug("~Argument: removing Attacks in inAttackList");
        graphicsWidget->removeItem(att);
    }
    foreach (Attack *att, outAttackList) {
        qDebug() <<"~Argument: removing Attacks in outAttackList";
                graphicsWidget->removeItem(att);
    }
    clear();
    this->hide();
    graphicsWidget->removeItem(this);
}
/**
 * @brief Argument::clear
 * used to clear Lists only.
 */
void Argument::clear() {
    inAttackList.clear();
    outAttackList.clear();
}

