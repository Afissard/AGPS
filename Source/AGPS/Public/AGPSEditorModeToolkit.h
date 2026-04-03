#pragma once

#include "Toolkits/BaseToolkit.h"

/**
 * @brief Toolkit for the AGPS editor mode.
 *
 * Provides the UI content and toolkit integration for the AGPS editor mode.
 * The toolkit builds and exposes the inline widget used in the editor tool panel.
 */
class FAGPSEditorModeToolkit : public FModeToolkit
{
public:
    /**
     * @brief Initialize the toolkit.
     *
     * Build and initialize the toolkit UI and bindings. The InitToolkitHost parameter
     * is provided by the editor and should be passed to parent toolkit initialization if needed.
     *
     * @param InitToolkitHost Host supplied by the editor for toolkit initialization.
     */
    virtual void Init(const TSharedPtr<IToolkitHost>& InitToolkitHost) override;

    /**
     * @brief Returns the toolkit's internal name.
     * @return Identifier name used by the editor for this toolkit.
     */
    virtual FName GetToolkitFName() const override { return FName("AGPSEditorMode"); }

    /**
     * @brief Returns the display name for the toolkit.
     * @return Human-readable toolkit name shown in the UI.
     */
    virtual FText GetBaseToolkitName() const override { return FText::FromString("AGPS"); }

    /**
     * @brief Return the editor mode associated with this toolkit.
     * @return Pointer to the FEdMode instance that this toolkit is bound to.
     */
    virtual class FEdMode* GetEditorMode() const override;

    /**
     * @brief Return the inline content widget for the toolkit.
     * @return Shared pointer to the widget displayed inside the editor's toolkit area.
     */
    virtual TSharedPtr<SWidget> GetInlineContent() const override;

private:
    /** The main widget built by the toolkit (inline content). */
    TSharedPtr<SWidget> ToolkitWidget;
};


// class FAGPSEditorModeToolkit : public FModeToolkit
// {
// public:
// 	virtual void Init(const TSharedPtr<IToolkitHost>& InitToolkitHost) override;
//
// 	virtual FName GetToolkitFName() const override { return FName("AGPSEditorMode"); }
// 	virtual FText GetBaseToolkitName() const override { return FText::FromString("AGPS"); }
// 	virtual class FEdMode* GetEditorMode() const override;
// 	virtual TSharedPtr<SWidget> GetInlineContent() const override;
//
// private:
// 	TSharedPtr<SWidget> ToolkitWidget;
// };
