// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AutoGuardComponent.h"
#include "GameFramework/Actor.h"
#include "DemoGuard.generated.h"

UCLASS()
class AGPS_API ADemoGuard : public AActor
{
	GENERATED_BODY()
	
	// Composant AutoGuard : visible dans l'éditeur, lecture seule en Blueprint
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Guard", meta = (AllowPrivateAccess = "true"))
	UAutoGuardComponent* AutoGuardComponent;

public:
	// Sets default values for this actor's properties
	ADemoGuard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	
};
