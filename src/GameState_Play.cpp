#include "GameState_Play.h"
#include "Common.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include "math.h"
using namespace std;

/*
 Student names:
 Victor Mutandwa -vtm806
 /Maria Beltres  - mda184

 Save does not really work but we got the code in but we did not have time to implement it completely
 Bonus - angry bird and patrol features, score 
 Grid - It's upside down
 Draggable - seems to be working on my computer but not on my partners laptop.. don't know why!
*/





Vec2 mousepos(0,0);  //position for draggable entity
Vec2 createPos(0,0); //position to create entity
bool save; 			 //to save
GameState_Play::GameState_Play(GameEngine & game, const std::string & levelPath)
    : GameState(game)
    , m_levelPath(levelPath)
{
    init(m_levelPath);
}

void GameState_Play::init(const std::string & levelPath)
{
	loadLevel(levelPath);
	//Setting the score
	m_scoreText.setString("SCORE: " + to_string(score));
	m_scoreText.setFont(m_game.getAssets().getFont("Arial"));
	m_scoreText.setFillColor(sf::Color::White);
	m_scoreText.setPosition(40,50);
	m_scoreText.setCharacterSize(20);



	spawnPlayer();
}
void GameState_Play::loadLevel(const std::string & filename)
{
    std::cout << "Loading level\n";
    // reset the entity manager every time we load a level
    m_entityManager = EntityManager();
    
    //loading entities
    std::ifstream file(filename);
    std::string str, animation, AI;
	size_t posx, posy, patrolSpeed, pp;
    bool bm, bv;

    while (file.good())
    {
        file >> str;

        if (str == "Tile")
        {
            file >> animation >> posx>>posy;
          auto entity = m_entityManager.addEntity("tile");
          	  //  if(tilepos && tileposy)
           if(animation=="Brick")
            {
                entity->addComponent<CDraggable>();
            }
            if(animation == "Enemy" || animation =="EnemyBird")
            {
				//auto entity = m_entityManager.addEntity("enemy");
                file >> bm >> bv >> AI;
			    auto tiledim1 = m_game.getAssets().getAnimation(animation).getSize();
            
                entity->addComponent<CTransform>(Vec2(posx, posy));
                entity->addComponent<CAnimation>(m_game.getAssets().getAnimation(animation), true);
                entity->addComponent<CBoundingBox>(m_game.getAssets().getAnimation(animation).getSize());
                if (AI == "Patrol")
			    {
				
				file >> patrolSpeed >> pp;
				std::vector<Vec2> patrolPos;

				for (int i = 0; i < pp; i++)
				{
					Vec2 pos;
					file >> pos.x >> pos.y;
					patrolPos.push_back(Vec2(pos.x, pos.y));
				}
	
				entity->addComponent<CPatrol>(patrolPos, patrolSpeed);
			}
            }

            if(animation != "Enemy"||animation != "EnemyBird")
            {
                
                entity->addComponent<CTransform>(Vec2(posx, posy));
                entity->addComponent<CAnimation>(m_game.getAssets().getAnimation(animation), true);
                entity->addComponent<CBoundingBox>(m_game.getAssets().getAnimation(animation).getSize());
            }
    
        }
        else if (str == "Dec")
        {
            file >> animation >> posx >> posy;
            auto entity = m_entityManager.addEntity("dec");
            entity->addComponent<CTransform>(Vec2(posx, posy));
            entity->addComponent<CAnimation>(m_game.getAssets().getAnimation(animation), false);
        }
        else if (str == "Player")
        {
            file >> m_playerConfig.X >> m_playerConfig.Y >> m_playerConfig.CX 
				>> m_playerConfig.CY >> m_playerConfig.SPEED >> m_playerConfig.JUMP 
				>> m_playerConfig.MAXSPEED >> m_playerConfig.GRAVITY >> m_playerConfig.WEAPON; 
        }

        else
        {
            std::cerr << "Unknown Asset Type: " << str << std::endl;
        }
    }
    


}
void GameState_Play::saveLevel() {
	if(save == true)
	{
		std::ofstream outfile("test.txt");

		for(auto & e: m_entityManager.getEntities())
		{
			//saving players position
			if(e == m_player)
			{
				Vec2 pos = m_player->getComponent<CTransform>()->pos;
				outfile<<"Player"<<pos.x<<pos.y<<m_player->getComponent<CBoundingBox>()->size.x<<m_player->getComponent<CBoundingBox>()->size.y<<m_playerConfig.SPEED<<m_playerConfig.JUMP<<m_playerConfig.MAXSPEED<<m_playerConfig.GRAVITY<<m_playerConfig.WEAPON;
			}

			//saving enemy's position
			if(e->getComponent<CAnimation>()->animation.getName()== "Enemy" || e->getComponent<CAnimation>()->animation.getName()== "BirdEnemy")
			{
				Vec2 pos = e->getComponent<CTransform>()->pos;
				outfile<<"Tile"<<e->getComponent<CAnimation>()->animation.getName()<<pos.x<<pos.y<<e->getComponent<CBoundingBox>()->size.x<<e->getComponent<CBoundingBox>()->size.y<<m_playerConfig.SPEED<<m_playerConfig.JUMP<<m_playerConfig.MAXSPEED<<m_playerConfig.GRAVITY<<m_playerConfig.WEAPON;
			}

			//saving score
			outfile<<"Score"<<score;
			save = false;
		}

		outfile.close();
	}
}


