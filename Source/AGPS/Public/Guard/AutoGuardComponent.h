// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "AutoGuardComponent.generated.h"

USTRUCT()
struct FGuardSettings
{
	GENERATED_BODY()
	
	// Field of View of the guard
	float FOV = 90.0f;
	
	// Max view distance
	float ViewRange = 500.0f;
	
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AGPS_API UAutoGuardComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UAutoGuardComponent();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Guard")
	float FOV = 90.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Guard")
	float ViewRange = 500.0f;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	void Init(const FGuardSettings* GuardSettings);
	
	void DebugDrawViewCone(const UWorld* WorldContextObject) const;
};
