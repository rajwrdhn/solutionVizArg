/***************************************************************************
    solutionVizArg: Visualization of Solutions of Abstract Argumentation Frameworks


    Written in Qt/C++

                             -------------------
    This software is written as a University project.
 ***************************************************************************/
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QtDebug>		//used for qDebug messages
#include <cmath>
#include "graphicswidget.h"
#include "attack.h"
#include "argument.h"


static const double Pi = 3.14159265;
static double TwoPi = 2.0 * Pi;

Attack::Attack( GraphicsWidget *gw,
                Argument *from,
                Argument *to,
                const Qt::PenStyle &style,
                const float &width,
                const int &ArgumentSize,
                const QString &color,
                const bool &reciprocal,
                const bool &drawArrows,
                const bool &bez ) : graphicsWidget(gw)
{
    Q_UNUSED(ArgumentSize);
    graphicsWidget->scene()->addItem(this);  //add Attack to scene to be displayed

    from->addOutLink( this );	//adds this Attack to sourceArgument
    to->addInLink( this );		//adds this Attack to targetArgument

    source=from;			//saves the sourceArgument
    target=to;			//Saves the targetArgument
    arg_style = style;
    arg_color=color;
    arg_drawArrows=drawArrows;
    arg_reciprocal=reciprocal; //for symmetric attack
    arg_startOffset=source->size();  //used to offset Attack from the centre of Argument
    arg_endOffset=target->size();  //used to offset Attack from the centre of Argument
    arg_arrowSize=4;		//controls the width of the Attack arrow
    eFrom = source->argumentName() ;
    eTo = target->argumentName() ;
    numFrom = source->argumentNumbe();
    numTo = target->argumentNumbe();
    arg_width = width ;
    arg_Bezier = bez; //self attack
    adjust();
}



void Attack::showArrows(bool drawArrows){
    prepareGeometryChange();
    arg_drawArrows=drawArrows;
}



void Attack::removeRefs(){
    //FIXME Need to disconnect signals from Argument to this "erased" Attack // DONE
    qDebug("Attack: removeRefs()");
    source->deleteOutLink(this);
    target->deleteInLink(this);
}

//set att color
void Attack::setColor( const QString &str) {
    arg_color=str;
    prepareGeometryChange();
}


//return att color
QString Attack::color() const{
    return arg_color;
}

/**
 * @brief Attack::colorToPajek
 * Called from Graph::saveGraphToPajekFormat()
 * @return
 */
QString Attack::colorToPajek() {
    if (arg_color.startsWith("#")) {
        return  ("RGB"+arg_color.right( arg_color.size()-1 )).toUpper()  ;
    }
    return arg_color;
}


void Attack::setStartOffset(int offset){
    arg_startOffset=offset;
}

void Attack::setEndOffset(int offset){
    arg_endOffset=offset;
}


//source arg
Argument *Attack::sourceArgument() const {
    return source;
}

void Attack::setSourceArgument(Argument *argument) {
    source = argument;
    adjust();
}

//target arg
Argument *Attack::targetArgument() const {
    return target;
}


void Attack::setTargetArgument(Argument *argument){
    target = argument;
    adjust();
}

//returns source arg
QString Attack::sourceArgumentName () {
    return eFrom;
}

//returns target arg
QString Attack::targetArgumentName() {
    return eTo;
}

//source arg num
int Attack::sourceArgumentNumber () {
    return numFrom;
}

//target arg num
int Attack::targetArgumentNumber () {
    return numTo;
}

/**
 * @brief Attack::adjust
 * leaves some empty space (offset) from Argument -
 * make the Attack width appear on the centre of the Attack
 */
void Attack::adjust(){
    // qDebug("Attack: adjust()");
    if (!source || !target)
        return;
    QLineF line(mapFromItem(source, 0, 0), mapFromItem(target, 0, 0));
    QPointF AttackOffset;
    if (source!=target) {
        qreal length = line.length();
        AttackOffset = QPointF((line.dx() * arg_endOffset) / length, (line.dy() *arg_endOffset) / length);
    }
    else AttackOffset = QPointF(0, 0);

    prepareGeometryChange();

    sourcePoint = line.p1() + AttackOffset;
    targetPoint = line.p2() - AttackOffset;
}