void GameState_Play::spawnPlayer()
{
        
    // TODO: be sure to add the remaining components to the player */
    m_player = m_entityManager.addEntity("player");
    m_player->addComponent<CTransform>(Vec2(m_playerConfig.X, m_playerConfig.Y));
    m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CX,m_playerConfig.CY));
    m_player->addComponent<CAnimation>(m_game.getAssets().getAnimation("Stand"), true);
  	m_player->addComponent<CInput>();
	m_player->addComponent<CGravity>(m_playerConfig.GRAVITY);
	m_player->addComponent<CState>("jumping");

}

void GameState_Play::spawnBullet(std::shared_ptr<Entity> entity)
{
	// TODO: this should spawn a bullet at the given entity, going in the direction the entity is facing


	float speedOfBullet = m_playerConfig.SPEED * 3;
	float scale = entity->getComponent<CTransform>()->scale.x;
	auto bullet = m_entityManager.addEntity("bullet");

	bullet->addComponent<CTransform>();
	bullet->getComponent<CTransform>()->speed = Vec2(speedOfBullet * scale, 0);
	bullet->getComponent<CTransform>()->pos = entity->getComponent<CTransform>()->pos;
	bullet->addComponent<CAnimation>(m_game.getAssets().getAnimation(m_playerConfig.WEAPON), false);
	bullet->addComponent<CBoundingBox>(m_game.getAssets().getAnimation("Buster").getSize());
	bullet->addComponent<CLifeSpan>(1200);

}

void GameState_Play::update()
{
    m_entityManager.update();

    //  pause functionality
    if (!m_paused)
    {
		sAI();
		sMovement();
        sLifespan();
        sCollision();
        sAnimation();
        sDrag();    //system for draggable components
    }
    sUserInput();
    sRender();
}
void GameState_Play::createEntity()
{
	if (m_drag != NULL) { return; }
	m_drag= m_entityManager.addEntity("tile");
    m_drag->addComponent<CTransform>(Vec2(createPos.x, createPos.y));
    m_drag->addComponent<CAnimation>(m_game.getAssets().getAnimation("Brick"), true);
    m_drag->addComponent<CBoundingBox>(m_game.getAssets().getAnimation("Brick").getSize());
    m_drag->addComponent<CDraggable>();
}

// Method to delete Entitites
void GameState_Play::deleteEntity() {
	
	if (m_drag != NULL) {
		
		m_drag->destroy();
		m_drag = NULL;
	}
}





