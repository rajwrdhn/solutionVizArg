/***************************************************************************
    solutionVizArg: Visualization of Solutions of Abstract Argumentation Frameworks


    Written in Qt/C++

                             -------------------
    This software is written as a University project.
 ***************************************************************************/
#include "graph.h"
#include <QFile>
#include <QtMath>
#include <QPointF>
#include <QDebug>		//used for qDebug messages
#include <QHash>
#include <QColor>
#include <QTextCodec>
#include <cstdlib>		//allows the use of RAND_MAX macro
#include <math.h>
#include <ctime>        // for makeThingsLookRandom



static qreal Pi = 3.14159265;

Graph::Graph() {
    vert_totalVertices=0;
    outboundAttacksVert=0;
    inboundAttacksVert=0;
    reciprocalAttacksVert=0;
    order=true;		//returns true if the indexes of the list is ordered.
    graphModified=false;
    symmetricadjacent=true;
    vert_precision = 5;
    dynamicMovement=false;
    timerId=0;
    layoutType=0;
}


/**
    main Argument creation slot, associated with homonymous signal MW.
    Adds a Vertex to the Graph and calls addArgument of GraphicsWidget
    p holds the desired position of the new Argument.
    The new Vertex is named argName.
*/
void Graph::createVertex(QString argName,int i, int size, QString ArgumentColor,
                         QPointF p, QString ArgumentShape, bool signalMW, bool upload){

    /*
    foreach (QString argN, allArgumentList) {
        if (argName == argN ) {
            qDebug() << "Argument Name already exists" << argN ;
            return;
        }
    }
    */

    allArgumentList << argName;
    qDebug("here create vertex");
    int value = 1;
    addVertex(i, value, size,  ArgumentColor, p, ArgumentShape);
    qDebug("vertex added");
    emit drawArgument(argName, i, size,  ArgumentColor,
                   p, ArgumentShape, upload);
    if (signalMW)
        emit graphChanged();
    //draw new user-clicked Arguments with the same color with that of the file loaded
    initVertexColor=ArgumentColor;
    initVertexShape=ArgumentShape;
    initVertexSize=size;
}


/**
    auxilliary Argument creation slot.
    Called from GW, with i and p as parameters.
    p holds the desired position of the new Argument.
    Calls the main creation slot with init Argument values.
*/
void Graph::createVertex(int i, QPointF p){
    if ( i < 0 )  i = lastVertexNumber() +1;
    qDebug() << "Graph::createVertex() " << i << " fixed coords.";
    createVertex(	QString::number(i),i, initVertexSize,  initVertexColor,
                    p, initVertexShape, true, false
                    );
}




/**
    second auxilliary Argument creation slot.
    Called from MW only with parameter i.
    Calculates a random position p from canvasWidth and Height.
    Then calls the main creation slot with init Argument values.
*/
void Graph::createVertex(int i, int cWidth, int cHeight){
    if ( i < 0 )  i = lastVertexNumber() +1;
    qDebug() << "Graph::createVertex() " << i << " random coords.";
    QPointF p;
    p.setX(rand()%cWidth);
    p.setY(rand()%cHeight);
    createVertex(   QString::number(i), i, initVertexSize, initVertexColor,
                    p, initVertexShape, true, false
                    );
}



/**
    third auxilliary Argument creation slot.
    Called after upload with parameter i.
    Calculates a random position p from canvasWidth and Height.
    Then calls the main creation slot with init Argument values.
*/

void Graph::createVertexUpload(bool upload, QString argName,int i, int cWidth, int cHeight) {
    if (allArgumentList.contains(argName)) {
        return;
    }
    if ( i < 0 )  i = lastVertexNumber() +1;
    qDebug() << "Graph::createVertexUpload() " << i << " rand coords";
    QPointF p;
    p.setX(rand()%cWidth);
    p.setY(rand()%cHeight);
    createVertex( argName, i, initVertexSize,  initVertexColor,
                    p, initVertexShape, true, upload
                    );

}

/**
 * @brief Graph::setCanvasDimensions
 * @param w width of canvas
 * @param h height of canvas
 */
void Graph::setCanvasDimensions(int w, int h){
    qDebug() << "Graph:: setCanvasDimensions() to " << w << " " << h ;
    canvasWidth = w;
    canvasHeight= h;
}


/**
 * @brief Graph::createAttack
    Adds an Attack to the Graph, then emits drawAttack() which calls
    GraphicsWidget::addAttack() to draw the new Attack.
    Also called from MW when user clicks on the "add link" button
    Alse called from GW (via createAttack() below) when user middle-clicks.
 * @param v1
 * @param v2
 * the parameter weight can be later used for weighted argumentation frmaework.
 * @param color
 * @param reciprocal
 * @param drawArrows
 * @param bezier
 */
void Graph::createAttack(QString argName_i,QString argName_j,int v1, int v2, float weight, QString color,
                       int reciprocal, bool drawArrows, bool bezier,bool upload){
    qDebug()<<"-- Graph::createAttack() - " << argName_i << " -> " << argName_j
              << " reciprocal " << reciprocal;
    if (argName_j.isEmpty()) {
        return;
    }
    // check whether there is already such an Attack
    if (!hasArc(v1,v2)){
        if ( reciprocal == 2) {
            qDebug()<< "-- Graph::createAttack() - "
                    << "Creating RECIPROCAL Attack - emitting drawAttack signal to GW";
            addAttack ( v1, v2, weight, color, reciprocal);
            emit drawAttack(argName_i,argName_j,v1, v2, weight, reciprocal, drawArrows, color, bezier, upload);
        }
        else if (this->hasArc( v2, v1) )  {
            qDebug()<<"-- Graph::createAttack() - Opposite arc exists. "
                   << "  Emitting drawAttackReciprocal to GW ";
            reciprocal = 1;
            addAttack ( v1, v2, weight, color, reciprocal);
            emit drawAttackReciprocal(argName_j,argName_i,v2, v1,upload);
        }
        else {
            qDebug()<< "-- Graph::createAttack() - "
                       << "Opposite arc does not exist. Emitting drawAttack to GW...";
            reciprocal = 0;
            addAttack ( v1, v2, weight, color, reciprocal);
            emit drawAttack(argName_i,argName_j,v1, v2, weight, reciprocal, drawArrows, color, bezier, upload);
        }
    }
    else {
        qDebug() << "-- Graph::createAttack() - "
                    << "Attack " << v1 << " -> " << v2
                 << " declared previously (exists) - nothing to do \n\n";
    }
    //draw new Attacks the same color with those of the file loaded,
    // on user clicks on the canvas
    initAttackColor=color;
    emit graphChanged();
}


