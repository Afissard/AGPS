// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ColorationVertice.h"
#include "../GuardPlacementAlgoBase.h"
#include "GPATricoloration.generated.h"

/**
 * 
 */
UCLASS()
class AGPS_API UGPATricoloration : public UGuardPlacementAlgoBase
{
	GENERATED_BODY()

public:
	UPROPERTY()
	ARecastNavMesh *NavMesh;
	
	TArray<FColorationVertice> Vertices;
	
	
private:
	void ComputeVertices();
	
	FColorationVertice* FindById(int32 Id);
	
	bool IsColorValid(int32 VertexIndex, int32 Color);
	
	bool ColorGraph(int32 VertexIndex);
	
	void ResetColors();
	
	UWorld* GetEditorWorld();
	
public:
	virtual FText GetDisplayName() const
	{
		return FText::FromName(FName("Tricoloration Algorithm"));
	}
	
	virtual void DebugView() override;
	
	virtual TArray<FGuardSpawnSetting> ComputeGuardLocations(ARecastNavMesh *InNavMesh) override;
	
};
