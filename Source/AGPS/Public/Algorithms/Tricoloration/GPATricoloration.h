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
	/**
	 * @brief Retrieve the map's corners based of the navmesh's triangulation.
	 *
	 * This method allow us to retrieve all vertices computed to calculate and triangulate the navmesh.
	 * Once all the vertices are retrieved, they are sorted only to get a unique instance of each
	 * (some may appear in multiple triangle) to return a list of vertices that are aware of their neighbors. 
	 *
	 * @return No return but sets Vertices, an array of Coloration vertices that represents the maps corners.
	 */
	void ComputeVertices();

	/**
	 * Return a specific coloration vertices of the navmesh based on its index.
	 * 
	 * @param Id Index of the wanted vertices.
	 * 
	 * @return A FColorationVertice that represents a vertices of the navmesh that is aware of its neighbors. 
	 */
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