void GameState_Play::sMovement()
{
    auto transform = m_player->getComponent<CTransform>();
    auto input = m_player->getComponent<CInput>();
    
    
    // TODO: Implement player movement / jumping based on its CInput component
	if (input->up  && !(m_player->getComponent<CState>()->state == "jumping"))
	{
		m_player->getComponent<CTransform>()->speed.y = m_playerConfig.JUMP;
		m_player->getComponent<CState>()->state = "jumping";
	}
	
	else if (input->left)
	{
		m_player->getComponent<CTransform>()->speed.x = -m_playerConfig.SPEED;
		m_player->getComponent<CState>()->state = "run";
			transform->scale.x = -1.0;
	
	}
	else if (input->right)
	{
		m_player->getComponent<CTransform>()->speed.x = m_playerConfig.SPEED;
		transform->scale.x = 1.0;
	} 

	else {

		m_player->getComponent<CTransform>()->speed.x = 0;
	}

	for (auto & e : m_entityManager.getEntities())
	{
        if (e->tag() == "player")
        {
            transform->prevPos = transform->pos;
            transform->pos += transform->speed;
           
        }
        
        else
        {
            auto eTransform = e->getComponent<CTransform>();
            eTransform->prevPos = eTransform->pos;
		    eTransform->pos += eTransform->speed;
        }
	}

	// TODO: Implement gravity's effect on the player
    for (auto & e : m_entityManager.getEntities())
    {
        if(e->hasComponent<CGravity>())
        {
            transform->speed.y += e->getComponent<CGravity>()->gravity;
        }
    }
    

}
    


void GameState_Play::sLifespan()
{
    //  Check lifespawn of entities that have them, and destroy them if they go over
    for(auto & e: m_entityManager.getEntities())
    {
        if(e->hasComponent<CLifeSpan>())
        {
            if(e->getComponent<CLifeSpan>()->clock.getElapsedTime().asMilliseconds() > e->getComponent<CLifeSpan>()->lifespan)
            {
                e->destroy();
            }
        }
    }
}


void GameState_Play::sDrag()
{
    
	if (m_drag == NULL) { return; }
    
	if (m_drag->getComponent<CDraggable>()->isDrag == true)
	{
		m_drag->getComponent<CTransform>()->pos = mousepos;
		std::cout << "Mouse: (" << mousepos.x << ", " << mousepos.y << " )" << endl;
	}
           
	//if grid mode is on
	if(m_drawGrid)
	{
		auto xpos = m_game.window().mapPixelToCoords(sf::Mouse::getPosition(m_game.window())).x;
		auto ypos = m_game.window().getSize().y - m_game.window().mapPixelToCoords(sf::Mouse::getPosition(m_game.window())).y;

		xpos = floor(xpos/64) * 64 + 32;
		ypos = floor(xpos/64) * 64 + 32;
		m_drag->getComponent<CTransform>()->pos = Vec2(xpos,ypos);
	}
    

	
}

void GameState_Play::sAI() 
{

	//Implementing patrol behaviour
   for (auto & e : m_entityManager.getEntities())
	{

		if (e->getComponent<CAnimation>()->animation.getName() == "Enemy" || e->getComponent<CAnimation>()->animation.getName() == "EnemyBird" )
		{
			if (e->hasComponent<CPatrol>()) {
				auto  entityTransform = e->getComponent<CTransform>();
				auto  currentpos = e->getComponent<CPatrol>()->currentPosition;
				auto  positionVec = e->getComponent<CPatrol>()->positions;
				auto  patrol = e->getComponent<CPatrol>();
				float angle;

				if (fabs(entityTransform->pos.x - patrol->positions[patrol->currentPosition].x) < 5 && fabs(entityTransform->pos.y - patrol->positions[patrol->currentPosition].y) < 5)
				{
					if (!patrol->currentPosition + 1 >= patrol->positions.size())
					{
						patrol->currentPosition += 1;

					}
					else
					{
						patrol->currentPosition = 0;
					}
				}
				//  Patrol behavior of NPCS
				Vec2 target = positionVec[currentpos] - entityTransform->pos;
				entityTransform->angle = atan2f(target.y, target.x);
				entityTransform->speed = Vec2(patrol->speed*cosf(entityTransform->angle), patrol->speed*sinf(entityTransform->angle));
				//stop
			   entityTransform->angle = 0;

			}
		}

	}
}



