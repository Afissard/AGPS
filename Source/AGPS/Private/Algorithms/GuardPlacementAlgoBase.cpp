// Fill out your copyright notice in the Description page of Project Settings.

#include "Algorithms/GuardPlacementAlgoBase.h"
#include "Editor.h"
#include "ScopedTransaction.h"
#include "Editor/UnrealEd/Public/Editor.h"

DEFINE_LOG_CATEGORY(LogGuardPlacementAlgo);

/**
 * @brief Perform debug output or visualization for this algorithm.
 *
 * Base implementation only logs a simple message. Concrete subclasses should
 * override this method to draw debug primitives (spheres, lines, text) or to
 * output richer debug information about internal algorithm state.
 *
 * Side effects:
 *  - Writes to the engine log (LogGuardPlacementAlgo).
 *  - Derived implementations may draw debug primitives into the world.
 */
void UGuardPlacementAlgoBase::DebugView()
{
	UE_LOG(LogGuardPlacementAlgo, Log, TEXT("UGuardPlacementAlgoBase::DebugView()"))
}

/**
 * @brief Compute spawn settings for guards on the provided navmesh.
 *
 * This is the primary extension point for algorithms. Given a pointer to an
 * ARecastNavMesh, implementations should analyze the navmesh and return a list
 * of candidate spawn settings (location + rotation) for guard actors.
 *
 * The base implementation returns an empty array; override this method in
 * subclasses to provide concrete strategies.
 *
 * @param NavMesh Pointer to the navmesh to analyze. Implementations should
 *                validate this pointer and return an empty array if null.
 * @return TArray<FGuardSpawnSetting> Array of computed guard spawn settings.
 */
TArray<FGuardSpawnSetting> UGuardPlacementAlgoBase::ComputeGuardLocations(ARecastNavMesh *NavMesh)
{
	TArray<FGuardSpawnSetting> GuardLocations;
	// Base implementation returns an empty array. Override this method in derived classes to implement specific algorithms.
	return GuardLocations;
}

/**
 * @brief Preview guard spawn locations in the editor by drawing debug primitives.
 *
 * Validates inputs and draws a sphere and a direction line at each spawn location.
 * This function is intended for editor-only visualization and does not persist
 * actors into the level.
 *
 * @param EditorWorld The editor UWorld to place debug drawings into. Must be valid.
 * @param GuardList Pointer to an array of FGuardSpawnSetting to preview. If null,
 *                  a warning is logged and the function returns.
 *
 * Side effects:
 *  - Emits log messages describing what is being previewed.
 *  - Draws persistent debug primitives (spheres and lines) in the provided world.
 */
void UGuardPlacementAlgoBase::PreviewGuardAtLocation(UWorld* EditorWorld, TArray<FGuardSpawnSetting>* GuardList)
{
	if (!EditorWorld)
	{
		UE_LOG(LogGuardPlacementAlgo, Error, TEXT("EditorWorld is missing or erroned"));
		return;
	}
	
	if (!GuardList)
	{
		UE_LOG(LogGuardPlacementAlgo, Warning, TEXT("PreviewGuardAtLocation called with null GuardList"));
		return;
	}
	
	UE_LOG(LogGuardPlacementAlgo, Log, TEXT("Previewing %d guard locations"), GuardList->Num());
	for (int i = 0; i < GuardList->Num(); ++i)
	{
		const auto& [SpawnLocation, SpawnRotation] = (*GuardList)[i];
		UE_LOG(LogGuardPlacementAlgo, Log, TEXT("Previewing guard at location %s and rotation %s"), *SpawnLocation.ToString(), *SpawnRotation.ToString())
		
		// Draw a orange sphere at the guard location
		DrawDebugSphere(
			EditorWorld,
			SpawnLocation,
			50.0f,				// radius
			12,					// segments
			FColor::Orange,		// color
			true,				// persistent lines
			-1.0f,				// life time
			0,					// depth priority
			2.0f				// thickness
		);
		
		// Draw a light blue line from the spawn location in the direction of the spawn rotation
		const FRotator Direction = SpawnRotation;
		DrawDebugLine(
			EditorWorld,
			SpawnLocation,
			Direction.RotateVector(FVector(100.0f, 0.0f, 0.0f)) + SpawnLocation, // Line length of 100 units in the direction of the rotation
			FColor::Cyan,	// color
			true,			// persistent lines
			-1.0f,			// life time
			0,				// depth priority
			2.0f			// thickness
		);
	}
}

