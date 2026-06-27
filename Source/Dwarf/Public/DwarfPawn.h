// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "C:\Program Files\Epic Games\UE_5.7\Engine\Plugins\2D\Paper2D\Source\Paper2D\Classes\PaperSpriteComponent.h"
#include "DwarfPlayerState.h"
#include "DwarfPawn.generated.h"

UCLASS()
class DWARF_API ADwarfPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADwarfPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Main camera (follows the dwarf)
	UPROPERTY(EditAnywhere)
	UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere)
	UPaperSpriteComponent* SpriteComponent;

	UPROPERTY(EditAnywhere)
	USceneComponent* Dwarf;

	ADwarfPlayerState* playerState;
	float metersWalked = 0;
	float targetMetersWalked = 0;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void MoveForward();
};
