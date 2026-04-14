#include "AGPSPanel.h"
#include "Editor.h"
#include "EngineUtils.h"
#include "NavigationSystem.h"
#include "UObject/UObjectIterator.h"
#include "UObject/Class.h"
#include "Algorithms/GuardPlacementAlgoBase.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "NavMesh/RecastNavMesh.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Input/SButton.h"
#include "Modules/ModuleManager.h"
#include "ClassViewerModule.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Widgets/Text/STextBlock.h"

/**
 * @brief Slate Construct implementation for the SAGPSPanel.
 *
 * Initializes available nav meshes and algorithm classes, and builds the widget
 * tree containing:
 *  - NavMesh selector combo box + refresh button
 *  - Algorithm selector combo box
 *  - ClassViewer for picking guard actor class
 *  - Buttons for previewing and placing guards
 *
 * Uses inline lambdas for combo box generation and selection callbacks.
 *
 * @param InArgs Slate construction arguments (ignored).
 */
void SAGPSPanel::Construct(const FArguments& InArgs)
{
    RefreshNavMeshes();
    RefreshAlgorithmClasses();

    ChildSlot
    [
        SNew(SVerticalBox)

        // Navmesh selector
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(4)
        [
            SNew(STextBlock)
            .Text(FText::FromString("Select NavMesh"))
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(4)
        [
            SNew(SComboBox<TSharedPtr<FAGPSNavMeshEntry>>)
            .OptionsSource(&NavMeshEntries)
            // Lambda: generate a widget for each navmesh entry (simple text block with Entry->Name).
            .OnGenerateWidget_Lambda([](TSharedPtr<FAGPSNavMeshEntry> Entry)
            {
                return SNew(STextBlock).Text(FText::FromString(Entry->Name));
            })
            // Lambda: store the newly selected navmesh entry.
            .OnSelectionChanged_Lambda(
                [this](TSharedPtr<FAGPSNavMeshEntry> NewSelection, ESelectInfo::Type)
                {
                    SelectedNavMeshEntry = NewSelection;
                })
            .Content()
            [
                SNew(STextBlock)
                // Lambda: display current selection name or "None".
                .Text_Lambda([this]()
                {
                    return SelectedNavMeshEntry.IsValid()? FText::FromString(SelectedNavMeshEntry->Name): FText::FromString("None");
                })
            ]
        ]
        
        // Refresh the navmesh selection
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(4)
        [
            SNew(SButton)
            .Text(FText::FromString("Refresh NavMeshes"))
            .OnClicked_Lambda([this]()
            {
                RefreshNavMeshes();
                return FReply::Handled();
            })
        ]
        
        /*
        // Selected NavMesh info
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(4)
        [
            SNew(STextBlock)
            .Text_Lambda([this]()
            {                
                if (SelectedNavMeshEntry.IsValid() && SelectedNavMeshEntry->NavMesh.IsValid())
                {
                    return GetSelectedNavMeshText();
                }
                return FText::FromString("Selected NavMesh: None");
            })
        ]
        */

        // Algorithm selector
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(4)
        [
            SNew(STextBlock)
            .Text(FText::FromString("Select an algorithm"))
        ]
        
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(4)
        [
            SAssignNew(AlgoComboBox, SComboBox<TSharedPtr<UClass>>)
            .OptionsSource(&AlgoEntries)
            // Lambda: show the class name for each algorithm entry (or "Invalid").
            .OnGenerateWidget_Lambda([](TSharedPtr<UClass> InItem)
            {
                return SNew(STextBlock).Text(InItem.IsValid()? FText::FromString(InItem->GetName()): FText::FromString("Invalid"));
            })
            // Lambda: store the chosen algorithm class into SelectedAlgo.
            .OnSelectionChanged_Lambda([this](TSharedPtr<UClass> NewSelection, ESelectInfo::Type)
            {
                SelectedAlgo = NewSelection;
            })

            .Content()
            [
                SNew(STextBlock)
                // Lambda: show the currently selected algorithm name or prompt text.
                .Text_Lambda([this]()
                {
                    return SelectedAlgo.IsValid()? FText::FromString(SelectedAlgo->GetName()): FText::FromString("None");
                })
            ]
        ]

        // Guard actor class selector
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(4)
        [
            SNew(STextBlock)
            .Text(FText::FromString("Select a Guard class"))
        ]

        // Build a Class Viewer, configured in ClassPicker mode, listing classes.
        // The FOnClassPicked callback binds to our OnGuardClassPicked method.
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(4)
        [
            // We create the class viewer widget inline below.
            // Build options:
            SNew(SBox)
            .MaxDesiredHeight(300)
            [
                // Create the Class Viewer widget instance
                FModuleManager::Get().LoadModuleChecked<FClassViewerModule>("ClassViewer")
                .CreateClassViewer(
                   []()->FClassViewerInitializationOptions
                    {
                       FClassViewerInitializationOptions Options;
                       Options.Mode = EClassViewerMode::ClassPicker;
                       Options.DisplayMode = EClassViewerDisplayMode::ListView;
                       Options.bShowNoneOption = true;
                       Options.bIsActorsOnly = true; // Only allow classes that are children of AActor
                       return Options;
                    }(),
                    FOnClassPicked::CreateSP(this, &SAGPSPanel::OnGuardClassPicked)
                )
            ]
        ]
        
        // Guard placement preview button
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(4)
        [
            SNew(SButton)
            .Text(FText::FromString("Preview guards placement"))
            // On click: instantiate the selected algorithm, compute guard spawn settings,
            // call debug/preview helpers to visualize in the editor.
            .OnClicked_Lambda([this]()
            {
                if (!SelectedNavMeshEntry.IsValid() || !SelectedNavMeshEntry->NavMesh.IsValid())
                {
                    FNotificationInfo Info(FText::FromString("Please select a NavMesh before previewing."));
                    Info.ExpireDuration = 5.0f;
                    Info.bUseSuccessFailIcons = true;
                    TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(Info);
                    if (Notification.IsValid())
                    {
                        Notification->SetCompletionState(SNotificationItem::CS_Fail);
                    }
                    return FReply::Handled();
                } else if(!SelectedAlgo.IsValid())
                {
                    FNotificationInfo Info(FText::FromString("Please select an algorithm before previewing."));
                    Info.ExpireDuration = 5.0f;
                    Info.bUseSuccessFailIcons = true;
                    TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(Info);
                    if (Notification.IsValid())
                    {
                        Notification->SetCompletionState(SNotificationItem::CS_Fail);
                    }
                    return FReply::Handled();
                } else {
                    UClass* AlgoClass = SelectedAlgo.Get();
                    // Create algorithm instance non-transactionally in transient package
                    UGuardPlacementAlgoBase* Algo = NewObject<UGuardPlacementAlgoBase>(GetTransientPackage(), AlgoClass);

                    if (Algo)
                    {
                        // Compute spawn settings from selected navmesh and preview them
                        GuardSpawnSettingsComputed = Algo->ComputeGuardLocations(SelectedNavMeshEntry->NavMesh.Get());
                        if (GuardSpawnSettingsComputed.Num() <= 0)
                        {
                            FNotificationInfo Info(FText::FromString("Failed to compute guard locations"));
                            Info.ExpireDuration = 5.0f;
                            Info.bUseSuccessFailIcons = true;
                            TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(Info);
                            if (Notification.IsValid())
                            {
                                Notification->SetCompletionState(SNotificationItem::CS_Fail);
                            }
                            return FReply::Handled();
                        }
                        Algo->DebugView();
                        Algo->PreviewGuardAtLocation(GetEditorWorld(), &GuardSpawnSettingsComputed);
                        FNotificationInfo Info(FText::FromString("Preview guard locations have been placed in the editor"));
                        Info.ExpireDuration = 5.0f;
                        Info.bUseSuccessFailIcons = true;
                        TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(Info);
                        if (Notification.IsValid())
                        {
                            Notification->SetCompletionState(SNotificationItem::CS_Success);
                        }
                    }
                }
                return FReply::Handled();
            })
        ]

        // Guard placement button
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(4)
        [
            SNew(SButton)
            .Text(FText::FromString("Place guards in the level"))
            // On click: instantiate the algorithm (if needed compute guard spawn settings),
            // then call PlaceGuardAtLocation to spawn actors in the editor world.
            .OnClicked_Lambda([this]()
            {
                if (!SelectedNavMeshEntry.IsValid() || !SelectedNavMeshEntry->NavMesh.IsValid())
                {
                    FNotificationInfo Info(FText::FromString("Please select a NavMesh before previewing."));
                    Info.ExpireDuration = 5.0f;
                    Info.bUseSuccessFailIcons = true;
                    TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(Info);
                    if (Notification.IsValid())
                    {
                        Notification->SetCompletionState(SNotificationItem::CS_Fail);
                    }
                    return FReply::Handled();
                } else if(!SelectedAlgo.IsValid())
                {
                    FNotificationInfo Info(FText::FromString("Please select an algorithm before previewing."));
                    Info.ExpireDuration = 5.0f;
                    Info.bUseSuccessFailIcons = true;
                    TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(Info);
                    if (Notification.IsValid())
                    {
                        Notification->SetCompletionState(SNotificationItem::CS_Fail);
                    }
                    return FReply::Handled();
                } else if (!SelectedGuardActorClass || !SelectedGuardActorClass->IsChildOf(AActor::StaticClass()))
                {
                    FNotificationInfo Info(FText::FromString("Please select a valid actor class."));
                    Info.ExpireDuration = 5.0f;
                    Info.bUseSuccessFailIcons = true;
                    TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(Info);
                    if (Notification.IsValid())
                    {
                        Notification->SetCompletionState(SNotificationItem::CS_Fail);
                    }
                    return FReply::Handled();
                } else {
                    UClass* AlgoClass = SelectedAlgo.Get();

                    UGuardPlacementAlgoBase* Algo = NewObject<UGuardPlacementAlgoBase>(GetTransientPackage(), AlgoClass);

                    if (Algo)
                    {
                        if (GuardSpawnSettingsComputed.Num()<=0)
                        {
                            GuardSpawnSettingsComputed = Algo->ComputeGuardLocations(SelectedNavMeshEntry->NavMesh.Get());
                        }
                        
                        if (GuardSpawnSettingsComputed.Num() <= 0)
                        {
                            FNotificationInfo Info(FText::FromString("Failed to compute guard locations"));
                            Info.ExpireDuration = 5.0f;
                            Info.bUseSuccessFailIcons = true;
                            TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(Info);
                            if (Notification.IsValid())
                            {
                                Notification->SetCompletionState(SNotificationItem::CS_Fail);
                            }
                            return FReply::Handled();
                        }
                        
                        Algo->PlaceGuardAtLocation(GetEditorWorld(), &GuardSpawnSettingsComputed, SelectedGuardActorClass);
                        FNotificationInfo Info(FText::FromString("Guard have been placed in the editor"));
                        Info.ExpireDuration = 5.0f;
                        Info.bUseSuccessFailIcons = true;
                        TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(Info);
                        if (Notification.IsValid())
                        {
                            Notification->SetCompletionState(SNotificationItem::CS_Success);
                        }
                    }
                }
                return FReply::Handled();
            })
        ]
    ];
}

/**
 * @brief Return the editor world context's UWorld.
 *
 * Uses GEditor to get the EditorWorldContext. May return nullptr if the editor
 * doesn't have a valid world (for example during shutdown).
 *
 * @return UWorld* pointer to the editor world, or nullptr if none.
 */
UWorld* SAGPSPanel::GetEditorWorld()
{
    return GEditor
        ? GEditor->GetEditorWorldContext().World()
        : nullptr;
}

/**
 * @brief Scan the editor world for ARecastNavMesh actors and populate NavMeshEntries.
 *
 * Clears current entries and selection, iterates over actors in the editor world,
 * constructs a display name from the navmesh config and stores weak references.
 * Refreshes the NavMeshComboBox UI if valid.
 */
void SAGPSPanel::RefreshNavMeshes()
{
    NavMeshEntries.Empty();
    SelectedNavMeshEntry.Reset();
    
    UWorld* World = GetEditorWorld();
    if (!World)
    {
        return;
    }
    
    for (TActorIterator<ARecastNavMesh> It(World); It; ++It)
    {
        //ARecastNavMesh* Recast = *It;
        if (ARecastNavMesh* RecastNavMesh = Cast<ARecastNavMesh>(*It))  
        {
            const FString DisplayName = FString::Printf(TEXT("%s"), *RecastNavMesh->GetConfig().Name.ToString());
            FAGPSNavMeshEntry NewEntry = { RecastNavMesh, DisplayName };
            NavMeshEntries.Add(MakeShared<FAGPSNavMeshEntry>(NewEntry));
        }
    }
    
    if (NavMeshComboBox.IsValid())
    {
        NavMeshComboBox->RefreshOptions();
    }
}

/**
 * @brief Build a descriptive text for the selected navmesh.
 *
 * Includes bounding box center/extent and nav config description.
 * If no navmesh is selected or the pointer is invalid, returns "Selected NavMesh: None".
 *
 * @return FText description for UI display.
 */
FText SAGPSPanel::GetSelectedNavMeshText() const
{
    if (SelectedNavMeshEntry.IsValid() && SelectedNavMeshEntry->NavMesh.IsValid())
    {
        ARecastNavMesh* NavMesh = SelectedNavMeshEntry->NavMesh.Get();

        // Bounds (centre et taille)
        FBox Bounds = NavMesh->GetComponentsBoundingBox(true);
        const FVector Center = Bounds.GetCenter();
        const FVector Extent = Bounds.GetExtent();
        
        FString Info = FString::Printf(
            TEXT("Selected NavMesh: %s\nCenter: %.1f, %.1f, %.1f\nExtent: %.1f, %.1f, %.1f\n"),
            *NavMesh->GetName(),
            Center.X, Center.Y, Center.Z,
            Extent.X, Extent.Y, Extent.Z
        );
        
        // Config général (tile size, cell size, agent radius, ...)
        const FNavDataConfig& NavConfig = NavMesh->GetConfig();
        Info += NavConfig.GetDescription(); // + FString::Printf(TEXT("\n"))

        return FText::FromString(Info);
    }

    return FText::FromString("Selected NavMesh: None");
}

/**
 * @brief Find and list UGuardPlacementAlgoBase-derived classes for the algorithm selector.
 *
 * Uses GetDerivedClasses to discover implementations (recursive), filters out abstract
 * classes, wraps them into shared pointers for the combo box, sorts them alphabetically,
 * and refreshes the AlgoComboBox.
 */
void SAGPSPanel::RefreshAlgorithmClasses()
{
    AlgoEntries.Empty();
    SelectedAlgo.Reset();

    TArray<UClass*> DerivedClasses;
    
    // Populate DerivedClasses with all subclasses of UGuardPlacementAlgoBase
    GetDerivedClasses(
        UGuardPlacementAlgoBase::StaticClass(),
        DerivedClasses,
        true // recursive
    );

    for (UClass* Class : DerivedClasses)
    {
        if (!Class)
        {
            continue;
        }
        
        // Ignore abstract classes that cannot be instantiated
        if (Class->HasAnyClassFlags(CLASS_Abstract))
        {
            continue;
        }

        // MakeShareable wrapper; the deleter is a no-op because UClass is GC-managed
        AlgoEntries.Add(MakeShareable(Class, [](UClass*){}));
    }
    
    // Sort entries by class name for predictable ordering
    AlgoEntries.Sort([](const TSharedPtr<UClass>& A, const TSharedPtr<UClass>& B)
    {
        return A->GetName() < B->GetName();
    });

    if (AlgoComboBox.IsValid())
    {
        AlgoComboBox->RefreshOptions();
    }
}

/**
 * @brief Handler called when a class is picked in the ClassViewer.
 *
 * Validates that the chosen class is a subclass of AActor and stores it as the
 * SelectedGuardActorClass. If invalid, clears the selection.
 *
 * @param ChosenClass The picked UClass pointer (may be nullptr).
 */
void SAGPSPanel::OnGuardClassPicked(UClass* ChosenClass)
{
    if (ChosenClass && ChosenClass->IsChildOf(AActor::StaticClass()))
    {
        SelectedGuardActorClass = ChosenClass;
    }
    else
    {
        SelectedGuardActorClass = nullptr;
    }
}