/**
    Called (via MW::addLink()) from GW when user middle-clicks on two Arguments.
    Calls the above createAttack() method with initAttackColor to set the default Attack color.
*/
void Graph::createAttack(QString argName_i, QString argName_j, int v1, int v2, float weight, int reciprocal=0,
                       bool drawArrows=true, bool bezier=false){
    qDebug()<< "Graph::createAttack() - " << v1<< " -> " << v2 ;
    createAttack(argName_i,argName_j,v1, v2, (float) weight, initAttackColor, reciprocal,
               drawArrows, bezier,false);
}


/**
    Called from MW Upload when it finds an new link
    Calls the above createAttack() method with initAttackColor
*/
void Graph::createAttackUpload (QString argName_i, QString argName_j, int v1, int v2, float weight, int reciprocal,
                                bool drawArrows, bool bezier) {
    weight = 1.0;
    reciprocal=false;
    drawArrows=true;
    bezier=false;
    createAttack(argName_i,argName_j,v1, v2, (float) weight, initAttackColor, reciprocal,
               drawArrows, bezier,true);
}


/**
 * @brief Graph::addVertex
 * @param v1
 * @param val
 * @param size
 * @param color
 * @param p
 * @param shape
 * adds vertex in a qhash.
 */
void Graph::addVertex (int v1, int val, int size, QString color,
        QPointF p, QString shape
        ){

    qDebug() << "Graph::addVertex() ";
    if (order)
        index[v1]=vert_totalVertices;
    else
        index[v1]=vert_graph.size();

    vert_graph.append(
                new Vertex
                (this, v1, val, size, color,
                 p, shape
                 )
                );
    vert_totalVertices++;
    graphModified=true;
}


/**	Returns the name of the last vertex.
    Used by slotRemoveArgument of MW
*/
int Graph::lastVertexNumber() {
    if (vert_totalVertices>0)
        return vert_graph.back()->name();
    else return 0;
}


/**	Returns the name of the first vertex.
    Used by slotRemoveArgument of MW
*/
int Graph::firstVertexNumber() {
    if (vert_totalVertices>0)
        return vert_graph.front()->name();
    else return 0;
}



/**	Removes the vertex named Doomed from the graph
    First, it removes all Attacks to Doomed from other vertices
    Then it changes the index of all subsequent vertices inside vert_graph
    Finally, it removes the vertex.
*/
void Graph::removeVertex(long int Doomed){
    qDebug() << "Graph: removeVertex - Doomed: "
             << vert_graph[ index[Doomed] ]->name()
             << "  indexOfDoomed= " << index[Doomed] ;
    long int indexOfDoomed=index[Doomed];

    //Remove links to Doomed from each other vertex
    QList<Vertex*>::const_iterator it;
    for (it=vert_graph.cbegin(); it!=vert_graph.cend(); ++it){
        if  ( (*it)->hasAttackTo(Doomed) != 0) {
            qDebug()<< "Graph: Vertex " << (*it)->name()
                    << " is linked to doomed "<< Doomed << " and has "
                    << (*it)->outAttacks();
            if ( (*it)->outAttacks() == 1 && (*it)->hasAttackFrom(Doomed) != 0 )	{
                (*it)->setReciprocalLinked(false);
            }
            (*it)->removeAttackTo(Doomed) ;
        }
        if (  (*it)->hasAttackFrom(Doomed) != 0 ) {
            (*it)->removeAttackFrom(Doomed);
        }
    }

    qDebug()<< "Graph: Finished with vertices. Update the index which maps vertices inside vert_graph " ;
    long int prevIndex=indexOfDoomed;

    qDebug () << " Updating index of all subsequent vertices ";
    H_Int::const_iterator it1=index.cbegin();
    while (it1 != index.cend()){
        if ( it1.value() > indexOfDoomed ) {
            prevIndex = it1.value();
            qDebug() << "Graph::removeVertex - vertex " << it1.key()
                     << " had prevIndex: " << prevIndex
                     << " > indexOfDoomed " << indexOfDoomed
                     << " Setting new index. Index size was: "<< index.size();
            index.insert( it1.key(), --prevIndex)  ;
            qDebug() << "Graph::removeVertex - vertex " << it1.key()
                     << " new index: " << index.value( it1.key(), -666)
                     << " Index size now: "<< index.size();

        }
        else {
            qDebug() << "Graph::removeVertex " << it1.key() << " with index "
                     << it1.value() << " < indexOfDoomed. CONTINUE";

        }
        ++it1;
    }

    //Now remove vertex Doomed from vert_graph
    qDebug() << "Graph: graph vertices=size="<< vertices() << "="
             << vert_graph.size() <<  " removing vertex at index " << indexOfDoomed ;
    vert_graph.removeAt( indexOfDoomed ) ;
    vert_totalVertices--;
    qDebug() << "Graph: Now graph vertices=size="<< vertices() << "="
             << vert_graph.size() <<  " total Attacks now  " << enabledAttacks();

    order=false;
    graphModified=true;

    emit graphChanged();
    emit eraseArgument(Doomed);
}



