// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/AGPSWall.h"

#include "Components/BoxComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"

// Sets default values
AAGPSWall::AAGPSWall()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(CollisionBox);
	
	
	// Les points d'extremités 
	LeftExtremity = CreateDefaultSubobject<USceneComponent>(TEXT("LeftExtremity"));
	LeftExtremity->SetupAttachment(RootComponent);
	RightExtremity = CreateDefaultSubobject<USceneComponent>(TEXT("RightExtremity"));
	RightExtremity->SetupAttachment(RootComponent);

	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus"));
	//Register sight and hearing as stimuli
	StimuliSource->RegisterForSense(TSubclassOf<UAISense_Sight>());
	StimuliSource->RegisterWithPerceptionSystem();
}

// Called when the game starts or when spawned
void AAGPSWall::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAGPSWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AAGPSWall::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor, const bool* bWasVisible, int32* UserData) const
{
	if (!LeftExtremity || !RightExtremity || !IgnoreActor)
	{
		return false;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	FVector LeftPos = LeftExtremity->GetComponentLocation();
	FVector RightPos = RightExtremity->GetComponentLocation();

	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WallSightTrace), true, IgnoreActor);
	TraceParams.AddIgnoredActor(this); 

	FHitResult HitResult;

	// Result trace vers l'extrémité gauche
	NumberOfLoSChecksPerformed++;
	bool bHitLeft = World->LineTraceSingleByChannel(HitResult, ObserverLocation, LeftPos, ECC_Visibility, TraceParams);
	if (bHitLeft)
	{
		return false; 
	}
	DrawDebugLine(World, ObserverLocation, LeftPos, bHitLeft ? FColor::Red : FColor::Green, false, 0.1f, 0, 2.0f);	// Result trace vers l'extrémité droite
	NumberOfLoSChecksPerformed++;
	bool bHitRight = World->LineTraceSingleByChannel(HitResult, ObserverLocation, RightPos, ECC_Visibility, TraceParams);
	if (bHitRight)
	{
		return false;
	}
	DrawDebugLine(World, ObserverLocation, RightPos, bHitRight ? FColor::Red : FColor::Green, false, 0.1f, 0, 2.0f);	OutSeenLocation = GetActorLocation(); 
	OutSightStrength = 1.0f;

	return true;
}

