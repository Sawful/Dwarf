#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DwarfGameState.h"
#include "DwarfPlayerController.generated.h"

class ADwarfPawn;
class ADwarfPlayerState;

UCLASS()
class DWARF_API ADwarfPlayerController : public APlayerController
{
	GENERATED_BODY()
	ADwarfPlayerController();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;
public:
	UPROPERTY(VisibleAnywhere)
	ADwarfPawn* DwarfPawn;

	UPROPERTY(VisibleAnywhere)
	ADwarfPlayerState* PState;

	UPROPERTY(VisibleAnywhere)
	ADwarfGameState* GState;

	virtual void SetupInputComponent() override;
	void Hit();

	// Debug
	void _AutoclickOn();
	void _AutoclickOff();
	bool _autoclick = false;


};