/**
* Creates an Attack between v1 and v2
*/
void Graph::addAttack (int v1, int v2, float weight, QString color, int reciprocal) {

    int source=index[v1];
    int target=index[v2];

    qDebug()<< "Graph: addAttack() from vertex "<< v1 << "["<< source
            << "] to vertex "<< v2 << "["<< target << "] of weight "<<weight;

    vert_graph [ source ]->addAttackTo(v2, weight );
    vert_graph [ target ]->addAttackFrom(v1, weight);

    if (reciprocal == 1){
        vert_graph [ source ]->setReciprocalLinked(true);
        vert_graph [ target ]->setReciprocalLinked(true);
    }
    else if (reciprocal == 2){
        vert_graph [ source ]->setReciprocalLinked(true);
        vert_graph [ target ]->setReciprocalLinked(true);
        vert_graph [ target ]->addAttackTo(v1, weight );
        vert_graph [ source ]->addAttackFrom(target, weight);
    }
    vert_graph[ source]->setOutLinkColor(v2, color);

    graphModified=true;
}


/** 	Removes the Attack (arc) between v1 and v2
*/
void Graph::removeAttack (int v1, int v2) {
    qDebug ()<< "\n\n Graph::removeAttack() Attack from " << v1 << " index " << index[v1]
                << " to " << v2 << " to be removed from graph";
    vert_graph [ index[v1] ]->removeAttackTo(v2);
    vert_graph [ index[v2] ]->removeAttackFrom(v1);

    if ( this->hasArc(v2,v1) !=0)
        symmetricadjacent=false;

    graphModified=true;

    emit eraseAttack(v1,v2);
    emit graphChanged();
}

void Graph::slotSetAttackVisibility ( int relation,  int source, int target, bool visible) {
    emit setAttackVisibility ( relation, source, target, visible);
}


/**	Checks if there is a specific vertex in the graph
    Returns the index or -1
    Complexity:  O(logN) for index retrieval
*/
int Graph::hasVertex(long int num){
    qDebug () << "Graph: hasVertex() v: " << num <<  " with index " << index[num]  << " named " << vert_graph[ index[num]] ->name();
    if (  vert_graph[ index[num]] ->name() == num)
        return index[num];
    else
        return -1;
}

/**
 * @brief Graph::setInitVertexSize
 * @param size
 * initial size of vertex.
 */
void Graph::setInitVertexSize (const long int size) {
    initVertexSize=size;
}


//Changes the size.of vertex v, HELPME::is it used?
void Graph::setVertexSize(const long int &v, const int &size) {
    vert_graph[ index[v] ]->setSize(size);
    graphModified=true;
    emit graphChanged();
    emit setArgumentSize(v, size);
}

//returns a vertex size
int Graph::vertexSize(const long &v ) {
    return vert_graph[ index[v] ]-> size();
}

//Changes the size.of all vertices. HELPME::Is it used?
void Graph::setAllVerticesSize(const int &size) {
    setInitVertexSize(size);
    QList<Vertex*>::const_iterator it;
    for ( it=vert_graph.cbegin(); it!=vert_graph.cend(); ++it){
        if ( ! (*it)->isEnabled() ){
            continue;
        }
        else {
            (*it)->setSize(size) ;
            emit setArgumentSize((*it)->name(), size);
        }
    }
    graphModified=true;
    emit graphChanged();
}

//set shape initial
void Graph::setInitVertexShape(const QString shape) {
    initVertexShape=shape;
}

//Changes the shape.of vertex v. HELPME::Is it used?
void Graph::setVertexShape(const int v1, const QString shape){
    vert_graph[ index[v1] ]->setShape(shape);
    emit setArgumentShape( v1, shape);
    graphModified=true;
    emit graphChanged();
}


//returns the shape of this vertex
QString Graph::vertexShape(const int &v1){
    return vert_graph[ index[v1] ]->shape();

}

void Graph::setAllVerticesShape(const QString shape) {
    setInitVertexShape(shape);
    QList<Vertex*>::const_iterator it;
    for ( it=vert_graph.cbegin(); it!=vert_graph.cend(); ++it){
        if ( ! (*it)->isEnabled() ){
            continue;
        }
        else {
            (*it)->setShape(shape);
            emit setArgumentShape((*it)->name(), shape);
        }
    }
    graphModified=true;
    emit graphChanged();
}


//Changes the initial color of vertices numbers
void Graph::setInitVertexNumberColor (QString color) {
    initVertexNumberColor = color;
}

//Changes the initial size of vertices numbers
void Graph::setInitVertexNumberSize (int size) {
    initVertexNumberSize = size;
}


//Changes the init size of new vertices labels
void Graph::setInitVertexLabelSize(int newSize) {
    initVertexLabelSize = newSize;
}

void Graph::setInitVertexLabelColor(QString color){
    initVertexLabelColor=color;
}


/**
    Changes the color of vertex v1
*/
void Graph::setVertexColor(const long int &v1, const QString &color){
    qDebug()<< "Graph: setVertexColor for "<< v1 << ", index " << index[v1]<< " with color "<< color;
    vert_graph[ index[v1] ]->setColor ( color );
    emit setArgumentColor ( vert_graph[ index[v1] ]-> name(), color );
    graphModified=true;
    emit graphChanged();
}

QColor Graph::vertexColor(const long int &v1){
    return  QColor ( vert_graph[ index[v1] ] -> color() ) ;
}

void Graph::setInitVertexColor(const QString &color){
    initVertexColor=color;
}


void Graph::setAllVerticesColor(const QString &color) {
    qDebug() << "*** Graph::setAllVerticesColor() "
                << " to " << color;
    setInitVertexColor(color);
    QList<Vertex*>::const_iterator it;
    for ( it=vert_graph.cbegin(); it!=vert_graph.cend(); ++it){
        if ( ! (*it)->isEnabled() ){
            continue;
        }
        else {
            qDebug() << "Graph::setAllVerticesColor() Vertex " << (*it)->name()
                     << " new color " << color;
            (*it)->setColor(color) ;
            emit setArgumentColor ( (*it)-> name(), color );
        }
    }
    graphModified=true;
    emit graphChanged();

}



void Graph::setInitAttackColor(const QString &color){
    initAttackColor=color;
}




//Returns the AttackColor
QString Graph::AttackColor (const long &v1, const long &v2){
    return vert_graph[ index[v1] ]->outLinkColor(v2);
}

