#include "Physics.h"
#include "Components.h"
#include "math.h"

Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    // TODO: Returns the overlap rectangle between the bounding boxes of entities a and b
	float dx,overlap_x, dy, overlap_y;
	dx 			= abs(a->getComponent<CTransform>()->pos.x - b->getComponent<CTransform>()->pos.x);
	overlap_x   = a->getComponent<CBoundingBox>()->halfSize.x + b->getComponent<CBoundingBox>()->halfSize.x - dx;
	dy 			= abs(a->getComponent<CTransform>()->pos.y - b->getComponent<CTransform>()->pos.y);
	overlap_y   = a->getComponent<CBoundingBox>()->halfSize.y + b->getComponent<CBoundingBox>()->halfSize.y - dy;

    return Vec2(overlap_x, overlap_y);
}
Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    // TODO: Returns the overlap rectangle between the bounding boxes of entities a and b
	float dx,overlap_x, dy, overlap_y;
	dx 			= abs(a->getComponent<CTransform>()->prevPos.x - b->getComponent<CTransform>()->prevPos.x);
	overlap_x   = a->getComponent<CBoundingBox>()->halfSize.x + b->getComponent<CBoundingBox>()->halfSize.x - dx;
	dy 			= abs(a->getComponent<CTransform>()->prevPos.y - b->getComponent<CTransform>()->prevPos.y);
	overlap_y   = a->getComponent<CBoundingBox>()->halfSize.y + b->getComponent<CBoundingBox>()->halfSize.y - dy;

    return Vec2(overlap_x, overlap_y);
}

bool Physics::isInside(std::shared_ptr<Entity> a, Vec2 pos)
{
	if(!a->hasComponent<CAnimation>()) 
	{
		return false;
	}

	else
	{
		Vec2 halfSize,delta;
		halfSize = Vec2 (a->getComponent<CAnimation>()->animation.getSize().x/2,a->getComponent<CAnimation>()->animation.getSize().y/2);
		delta= Vec2(a->getComponent<CTransform>()->pos - pos).abs();
		if(delta.y < halfSize.y/2 && delta.x < halfSize.x/2){
			return true;
			//std::cout<<"Inside!";
		}

	}
}

