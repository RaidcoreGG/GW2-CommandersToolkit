#ifndef NEXUS_H
#define NEXUS_H

#include <Windows.h>
#include <d3d11.h>
#include <mutex>

#include "imgui/imgui.h"

#ifndef __cplusplus 
#include <stdbool.h>
#endif

// Your addon must use the same IMGUI Version 1.80
#define IMGUI_VERSION_NUM 18000
#define NEXUS_API_VERSION 1

typedef enum ERenderType
{
	ERenderType_PreRender,
	ERenderType_Render,
	ERenderType_PostRender,
	ERenderType_OptionsRender
} ERenderType;

typedef void (*GUI_RENDER) ();
typedef void (*GUI_ADDRENDER) (ERenderType aRenderType, GUI_RENDER aRenderCallback);
typedef void (*GUI_REMRENDER) (GUI_RENDER aRenderCallback);

typedef const char* (*PATHS_GETGAMEDIR)();
typedef const char* (*PATHS_GETADDONDIR)(const char* aName);
typedef const char* (*PATHS_GETCOMMONDIR)();

typedef enum EMHStatus
{
	MH_UNKNOWN = -1,
	MH_OK = 0,
	MH_ERROR_ALREADY_INITIALIZED,
	MH_ERROR_NOT_INITIALIZED,
	MH_ERROR_ALREADY_CREATED,
	MH_ERROR_NOT_CREATED,
	MH_ERROR_ENABLED,
	MH_ERROR_DISABLED,
	MH_ERROR_NOT_EXECUTABLE,
	MH_ERROR_UNSUPPORTED_FUNCTION,
	MH_ERROR_MEMORY_ALLOC,
	MH_ERROR_MEMORY_PROTECT,
	MH_ERROR_MODULE_NOT_FOUND,
	MH_ERROR_FUNCTION_NOT_FOUND
} EMHStatus;

typedef EMHStatus (__stdcall* MINHOOK_CREATE)(LPVOID pTarget, LPVOID pDetour, LPVOID* ppOriginal);
typedef EMHStatus (__stdcall* MINHOOK_REMOVE)(LPVOID pTarget);
typedef EMHStatus (__stdcall* MINHOOK_ENABLE)(LPVOID pTarget);
typedef EMHStatus (__stdcall* MINHOOK_DISABLE)(LPVOID pTarget);

typedef enum ELogLevel
{
	ELogLevel_OFF         = 0,
	ELogLevel_CRITICAL    = 1,
	ELogLevel_WARNING     = 2,
	ELogLevel_INFO        = 3,
	ELogLevel_DEBUG       = 4,
	ELogLevel_TRACE       = 5,
	ELogLevel_ALL
} ELogLevel;

typedef void (*LOGGER_LOGA)(ELogLevel aLogLevel, const char* aStr);

typedef void (*EVENT_CONSUME)(void* aEventArgs);
typedef void (*EVENTS_RAISE)(const char* aIdentifier, void* aEventData);
typedef void (*EVENTS_SUBSCRIBE)(const char* aIdentifier, EVENT_CONSUME aConsumeEventCallback);

typedef UINT (*WNDPROC_CALLBACK)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
typedef void (*WNDPROC_ADDREM)(WNDPROC_CALLBACK aWndProcCallback);

typedef struct Keybind
{
	unsigned short	Key;
	bool			Alt;
	bool			Ctrl;
	bool			Shift;
} Keybind;

typedef void (*KEYBINDS_PROCESS)(const char* aIdentifier);
typedef void (*KEYBINDS_REGISTERWITHSTRING)(const char* aIdentifier, KEYBINDS_PROCESS aKeybindHandler, const char* aKeybind);
typedef void (*KEYBINDS_REGISTERWITHSTRUCT)(const char* aIdentifier, KEYBINDS_PROCESS aKeybindHandler, Keybind aKeybind);
typedef void (*KEYBINDS_UNREGISTER)(const char* aIdentifier);

typedef void* (*DATALINK_GETRESOURCE)(const char* aIdentifier);
typedef void* (*DATALINK_SHARERESOURCE)(const char* aIdentifier, size_t aResourceSize);

typedef struct Texture
{
	unsigned Width;
	unsigned Height;
	ID3D11ShaderResourceView* Resource;
} Texture;

typedef void (*TEXTURES_RECEIVECALLBACK)(const char* aIdentifier, Texture* aTexture);
typedef Texture* (*TEXTURES_GET)(const char* aIdentifier);
typedef void (*TEXTURES_LOADFROMFILE)(const char* aIdentifier, const char* aFilename, TEXTURES_RECEIVECALLBACK aCallback);
typedef void (*TEXTURES_LOADFROMRESOURCE)(const char* aIdentifier, unsigned aResourceID, HMODULE aModule, TEXTURES_RECEIVECALLBACK aCallback);
typedef void (*TEXTURES_LOADFROMURL)(const char* aIdentifier, const char* aRemote, const char* aEndpoint, TEXTURES_RECEIVECALLBACK aCallback);