/**	Checks if there is a directed Attack (arc) from v1 to v2
    Complexity:  O(logN) for index retrieval + O(1) for QList index retrieval + O(logN) for checking Attack(v2)
*/
float Graph::hasArc (const long int &v1, const long int &v2) {
    //qDebug() << "Graph::hasArc() " << v1 << " -> " << v2 << " ? " ;
    return vert_graph[ index[v1] ]->hasAttackTo(v2);
}

/**	Checks if there is a Attack between v1 and v2 (both arcs exist)
*/
bool Graph::hasAttack (const int &v1, const long &v2) {
    qDebug() << "Graph::hasAttack() " << v1 << " <-> " << v2 << " ? " ;
    return ( ( vert_graph[ index[v1] ]->hasAttackTo(v2) != 0 )
            && ( vert_graph[ index[v2] ]->hasAttackTo(v1) != 0) ) ? true: false;
}



/**
    Returns |E| of graph
*/
int Graph::enabledAttacks() {

    int recountedAttacks=0;
    QList<Vertex*>::const_iterator it;
    for (it=vert_graph.cbegin(); it!=vert_graph.cend(); ++it){
        recountedAttacks+=(*it)->outAttacks();
    }
    qDebug() << "Graph::enabledAttacks() - Attacks recounted: " <<  recountedAttacks;

    return recountedAttacks;
}


void Graph::Attacks(){
    H_Attacks::const_iterator it1;
    QList<Vertex*>::const_iterator it;
    int  source=0, target=0, w=0;
    float weight=0; Q_UNUSED(weight);
    bool AttackStatus=false;

    for (it=vert_graph.cbegin(); it!=vert_graph.cend(); ++it) {
        if ( ! (*it)->isEnabled() )
            continue ;
        source = index[ (*it)->name() ];
        it1=vert_graph [ source ] ->vert_outAttacks.cbegin();
        while ( it1!=vert_graph [ source ] -> vert_outAttacks.cend() ){

            AttackStatus=it1.value().second.second;
            if ( AttackStatus != true)   {
                ++it1;
                continue;
            }
            target = it1.key();
            w=index[ target ];
            weight = it1.value().second.first;
            qDebug("u=%i is connected with Argument %i of index w=%i. ", source, target, w);
            ++it1;
        }
    }

}

/**
    Called from MainWindow
*/
void Graph::updateVertCoords(int v1, int  x, int y){
    //qDebug("Graph: updateVertCoords() for %i with index %i with %i, %i", v1, index[v1], x,y);
    vert_graph[ index[v1] ]->setX( x );
    vert_graph[ index[v1] ]->setY( y );
    graphModified=true;
}



/**
 * @brief Graph::outboundAttacks
 * *Returns the number of outbound Attacks (arcs) from vertex v1
 * @param v1
 * @return
 */
int Graph::outboundAttacks(int v1) {
    qDebug("Graph: outboundAttacks()");
    return vert_graph[ index[v1] ]->outAttacks();
}


/**
 * @brief Graph::inboundAttacks
 * Returns the number of inbound Attacks (arcs) to vertex v1
 * @param v1
 * @return int
 */
int Graph::inboundAttacks (int v1) {
    qDebug("Graph: inboundAttacks()");
    return vert_graph[ index[v1] ]->inAttacks();
}

/**
    Returns |V| of graph
*/
int Graph::vertices(const bool dropIsolates, const bool countAll) {
    qDebug("Graph: vertices()");
    vert_totalVertices=0;
    QList<Vertex*>::const_iterator it;
    for (it=vert_graph.cbegin(); it!=vert_graph.cend(); ++it){
        if (countAll) {
            ++vert_totalVertices;
        }
        else {
            if (dropIsolates && (*it)->isIsolated())
                continue;
                ++vert_totalVertices;
        }
    }
    return vert_totalVertices;
}

/**
    Returns a list of all isolated vertices inside the graph
*/
QList<int> Graph::verticesIsolated(){
    qDebug()<< "Graph::verticesIsolated()";
    if (!graphModified){
        return vert_isolatedVerticesList;
    }
    QList<Vertex*>::const_iterator it;
    vert_isolatedVerticesList.clear();
    for (it=vert_graph.cbegin(); it!=vert_graph.cend(); ++it){
//        if ( ! (*it)->isEnabled() )
//            continue;
        if ((*it)->isIsolated()) {
            vert_isolatedVerticesList << (*it)->name();
            qDebug()<< "Graph::verticesIsolated() - Argument " << (*it)->name()
                    << " is isolated. Marking it." ;
        }
    }
    return vert_isolatedVerticesList ;
}

/**
 * @brief Graph::isSymmetric
 * Returns TRUE if the adjacency matrix of the current relation is symmetric
 * @return bool
 */
bool Graph::isSymmetric(){
    qDebug() << "Graph::isSymmetric() ";
    if (!graphModified){
        return symmetricadjacent;
    }
    symmetricadjacent=true;
    int y=0, v2=0, v1=0;

    QHash<int,float> *enabledOutAttacks = new QHash<int,float>;

    QHash<int,float>::const_iterator hit;
    QList<Vertex*>::const_iterator lit;


    for ( lit = vert_graph.cbegin(); lit != vert_graph.cend(); ++lit)
    {
        v1 = (*lit) -> name();

        if ( ! (*lit)->isEnabled() )
            continue;
        qDebug() << "Graph::isSymmetric() - Graph modified! " <<
                    " Iterate over all Attacks of " << v1 ;

        enabledOutAttacks=(*lit)->returnEnabledOutAttacks();

        hit=enabledOutAttacks->cbegin();

        while ( hit!=enabledOutAttacks->cend() ){
            v2 = hit.key();
            y=index[ v2 ];
            float weight = hit.value();
            if (  vert_graph[y]->hasAttackTo( v1) != weight) {
                symmetricadjacent=false;
//                qDebug() <<"Graph::isSymmetric() - "
//                         << " graph not symmetric because "
//                         << v1 << " -> " << v2 << " weight " << weight
//                         << " differs from " << v2 << " -> " << v1 ;

                break;
            }
            ++hit;
        }
    }
    delete enabledOutAttacks;
    qDebug() << "Graph: isSymmetric() -"  << symmetricadjacent;
    return symmetricadjacent;
}



