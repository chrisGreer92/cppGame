//
// Created by Chris Greer on 16/04/2024.
//

#ifndef G52CPP_ASTAR_H
#define G52CPP_ASTAR_H

#include "../../header.h"
#include <list>
#include "../ZEngine.h"
#include "../ZUtility/MathUtil.h"
#include "../ZUtility/TileCodes.h"

using namespace std;

class AStar {
public:
    //Grid will be one node for 3x3 tiles (for efficiency)
    ~AStar() {
        delete[] nodes;
    }
    AStar(ZEngine* pEngine, MapTileManager* collisionMap)
    : m_pEngine(pEngine), m_collisionMap(collisionMap),
    m_mapWidth(pEngine->getTilesX()),
    m_mapHeight(pEngine->getTilesY()),
    m_tileSize(pEngine->getTileSize()){

        nodes = new aNode[m_mapWidth * m_mapHeight];
        createMap();
    };
    struct aNode{
        ~aNode() { neighbours.clear(); }
        bool barrier = false; //Any nodes containing barriers
        bool visited = false; //Tried already
        float globalGoal{}; //Distance to goal all together
        float localGoal{}; //Current distance to goal
        int tileX{}; //Which tiles this represents
        int tileY{};
        vector<aNode*> neighbours{};	//All neighbors of this node
        aNode* parent{}; //Previous node on this path
    };
    void resetMap(MapTileManager* collisionMap) {
        m_collisionMap = collisionMap;
        createMap(); //Re-initialse our map based on new collisionMap

    }

    //Solve path and return pointer to the 'end' node,
    //The end node actually represents the starting point
    //This makes it easier to iterate through the parents to get the direction to go
    aNode* solvePath(int startX, int startY, int endX, int endY){ //Will update pointer

        //First reset all the nodes as doing a new search
        resetNodes();

        //Get which nodes these tile values relate to
        //Our starting node is the location of the goal
        //End node is the location of the object that wants to move
        int iStartNode = nodeVal(endX / (m_tileSize), endY / (m_tileSize));
        int iEndNode = nodeVal(startX / (m_tileSize), startY / (m_tileSize));

        aNode *startNode = &nodes[iStartNode];
        aNode *endNode = &nodes[iEndNode];
        aNode *currentNode = nullptr;

        //Already there, just move directly
        //Will check the line of sight so should know!
        if (startNode->tileX == endNode->tileX && startNode->tileY == endNode->tileY) return endNode;

        startNode->localGoal = 0.0f;
        startNode->globalGoal = distanceHeuristic(startNode, endNode);

        //Create list of nodes yet to be tested, add our starting node
        list<aNode*> untested;
        untested.push_back(startNode);

        //Don't need the absolute shortest path (would increase calculations)
        //So instead of testing everything, will return if we reach end node
        while (!untested.empty() && currentNode != endNode)
        {
            //Sort untested list based on the global goal
            untested.sort([](const aNode* left, const aNode* right){
                return left->globalGoal < right->globalGoal;
            } );

            //Remove any untested nodes
            while(!untested.empty() && untested.front()->visited)
                untested.pop_front();

            //If all tested and empty, means no path!
            if (untested.empty())
                break;

            //Take the front (shortest global) node
            currentNode = untested.front();
            //Set it to visted (only want to explore it once)
            currentNode->visited = true;

            //Check all of it's neighbours
            for (auto neighbour : currentNode->neighbours)
            {
                //As long as neighbor isn't a barrier (Contains blocking tile)
                //And it's not already been visited, add it to our untested list;
                if (!neighbour->visited && !neighbour->barrier)
                    untested.push_back(neighbour);

                //Calculate the potentially shorter distance
                float potentialGoal = currentNode->localGoal + distanceHeuristic(currentNode, neighbour);


                //If this is a shorter path than the neighbor currently has locally
                //then update its local path value to this and set this as the current node
                if (potentialGoal < neighbour->localGoal)
                {
                    neighbour->parent = currentNode;
                    neighbour->localGoal = potentialGoal;

                    // Update the neighbour's global goal, will ensure only the best paths
                    //continue to be considered (don't need to check worsening paths)
                    neighbour->globalGoal = neighbour->localGoal + distanceHeuristic(neighbour, endNode);
                }
            }
        }
        if (!currentNode) return nullptr;
        if (currentNode->tileX == endNode->tileX && currentNode->tileY == endNode->tileY)
            return endNode;
        else
            return nullptr;

    }

