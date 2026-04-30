// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/AGPSWall.h"
#include "GameFramework/GameModeBase.h"
#include "AGPSGameMode.generated.h"

/**
 * 
 */
UCLASS()
class AGPS_API AAGPSGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AAGPSWall*> CoveredActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> LevelWalls;

	UFUNCTION(BlueprintCallable)
	void InitLevel();
};
