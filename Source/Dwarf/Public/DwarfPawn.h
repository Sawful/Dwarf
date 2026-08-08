// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "C:\Program Files\Epic Games\UE_5.7\Engine\Plugins\2D\Paper2D\Source\Paper2D\Classes\PaperSpriteComponent.h"
#include "DwarfPawn.generated.h"

class Cave;

UCLASS()
class DWARF_API ADwarfPawn : public APawn
{
	GENERATED_BODY()

public:
	ADwarfPawn();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UPaperSprite* idle;
	UPROPERTY(EditAnywhere)
	UPaperSprite* hit;
	UPROPERTY(EditAnywhere)
	float hitAnimationDuration = 0.2f;
	float hitAnimationClock;
	bool isHitting;

	UPROPERTY(EditAnywhere)
	UPaperSpriteComponent* SpriteComponent;

	UPROPERTY(EditAnywhere)
	USceneComponent* Dwarf;

	float metersWalked = 0;
	float movementSpeed = 25;

	UPROPERTY(EditAnywhere)
	FVector pawnOffset = FVector(0, 0, 0);
public:	
	float TimeFactor = 1.0f;
	float targetMetersWalked = 0;
	
	virtual void Tick(float DeltaTime) override;
	void MoveForward();
	void ResetDwarfPawn();
	void PositionToCave(Cave* _cave);
	void HitAnimation();
};
