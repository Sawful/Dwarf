// Fill out your copyright notice in the Description page of Project Settings.


#include "DwarfCameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraComponent.h"

ADwarfCameraActor::ADwarfCameraActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADwarfCameraActor::BeginPlay()
{
	Super::BeginPlay();
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTarget(this);
}

void ADwarfCameraActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	switch (cameraState)
	{
	case MENU:
	{
		targetPosition = pawn->GetActorLocation() + currentOffset;
		break;
	}
	case IDLE:
	{
		targetPosition = pawn->GetActorLocation() + currentOffset;
		break;
	}
	}

	SetActorLocation(FMath::Lerp(GetActorLocation(), targetPosition, 5.0f * DeltaTime));
}

void ADwarfCameraActor::ForcePos()
{
	switch (cameraState)
	{
	case MENU:
	{
		targetPosition = pawn->GetActorLocation() + currentOffset;
			break;
	}
	case IDLE:
	{
		targetPosition = pawn->GetActorLocation() + currentOffset;
		break;
	}
	}
	SetActorLocation(targetPosition);
}

void ADwarfCameraActor::SetState(CameraState _camState)
{
	cameraState = _camState;
	switch (cameraState)
	{
	case MENU:
	{
		currentOffset = FVector(-5000, 2000, 500);
		targetPosition = pawn->GetActorLocation() + currentOffset;

		break;
	}

	case IDLE:
	{
		currentOffset = FVector(-1500, 0, 100);
		targetPosition = pawn->GetActorLocation() + currentOffset;

		break;
	}
	}
}
