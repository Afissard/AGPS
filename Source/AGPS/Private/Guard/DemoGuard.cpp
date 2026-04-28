// Fill out your copyright notice in the Description page of Project Settings.


#include "Guard/DemoGuard.h"
#include "Guard/AutoGuardComponent.h"
#include "Components/SceneComponent.h"

// Sets default values
ADemoGuard::ADemoGuard()
{
	PrimaryActorTick.bCanEverTick = false;
	
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = SceneRoot;

	AutoGuardComponent = CreateDefaultSubobject<UAutoGuardComponent>(TEXT("AutoGuardComponent"));
	AutoGuardComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ADemoGuard::BeginPlay()
{
	Super::BeginPlay();
	
}
