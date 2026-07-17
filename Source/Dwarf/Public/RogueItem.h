#pragma once

struct RoguePlayerData;

class RogueItem
{
public:
	virtual void Bind(RoguePlayerData* _player) = 0;
};

class DamageRogueItem : public RogueItem
{
	void DamageCalc(int& _damage);
	FDelegateHandle handle;
public:
	virtual void Bind(RoguePlayerData* _player);
	virtual void UnBind(RoguePlayerData* _player);
};