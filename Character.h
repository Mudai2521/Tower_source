#pragma once
#include "stdafx.h"
#include "Sprite.h"

class Character
{
public:
	Character();
	~Character();
	bool Init();
	void Term();
private:
	DirectX::XMFLOAT2 Scale;
	float Rotate;
	DirectX::XMFLOAT2 Trans;

	Character(const  Character&) = delete;
	Character& operator=(const Character&) = delete;
};
