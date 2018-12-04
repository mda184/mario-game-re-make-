#pragma once

#include "Common.h"
#include "Entity.h"

namespace Physics
{
    Vec2 GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
    Vec2 GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
    bool isInside(std::shared_ptr<Entity> a, Vec2 pos);
}