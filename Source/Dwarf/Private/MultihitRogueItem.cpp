#include "MultihitRogueItem.h"
#include "DwarfPlayerState.h"

void UMultihitRogueItem::CooldownCalc(float& _cd)
{
	if (reloading)
	{
		reloading = false;
		hitCounter = level;
		return;
	}

	_cd *= 0.01f;

	hitCounter--;
	if (hitCounter <= 0)
	{
		reloading = true;
	}
}

void UMultihitRogueItem::Bind(RoguePlayerData* _player, Cave* _cave)
{
	 
	handle = _player->OnCooldownCalc.AddUObject(this, &UMultihitRogueItem::CooldownCalc);
}

void UMultihitRogueItem::UnBind(RoguePlayerData* _player, Cave* _cave)
{
	_player->OnCooldownCalc.Remove(handle);
}

FString UMultihitRogueItem::GetDescriptionText(int _level)
{
	return "Everytime you hit, hit " + FString::FromInt(_level) + " more time(s).\n\"What if I just attach it to mine?\"";
}