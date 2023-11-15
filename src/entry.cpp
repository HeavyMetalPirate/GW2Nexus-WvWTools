#include <Windows.h>
#include <string>
#include <cmath>
#include <vector>

#include "nexus/Nexus.h"
#include "imgui/imgui.h"
#include "imgui/imgui_extensions.h"

/* proto/globals */
void AddonLoad(AddonAPI* aApi);
void AddonUnload();
void AddonRender();
void AddonOptions();

HMODULE hSelf;
AddonAPI* APIDefs;
AddonDefinition* AddonDef;
NexusLinkData* NexusLink;

bool someSetting = false;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH: hSelf = hModule; break;
		case DLL_PROCESS_DETACH: break;
		case DLL_THREAD_ATTACH: break;
		case DLL_THREAD_DETACH: break;
	}
	return TRUE;
}

extern "C" __declspec(dllexport) AddonDefinition * GetAddonDef()
{
	AddonDef = new AddonDefinition();
	AddonDef->Signature = -12345; // set to random unused negative integer
	AddonDef->APIVersion = NEXUS_API_VERSION;
	AddonDef->Name = "My First Nexus Addon";
	AddonDef->Version.Major = 1;
	AddonDef->Version.Minor = 0;
	AddonDef->Version.Build = 0;
	AddonDef->Version.Revision = 1;
	AddonDef->Author = "Me, Myself and I";
	AddonDef->Description = "This is my first Nexus addon.";
	AddonDef->Load = AddonLoad;
	AddonDef->Unload = AddonUnload;
	AddonDef->Flags = EAddonFlags_None;

	/* not necessary if hosted on Raidcore, but shown anyway for the example also useful as a backup resource */
	//AddonDef->Provider = EUpdateProvider_GitHub;
	//AddonDef->UpdateLink = "https://github.com/RaidcoreGG/GW2Nexus-AddonTemplate";

	return AddonDef;
}

void AddonLoad(AddonAPI* aApi)
{
	APIDefs = aApi; // store the api somewhere easily accessible

	ImGui::SetCurrentContext(APIDefs->ImguiContext);
	ImGui::SetAllocatorFunctions((void* (*)(size_t, void*))APIDefs->ImguiMalloc, (void(*)(void*, void*))APIDefs->ImguiFree); // on imgui 1.80+

	NexusLink = (NexusLinkData*)APIDefs->GetResource("DL_NEXUS_LINK");

	// Add an options window and a regular render callback
	APIDefs->RegisterRender(ERenderType_Render, AddonRender);
	APIDefs->RegisterRender(ERenderType_OptionsRender, AddonOptions);

	APIDefs->Log(ELogLevel_DEBUG, "My first addon was loaded.");
}

void AddonUnload()
{
	/* let's clean up after ourselves */
	APIDefs->UnregisterRender(AddonRender);
	APIDefs->UnregisterRender(AddonOptions);
}

void AddonRender()
{
	ImGuiIO& io = ImGui::GetIO();

	if (ImGui::Begin("MyFirstImGuiWindow"))
	{
		ImGui::Text("Hello Tyria!");
	}
	ImGui::End();
}

void AddonOptions()
{
	ImGui::Separator();
	ImGui::TextDisabled("My first Nexus addon");
	ImGui::Checkbox("Some setting", &someSetting);
}