/**
* Orion2 - A MapleStory2 Dynamic Link Library Localhost
*
* @author Eric
*
*/
#include "OrionHacks.h"
#include "WinSockHook.h"
#include "NMCOHook.h"

void MapleStory2_Bypass(void) {
	int nAttempt = 1;
	while (true) {
		bool bInit = InitializeOrion2();
		if (bInit) {
			Log("Successfully initialized Orion2 after %d attempt(s)!", nAttempt);
			break;
		}
		++nAttempt;
	}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
	/* Initial injection from process attach. */
	if (dwReason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(hModule);

		/* Update the locale to properly decode foreign StringPools. */
		setlocale(LC_ALL, STRING_LOCALE);

#if MULTI_CLIENT
		/* Hook CreateMutexA and enable the use of Multi-Client for MS2. */
		if (!Hook_CreateMutexA(true)) {
			NotifyMessage("Failed to hook CreateMutexA", Orion::NotifyType::Error);
			return FALSE;
		}
#endif

		/* Redirect the client process if necessary. */
		if (!RedirectProcess()) {
			NotifyMessage("Failed to redirect process", Orion::NotifyType::Error);
			return FALSE;
		}

		//InitUnhandledExceptionFilter();		
		InitVectoredExceptionHandler();

#if DEBUG_MODE == 2
		NotifyMessage("Injection successful - haulting process for memory alterations.", Orion::NotifyType::Information);
#endif

#if DEBUG_MODE
		AllocConsole();
		SetConsoleTitleA(CLIENT_NAME);
		AttachConsole(GetCurrentProcessId());
		freopen("CON", "w", stdout);
#endif

		/* Initiate the winsock hook for socket spoofing and redirection. */
		if (!Hook_WSPStartup(true)) {
			NotifyMessage("Failed to hook WSPStartup", Orion::NotifyType::Error);
			return FALSE;
		}

		/* Initiate the NMCO hook to fix our login passport handling. */
		if (!NMCOHook_Init()) {
			NotifyMessage("Failed to hook CallNMFunc", Orion::NotifyType::Error);
			return FALSE;
		}

		/* Initiate the CreateWindowExA hook to customize the main window. */
		if (!Hook_CreateWindowExA(true)) {
			NotifyMessage("Failed to hook CreateWindowExA", Orion::NotifyType::Error);
			return FALSE;
		}

#if DEBUG_MODE == 2
		/* Hook GetCurrentDirectoryA and begin to rape the client. */
		if (!Hook_GetCurrentDirectoryA(true)) {
			NotifyMessage("Failed to hook GetCurrentDirectoryA", Orion::NotifyType::Error);
			return FALSE;
		}
#endif

		Log("Initializing the Orion2 Dynamic Link Library Localhost...\r\nRuntime: %s", GetCommandLine());
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MapleStory2_Bypass, 0, 0, 0);
	} else if (dwReason == DLL_PROCESS_DETACH) {
#if DEBUG_MODE
		FreeConsole();
#endif
	}

	return TRUE;
}
