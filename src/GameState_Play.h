#pragma once

#include "Common.h"
#include "GameState.h"
#include <map>
#include <memory>
#include <deque>

#include "EntityManager.h"

struct PlayerConfig 
{ 
    float X, Y, CX, CY, SPEED, MAXSPEED, JUMP, GRAVITY; 
    std::string WEAPON; 
};

class GameState_Play : public GameState
{

protected:

    EntityManager           m_entityManager;
    std::shared_ptr<Entity> m_player;
	std::shared_ptr<Entity> m_drag;
    std::string             m_levelPath;
    PlayerConfig            m_playerConfig;
    Vec2 mousepos;          //position for draggable entities
    Vec2 createPos;         //position to insert entity
    sf::Text                m_scoreText;


    int                     score;
    bool                    m_drawTextures = true;
    bool                    m_drawCollision = false;
    bool	                m_drawGrid = false;
    
    void init(const std::string & levelPath);

    void loadLevel(const std::string & filename);

    void update();
    void spawnPlayer();
    void spawnBullet(std::shared_ptr<Entity> entity);
	void saveLevel();
    void sMovement();
    void sLifespan();
    void sUserInput();
    void sAnimation();
    void sCollision();
	void sAI();
    void sRender();
    void sDrag();
    void createEntity();
	void deleteEntity();

public:

    GameState_Play(GameEngine & game, const std::string & levelPath);

};