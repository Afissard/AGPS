#include "AGPSModule.h"
#include "AGPSEditorMode.h"
#include "EditorModeRegistry.h"

#define LOCTEXT_NAMESPACE "AGPS"

void FAGPSModule::StartupModule()
{
	FEditorModeRegistry::Get().RegisterMode<FAGPSEditorMode>(
		FAGPSEditorMode::EM_AGPS,
		LOCTEXT("AGPSModeName", "AGPS"),
		FSlateIcon(),
		true
	);
}

void FAGPSModule::ShutdownModule()
{
	FEditorModeRegistry::Get().UnregisterMode(
		FAGPSEditorMode::EM_AGPS
	);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAGPSModule, AGPS)