/**
 * @brief Spawn actual guard actors in the editor world at the provided locations.
 *
 * Validates inputs, begins an editor transaction (so the whole operation is undoable),
 * and spawns the actors with appropriate spawn parameters. Each spawned actor is
 * marked transactional and selected in the editor.
 *
 * Important notes:
 *  - This function should only be called from editor code. Ensure EditorWorld is valid.
 *  - GuardClass must be a valid TSubclassOf<AActor>. Use a simple null-check on GuardClass
 *    (e.g. `if (!GuardClass)`) rather than dereferencing.
 *
 * @param EditorWorld The editor UWorld in which to spawn actors (must be valid).
 * @param GuardList Pointer to the array of spawn settings to apply (must be valid).
 * @param GuardClass The actor class to spawn for each guard (must be valid).
 *
 * Side effects:
 *  - Emits multiple log messages.
 *  - Starts a FScopedTransaction to make the spawns undoable.
 *  - Spawns actors, sets them transactional, labels them and marks packages dirty.
 *  - Selects the newly spawned actor in the editor.
 */
void UGuardPlacementAlgoBase::PlaceGuardAtLocation(UWorld* EditorWorld, TArray<FGuardSpawnSetting>* GuardList, TSubclassOf<AActor> GuardClass)
{
	if (!EditorWorld)
	{
		UE_LOG(LogGuardPlacementAlgo, Error, TEXT("EditorWorld is missing or incorrect"));
		return;
	}
	
	if (!GuardList)
	{
		UE_LOG(LogGuardPlacementAlgo, Warning, TEXT("PreviewGuardAtLocation called with null GuardList"));
		return;
	}
	
	if (!GuardClass)
	{
		UE_LOG(LogGuardPlacementAlgo, Error, TEXT("GuardClass is missing or incorrect"));
		return;
	}
	
	// Commencez une transaction unique pour toute l'opération (facilite undo multiple spawns en une seule commande)
	UE_LOG(LogGuardPlacementAlgo, Log, TEXT("Placing %d guards in the editor"), GuardList->Num());
	const FText TransactionText = FText::FromString(TEXT("Place Guards"));
	FScopedTransaction Transaction(TransactionText);
	EditorWorld->Modify(); // mark the world as modified for the undo system

	
	for (int i = 0; i < GuardList->Num(); ++i)
	{
		const auto& [SpawnLocation, SpawnRotation] = (*GuardList)[i];
		UE_LOG(LogGuardPlacementAlgo, Log, TEXT("Spawning guard at location %s and rotation %s"), *SpawnLocation.ToString(), *SpawnRotation.ToString());

		if (!*GuardClass)
		{
			UE_LOG(LogGuardPlacementAlgo, Error, TEXT("Invalid GuardClass"));
			continue;
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.Name = NAME_None;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		SpawnParams.OverrideLevel = EditorWorld->GetCurrentLevel(); // spawn dans le level courant de l'éditeur
		SpawnParams.bAllowDuringConstructionScript = true;

		// Marquer transaction pour l'acteur créé
		SpawnParams.bNoFail = true;

		// Spawn l'acteur dans l'éditeur/world
		AActor* NewActor = EditorWorld->SpawnActor<AActor>(*GuardClass, SpawnLocation, SpawnRotation, SpawnParams);
		if (!NewActor)
		{
			UE_LOG(LogGuardPlacementAlgo, Error, TEXT("Failed to spawn guard actor"));
			continue;
		}

		// Rendre l'acteur transactionnel (undo/redo)
		NewActor->SetFlags(RF_Transactional);
		NewActor->SetActorLabel(FString::Printf(TEXT("Guard_%d"), i));
		NewActor->Modify();

		// Notifier l'éditeur que l'acteur a changé (PostEditChange)
		NewActor->PostEditChange();

		// Marquer le package dirty pour sauvegarde
		NewActor->MarkPackageDirty();

		// Sélectionner le nouvel acteur (optionnel)
		if (GEditor)
		{
			GEditor->SelectNone(false, false);
			GEditor->SelectActor(NewActor, true, true);
		}
		UE_LOG(LogGuardPlacementAlgo, Log, TEXT("Guard n°%d have been placed in the editor"), i);
	}
}

