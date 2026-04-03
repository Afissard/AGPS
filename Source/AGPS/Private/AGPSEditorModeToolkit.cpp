#include "AGPSEditorModeToolkit.h"
#include "AGPSEditorMode.h"
#include "AGPSPanel.h"
#include "EditorModeManager.h"

void FAGPSEditorModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost)
{
	ToolkitWidget = SNew(SAGPSPanel);
	FModeToolkit::Init(InitToolkitHost);
}

FEdMode* FAGPSEditorModeToolkit::GetEditorMode() const
{
	return GLevelEditorModeTools().GetActiveMode(FAGPSEditorMode::EM_AGPS);
}

TSharedPtr<SWidget> FAGPSEditorModeToolkit::GetInlineContent() const
{
	return ToolkitWidget;
}	
