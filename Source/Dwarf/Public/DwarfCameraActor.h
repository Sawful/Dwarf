// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "DwarfCameraActor.generated.h"

class AActor;

UCLASS()
class DWARF_API ADwarfCameraActor : public ACameraActor
{
	GENERATED_BODY()
	ADwarfCameraActor();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	AActor* pawn;

	void ForcePos();
	enum CameraState
	{
		IDLE,
		MENU,
		ROGUE
	};
	void SetState(CameraState _camState);
	CameraState cameraState = MENU;
	FVector currentOffset;
	FVector targetPosition;

};
