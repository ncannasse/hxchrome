#include <Windows.h>
#include "include/cef_app.h"

const char *start() {
	CefSettings settings;
	CefRefPtr<CefApp> app = NULL;
	if( !CefInitialize(settings,app) )
		return "Failed to init CEF";
	return NULL;
}

int CALLBACK WinMain(
  _In_  HINSTANCE hInstance,
  _In_  HINSTANCE hPrevInstance,
  _In_  LPSTR lpCmdLine,
  _In_  int nCmdShow
) {
	const char *err = start();
	if( err != NULL )
		MessageBoxA(NULL,err,"Error",MB_ICONERROR);
	return 0;
}