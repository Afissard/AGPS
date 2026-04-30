// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "AGPSGuardAIController.generated.h"

/**
 * 
 */
UCLASS()
class AGPS_API AAGPSGuardAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AAGPSGuardAIController(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI", meta=(AllowPrivateAccess=true))
	UAIPerceptionComponent* AIPerceptionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI", meta=(AllowPrivateAccess=true))
	UAISenseConfig_Sight* SightConfig;

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
	UFUNCTION()
	void OnTargetPerceptionForgotten(AActor* Actor);
	
	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(Transient)
	class UBehaviorTreeComponent* BehaviorTreeComponent;

	UPROPERTY(Transient)
	class UBlackboardComponent* BlackboardComponent;
};
