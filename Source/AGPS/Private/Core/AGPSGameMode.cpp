// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/AGPSGameMode.h"

#include "Kismet/GameplayStatics.h"

void AAGPSGameMode::InitLevel()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAGPSWall::StaticClass(), LevelWalls);
}
