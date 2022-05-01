// dllmain.cpp : Defines the entry point for the DLL application.
#pragma comment(lib, "winmm.lib")
#include <stdint.h>
#include <stdio.h>
#include <Windows.h>
#include <string>
#include "arcdps_structs.h"
#include "imgui\imgui.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
		case DLL_PROCESS_ATTACH: break;
		case DLL_PROCESS_DETACH: break;
		case DLL_THREAD_ATTACH: break;
		case DLL_THREAD_DETACH: break;
    }
    return TRUE;
}

/* proto */
extern "C" __declspec(dllexport) void* get_init_addr(char* arcversion, ImGuiContext* imguictx, void* id3dptr, HANDLE arcdll, void* mallocfn, void* freefn, uint32_t d3dversion);
extern "C" __declspec(dllexport) void* get_release_addr();

arcdps_exports* mod_init();
uintptr_t mod_release();
uintptr_t mod_imgui(uint32_t not_charsel_or_loading);
uintptr_t mod_options_windows(const char* windowname); // Windows check boxes in main menu
uintptr_t mod_combat(cbtevent* ev, ag* src, ag* dst, char* skillname, uint64_t id, uint64_t revision);

void log(char* str);
void log_file(char* str);
void log_arc(char* str);

/* globals */
arcdps_exports arc_exports;
void* filelog;
void* arclog;

bool show_squadmanager = false;

void log(char* str) /* log to arcdps.log and log window*/
{
	log_file(str);
	log_arc(str);
	return;
}
void log_file(char* str) /* log to arcdps.log, thread/async safe */
{
	size_t(*log)(char*) = (size_t(*)(char*))filelog;
	if (log) (*log)(str);
	return;
}
void log_arc(char* str) /* log to extensions tab in arcdps log window, thread/async safe */
{
	size_t(*log)(char*) = (size_t(*)(char*))arclog;
	if (log) (*log)(str);
	return;
}

/* export -- arcdps looks for this exported function and calls the address it returns on client load */
extern "C" __declspec(dllexport) void* get_init_addr(char* arcversion, ImGuiContext* imguictx, void* id3dptr, HANDLE arcdll, void* mallocfn, void* freefn, uint32_t d3dversion)
{
	ImGui::SetCurrentContext((ImGuiContext*)imguictx);
	ImGui::SetAllocatorFunctions((void *(*)(size_t, void*))mallocfn, (void(*)(void*, void*))freefn); // on imgui 1.80+

	filelog = (void*)GetProcAddress((HMODULE)arcdll, "e3");
	arclog = (void*)GetProcAddress((HMODULE)arcdll, "e8");

	return mod_init;
}
/* export -- arcdps looks for this exported function and calls the address it returns on client exit */
extern "C" __declspec(dllexport) void* get_release_addr()
{
	return mod_release;
}

/* initialize mod -- return table that arcdps will use for callbacks. exports struct and strings are copied to arcdps memory only once at init */
arcdps_exports* mod_init()
{
	/* for arcdps */
	memset(&arc_exports, 0, sizeof(arc_exports));
	arc_exports.sig = 0x4A584326;
	arc_exports.imguivers = IMGUI_VERSION_NUM;
	arc_exports.size = sizeof(arc_exports);
	arc_exports.out_name = "Commander's Toolkit";
	arc_exports.out_build = __DATE__ " " __TIME__;
	arc_exports.imgui = mod_imgui;
	arc_exports.options_windows = mod_options_windows;
	arc_exports.combat = mod_combat;

	return &arc_exports;
}
/* release mod -- return ignored */
uintptr_t mod_release()
{
	FreeConsole();
	return 0;
}

uintptr_t UISquadManager()
{
	ImGui::Begin("Squad Manager", &show_squadmanager);

	ImGui::End();

	return 0;
}

uintptr_t mod_imgui(uint32_t not_charsel_or_loading)
{
	if (show_squadmanager) { UISquadManager(); }

	return 0;
}

uintptr_t mod_options_windows(const char* windowname)
{
	if (windowname)
	{
		if (strcmp(windowname, "squad") == 0)
		{
			ImGui::Checkbox("Squad Manager", &show_squadmanager);
		}
	}

	return 0;
}

uint32_t cbtcount = 0;
/* combat callback -- may be called asynchronously, use id param to keep track of order, first event id will be 2. return ignored */
/* at least one participant will be party/squad or minion of, or a buff applied by squad in the case of buff remove. not all statechanges present, see evtc statechange enum */
uintptr_t mod_combat(cbtevent* ev, ag* src, ag* dst, char* skillname, uint64_t id, uint64_t revision)
{
	/*if (!ev) {
		// notify tracking change
		if (!src->elite) {
			// only run, when names are set and not null
			if (src->name != nullptr && src->name[0] != '\0' && dst->name != nullptr && dst->name[0] != '\0') {

				std::string username(dst->name);

				// remove ':' at the beginning of the name.
				if (username.at(0) == ':') {
					username.erase(0, 1);
				}

				// add
				if (src->prof) {
					log_arc(src->name);
					log_arc((char*)username.c_str());
				}
			}
		}
	}*/

	/* much lazy */
	char buff[4096];
	char* p = &buff[0];

	/* ev is null. dst will only be valid on tracking add. skillname will also be null */
	if (!ev)
	{
		/* notify tracking change */
		if (!src->elite)
		{
			/* add */
			if (src->prof)
			{
				p += _snprintf_s(p, 400, _TRUNCATE, "==== cbtnotify ====\n");
				p += _snprintf_s(p, 400, _TRUNCATE, "agent added: %s:%s (%0llx), instid: %u, prof: %u, elite: %u, self: %u, team: %u, subgroup: %u\n", src->name, dst->name, src->id, dst->id, dst->prof, dst->elite, dst->self, src->team, dst->team);
			}
			else /* remove */
			{
				p += _snprintf_s(p, 400, _TRUNCATE, "==== cbtnotify ====\n");
				p += _snprintf_s(p, 400, _TRUNCATE, "agent removed: %s (%0llx)\n", src->name, src->id);
			}

			log_arc(&buff[0]);
		}
	}
	return 0;
}