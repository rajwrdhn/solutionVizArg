/***************************************************************************
    solutionVizArg: Visualization of Solutions of Abstract Argumentation Frameworks


    Written in Qt/C++

                             -------------------
    This software is written as a University project.
 ***************************************************************************/
#include "vertex.h"
#include <QtDebug>
#include "graph.h"

Vertex::Vertex(Graph* parent,
               const long &name,
               const int &val,
               const int &size,
               const QString &color,
               const QPointF &p,
               const QString &shape): parentGraph (parent)
{
    qDebug() << "Vertex::Vertex() - "<<  name << " setting values";
    vert_name=name;
    vert_value=val;
    vert_size=size;
    vert_color=color;
    vert_shape=shape;
    vert_x=p.x();
    vert_y=p.y();
    vert_outAttacks.reserve(100);
    vert_inAttacks.reserve(100);
    vert_outAttacksCounter=0;
    vert_inAttacksCounter=0;
    vert_outDegree=0;
    vert_inDegree=0;
    vert_localDegree=0;
    vert_Eccentricity=0;
    vert_reciprocalLinked=false;
    vert_enabled = true;

    connect (this, SIGNAL (setAttackVisibility ( int, int, int, bool) ),
             parent, SLOT (slotSetAttackVisibility (int, int, int, bool)) );

}

// constructor with default values
// HELPME::am i using this??
Vertex::Vertex(const long int &name) {
    qDebug() << "Vertex::Vertex() - "<<  name << " using default values";
    vert_name=name;
    vert_value=1;
    vert_size=9;
    vert_color="black";
    vert_shape="circle";
    vert_outAttacksCounter=0;
    vert_inAttacksCounter=0;
    vert_Eccentricity=0;
    vert_curRelation=0;
    vert_reciprocalLinked=false;
}

/**
 * @brief Vertex::addAttackTo
 * Adds an outLink to target with weight w
 * @param target
 * @param weight
 */
void Vertex::addAttackTo (const long &v2, const float &weight) {
    qDebug() <<"Vertex::addAttackTo() - new link "
            << name() << " -> "<< v2 << " weight "<< weight
               << " relation " << vert_curRelation;
    // do not use [] operator - silently creates an item if key do not exist
    vert_outAttacks.insertMulti(
                v2, rel_w_bool(vert_curRelation, pair_f_b(weight, true) ) );
}


/**
 * @brief Vertex::addAttackFrom
 * @param source
 * @param weight
 */
void Vertex::addAttackFrom (const long int &v1, const float &weight) {
//    qDebug() <<"Vertex: "<< name() << " addAttackFrom() "<< source;
    vert_inAttacks.insertMulti(
                v1, rel_w_bool (vert_curRelation, pair_f_b(weight, true) ) );
}

/**
 * @brief Vertex::removeAttackTo
 * finds and removes a link to vertex v2
 * @param v2
 */
void Vertex::removeAttackTo (long int v2) {
    qDebug() << "Vertex: removeAttackTo() - vertex " << vert_name
             << " has " <<outAttacks() << " out-links. Removing link to "<< v2 ;

    if (outAttacks()>0) {
        qDebug () << "checking all_outAttacks";
        H_Attacks::iterator it1=vert_outAttacks.find(v2);
        while (it1 != vert_outAttacks.end() && it1.key() == v2 ) {
            if ( it1.value().first == vert_curRelation ) {
                qDebug() << " *** vertex " << vert_name << " connected to "
                         << it1.key() << " relation " << it1.value().first
                         << " weight " << it1.value().second.first
                         << " enabled ? " << it1.value().second.second;
                qDebug() << " *** erasing outAttack from vert_outAttacks ";
                it1=vert_outAttacks.erase(it1);
            }
            else {
                ++it1;
            }
        }
        qDebug() << "Vertex: vertex " <<  vert_name << " now has " <<  outAttacks() << " out-Attacks";
    }
    else {
        qDebug() << "Vertex: vertex " <<  vert_name << " has no Attacks" ;
    }
}


/**
 * @brief Vertex::removeAttackFrom
 * @param v2
 */
void Vertex::removeAttackFrom(long int v2){
    qDebug() << "Vertex: removeAttackFrom() vertex " << vert_name
             << " has " <<  inAttacks() << "  in-Attacks. RemovingAttackFrom " << v2 ;

    if (inAttacks()>0) {
        qDebug () << "checking all_inAttacks";
        H_Attacks::iterator it=vert_inAttacks.find(v2);
        while (it != vert_inAttacks.end() ) {
            if ( it.key() == v2 && it.value().first == vert_curRelation ) {
                qDebug() << " *** vertex " << vert_name << " connected from  "
                         << it.key() << " relation " << it.value().first
                         << " weight " << it.value().second.first
                         << " enabled ? " << it.value().second.second;
                qDebug() << " *** erasing inAttack from vert_inAttacks ";
                it=vert_inAttacks.erase(it);
            }
            else {
                ++it;
            }
        }
        qDebug() << "Vertex: vertex " << vert_name << " now has "
                 << inAttacks() << " in-links"  ;
    }
    else {
        qDebug() << "Vertex: vertex " << vert_name << " has no Attacks";
    }
}


/**
 * @brief Vertex::outAttacks
 * Returns the number of active outbound arcs, aka the number of
 * outAttacks, from this vertex for the current relation
 * @return long int
 */
