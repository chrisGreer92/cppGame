//
// Created by Chris Greer on 01/04/2024.
//

#include "ZEnemy.h"
#include "StaticObjectFactory.h"
#include "../ZUtility/MathUtil.h"
#include "../ZMovement/AutomatedMovement.h"

using namespace std;

ZEnemy::~ZEnemy(){
    delete(m_movement);
};
ZEnemy::ZEnemy(ZEngine *pEngine, const string& directoryPath,
                 ObjectInfo info,
                 double centerX, double centerY)
        : LivingObject(pEngine, directoryPath, info, centerX, centerY),
          m_player(pEngine->getPlayer()){
    //Also need a map filter (unlike player)
    m_mapFilter = pEngine->getMapFilter().get();

}

//Sets up and initialises images then sets a random rotation
void ZEnemy::initialise() {
    //Initialise Images
    setUpImages();
    initialiseImages();

    //Initialise with a random rotation
    double randomRotation = -M_PI + static_cast<double>(rand()) / RAND_MAX * (2 * M_PI);
    /*cout << randomRotation << endl;
    setRotation(randomRotation);*/
}

void ZEnemy::setUpImages() {

    //Set the A1/Z2 etc strings then add them to the below to fill
    m_movingImages = (*ImagePixelRepo::getMultiImages()).at(m_type + "Walk");
    m_movementPixelMaps = (*ImagePixelRepo::getMultiPixelMaps()).at(m_type + "Walk");
    m_image = (*m_movingImages)[0];
    m_pixelMap = &(*m_movementPixelMaps)[0];
    m_defaultPixelMap = &(*m_movementPixelMaps)[0];

    m_meleeImages = (*ImagePixelRepo::getMultiImages()).at(m_type + "Attack");
    m_meleePixelMaps = (*ImagePixelRepo::getMultiPixelMaps()).at(m_type + "Attack");
    m_deathImages = (*ImagePixelRepo::getMultiImages()).at(m_type + "Death");
}

void ZEnemy::virtDraw(){

    //First draw as per base class
    LivingObject::virtDraw();

    //Don't need to draw health/armour if dead
    if (isDead() || !isVisible()) return;


    drawStatBar(m_health,60,35,0xFF0000,0x03C04A);
    if (m_armour > 0)
        drawStatBar(m_armour,80,40,0x808080,0x0CCCCC);
}
//Updates the astar algorithm (if needed) in their movement util
void ZEnemy::updateAStar(AStar* newAstar){
    m_movement->updateAStar(newAstar);
}

void ZEnemy::drawStatBar(int stat, int barSize, int yOffset, int backgroundColour, int fillColour){

    int locX = getExactVirtCenterX();
    int locY = getExactVirtCenterY();

    int halfBar = barSize/2;

    //Draw background bar
    m_pEngine->getForegroundSurface()->drawThickLine(locX - halfBar, locY - yOffset,
                                                     locX + halfBar, locY - yOffset,
                                                     backgroundColour,2);

    //Then draw the bar up to where it's filled
    float totalLeft = static_cast<float>(locX - halfBar) //Starting point
                      + static_cast<float>(barSize) * (static_cast<float>(stat)/100); //Based on health
    m_pEngine->getForegroundSurface()->drawThickLine(locX - halfBar, locY - yOffset,
                                                     totalLeft, locY - yOffset,
                                                     fillColour, 2);

}

void ZEnemy::die(){
    //Bleed out at location
    Animator::paintBlood(dynamic_cast<ZEngine*>(m_pEngine), getExactRealCenterX(), getExactRealCenterY());
    m_dead = true;
    m_animationCounter = 0; //Reset to go back to standard pose
    m_image = (*m_deathImages)[m_animationCounter];
}

//This should all be able to be declared here (i.e. shared with subclasses)
void ZEnemy::virtDoUpdate(int iCurrentTime) {

    //If set to not visable  don't need to do anything
    //If they're not on the screen and not yet m_spotted also don't do anything
    //BUT, if they've m_spotted the character, they will continue to move
    if (!isVisible() || (!isInScreen() && !m_spotted)) return;

    //Limit how often they update
    /*int ticks = iCurrentTime - m_actionLastUpdated;
    if (ticks < 20) return;
    else m_actionLastUpdated = ticks;*/

    //If on screen but not yet m_spotted, check if you have line of sight
    if (!m_spotted) {
        if (RayTrace::lineOfSightToPlayer(dynamic_cast<ZEngine *>(m_pEngine),
                                          getExactRealCenterX(), getExactRealCenterY()))
            m_spotted = true;
        else
            return; //Still not spotted so don't need to check anything else
    }

    if (m_dead) {
        //If already at the last image, tell engine to delete (later)
        if (m_animationCounter == (*m_deathImages).size() - 1) {
            dynamic_cast<ZEngine *>(m_pEngine)->addToDelete(this); //Add itself to list of objects to delete
            if (m_type[0] == 'A') { //If this is an armoured zombie, drop ammo/armour at current location
                int random = rand() % 30;
                char drop = random == 0 ? 'B' : 'R'; //Drop either ammo, or occasionally body armour
                dynamic_cast<ZEngine *>(m_pEngine)->addToBeAdded(
                        StaticObjectFactory::createObject(dynamic_cast<ZEngine *>(m_pEngine),
                                                          {getExactRealCenterX(), getExactRealCenterY(), drop}));
            }
            return;
        }//Otherwise animate death
        Animator::animate(m_image, *m_deathImages, iCurrentTime, m_animationCounter, m_imagesLastUpdated, 80);
        return;
    }

    //Check how close we are to the player
    int distance = MathUtil::distanceBetween(getExactVirtCenterX(),
                                             getExactVirtCenterY(),
                                             m_player->getExactVirtCenterX(),
                                             m_player->getExactVirtCenterY());
    //If close enough to attack, do so...
    if (distance < m_halfBuffer * 2 || attacking) {
        attacking = true;
        if (animateAndUpdatePixelMaps(*m_meleeImages, *m_meleePixelMaps, iCurrentTime, 100)
            && (m_animationCounter % 3 == 1)) { //Only check at certain points of the animation (Swings)
            //Check if they're colliding
            if (*this == *m_player) {
                int crit = (m_type.at(0) == 'Z') ? 20 : 10;
                m_player->beenHit(crit); //Hot the player
            }
        }
        if (m_animationCounter == 0)
            attacking = false; //if we've gone all the way round, reset our attacking (may start moving)

        //If on screen or already m_spotted, start moving towards player
        //Maybe only m_spotted when in line of sight?
    } else if (m_spotted) { //|| isInScreen()) {
       
        m_moving = m_movement->automateMovement(m_iCurrentScreenX, m_iCurrentScreenY);
        //If we're moving, then animate!
        if (m_moving) {
            //Update the image based on animation counter
            animateAndUpdatePixelMaps(*m_movingImages, *m_movementPixelMaps, iCurrentTime, 100);
        }
    } //Otherwise, remain idle

}