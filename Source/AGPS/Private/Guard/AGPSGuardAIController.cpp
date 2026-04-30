// Fill out your copyright notice in the Description page of Project Settings.


#include "Guard/AGPSGuardAIController.h"

#include "Actors/AGPSWall.h"
#include "Core/AGPSGameMode.h"
#include "Guard/AGPSGuard.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Sight.h"

AAGPSGuardAIController::AAGPSGuardAIController(const FObjectInitializer& ObjectInitializer)
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	SetPerceptionComponent(*AIPerceptionComponent);
	
	//Sight Config
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	
	SightConfig->SightRadius = 1500.f;
	SightConfig->LoseSightRadius = 1700.f;
	SightConfig->PeripheralVisionAngleDegrees = 90.f;
	SightConfig->SetMaxAge(0.1f); 
	
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
}

// Called when the game starts or when spawned
void AAGPSGuardAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		if (AAGPSWall* Wall = Cast<AAGPSWall>(Actor))
		{
			Wall->GuardSeen++;
			if (AAGPSGameMode* GameMode = Cast<AAGPSGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
			{
				if (GameMode->CoveredActors.Contains(Wall))
				{
					return;
				}
				GameMode->CoveredActors.Add(Wall);
			}
		}
	}
	else
	{
		if (AAGPSWall* Wall = Cast<AAGPSWall>(Actor))
		{
			if (AAGPSGameMode* GameMode = Cast<AAGPSGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
			{
				Wall->GuardSeen--;
				if (Wall->GuardSeen == 0)
				{
					GameMode->CoveredActors.Remove(Wall);
				}
			}
		}
	}
	
}

void AAGPSGuardAIController::OnTargetPerceptionForgotten(AActor* Actor)
{
	if (AAGPSWall* Wall = Cast<AAGPSWall>(Actor))
	{
		if (Wall->GuardSeen > 0)
		{
			Wall->GuardSeen--;
		}
	}
}

// Called every frame
void AAGPSGuardAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (auto Guard = Cast<AAGPSGuard>(InPawn))
	{
		if (AIPerceptionComponent)
		{
			AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AAGPSGuardAIController::OnTargetPerceptionUpdated);
			AIPerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this, &AAGPSGuardAIController::OnTargetPerceptionForgotten);
		}
	}
}