void GameState_Play::sCollision()
{
	for (auto & brick : m_entityManager.getEntities("tile"))
	{
		if (brick->hasComponent<CBoundingBox>())
		{
			for (auto & bullet : m_entityManager.getEntities("bullet"))
			{
				Vec2 overlap = Physics::GetOverlap(bullet, brick);

				if (overlap.x > 0 && overlap.y > 0)
				{
					if (brick->getComponent<CAnimation>()->animation.getName() == "Brick")
					{
						brick->getComponent<CAnimation>()->animation = m_game.getAssets().getAnimation("Explosion");
						brick->getComponent<CAnimation>()->repeat = false;
						brick->removeComponent<CBoundingBox>();

					}//collison checking for bullet and enemy
					else if (brick->getComponent<CAnimation>()->animation.getName() == "Enemy" || brick->getComponent<CAnimation>()->animation.getName() == "EnemyBird") {
						brick->getComponent<CAnimation>()->animation = m_game.getAssets().getAnimation("Explosion");
						brick->getComponent<CAnimation>()->repeat = false;
						brick->removeComponent<CBoundingBox>();
					}
					bullet->destroy();
					break;
				}
				

				
				
			}
		}

		if (brick->hasComponent<CBoundingBox>())
		{
			// between Tile and Player
			Vec2 tile_playerOverlap = Physics::GetOverlap(m_player, brick);

			Vec2 prevOverlap = Physics::GetPreviousOverlap(m_player, brick);

			auto playerPrevPos = m_player->getComponent<CTransform>()->prevPos;

			if (tile_playerOverlap.x > 0 && tile_playerOverlap.y > 0)
			{
				//between playerand enemy
				if (brick->getComponent<CAnimation>()->animation.getName() == "Enemy" || brick->getComponent<CAnimation>()->animation.getName() == "EnemyBird") {

					m_player->destroy();
					spawnPlayer();
					break;
				}
				if (prevOverlap.x > 0)
				{
					// collision from the top or bottom
					if ((m_player->getComponent<CTransform>()->pos.y - playerPrevPos.y) < 0)
					{
						// came from top of tile;
						if (m_player->getComponent<CState>()->state == "jumping")
						{
							m_player->getComponent<CState>()->state = "standing";
						}
						m_player->getComponent<CTransform>()->speed.y = 0;
						m_player->getComponent<CTransform>()->pos.y += tile_playerOverlap.y;

					}
					else if ((m_player->getComponent<CTransform>()->pos.y - playerPrevPos.y) > 0)
					{
						// Collision came from bottom of tile;
						m_player->getComponent<CTransform>()->speed.y = 0;
						m_player->getComponent<CTransform>()->pos.y -= tile_playerOverlap.y;
						if (brick->getComponent<CAnimation>()->animation.getName() == "Question")
						{
							brick->getComponent<CAnimation>()->animation = m_game.getAssets().getAnimation("Question2");
							auto entity = m_entityManager.addEntity("Dec");
							entity->addComponent<CTransform>(Vec2(brick->getComponent<CTransform>()->pos.x, brick->getComponent<CTransform>()->pos.y + brick->getComponent<CAnimation>()->animation.getSize().y));
							entity->addComponent<CAnimation>(m_game.getAssets().getAnimation("Coin"), true);
							entity->addComponent<CLifeSpan>(600);
							//Adds score
							score += 10;
							m_scoreText.setString("SCORE: " + to_string(score));
						}
						else if (brick->getComponent<CAnimation>()->animation.getName() == "Brick")
						{
							brick->getComponent<CAnimation>()->animation = m_game.getAssets().getAnimation("Explosion");
							brick->getComponent<CAnimation>()->repeat = false;
							brick->removeComponent<CBoundingBox>();


						}
					}
				}
				if (prevOverlap.y > 0)
				{

					// Collision from the sides;
					if ((m_player->getComponent<CTransform>()->pos.x - playerPrevPos.x) < 0)
					{
						// came from the right of the player;
						m_player->getComponent<CTransform>()->speed.x = 0;
						m_player->getComponent<CTransform>()->pos.x += tile_playerOverlap.x;
					}
					else if ((m_player->getComponent<CTransform>()->pos.x - playerPrevPos.x) > 0)
					{
						// collision from left;
						m_player->getComponent<CTransform>()->speed.x = 0;
						m_player->getComponent<CTransform>()->pos.x -= tile_playerOverlap.x;
					}

				}
			}
		}

	}


		if (m_player->getComponent<CTransform>()->pos.y < 0) {
			m_player->destroy();
			spawnPlayer();
		}

		// don't let the player walk off the left side 
		if (m_player->getComponent<CTransform>()->pos.x < 0)
		{
			m_player->getComponent<CTransform>()->pos.x = m_player->getComponent<CTransform>()->prevPos.x;
			m_player->getComponent<CTransform>()->speed.x = 0;
		}


}
	


