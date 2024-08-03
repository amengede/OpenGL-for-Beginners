#pragma once
#include <glm/glm.hpp>
#include <unordered_map>
#include <string>

std::vector<std::string> split(std::string line, std::string delimiter);
int random_int_in_range(int range);
float random_float();

enum class ObjectType {
    eAlice,
    eBall,
    eBoard,
    eCorner,
    eCube,
    eDemon,
    eDemon2,
    eGirl2,
    eGirl3,
    eHands,
    ePlant,
    eSkull,
    eTree,
    eWall,
};

static std::vector<const char*> modelNames = {
    "models/alice.obj",     //Alice
    "models/ball.obj",      //Ball
    "models/board.obj",     //Board
    "models/corner.obj",    //Corner
    "models/cube.obj",      //Cube
    "models/demon.obj",     //Demon
    "models/demon2.OBJ",    //Demon2
    "models/girl2.obj",     //Girl2
    "models/girl3.obj",     //Girl3
    "models/hands.obj",     //Hands
    "models/Plant_001.OBJ", //Plant
    "models/skull.obj",     //Skull
    "models/tree.obj",      //Tree
    "models/wall.obj",      //Wall
};

static std::vector<const char*> imageNames = {
    "img/Alice_baseColor.png",  //Alice
    "img/glass.png",            //Ball
    "img/board.png",            //Board
    "img/red_brick.png",        //Corner
    "img/wood.png",             //Cube
    "img/demon.png",            //Demon
    "img/demon2.png",           //Demon2
    "img/stargirl.png",         //Girl2
    "img/none.png",             //Girl3
    "img/T_Hands_D02.png",      //Hands
    "img/PLANT_CLR_002.png",    //Plant
    "img/skull.png",            //Skull
    "img/Tree_Texture.png",     //Tree
    "img/wall.png",             //Wall
};

static std::vector<float> scales = {
    0.01f,  //Alice
    0.1f,   //Ball
    0.01f,  //Board
    1.0f,   //Corner
    1.0f,   //Cube
    1.0f,   //Demon
    1.0f,   //Demon2
    1.5f,   //Girl2
    2.0f,   //Girl3
    2.0f,   //Hands
    0.01f,  //Plant
    1.0f,   //Skull
    2.0f,   //Tree
    0.1f,   //Wall
};

struct Mesh {
    uint32_t elementCount, VAO, VBO, EBO, material;
};

constexpr float windowWidth = 1920.0f;
constexpr float windowHeight = 1080.0f;

constexpr uint32_t maxObjectCount = 1000;
constexpr uint32_t objectTypeCount = 14;