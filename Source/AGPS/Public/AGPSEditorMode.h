#pragma once

#include "AGPSPanel.h"
#include "EdMode.h"

class SAGSPanel;

/**
 * @brief AGPS editor mode.
 *
 * Custom editor mode that exposes AGPS tools to the editor. Responsible for
 * mode activation/deactivation and for creating the toolkit widget displayed
 * in the editor when the mode is active.
 */
class FAGPSEditorMode : public FEdMode
{
public:
    /** Mode identifier used to register / reference this editor mode. */
    static const FEditorModeID EM_AGPS;

    /** Default constructor. */
    FAGPSEditorMode();

    /** Destructor. */
    virtual ~FAGPSEditorMode() override;

    /**
     * @brief Called when the mode is entered.
     *
     * Initialize mode-specific state, register any input handlers or callbacks,
     * and create UI/tooling needed while the mode is active.
     */
    virtual void Enter() override;

    /**
     * @brief Called when the mode is exited.
     *
     * Clean up state and unregister callbacks or handlers registered in Enter().
     */
    virtual void Exit() override;

    /**
     * @brief Create the toolkit widget for this mode.
     *
     * Builds and returns the widget that will be presented in the editor's
     * toolkit area for this mode.
     *
     * @return A shared reference to the toolkit widget (SWidget).
     */
    virtual TSharedRef<SWidget> CreateToolkitWidget();

private:
    /** Panel widget instance used by the toolkit (AGPS UI panel). */
    TSharedPtr<SAGPSPanel> PanelWidget;
};


// class FAGPSEditorMode : public FEdMode
// {
// public:
// 	static const FEditorModeID EM_AGPS;
//
// 	FAGPSEditorMode();
// 	virtual ~FAGPSEditorMode() override;
//
// 	virtual void Enter() override;
// 	virtual void Exit() override;
//
// 	virtual TSharedRef<SWidget> CreateToolkitWidget();
//
// private:
// 	TSharedPtr<SAGPSPanel> PanelWidget;
//
// };