void GameState_Play::sUserInput()
{
    //  implement the rest of the player input

    sf::Event event;
    while (m_game.window().pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            m_game.quit();
        }

        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::Escape:  { m_game.popState(); break; }
                case sf::Keyboard::Z:       { init(m_levelPath); break; }
                case sf::Keyboard::R:       { m_drawTextures = !m_drawTextures; break; }
                case sf::Keyboard::F:       { m_drawCollision = !m_drawCollision; break; }
                case sf::Keyboard::P:       { setPaused(!m_paused); break; }
				case sf::Keyboard::G:       
				{ 
					if(m_drawGrid == true)
					{
						m_drawGrid = false;
					};
					if(m_drawGrid == false)
					{
						m_drawGrid = true;
					};
				 	break;
				}
                case sf::Keyboard::Insert:       { createEntity();break;}
				case sf::Keyboard::Delete:       { deleteEntity();break;}
				//Saves the game 
				case sf::Keyboard::T:{ saveLevel(); break; }


                

                //WASD Movement
                case sf::Keyboard::W:       { m_player->getComponent<CInput>()->up = true; break; }
                case sf::Keyboard::A:       { m_player->getComponent<CInput>()->left = true; break; }
                case sf::Keyboard::D:       { m_player->getComponent<CInput>()->right = true; break; }
                case sf::Keyboard::S:       { m_player->getComponent<CInput>()->down = true; break; }
		
				case sf::Keyboard::Space:
				{
					spawnBullet(m_player);
					break;
				}
				default:
					break;

            }
        }

        if (event.type == sf::Event::KeyReleased)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::Escape: { break; }

                //WASD Movement
                case sf::Keyboard::W:       { m_player->getComponent<CInput>()->up = false; break; }
                case sf::Keyboard::A:       { m_player->getComponent<CInput>()->left = false; 
					m_player->getComponent<CAnimation>()->animation = m_game.getAssets().getAnimation("Stand");	break; }
                case sf::Keyboard::D:       { m_player->getComponent<CInput>()->right = false; break; }
                case sf::Keyboard::S:       { m_player->getComponent<CInput>()->down = false; break; }
			    //To creates entity
				case sf::Keyboard::Insert: { break; }
				case sf::Keyboard::Delete: { break; }
				default:
					break;
			
			}
        }

        if (event.type == sf::Event::MouseMoved)
        {
           //update mouse position
            mousepos = Vec2 (event.mouseMove.x, m_game.window().getSize().y - event.mouseMove.y);
                
        }

        if(event.type == sf::Event::MouseButtonPressed)
        {
			
            for(auto & e: m_entityManager.getEntities())
            {
                if(e->hasComponent<CDraggable>())
                {  
					if (Physics::isInside(e, mousepos)) 
					{
						m_drag = e;
						m_drag->getComponent<CDraggable>()->isDrag = true;
								
					}
					            
                }
                
            }
        
		}

        if(event.type == sf::Event::MouseButtonReleased)
        {
            if(event.mouseButton.button == sf::Mouse::Left)
			{
					if(m_drag != NULL)
                    {
                        m_drag->getComponent<CDraggable>()->isDrag = false;
						m_drag->getComponent<CTransform>()->pos = mousepos;
						m_drag = nullptr;
						
						

                    }
                }

            }
            
        }

        
    }



