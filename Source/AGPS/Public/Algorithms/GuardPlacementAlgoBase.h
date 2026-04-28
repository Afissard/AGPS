// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NavMesh/RecastNavMesh.h"
#include "CoreMinimal.h"
#include "Guard/AutoGuardComponent.h"
#include "UObject/Object.h"
#include "GuardPlacementAlgoBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGuardPlacementAlgo, Log, All);

/**
* @brief Settings that describe where and how a guard actor should be spawned.
*
* Instances of this struct represent a single spawn candidate produced by an
* algorithm: the location and rotation to use when previewing or placing an actor.
* Additional fields (path, vision parameters, waypoints, etc.) can be added later.
*/
USTRUCT()
struct FGuardSpawnSetting
{
	GENERATED_BODY()
	
	// World-space location where the guard actor should be spawned.
	FVector SpawnLocation;
	
	// Orientation to apply to the spawned guard actor.
    FRotator SpawnRotation;
	
	// Guard settings
	FGuardSettings GuardSettings;
};

/**
 * @brief Base class for guard placement algorithms.
 *
 * Derive from this UObject to implement different strategies that compute
 * guard spawn points on a navmesh. This base class provides a simple API
 * used by the AGPS editor tooling to compute spawn locations, preview them in
 * the editor and place guard actors into the level.
 *
 * Expected usage:
 *  - Override ComputeGuardLocations to return candidate spawn settings for a given ARecastNavMesh.
 *  - Optionally override DebugView to draw debug visuals (lines, spheres) for the computed points.
 *  - Use PreviewGuardAtLocation and PlaceGuardAtLocation helpers from the editor/plugin UI.
 */
UCLASS()
class AGPS_API UGuardPlacementAlgoBase : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * @brief Return a human-readable display name for this algorithm.
	 *
	 * Marked BlueprintCallable so it can be queried from Blueprints if needed.
	 *
	 * @return FText Display name shown in UI lists.
	 */
	UFUNCTION(BlueprintCallable)
	virtual FText GetDisplayName() const
	{
		return FText::FromName(FName("GuardPlacementAlgoBase"));
	}
	
	/**
	 * @brief Optional: Draw debug visualization for the algorithm.
	 *
	 * Implementations should draw lines/spheres/text in the editor/world to help
	 * understand computed positions or algorithm state. Called from UI when the
	 * user requests a debug view.
	 */
	virtual void DebugView();
	
	/**
	 * @brief Compute guard spawn settings for the provided navmesh.
	 *
	 * This is the core method to implement. It should analyze the given navmesh
	 * (ARecastNavMesh) and return an array of FGuardSpawnSetting describing
	 * where guards should be spawned.
	 *
	 * @param NavMesh Pointer to the navmesh to analyze. May be nullptr — implementations
	 *                should validate input and return an empty array when appropriate.
	 * @return TArray<FGuardSpawnSetting> Array of spawn settings (may be empty).
	 */
	virtual TArray<FGuardSpawnSetting> ComputeGuardLocations(ARecastNavMesh *NavMesh);
	
	/**
	 * @brief Preview guard actors at the given locations inside the editor world.
	 *
	 * Typically spawns transient preview actors or draws visual proxies that are
	 * not persisted in the level. This helper is intended to be called by editor UI.
	 *
	 * @param EditorWorld The editor UWorld used for preview (may be nullptr).
	 * @param GuardList Pointer to the list of computed spawn settings to preview.
	 */
	void PreviewGuardAtLocation(UWorld* EditorWorld, TArray<FGuardSpawnSetting>* GuardList,FGuardSettings GuardSettings);
	
	/**
	 * @brief Place actual guard actors in the editor world at the provided locations.
	 *
	 * Spawns actors of the provided class using the spawn settings. Implementations
	 * should ensure spawns happen with proper outer/flags and that operations are
	 * undoable (editor transaction) if called from editor UI.
	 *
	 * @param EditorWorld The editor UWorld where actors should be spawned (must be valid to place).
	 * @param GuardList Pointer to the list of spawn settings to apply.
	 * @param GuardClass Class of the actor to spawn for each setting. If null, the method
	 *                   should either skip spawning or use a sensible default.
	 */
	void PlaceGuardAtLocation(UWorld* EditorWorld, TArray<FGuardSpawnSetting>* GuardList, TSubclassOf<AActor> GuardClass);
};