//Returns TRUE if (v1, v2) is symmetric.
bool Graph::symmetricAttack(int v1, int v2){
    qDebug("***Graph: symmetricAttack()");
    if ( (this->hasArc ( v1, v2 ) ) > 0  &&  (this->hasArc ( v2, v1 ) ) > 0   ) {
        qDebug("Graph: symmetricAttack: YES");
        return true;
    }
    else {
        qDebug("Graph: symmetricAttack: NO");
        return false;
    }

}



/**
*	 Repositions all Arguments on different random positions
* 	Emits moveArgument(i, x,y) to tell GW that the Argument item should be moved.
*/
void Graph::layoutRandom(double maxWidth, double maxHeight){
    qDebug("Graph: layoutRandom...");
    double new_x=0, new_y=0;
    for (Vertices::iterator it=vert_graph.begin(); it!=vert_graph.end(); it++){
        new_x= rand() % ( static_cast<int> (maxWidth) );
        new_y= rand() % ( static_cast<int> (maxHeight) );
        (*it)->setX( new_x );
        (*it)->setY( new_y );
        qDebug()<< "Graph: Emitting moveArgument to move Vertice " << (*it)->name()
                   //<< "indexed " << index((*it)->name())
                << " to new position " << new_x << " , "<< new_y;
        emit moveArgument((*it)->name(),  new_x,  new_y);
    }
    graphModified=true;
}



/**
 * @brief Graph::layoutCircularRandom
 * Repositions all Arguments on the periphery of different circles with random radius
 * @param x0
 * @param y0
 * @param maxRadius
 */
void Graph::layoutCircularRandom(double x0, double y0, double maxRadius){
    qDebug() << "Graph::layoutCircularRandom - ";
    double rad=0, new_radius=0, new_x=0, new_y=0;
    double i=0;
    //offset controls how far from the centre the central Arguments be positioned
    float offset=0.06, randomDecimal=0;
    int vert=vertices();
    QList<Vertex*>::const_iterator it;
    for (it=vert_graph.cbegin(); it!=vert_graph.cend(); ++it){
        randomDecimal = (float ) ( rand()%100 ) / 100.0;
        new_radius=(maxRadius- (randomDecimal - offset)*maxRadius);
        qDebug () << "Vertice " << (*it)->name()
                  << " at x=" << (*it)->x()
                  << ", y= "<< (*it)->y()
                  << ", maxradius " <<  maxRadius
                  << " randomDecimal " << randomDecimal
                  << " new radius " << new_radius;

        //Calculate new position
        rad= (2.0* Pi/ vert );
        new_x=x0 + new_radius * cos(i * rad);
        new_y=y0 + new_radius * sin(i * rad);
        (*it)->setX( new_x );
        (*it)->setY( new_y );
        qDebug("Vertice will move to x=%f and y=%f ", new_x, new_y);
        //Move Argument to new position
        emit moveArgument((*it)->name(),  new_x,  new_y);
        i++;
        emit addGuideCircle (
                    static_cast<int> (x0),
                    static_cast<int> (y0),
                    static_cast<int> (new_radius)
                    );
    }
    graphModified=true;
}


/**
    Adds a little universal randomness :)
*/
void Graph::makeThingsLookRandom()   {
    time_t now;				/* define 'now'. time_t is probably a typedef	*/
    now = time((time_t *)NULL);		/* Get the system time and put it
                     * into 'now' as 'calender time' the number of seconds since  1/1/1970   	*/

    srand( (unsigned int ) now);
}




/**
    Calculates and returns x! factorial...
    used in (n 2)p Attacks calculation
*/
int Graph:: factorial(int x) {
    int tmp;
    if(x <= 1) return 1;
    tmp = x * factorial(x - 1);
    return tmp;
}


/**
    This slot is activated when the user clicks on the relevant MainWindow checkbox
    (SpringEmbedder, Fruchterman)
    to start or stop the movement of Arguments, according to the requested model.
    PARAMETERS:
    state: movement on/off toggle
    type:  controls the type of layout model requested. Available options
            1: Spring Embedder
            2: FruchtermanReingold
    cW, cH: control the current canvasWidth and canvasHeight
*/
void Graph::ArgumentMovement(bool state, int type, int cW, int cH){
    qDebug()<< "Graph: startArgumentMovement() - state " << state;
    canvasWidth = cW;
    canvasHeight = cH;
    //factor controls speed. Decrease it to increase speed...
    // the smaller the factor is, the less responsive is the application
    // when there are many Arguments.
    int factor=50;
    if (state == true){
        qDebug()<< "Graph: startArgumentMovement() - STARTING dynamicMovement" ;
        dynamicMovement = true;
        layoutType=type;
        if (!timerId) {
            qDebug("Graph: startTimer()");
            timerId = startTimer(factor);
        }
    }
    else {
        qDebug()<< "Graph: startArgumentMovement() - STOPPING dynamicMovement" ;
        dynamicMovement = false;
        killTimer(timerId);
        timerId = 0;
    }
}




/**
    This method is automatically invoked when a QTimerEvent occurs
    It checks layoutType to call the appropriate method with the Force Directed Placement algorithm.
*/
void Graph::timerEvent(QTimerEvent *event) {
    qDebug("Graph: timerEvent()");
    Q_UNUSED(event);
    switch (layoutType){
    case 1: {
        layoutForceDirectedSpringEmbedder(dynamicMovement);
        break;
    }
    case 2: {
        layoutForceDirectedFruchtermanReingold(dynamicMovement);
        break;
    }
    }
    if (!graphModified) {
        qDebug("Timer will be KILLED since no vertex is movin any more...");
        killTimer(timerId);
        timerId = 0;
    }
}




/**
    The Spring Embedder model (Eades, 1984), part of the Force Directed Placement (FDP) family,
    assigns forces to all vertices and Attacks, as if Arguments were electrically charged particles (Coulomb's law)
    and all Attacks were springs (i.e. Hooke's law).

    These forces are applied to the Arguments iteratively, pulling them closer together or pushing them further apart,
    until the system comes to an equilibrium state (Argument positions do not change anymore).

    Note that, following Eades, we do not need to have a faithful simulation;
    we can -and we do- apply unrealistic forces in an unrealistic manner.
    For instance, instead of the forces described by Hooke's law,
    we will assume weaker logarithmic forces between far apart vertices...
*/