void GameState_Play::sAnimation()
{
	
	// TODO: for each entity with an animation, call entity->getComponent<CAnimation>()->animation.update()
	auto animation = m_player->getComponent<CAnimation>()->animation;
	auto state = m_player->getComponent<CState>()->state;

	if (state == "standing" && !(m_player->getComponent<CInput>()->left || m_player->getComponent<CInput>()->right))
	{
		m_player->getComponent<CAnimation>()->animation = m_game.getAssets().getAnimation("Stand");
	}
	else if (state == "jumping")
	{
	m_player->getComponent<CAnimation>()->animation = m_game.getAssets().getAnimation("Air");

	}
	else if ((m_player->getComponent<CInput>()->left || m_player->getComponent<CInput>()->right) && !(animation.getName() == "Run"))
	{
		m_player->getComponent<CAnimation>()->animation = m_game.getAssets().getAnimation("Run");
	}



	for (auto  e : m_entityManager.getEntities())
	{
		if (e->hasComponent<CAnimation>())
		{
			e->getComponent<CAnimation>()->animation.update();


			if (e->getComponent<CAnimation>()->animation.hasEnded() && e->getComponent<CAnimation>()->repeat == false)
			{
				e->destroy();
			}
		}
	}
}

void GameState_Play::sRender()
{
    // color the background darker so you know that the game is paused
    if (!m_paused)  { m_game.window().clear(sf::Color(100, 100, 255)); }
    else            { m_game.window().clear(sf::Color(50, 50, 150)); }

    // set the viewport of the window to be centered on the player if it's far enough right
    auto pPos = m_player->getComponent<CTransform>()->pos;
    float windowCenterX = fmax(m_game.window().getSize().x / 2.0f, pPos.x);
    sf::View view = m_game.window().getView();
    view.setCenter(windowCenterX, m_game.window().getSize().y - view.getCenter().y);
    m_game.window().setView(view);

	float scorePos=windowCenterX- m_game.window().getSize().x/2;
	m_scoreText.setPosition(scorePos, 10);


    // draw all Entity textures / animations
    if (m_drawTextures)
    {
        for (auto e : m_entityManager.getEntities())
        {
            auto transform = e->getComponent<CTransform>();

            if (e->hasComponent<CAnimation>())
            {
                auto animation = e->getComponent<CAnimation>()->animation;
                animation.getSprite().setRotation(transform->angle);
                animation.getSprite().setPosition(transform->pos.x, m_game.window().getSize().y - transform->pos.y);
                animation.getSprite().setScale(transform->scale.x, transform->scale.y);
                m_game.window().draw(animation.getSprite());
            }
        }
    }

	// draw all Entity grid bounding boxes with a rectangleshape
	if (m_drawGrid)
	{
		for (auto e : m_entityManager.getEntities())
		{
			if (e->hasComponent<CBoundingBox>())
			{
				auto box = e->getComponent<CBoundingBox>();
				auto transform = e->getComponent<CTransform>();
				sf::RectangleShape rect;
				rect.setSize(sf::Vector2f(box->size.x - 1,  box->size.y - 1));
				rect.setOrigin(sf::Vector2f(box->halfSize.x, box->halfSize.y));
				rect.setPosition(transform->pos.x, m_game.window().getSize().y - transform->pos.y);
				rect.setFillColor(sf::Color(0, 0, 0, 0));

				if (!box) { rect.setOutlineColor(sf::Color::Black); }

				rect.setOutlineThickness(1);
				m_game.window().draw(rect);
			}
		}
	}

    // draw all Entity collision bounding boxes with a rectangleshape
    if (m_drawCollision)
    {
        for (auto e : m_entityManager.getEntities())
        {
            if (e->hasComponent<CBoundingBox>())
            {
                auto box = e->getComponent<CBoundingBox>();
                auto transform = e->getComponent<CTransform>();
                sf::RectangleShape rect;
                rect.setSize(sf::Vector2f(box->size.x-1, box->size.y-1));
                rect.setOrigin(sf::Vector2f(box->halfSize.x, box->halfSize.y));
                rect.setPosition(transform->pos.x, m_game.window().getSize().y - transform->pos.y);
                rect.setFillColor(sf::Color(0, 0, 0, 0));
                rect.setOutlineColor(sf::Color(255, 255, 255, 255));
                rect.setOutlineThickness(1);
                m_game.window().draw(rect);
            }
        }
    }
	m_game.window().draw(m_scoreText);
    m_game.window().display();
}