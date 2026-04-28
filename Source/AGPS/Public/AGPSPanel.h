#pragma once

#include "Algorithms/GuardPlacementAlgoBase.h"
#include "Guard/AutoGuardComponent.h"
#include "Widgets/SCompoundWidget.h"
#include "NavMesh/RecastNavMesh.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
/**
 * @brief Entry representing a navmesh found in the editor world.
 *
 * This struct stores a weak reference to an ARecastNavMesh and a human-readable name
 * for display in the UI combo box.
 */
struct FAGPSNavMeshEntry
{
    /** Weak pointer to the navmesh actor (so we don't keep it alive). */
    TWeakObjectPtr<ARecastNavMesh> NavMesh;

    /** Display name shown in the combo box. */
    FString Name;
};

/**
 * @brief Slate panel widget for the AGPS plugin.
 *
 * Provides UI to select a navigation mesh, select an algorithm implementation,
 * choose a Guard actor class, preview computed guard spawn points and place
 * guard actors in the current editor level.
 */
class SAGPSPanel : public SCompoundWidget
{
public:
	// Slate arguments for the SAGPSPanel widget (none currently).
	SLATE_BEGIN_ARGS(SAGPSPanel) {}
	SLATE_END_ARGS()
	
	// Widget switcher for the top-level view (0 = Settings, 1 = Placement)
	TSharedPtr<SWidgetSwitcher> ViewSwitcher;
	
	// All discovered navigation meshes in the current editor world (for the combo box).
    TArray<TSharedPtr<FAGPSNavMeshEntry>> NavMeshEntries;
    
    // Currently selected navmesh entry from the combo box; may be invalid/null.
    TSharedPtr<FAGPSNavMeshEntry> SelectedNavMeshEntry;
    
    // Combo box widget for selecting a navmesh.
    TSharedPtr<SComboBox<TSharedPtr<FAGPSNavMeshEntry>>> NavMeshComboBox;
    
    // Available algorithm classes (derived from UGuardPlacementAlgoBase) for selection.
    TArray<TSharedPtr<UClass>> AlgoEntries;
    
    // Currently selected algorithm class (may be null/invalid).
    TSharedPtr<UClass> SelectedAlgo;
    
    // Combo box widget for selecting the algorithm class.
    TSharedPtr<SComboBox<TSharedPtr<UClass>>> AlgoComboBox;
    
    // The actor class chosen to spawn as guards (set via the class picker).
    TSubclassOf<AActor> SelectedGuardActorClass;
	
	// Guard settings
	FGuardSettings GuardSettings;

private:
	// Computed guard spawn settings produced by the selected algorithm (cached for preview/place).
	TArray<FGuardSpawnSetting> GuardSpawnSettingsComputed;
	
public:
	/**
	 * @brief Build the widget UI and initialize internal lists.
	 * 
	 * Called by Slate to construct the panel. It initializes the navmesh and
	 * algorithm lists and builds the child widgets (combo boxes, class viewer,
	 * preview/place buttons).
	 *
	 * @param InArgs Slate construction arguments.
	 */
	void Construct(const FArguments& InArgs);

private:
    /**
     * @brief Refresh the list of NavMesh actors found in the editor world.
     *
     * Scans the current editor UWorld for ARecastNavMesh actors and populates
     * NavMeshEntries and refreshes the navmesh combo box.
     */
    void RefreshNavMeshes();
    
    /**
     * @brief Get a textual summary for the currently selected navmesh.
     *
     * Returns information such as bounds center/extent and nav config description
     * for display in the UI. If none selected, returns a "none" text.
     *
     * @return FText with the selected navmesh description.
     */
    FText GetSelectedNavMeshText() const;
    
    /**
     * @brief Discover and populate classes derived from UGuardPlacementAlgoBase.
     *
     * Filters out abstract classes, sorts result alphabetically and refreshes
     * AlgoComboBox.
     */
    void RefreshAlgorithmClasses();
    
    /**
     * @brief Callback invoked when the user picks a guard actor class via the class viewer.
     *
     * Validates that the chosen class is a child of AActor and stores it in
     * SelectedGuardActorClass; clears selection otherwise.
     *
     * @param ChosenClass The class chosen by the user (may be null).
     */
    void OnGuardClassPicked(UClass* ChosenClass);
    
    /**
     * @brief Get the current editor UWorld (editor context).
     *
     * Returns the UWorld used by the editor (or nullptr if not available).
     * Use this when spawning or previewing guard actors in the editor.
     *
     * @return UWorld* pointer or nullptr.
     */
    static UWorld* GetEditorWorld();
	
};
