#include "ResistanceRogueItem.h"
#include "DwarfPlayerState.h"

void UResistanceRogueItem::Bind(URoguePlayerData* _player, Cave* _cave)
{
	handle = _player->OnPressureCalc.AddUObject(this, &UResistanceRogueItem::PressureCalc);
}
void UResistanceRogueItem::UnBind(URoguePlayerData* _player, Cave* _cave)
{
	handle.Reset();
}

void UResistanceRogueItem::PressureCalc(BigNumber& _pressure)
{
	_pressure *= multiplier;
}

float UResistanceRogueItem::GetMult(int _level)
{
	switch (_level)
	{
	case 1: return 0.9f;
	case 2: return 0.8f;
	case 3: return 0.7f;
	case 4: return 0.55f;
	case 5: return 0.4f;
	case 6: return 0.25f;
	case 7: return 0.1f;
	}

	return 1.0f;
}
void UResistanceRogueItem::OnLevelUp()
{
	multiplier = GetMult(level);
}
FString UResistanceRogueItem::GetDescriptionText(int _level)
{
	return "Increases your resistance to pressure by +" + FString::FromInt((1.0f / (GetMult(_level))-1) * 100) + "%.";
}