#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "DwarfPlayerState.h"
#include "DwarfSaveGame.generated.h"

UCLASS()
class DWARF_API UDwarfSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY()
	int upgradeLevels[UPGRADE_COUNT];

	UPROPERTY()
	int resources[RESOURCE_COUNT];
	UPROPERTY()
	int isResourceUnlocked[RESOURCE_COUNT];
	UPROPERTY()
	int relicCounts[RELIC_COUNT]; 

	UPROPERTY()
	int savedStats[4];

	UPROPERTY()
	int level;
	UPROPERTY()
	int exp;
	UPROPERTY()
	int blockBroken;
	UPROPERTY()
	uint32 saveTime;
};