    //Converts the tile value in a node structure to the real location on map
    int tileToLoc(int tile) const { return (tile * m_tileSize) + m_tileSize/2;};
//    int tileToLocY(int tileX){ return tileX * nodeDim * m_tileSize;};
private:
    //Set up the map based on our tile map
    void createMap(){
       

        for (int x = 0; x < m_mapWidth; ++x) {
            for (int y = 0; y < m_mapHeight; ++y) {

                int node = nodeVal(x,y);
                nodes[node].tileX = x;
                nodes[node].tileY = y;
                nodes[node].barrier = checkIfBarrier(x, y);
                nodes[node].parent = nullptr;
                nodes[node].visited = false;
            }
        }

        //Now link all the nodes together

        for (int x = 0; x < m_mapWidth; ++x) {
            for (int y = 0; y < m_mapHeight; ++y) {
                int node = nodeVal(x,y);
                if(y>0){
                    nodes[node].neighbours.push_back(&nodes[(y - 1) * m_mapWidth + (x + 0)]); //Add above
                    //if (x > 0) //Also add diagonally NW
                    //    nodes[node].neighbours.push_back(&nodes[(y - 1) * m_mapWidth + (x - 1)]); //Add NW
                    //if (x<m_mapWidth-1) //Also add diagonally NE
                    //    nodes[node].neighbours.push_back(&nodes[(y - 1) * m_mapWidth + (x + 1)]); //Add NE
                }
                if(y < (m_mapHeight-1)){
                    nodes[node].neighbours.push_back(&nodes[(y + 1) * m_mapWidth + (x + 0)]); //Add below
                    //if (x > 0) //Also add diagonally SW
                    //    nodes[node].neighbours.push_back(&nodes[(y + 1) * m_mapWidth + (x - 1)]); //Add SW
                    //if (y < m_mapHeight - 1 && x < m_mapWidth-1) //Also add diagonally SE
                    //    nodes[node].neighbours.push_back(&nodes[(y + 1) * m_mapWidth + (x + 1)]); //Add SE
                }
                if (x > 0){
                    nodes[node].neighbours.push_back(&nodes[(y + 0) * m_mapWidth + (x - 1)]); //Add Left
                }
                if(x<m_mapWidth-1){
                    nodes[node].neighbours.push_back(&nodes[(y + 0) * m_mapWidth + (x + 1)]); //Add right
                }
            }
        }

    }
    int nodeVal(int x, int y) const { return x + (y * m_mapWidth); }
    //Reset/initialise nodes for a new search
    void resetNodes(){
        //Reset all nodes to their default values
        //(For a fresh search)
        for (int x = 0; x < m_mapWidth; x++)
            for (int y = 0; y < m_mapHeight; y++)
            {
                int node = nodeVal(x,y);
                nodes[node].visited = false;
                nodes[node].globalGoal = INFINITY;
                nodes[node].localGoal = INFINITY;
                nodes[node].parent = nullptr;	// No parents
            }
    }
    //Calculate the distance between two nodes, will be our heuristic
    static float distanceHeuristic(aNode* startNode, aNode* endNode)
    {
        return static_cast<float>(abs(startNode->tileX - endNode->tileX) + abs(startNode->tileY - endNode->tileY));
        //return MathUtil::fDistanceBetween(startNode->tileX,startNode->tileY,
                                          //endNode->tileX,endNode->tileY);
    }
    //Work out index of given coordinates
    //Confirm whether node contains a blocking tile
    bool checkIfBarrier(int xStart, int yStart){
        //X/Y represents the top right corner of the 3x3 nodes
        for (int x = 0; x < 1; ++x) {
            for (int y = 0; y < 1; ++y) {
                int mapValue = m_collisionMap //Work out which tiles are within this node
                                ->getMapValue(xStart + x,yStart + y);
                if(TileCodes::isCollisionTile(mapValue))
                    return true;
            }
        }
        return false; //None of the tiles in our node are obstacles
    }


private:
    aNode* nodes = nullptr; //Unique pointer to our nodes
    ZEngine* m_pEngine;
    MapTileManager* m_collisionMap;
    int m_mapWidth;
    int m_mapHeight;
    int m_tileSize;

//    aNode* startingNode = nullptr;
//    aNode* endingNode = nullptr; //Just set these per search?
};


#endif //G52CPP_ASTAR_H
