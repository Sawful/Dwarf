#include "Block.h"

ABlock::ABlock()
{
}

void ABlock::BeginPlay()
{
	Super::BeginPlay();
}

void ABlock::DisconnectFromList()
{
	if (next) { next->previous = previous; }
	if (previous) { previous->next = next; }
}

void ABlock::UpdateBreakFactor()
{
	mat->SetScalarParameterValue("BrokenFactor", 1.0f - (float)(Data.health / Data.maxHealth));
}

FString GetResourceName(ResourceType _type)
{
	switch (_type)
	{
	case MUDROCK: return "Mudrock";
	case COAL: return "Coal";
	case COPPER: return "Copper";
	case TIN: return "Tin";
	case IRON: return "Iron";
	case SULFUR: return "Sulfur";
	case SILVER: return "Silver";
	case OBSIDIAN: return "Obsidian";
	case PLATINUM: return "Platinum";
	case DIAMOND: return "Diamond";
	}
	return "?Unknown Resource?";
}

FString GetResourceIcon(ResourceType _type)
{
	return "<img id=\"Resource." + GetResourceName(_type) + "\"/>";
}
