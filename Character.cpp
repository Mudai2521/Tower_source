#include "Character.h"

using namespace DirectX;

Character::Character():
	Scale(1.0f,1.0f),
	Rotate(0.0f),
	Trans(0.0f,0.0f)
{
}

Character::~Character()
{
	Term();
}

bool Character::Init()
{
}

void Character::Term()
{
}