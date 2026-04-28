// Fill out your copyright notice in the Description page of Project Settings.


#include "Guard/AutoGuardComponent.h"


// Sets default values for this component's properties
UAutoGuardComponent::UAutoGuardComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UAutoGuardComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UAutoGuardComponent::Init(const FGuardSettings* GuardSettings)
{
	if (GuardSettings)
	{
		FOV = GuardSettings->FOV;
		ViewRange = GuardSettings->ViewRange;
	}
}

void UAutoGuardComponent::DebugDrawViewCone(const UWorld* WorldContextObject) const
{
	if (WorldContextObject)
	{
		const FVector Location = GetOwner()->GetActorLocation();
		const FRotator Rotation = GetOwner()->GetActorRotation();

		const FVector ForwardVector = Rotation.Vector();
		FVector RightVector = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);

		const float HalfFOV = FOV / 2.0f;
		const FVector LeftBoundary = ForwardVector.RotateAngleAxis(-HalfFOV, FVector::UpVector) * ViewRange;
		const FVector RightBoundary = ForwardVector.RotateAngleAxis(HalfFOV, FVector::UpVector) * ViewRange;

		DrawDebugLine(WorldContextObject, Location, Location + LeftBoundary, FColor::Green, false, -1.0f, 0, 2.0f);
		DrawDebugLine(WorldContextObject, Location, Location + RightBoundary, FColor::Green, false, -1.0f, 0, 2.0f);
		// TODO: Draw a true 3D Cone
	}
}
