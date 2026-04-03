#include "AGPSEditorMode.h"

#include "AGPSPanel.h"
#include "AGPSEditorMode.h"
#include "AGPSEditorModeToolkit.h"
#include "EditorModeManager.h"

#include "Widgets/SBoxPanel.h"

const FEditorModeID FAGPSEditorMode::EM_AGPS = TEXT("EM_AGPS");

FAGPSEditorMode::FAGPSEditorMode()
{
}

FAGPSEditorMode::~FAGPSEditorMode()
{
}


void FAGPSEditorMode::Enter()
{
	FEdMode::Enter();

	if (!Toolkit.IsValid())
	{
		Toolkit = MakeShared<FAGPSEditorModeToolkit>();
		Toolkit->Init(Owner->GetToolkitHost());
	}
}

void FAGPSEditorMode::Exit()
{
	Toolkit.Reset();
	FEdMode::Exit();
}


TSharedRef<SWidget> FAGPSEditorMode::CreateToolkitWidget()
{
	PanelWidget = SNew(SAGPSPanel);
	return PanelWidget.ToSharedRef();
}