/**
 * @brief Attack::shape
 * Returns the shape of this Attack as a QPainterPath in local coordinates.
 * The shape is used for many things, including collision detection, hit tests,
 * and for the QGraphicsScene::items() functions.
 * The default implementation calls boundingRect() to return a simple rectangular shape,
 * but we reimplement it to return a more accurate shape for non-rectangular items.
 * @return QPainterPath
 */
QPainterPath Attack::shape () const {
    //qDebug()<<"Attack::shape()";		//too many debug messages...
    QPainterPath path;
    qreal extra = ( width() + arg_arrowSize);
    QLineF line(sourcePoint, targetPoint);
    QPolygonF poly;
    line.translate(extra,extra);
    poly.push_back(line.p1());
    poly.push_back(line.p2());
    line.translate(-extra,-extra);
    poly.push_back(line.p1());
    poly.push_back(line.p2());
    path.addPolygon(poly);
    path.closeSubpath();
    //path.addRegion(boundingRegion(QTransform()));
    //   path.addRect(boundingRect());
    return path;
} 


/**
 * @brief Attack::boundingRect
 * Defines the outer bounds of the Attack as a rectangle;
 * All painting will be restricted to inside the Attack's bounding rect.
 * Qt uses this bounding rect to determine whether the Attack requires redrawing.
 * @return
 */
QRectF Attack::boundingRect() const {
    if (!source || !target)
        return QRectF();
    qreal penWidth = 1;
    qreal extra = ( penWidth + arg_arrowSize) / 2.0;
    QRectF a = QRectF (
                sourcePoint,
                QSizeF(
                    targetPoint.x() - sourcePoint.x(), targetPoint.y() - sourcePoint.y())
                ).normalized().adjusted(-extra, -extra, extra, extra);
    //qDebug()<<"Attack::boundingRect() extra = " << extra << "QSizeF width "<< a.width() << " QSizeF height "<< a.height();
    if (source==target) {		//self-Attack has different bounding rect.
        return QRectF (
                    sourcePoint-QPointF(30,30),
                    QSizeF(60,30)).normalized().adjusted(-extra, -extra, extra, extra);
    }
    return a;
}



void Attack::makeReciprocal(){
    qDebug("Attack::makeReciprocal()");
    prepareGeometryChange();
    arg_reciprocal= true;
}



void Attack::unmakeReciprocal(){
    qDebug("Attack::unmakeReciprocal()");
    //prepareGeometryChange();
    arg_reciprocal= false;
}

//for symmetric attacks
bool Attack::isReciprocal() {
    return arg_reciprocal;
}


void Attack::setStyle( const Qt::PenStyle  &style ) {
    arg_style = style;
}

Qt::PenStyle Attack::style() const{
    return arg_style;

}