void Graph::layoutForceDirectedSpringEmbedder(bool &dynamicMovement){
    qreal dist = 0;
    qreal f_rep=0, f_att=0;
    QPointF DV;
    qreal c4=0.1; //normalization factor for final displacement

    QList<Vertex*>::const_iterator v1;
    QList<Vertex*>::const_iterator v2;

    /**
     * compute max spring length as function of canvas area divided by the
     * total vertices area
    */
    qreal V = (qreal) vertices() ;
    qreal naturalLength= computeOptimalDistance(V);

    qDebug() << "\n\n layoutForceDirectedSpringEmbedder() "
             << " vertices " << V
             << " naturalLength " << naturalLength;

    if (dynamicMovement){

        //setup init disp

        for (v1=vert_graph.cbegin(); v1!=vert_graph.cend(); ++v1)
        {
            (*v1) -> disp().rx() = 0;
            (*v1) -> disp().ry() = 0;
            qDebug() << " 0000 s " << (*v1)->name() << " zeroing rx/ry";
        }

        for (v1=vert_graph.cbegin(); v1!=vert_graph.cend(); ++v1)
        {
            qDebug() << "*********  Calculate forces for source s  "
                     << (*v1) -> name()
                     <<" pos "<< (*v1) -> x()<< ", "<< (*v1) -> y();

            if ( ! (*v1)->isEnabled() ) {
                qDebug() << "  vertex s disabled. Continue";
                continue;
            }

            for (v2=vert_graph.cbegin(); v2!=vert_graph.cend(); ++v2)
            {
                if ( ! (*v2)->isEnabled() ) {
                    qDebug() << "   t " << (*v1)->name() << " disabled. Continue";
                    continue;
                }

                if (v2 == v1) {
                    qDebug() << "   s==t, continuing";
                    continue;
                }

                DV.setX( (*v2) -> x() - (*v1)->x());
                DV.setY( (*v2) -> y() - (*v1)->y());

                dist = euclideian_distance(DV);

                /**
                  *  calculate electric (repulsive) forces between
                  *  all vertices.
                  */
                f_rep = layoutForceDirected_F_rep (dist, naturalLength) ;
                (*v1)->disp().rx() += sign( DV.x() ) * f_rep ;
                (*v1)->disp().ry() += sign( DV.y() ) * f_rep  ;
                qDebug() <<"  s = "<< (*v1)->name()
                         <<" pushed away from t = " << (*v2) -> name()
                           << " dist " <<dist
                        << " f_rep=" << f_rep
                        << " sign * f_repx " << sign( DV.x() ) * f_rep
                        << " sign * f_repy " << sign( DV.y() ) * f_rep ;

                /**
                * calculate spring forces between adjacent Arguments
                * that pull them together (if d > naturalLength)
                * or push them apart (if d < naturalLength)
                */
                if ( this->hasArc ( (*v1) ->name(), (*v2) -> name()) ) {

                    f_att = layoutForceDirected_F_att (dist, naturalLength) ;

                    (*v1)->disp().rx() += sign( DV.x() ) * f_att ;
                    (*v1)->disp().ry() += sign( DV.y() ) * f_att ;
                    (*v2)->disp().rx() -= sign( DV.x() ) * f_att ;
                    (*v2)->disp().ry() -= sign( DV.y() ) * f_att ;

                    qDebug() << "  s= "<<(*v1)->name()
                             << " attracted by t= "<< (*v2)->name()
                                << " dist " <<dist
                             << " f_att="<< f_att
                             << " sdx * f_att " <<sign( DV.x() ) * f_att
                             << " sdy * f_att " <<sign( DV.y() ) * f_att
                             << " disp_s.x="<< (*v2)->disp().rx()
                             << " disp_s.y="<< (*v2)->disp().ry()
                             << " disp_t.x="<< (*v2)->disp().rx()
                             << " disp_t.y="<< (*v2)->disp().ry();

                }  // end if hasArc

            } //end for v2
            //recompute naturalLength (in case the user resized the window)
            naturalLength= computeOptimalDistance(V);
            qDebug() << "  >>> final s = "<< (*v1)->name()
                     << " disp_s.x="<< (*v1)->disp().rx()
                      << " disp_s.y="<< (*v1)->disp().ry();

        } // end for v1


        layoutForceDirected_Eades_moveArguments(c4) ;

    } //end dynamicMovement
}


