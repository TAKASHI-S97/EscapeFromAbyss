#include "Common.h"
#include "App.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	App app(hInstance);

	if (!app.Init())
	{
		OutputDebugFormatString("Error: Failed to initialize this application.");
		return -1;
	}
	
	app.Run();

	return 0;
}