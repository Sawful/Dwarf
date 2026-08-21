#include "RegenRogueItem.h"
#include "DwarfPlayerState.h"

void URegenRogueItem::Bind(RoguePlayerData* _player, Cave* _cave)
{
	handle = _player->OnPressureRegenCalc.AddUObject(this, &URegenRogueItem::PressureRegenCalc);
}
void URegenRogueItem::UnBind(RoguePlayerData* _player, Cave* _cave)
{
	handle.Reset();
}

void URegenRogueItem::PressureRegenCalc(BigNumber& _pressure)
{
	_pressure *= multiplier;
}

BigNumber URegenRogueItem::GetMult(int _level)
{
	switch(_level)
	{ 
	case 1: return 2.0f;
	case 2: return 3.0f;
	case 3: return 4.0f;
	case 4: return 5.5f;
	case 5: return 7.0f;
	case 6: return 8.5f;
	case 7: return 10.0f;
	}

	return 1.0f;
}

void URegenRogueItem::OnLevelUp()
{
	multiplier = GetMult(level);
}
FString URegenRogueItem::GetDescriptionText(int _level)
{
	return "Increases the pressure you gain when breaking blocks by +" + (((BigNumber)1.0f / (GetMult(_level) - 1)) * 100.0f).ToStringTrunc() + "%.";
}