/**
    Fruchterman and Reingold (1991) refined the Spring Embedder model by replacing the forces.
    In this model, "the vertices behave as atomic particles or celestial bodies,
    exerting attractive and repulsive forces on one another." (ibid).
    Again, only vertices that are neighbours attract each other but, unlike Spring Embedder,
    all vertices repel each other.
    These forces induce movement. The algorithm might resemble molecular or planetary simulations,
    sometimes called n-body problems.
*/
void Graph::layoutForceDirectedFruchtermanReingold(bool dynamicMovement){
    qreal dist = 0;
    qreal f_att, f_rep;
    QPointF DV;   //difference vector
    //qreal temperature=canvasWidth / 10.0; //limits the displacement of the vertex
    qreal temperature=2.0; //limits the displacement of the vertex
    qreal V = (qreal) vertices() ;
    qreal C=0.9; //this is found experimentally
    // optimalDistance (or k) is the radius of the empty area around a  vertex -
    // we add vertexWidth to it
    qreal optimalDistance= C * computeOptimalDistance(V);

    QList<Vertex*>::const_iterator v1;
    QList<Vertex*>::const_iterator v2;

    if (dynamicMovement){
        qDebug() << "Graph: layoutForceDirectedFruchtermanReingold() ";
        qDebug () << "Graph: Setting optimalDistance = "<<  optimalDistance
                  << "...following Fruchterman-Reingold (1991) formula ";

        //setup init disp
        for (v1=vert_graph.cbegin(); v1!=vert_graph.cend(); ++v1)
        {
            (*v1)->disp().rx() = 0;
            (*v1)->disp().ry() = 0;
            qDebug() << " 0000 s " << (*v1)->name() << " zeroing rx/ry";
        }

        for (v1=vert_graph.cbegin(); v1!=vert_graph.cend(); ++v1)
        {
            qDebug() << "*****  Calculate forces for s " << (*v1)->name()
                     << " index " <<  index[(*v1)->name()]
                     << " pos "<< (*v1)->x() << ", "<< (*v1)->y();

            if ( ! (*v1)->isEnabled() ) {
                qDebug() << "  vertex s " << (*v1)->name() << " disabled. Continue";
                continue;
            }

            for (v2=vert_graph.cbegin(); v2!=vert_graph.cend(); ++v2)
            {
                qDebug () << "  t = "<< (*v2)->name()
                          << "  pos (" <<  (*v2)->x() << "," << (*v2)->y() << ")";

                if ( ! (*v2)->isEnabled() ) {
                    qDebug()<< " t "<< (*v2)->name()<< " disabled. Continue";
                    continue;
                }

                if (v2 == v1) {
                    qDebug() << "  s==t, continuing";
                    continue;
                }

                DV.setX( (*v2)->x() - (*v1)->x() );
                DV.setY( (*v2)->y() - (*v1)->y() );

                dist = euclideian_distance( DV );

                //calculate repulsive force from _near_ vertices
                f_rep = layoutForceDirected_F_rep(dist, optimalDistance);
                (*v1)->disp().rx() += sign( DV.x() ) * f_rep;
                (*v1)->disp().ry() += sign( DV.y() ) * f_rep ;

                qDebug()<< " dist( " << (*v1)->name() <<  "," <<  (*v2)->name() <<  " = "
                        << dist
                        << " f_rep " << f_rep
                        << " disp_s.x="<< (*v1)->disp().rx()
                        << " disp_s.y="<< (*v1)->disp().ry();

                if ( this->hasArc ((*v1)->name(), (*v2)->name()) ) {
                    //calculate attracting force
                    f_att = layoutForceDirected_F_att (dist, optimalDistance);
                    (*v1)->disp().rx() += sign( DV.x() ) * f_att;
                    (*v1)->disp().ry() += sign( DV.y() ) * f_att;
                    (*v2)->disp().rx() -= sign( DV.x() ) * f_att ;
                    (*v2)->disp().ry() -= sign( DV.y() ) * f_att ;

                    qDebug() << "  s= "<<(*v1)->name()
                             << " attracted by t= "<< (*v2)->name()
                             <<"  optimalDistance =" << optimalDistance
                             << " f_att " << f_att
                             << " disp_s.x="<< (*v1)->disp().rx()
                             << " disp_s.y="<< (*v1)->disp().ry()
                             << " disp_t.x="<< (*v2)->disp().rx()
                             << " disp_t.y="<< (*v2)->disp().ry();
                } //endif

            }//end for v2
            //recompute optimalDistance (in case the user resized the window)
            optimalDistance= C * computeOptimalDistance(V);
        } //end for v1

        layoutForceDirected_FR_moveArguments(temperature);

        // reduce the temperature as the layout approaches a better configuration
        //cool(temperature);
    }

}



/**
 * @brief Graph::computeOptimalDistance
 * @return qreal optimalDistance
 */
qreal Graph::computeOptimalDistance(const int &Vertices){
    qreal vertexWidth = (qreal)  2.0 * initVertexSize ;
    qreal screenArea = canvasHeight*canvasWidth;
    qreal vertexArea =  qCeil ( qSqrt( screenArea / Vertices ) ) ;
    // optimalDistance (or k) is the radius of the empty area around a  vertex -
    // we add vertexWidth to it
    return (vertexWidth + vertexArea);
}
/**
 * @brief Graph::layoutForceDirected_F_att
 * @param dist
 * @param optimalDistance
 * @return
 */
qreal Graph::layoutForceDirected_F_att( const qreal &dist, const qreal &optimalDistance) {
    qreal f_att;
    if (layoutType == 1) {  //layoutType -> Eades
        qreal c_spring=2;
        f_att = c_spring * log10 ( dist / optimalDistance );
    }
    else {   // layoutType -> FR
        f_att= ( dist * dist ) / optimalDistance;
    }

    return f_att;
}

/**
 * @brief Graph::layoutForceDirected_F_rep
 * @param dist
 * @param optimalDistance
 * @return
 */
qreal Graph::layoutForceDirected_F_rep(const qreal &dist, const qreal &optimalDistance) {
    qreal f_rep;
    if (layoutType == 1) { //layoutType -> Eades
        if (dist !=0){
            qreal c_rep= 1.0;
            f_rep =  c_rep / (dist * dist);
            if ( dist > (2.0 * optimalDistance)   ) {
                //neglect vertices outside circular area of radius 2 * optimalDistance
                f_rep=0;
            }
        }
        else {
            f_rep = optimalDistance ; //move away
        }

    }
    else {  // layoutType -> FR
        if ( (2.0 * optimalDistance) < dist ) {
            //neglect vertices outside circular area of radius 2*optimalDistance
            f_rep=0;
        }
        else {
            // repelsive forces are computed only for vertices within a circular area
            // of radius 2*optimalDistance
            f_rep = (optimalDistance * optimalDistance  /  dist) ;
        }
    }

    return -f_rep;
}

/**
 * @brief Graph::sign
 * returns the sign of number D as integer (1 or -1)
 * @param D
 * @return
 */
int Graph::sign(const qreal &D) {
    if (D != 0 ) {
        return ( D / qAbs(D) );
    }
    else {
        return 0;
    }
}

/**
 * @brief Graph::compute_angles
 * Computes the two angles of the orthogonal triangle shaped by two points
 * of difference vector DV and distance dist
 * A = 90
 * B = angle1
 * C = angle2
 *
 * @param DV
 * @param dist
 * @param angle1
 * @param angle2
 * @param degrees1
 * @param degrees2
 */
