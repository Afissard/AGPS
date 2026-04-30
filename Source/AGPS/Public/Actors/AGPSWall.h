// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"
#include "Perception/AISightTargetInterface.h"
#include "AGPSWall.generated.h"

UCLASS()
class AGPS_API AAGPSWall : public APawn, public IGameplayTagAssetInterface, public IAISightTargetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAGPSWall();

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere , BlueprintReadWrite , meta = (AllowPrivateAccess = "true"))
	class UAIPerceptionStimuliSourceComponent* StimuliSource;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sight")
	USceneComponent* LeftExtremity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sight")
	USceneComponent* RightExtremity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tags")
	FGameplayTagContainer WallGameplayTags;

	// Implement the interface function
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override 
	{ 
		TagContainer = WallGameplayTags; 
	}

	UPROPERTY(VisibleAnywhere)
	int GuardSeen;
	
	virtual bool CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor = nullptr, const bool* bWasVisible = nullptr, int32* UserData = nullptr) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