typedef void (*QUICKACCESS_ADDSHORTCUT) (const char* aIdentifier, const char* aTextureIdentifier, const char* aTextureHoverIdentifier, const char* aKeybindIdentifier, const char* aTooltipText);
typedef void (*QUICKACCESS_ADDSIMPLE) (const char* aIdentifier, GUI_RENDER aShortcutRenderCallback);
typedef void (*QUICKACCESS_REMOVE) (const char* aIdentifier);

typedef struct NexusLinkData
{
	unsigned	Width;
	unsigned	Height;
	float		Scaling;

	bool		IsMoving;
	bool		IsCameraMoving;
	bool		IsGameplay;

	ImFont*		Font;
	ImFont*		FontBig;
	ImFont*		FontUI;
} NexusLinkData;

// Revision 1
typedef struct AddonAPI
{
	/* Renderer */
	IDXGISwapChain*				SwapChain;
	ImGuiContext*				ImguiContext;
	void*						ImguiMalloc;
	void*						ImguiFree;
	GUI_ADDRENDER				RegisterRender;
	GUI_REMRENDER				UnregisterRender;

	/* Paths */
	PATHS_GETGAMEDIR			GetGameDirectory;
	PATHS_GETADDONDIR			GetAddonDirectory;
	PATHS_GETCOMMONDIR			GetCommonDirectory;

	/* Minhook */
	MINHOOK_CREATE				CreateHook;
	MINHOOK_REMOVE				RemoveHook;
	MINHOOK_ENABLE				EnableHook;
	MINHOOK_DISABLE				DisableHook;

	/* Logging */
	LOGGER_LOGA					Log;

	/* Events */
	EVENTS_RAISE				RaiseEvent;
	EVENTS_SUBSCRIBE			SubscribeEvent;
	EVENTS_SUBSCRIBE			UnsubscribeEvent;

	/* WndProc */
	WNDPROC_ADDREM				RegisterWndProc;
	WNDPROC_ADDREM				UnregisterWndProc;

	/* Keybinds */
	KEYBINDS_REGISTERWITHSTRING	RegisterKeybindWithString;
	KEYBINDS_REGISTERWITHSTRUCT	RegisterKeybindWithStruct;
	KEYBINDS_UNREGISTER			UnregisterKeybind;

	/* DataLink */
	DATALINK_GETRESOURCE		GetResource;
	DATALINK_SHARERESOURCE		ShareResource;

	/* Textures */
	TEXTURES_GET				GetTexture;
	TEXTURES_LOADFROMFILE		LoadTextureFromFile;
	TEXTURES_LOADFROMRESOURCE	LoadTextureFromResource;
	TEXTURES_LOADFROMURL		LoadTextureFromURL;

	/* Shortcuts */
	QUICKACCESS_ADDSHORTCUT		AddShortcut;
	QUICKACCESS_REMOVE			RemoveShortcut;
	QUICKACCESS_ADDSIMPLE		AddSimpleShortcut;
	QUICKACCESS_REMOVE			RemoveSimpleShortcut;
} AddonAPI;

typedef void (*ADDON_LOAD) (AddonAPI* aAPI);
typedef void (*ADDON_UNLOAD) ();

typedef struct AddonVersion
{
	signed short	Major;
	signed short	Minor;
	signed short	Build;
	signed short	Revision;
} AddonVersion;

typedef enum EAddonFlags
{
	EAddonFlags_None = 0,
	EAddonFlags_IsVolatile = 1,				/* is hooking functions or doing anything else that's volatile and game build dependant */
	EAddonFlags_DisableHotloading = 2		/* prevents unloading at runtime, aka. will require a restart if updated, etc. */
} EAddonFlags;

typedef enum EUpdateProvider
{
	EUpdateProvider_None		= 0,	/* Does not support auto updating */
	EUpdateProvider_Raidcore	= 1,	/* Provider is Raidcore (via API) */
	EUpdateProvider_GitHub		= 2,	/* Provider is GitHub Releases */
	EUpdateProvider_Direct		= 3		/* Provider is direct file link */
} EUpdateProvider;

typedef struct AddonDefinition
{
	/* required */
	signed int      Signature;      /* Raidcore Addon ID, set to random unqiue negative integer if not on Raidcore */
	signed int		APIVersion;		/* Determines which AddonAPI struct revision the Loader will pass, use the NEXUS_API_VERSION define from Nexus.h */
	const char*     Name;           /* Name of the addon as shown in the library */
	AddonVersion	Version;
	const char*     Author;         /* Author of the addon */
	const char*     Description;    /* Short description */
	ADDON_LOAD      Load;           /* Pointer to Load Function of the addon */
	ADDON_UNLOAD    Unload;         /* Pointer to Unload Function of the addon. Not required if EAddonFlags::DisableHotloading is set. */
	EAddonFlags     Flags;          /* Information about the addon */

	/* update fallback */
	EUpdateProvider Provider;       /* What platform is the the addon hosted on */
	const char*     UpdateLink;     /* Link to the update resource */
} AddonDefinition;

#endif