void Graph::compute_angles(const QPointF &DV,
                           const qreal &dist,
                           qreal &angle1,
                           qreal &angle2,
                           qreal &degrees1,
                           qreal &degrees2 )
{
    if ( dist >0 ) {
        angle1 = qAcos( qAbs(DV.x()) / dist );   // radians
        angle2 = (Pi  / 2.0) -angle1;   // radians (pi/2 -a1)
    }
    else {
        angle1 =0;
        angle2 =0;
    }
    degrees1 = angle1 * 180.0 / Pi; // convert to degrees
    degrees2 = angle2 * 180.0 / Pi; // convert to degrees
    qDebug () << "angle1 " <<angle1 << " angle2 "<<angle2
                 <<"deg1 " <<degrees1 << " deg2 "<<degrees2
                   << " qCos " << qCos(angle1) << " qSin" << qSin(angle2) ;
}


/**
 * @brief Graph::euclideian_distance
 * @param a
 * @param b
 * @return  the euclideian distance of QPointF a and b
 */
qreal Graph::euclideian_distance (const QPointF & a, const QPointF & b){
    return qSqrt (
                 ( b.x() - a.x() ) * (b.x() - a.x() ) +
                 ( b.y() - a.y())  * (b.y() - a.y() )
                );
}


/**
 * @brief Graph::euclideian_distance
 * @param a
 * @return  the euclideian distance of QPointF a (where a is difference vector)
 */
qreal Graph::euclideian_distance (const QPointF & a){
    return qSqrt (
                  a.x()  * a.x()  +
                  a.y() * a.y()
                );
}


/**
 * @brief Graph::layoutForceDirected_Eades_moveArguments
 * @param c4
 */
void Graph::layoutForceDirected_Eades_moveArguments(const qreal &c4) {
    qDebug() << "\n *****  layoutForceDirected_Eades_moveArguments() " ;
    QPointF newPos;
    qreal xvel = 0, yvel = 0;
    QList<Vertex*>::const_iterator v1;

    for (v1=vert_graph.cbegin(); v1!=vert_graph.cend(); ++v1)
    {
        // calculate new overall velocity vector
        xvel =  c4 * (*v1)->disp().rx();
        yvel =  c4 * (*v1)->disp().ry();
        qDebug() << " ##### source vertex  " <<  (*v1)->name()
                 << " xvel,yvel = ("<< xvel << ", "<< yvel << ")";

         //fix Qt error a positive QPoint to the floor
        // when we ask for moveArgument to happen.
         xvel < 1 && xvel > 0 ? xvel = 1 : xvel = xvel;
         yvel < 1 && yvel > 0 ? yvel = 1 : yvel = yvel;

        //Move source Argument to new position according to overall velocity
        newPos = QPointF( (qreal) (*v1)->x() + xvel, (qreal) (*v1)->y() + yvel);

        qDebug() << " source vertex v1 " << (*v1)->name()
                    << " current pos: (" <<  (*v1)->x()
                    << " , " << (*v1)->y()
                    << " Possible new pos (" <<  newPos.x()
                    << " , " <<  newPos.y();

        // check if new pos is out of screen and adjust
        newPos.rx() = qMin (
                    canvasWidth - 42.0 , qMax (42.0 , newPos.x() )
                    );
        newPos.ry() = qMin (
                    canvasHeight -42.0 , qMax (42.0 , newPos.y() )
                    );


        qDebug() << "  Final new pos (" <<  newPos.x() << ","
                 << newPos.y()<< ")";
        emit moveArgument((*v1)->name(),  newPos.x(),  newPos.y());

    }

}

/**
 * @brief Graph::layoutForceDirected_FR_moveArguments
 * @param temperature
 */
void Graph::layoutForceDirected_FR_moveArguments(const qreal &temperature) {

    qDebug() << "\n\n *****  layoutForceDirected_FR_moveArguments() \n\n " ;
    QPointF newPos;
    qreal xvel = 0, yvel = 0;
    QList<Vertex*>::const_iterator v1;

    for (v1=vert_graph.cbegin(); v1!=vert_graph.cend(); ++v1)
    {
        // compute the new position
        // limit the maximum displacement to a maximum temperature
        xvel = sign((*v1)->disp().rx()) * qMin( qAbs((*v1)->disp().rx()), temperature) ;
        yvel = sign((*v1)->disp().ry()) * qMin( qAbs((*v1)->disp().ry()), temperature) ;
        newPos = QPointF((*v1)->x()+ xvel, (*v1)->y()+yvel);
        qDebug()<< " source vertex v1 " << (*v1)->name()
                << " current pos: (" << (*v1)->x() << "," << (*v1)->y() << ")"
                << "Possible new pos (" <<  newPos.x() << ","
                << newPos.y()<< ")";

        newPos.rx() = qMin (
                    canvasWidth - 42.0 , qMax (42.0 , newPos.x() )
                    );
        newPos.ry() = qMin (
                    canvasHeight -42.0 , qMax (42.0 , newPos.y() )
                    );
        //Move Argument to new position
        if ( newPos.x() < 5.0  ||newPos.y() < 5.0   ||
                newPos.x() >= (canvasWidth -5)||
                newPos.y() >= (canvasHeight-5)||
                ((*v1)->x() == newPos.x() && (*v1)->y() == newPos.y() )
                )
            continue;
        qDebug()<< " final new pos "
                <<  newPos.x() << ","
                << newPos.y()<< ")";
        emit moveArgument((*v1)->name(),  newPos.x(),  newPos.y());
    }
}


/**
    Clears all vertices
*/
void Graph::clear() {
   qDebug("Graph::clear() vert_graph reports size %i", vert_graph.size());
    qDeleteAll(vert_graph.begin(), vert_graph.end());
    vert_graph.clear();
    index.clear();


    allArgumentList.clear();
    vert_isolatedVerticesList.clear();

    vert_totalVertices = 0;

    order=true;		//returns true if the indexes of the list is ordered.

    graphModified=false;
    symmetricadjacent=true;

    qDebug("Graph: vert_graph cleared. Now reports size %i", vert_graph.size());
}

Graph::~Graph() {

}