QPen Attack::pen() const {
    if (arg_width < 0 ){
        return  QPen(QColor("red"), width(), Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    }
    return QPen(QColor(arg_color), width(), style(), Qt::RoundCap, Qt::RoundJoin);
}



void Attack::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *){
    if (!source || !target)
        return;
    Q_UNUSED(option); //	painter->setClipRect( option->exposedRect );

    //Define the path upon which we' ll draw the line
    QPainterPath line(sourcePoint);

    //Construct the path
    if (source!=target) {
        if ( !arg_Bezier){
            //   qDebug()<< "*** Attack::paint(). Constructing a line";
            line.lineTo(targetPoint);
        }
        else {
            qDebug() << "*** Attack::paint(). Constructing a bezier curve";
        }
    }
    else { //self-link
        QPointF c1 = QPointF( targetPoint.x() -30,  targetPoint.y() -30 );
        QPointF c2 = QPointF( targetPoint.x() +30,  targetPoint.y() -30 );
        //        qDebug()<< "*** Attack::paint(). Constructing a bezier self curve c1 "
        //                <<c1.x()<<","<<c1.y()  << " and c2 "<<c2.x()<<","<<c2.y();
        line.cubicTo( c1, c2, targetPoint);
    }

    //Prepare the pen
    //	qDebug()<<"*** Attack::paint(). Preparing the pen with width "<< width();

    painter->setPen( pen() );

    //Draw the arrows only if we have different Arguments.
    if (arg_drawArrows && source!=target) {
        angle = 0;
        line_length = line.length();
        line_dx = targetPoint.x()-sourcePoint.x();
        line_dy = targetPoint.y()-sourcePoint.y();
        if ( line.length() >0 )
            angle = ::acos( line_dx / line_length );
        //		qDebug() << " acos() " << ::acos( line_dx  / line_length ) ;

        if ( line_dy  >= 0)
            angle = TwoPi - angle;

        QPointF destArrowP1 = targetPoint + QPointF(sin(angle - Pi / 3) * arg_arrowSize,
                                                    cos(angle - Pi / 3) * arg_arrowSize);
        QPointF destArrowP2 = targetPoint + QPointF(sin(angle - Pi + Pi / 3) * arg_arrowSize,
                                                    cos(angle - Pi + Pi / 3) * arg_arrowSize);
        //		qDebug() << "*** Attack::paint() destArrowP1 "
        //                 <<  destArrowP1.x() << "," << destArrowP1.y()
        //                  << "  destArrowP2 " <<  destArrowP2.x() << "," << destArrowP2.y();
        painter->setBrush(QColor(arg_color));
        QPolygonF destP;
        destP << targetPoint << destArrowP1 << destArrowP2;
        line.addPolygon ( destP);

        if (arg_reciprocal) {
            //            qDebug() << "**** Attack::paint() This Attack is SYMMETRIC! "
            //                     << " So, we need to create Arrow at src Argument as well";
            QPointF srcArrowP1 = sourcePoint + QPointF(sin(angle +Pi / 3) * arg_arrowSize,
                                                       cos(angle +Pi / 3) * arg_arrowSize);
            QPointF srcArrowP2 = sourcePoint + QPointF(sin(angle +Pi - Pi  / 3) * arg_arrowSize,
                                                       cos(angle +Pi - Pi / 3) * arg_arrowSize);
            QPolygonF srcP;
            srcP << sourcePoint<< srcArrowP1<< srcArrowP2;
            line.addPolygon ( srcP);
        }
        else {
            // qDebug() << "*** Attack::paint() Not symmetric Attack. Finish";
        }
    }
    else {
        //        qDebug()<< "*** Attack::paint(). This Attack is self-link - CONTINUE!";
    }
    //	qDebug()<< "### Attack::paint(). DrawPath now....";
    painter->drawPath(line);
}


/** 
    Controls the width of the Attack; is a function of Attack width
*/
float Attack::width() const{
    if ( fabs(arg_width) > 1  )
        return  1  + fabs(arg_width)/10;
    return 1;	//	Default, if  arg_width in (-1, 1) space
}

/** handles the events of a click on an Attack*/
void Attack::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    qDebug("Attack: pressEvent() emitting AttackClicked");
    graphicsWidget->AttackClicked(this);
    if ( event->button()==Qt::LeftButton ) {
        qDebug() << "Attack: Attack pressEvent() left click > ";
    }
    if ( event->button()==Qt::RightButton ) {
        qDebug("Attack: Right-click on an Attack,  at %i, %i", event->screenPos().x(), event->screenPos().y());
        graphicsWidget->openAttackContextMenu();
    }
}

/**
 * @brief Attack::mouseReleaseEvent
 * @param event
 * releases the selection of an attack from the view.
 */
void Attack::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

/**
 * @brief Attack::~Attack
 * Destructs the references
 */
Attack::~Attack(){
    qDebug() << "\n\n\n *** ~Attack() " << sourceArgumentNumber()<< "->" << targetArgumentNumber();
    removeRefs();
    widthList.clear();
    this->hide();
}

