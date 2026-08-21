#include "RogueItem.h"
#include "DwarfPlayerState.h"
#include "IdleRelic.h"
#include "Cave.h"

int URogueItem::GetRelicWeight()
{
	switch (rarity)
	{
	case COMMON: return 25 * level;
	case UNCOMMON: return 20 * level;
	case RARE: return 15 * level;
	case EPIC: return 10 * level;
	case LEGENDARY: return 5 * level;
	default: return 0;
	}
}

FSlateColor GetRarityColor(ItemRarity _rarity)
{
	switch(_rarity)
	{
		case COMMON:
		{
			return FSlateColor(FLinearColor(0.9, 0.9, 0.9, 1));
		}
		case UNCOMMON:
		{
			return FSlateColor(FLinearColor(0.2, 0.9, 0.2, 1));
		}
		case RARE:
		{
			return FSlateColor(FLinearColor(0.2, 0.2, 0.9, 1));
		}
		case EPIC:
		{
			return FSlateColor(FLinearColor(0.7, 0.2, 0.9, 1));
		}
		case LEGENDARY:
		{
			return FSlateColor(FLinearColor(0.9, 0.7, 0.2, 1));
		}
	}
	return FSlateColor();
}