long int Vertex::outAttacks() {
    vert_outAttacksCounter = 0;
    int relation=0;
    bool AttackStatus = false;
    H_Attacks::const_iterator it1=vert_outAttacks.constBegin();
    while (it1 != vert_outAttacks.constEnd() ) {
        relation = it1.value().first;
        if ( relation == vert_curRelation ) {
            AttackStatus=it1.value().second.second;
            if ( AttackStatus == true) {
                vert_outAttacksCounter++;
            }
        }
        ++it1;
    }
    return vert_outAttacksCounter;
}


/**
 * @brief Vertex::returnEnabledOutAttacks
 * Returns a qhash of all enabled outAttacks in the active relation
 * @return  QHash<int,float>*
 */
QHash<int,float>* Vertex::returnEnabledOutAttacks(){
    //qDebug() << " Vertex::returnEnabledOutAttacks() vertex " << this->name();
    QHash<int,float> *enabledOutAttacks = new QHash<int,float>;
    float vert_weight=0;
    int relation = 0;
    bool AttackStatus=false;
    H_Attacks::const_iterator it1=vert_outAttacks.constBegin();
    while (it1 != vert_outAttacks.constEnd() ) {
        relation = it1.value().first;
        if ( relation == vert_curRelation ) {
            AttackStatus=it1.value().second.second;
            if ( AttackStatus == true) {
                vert_weight=it1.value().second.first;
                enabledOutAttacks->insert(it1.key(), vert_weight);
//                qDebug() <<  " Vertex::returnEnabledOutAttacks() count:"
//                             << enabledOutAttacks->count();
            }
        }
        ++it1;
    }
//    qDebug() << " Vertex::returnEnabledOutAttacks() vertex " << this->name()
//                << " outAttacks count:"
//                 << enabledOutAttacks->count();
    return enabledOutAttacks;
}


/**
 * @brief Vertex::inAttacks
 * Returns the number of active inbound arcs, aka the number of
 * inAttacks, to this vertex for the current relation
 * @return long int
 */
long int Vertex::inAttacks() {
    vert_inAttacksCounter = 0;
    int relation=0;
    bool AttackStatus = false;
    H_Attacks::const_iterator it1=vert_inAttacks.constBegin();
    while (it1 != vert_inAttacks.constEnd() ) {
        relation = it1.value().first;
        if ( relation == vert_curRelation ) {
            AttackStatus=it1.value().second.second;
            if ( AttackStatus == true) {
                vert_inAttacksCounter++;
            }
        }
        ++it1;
    }
    return vert_inAttacksCounter;
}


/**
 * @brief Vertex::inAttacksConst
 * Returns the number of active inbound arcs
 * Needs to have inAttacks called before the call to this method
 * @return long int
 */
long int Vertex::inAttacksConst() const {
    return vert_inAttacksCounter;
}


/**
 * @brief Vertex::hasAttackTo
 * Checks if this vertex is outlinked to v2 and returns the weight of the link
 * only if the outLink is enabled.
 * @param v2
 * @return
 */
float Vertex::hasAttackTo(long int v2){
//    qDebug()<< "Vertex::hasAttackTo() " << name() << " -> " << v2 ;
    float vert_weight=0;
    bool AttackStatus=false;
    H_Attacks::iterator it1=vert_outAttacks.find(v2);
    while (it1 != vert_outAttacks.end() && it1.key() == v2 ) {
        if ( it1.value().first == vert_curRelation ) {
            AttackStatus=it1.value().second.second;
            if ( AttackStatus == true) {
                vert_weight=it1.value().second.first;
                return vert_weight;
            }
            else
                qDebug()<< "Vertex::hasAttackTo() - relation "
                           << it1.value().first
                        <<" link "  <<  this->name()
                        << " -> " << v2 << "exists, weight "<< vert_weight
                        << " but AttackStatus " << AttackStatus;
                return 0;
        }
        ++it1;
    }
   // qDebug()<< "Vertex::hasAttackTo() - INEXISTENT LINK IN RELATION " << vert_curRelation;
    return 0;
}


/**
 * @brief Vertex::hasAttackFrom
 * Checks if this vertex is inLinked from v2
 * only if the inLink is enabled.
 * @param v2
 * @return
 */
float Vertex::hasAttackFrom(long int v2){
    qDebug()<< "Vertex::hasAttackFrom()" ;
    float vert_weight=0;
    bool AttackStatus=false;
    H_Attacks::iterator it1=vert_inAttacks.find(v2);
    while (it1 != vert_inAttacks.end() && it1.key() == v2) {
        if ( it1.value().first == vert_curRelation ) {
            AttackStatus=it1.value().second.second;
            if ( AttackStatus == true) {
                vert_weight=it1.value().second.first;
                qDebug()<< "Vertex::hasAttackFrom() - a ("  <<  this->name()
                        << ", " << v2 << ") = "<< vert_weight;
                return vert_weight;
            }
            else
                qDebug()<< "Vertex::hasAttackFrom() - a ("  <<  this->name()
                        << ", " << v2 << ") = "<< vert_weight
                        << " but AttackStatus " << AttackStatus;
                return 0;

        }
        ++it1;
    }
    qDebug()<< "Vertex::hasAttackFrom() - a ("  <<  this->name()  << ", " << v2 << ") = 0 ";
    return 0;
}

//Destructor
Vertex::~Vertex() {
    qDebug() << " Vertex:: destroying my data";
    vert_outAttacks.clear();
    outLinkColors.clear();
    vert_inAttacks.clear();
    myPs.clear();
}


