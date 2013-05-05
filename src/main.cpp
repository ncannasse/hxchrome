#define _HAS_ITERATOR_DEBUGGING 0
#define _CRT_SECURE_NO_WARNINGS
#undef UNICODE
#include <Windows.h>
#include <sstream>
#include "include/cef_app.h"
#include "include/cef_client.h"

static CefRefPtr<CefBrowser> BROWSER = NULL;

class ClientHandler : public CefClient,
                      public CefLifeSpanHandler,
                      public CefLoadHandler,
                      public CefRequestHandler,
                      public CefDisplayHandler,
                      public CefFocusHandler,
                      public CefKeyboardHandler,
                      public CefPrintHandler,
                      public CefV8ContextHandler,
                      public CefDragHandler,
                      public CefPermissionHandler,
                      public CefGeolocationHandler {

	IMPLEMENT_REFCOUNTING(ClientHandler);

	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE {
		return this;
	}
	virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE {
		return this;
	}
	virtual CefRefPtr<CefRequestHandler> GetRequestHandler() OVERRIDE {
		return this;
	}
	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE {
		return this;
	}
	virtual CefRefPtr<CefFocusHandler> GetFocusHandler() OVERRIDE {
		return this;
	}
	virtual CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() OVERRIDE {
		return this;
	}
	virtual CefRefPtr<CefPrintHandler> GetPrintHandler() OVERRIDE {
		return this;
	}
	virtual CefRefPtr<CefV8ContextHandler> GetV8ContextHandler() OVERRIDE {
		return this;
	}
	virtual CefRefPtr<CefDragHandler> GetDragHandler() OVERRIDE {
		return this;
	}
	virtual CefRefPtr<CefPermissionHandler> GetPermissionHandler() OVERRIDE {
		return this;
	}
	virtual CefRefPtr<CefGeolocationHandler> GetGeolocationHandler() OVERRIDE {
		return this;
	}
	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE {
		BROWSER = browser;
		ShowWindow(wnd,SW_NORMAL);
	}
	virtual bool OnLoadError(CefRefPtr<CefBrowser> browser,CefRefPtr<CefFrame> frame,ErrorCode errorCode, const CefString& failedUrl,CefString& errorText) OVERRIDE {
		if (errorCode == ERR_CACHE_MISS) {
			// Usually caused by navigating to a page with POST data via back or
			// forward buttons.
			errorText = "<html><head><title>Expired Form Data</title></head>"
						"<body><h1>Expired Form Data</h1>"
						"<h2>Your form request has expired. "
						"Click reload to re-submit the form data.</h2></body>"
						"</html>";
		} else {
			// All other messages.
			std::stringstream ss;
			ss <<       "<html><head><title>Load Failed</title></head>"
						"<body><h1>Load Failed</h1>"
						"<h2>Load of URL " << std::string(failedUrl) <<
						" failed with error code " << static_cast<int>(errorCode) <<
						".</h2></body>"
						"</html>";
			errorText = ss.str();
		}
		return false;
	}

	virtual bool OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefRequest> request,
                                     CefString& redirectUrl,
                                     CefRefPtr<CefStreamReader>& resourceStream,
                                     CefRefPtr<CefResponse> response,
                                     int loadFlags) OVERRIDE {
		std::string url = request->GetURL().ToString();
		if( url.substr(0,11) == "http://app/" ) {
			std::string file = basePath.ToString().append(url.substr(10));
			resourceStream = CefStreamReader::CreateForFile(file);
			response->SetMimeType("text/html");
			response->SetStatus(200);
		}
		return false;
	}


public:
	CefWindowHandle wnd;
	CefString basePath;
};

static CefRefPtr<ClientHandler> CLIENT = NULL;

const char *start() {
	CefSettings settings;
	CefRefPtr<CefApp> app = NULL;
	if( !CefInitialize(settings,app) )
		return "Failed to init CEF";
	return NULL;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch( msg ) {
	case WM_CREATE: {
		CLIENT = new ClientHandler();
		CLIENT->wnd = hwnd;

		char cpath[MAX_PATH];
		GetModuleFileName(NULL,cpath,MAX_PATH);
		char *slash = strrchr(cpath,'\\') + 1;
		strcpy(slash,"www");

		DWORD att = GetFileAttributes(cpath);
		if( att == INVALID_FILE_ATTRIBUTES ) {
			strcpy(slash,"..\\www");
			GetFullPathName(cpath, MAX_PATH,cpath,NULL);
		}
		CLIENT->basePath = cpath;


		RECT rect;
		CefWindowInfo info;
		CefBrowserSettings settings;
		GetWindowRect(hwnd,&rect);
	    info.SetAsChild(hwnd, rect);

	    CefBrowser::CreateBrowser(info,static_cast<CefRefPtr<CefClient> >(CLIENT),"http://app/index.html", settings);
		break;
		};
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_ERASEBKGND:
		return 0;
    case WM_SIZE:
		if( BROWSER.get() ) {
			RECT rect;
			GetClientRect(hwnd, &rect);
			HDWP hdwp = BeginDeferWindowPos(1);
			hdwp = DeferWindowPos(hdwp, BROWSER->GetWindowHandle(), NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,SWP_NOZORDER);
			EndDeferWindowPos(hdwp);
		}
		break;
	case WM_CLOSE:
		if( BROWSER.get() ) 
			BROWSER->ParentWindowWillClose();
		break;
	default:
		break;
	}
	return DefWindowProc(hwnd,msg,wParam,lParam);
}

int CALLBACK WinMain(
  _In_  HINSTANCE hInstance,
  _In_  HINSTANCE hPrevInstance,
  _In_  LPSTR lpCmdLine,
  _In_  int nCmdShow
) {

	const char *err = start();
	if( err != NULL )
		MessageBox(NULL,err,"Error",MB_ICONERROR);

	WNDCLASSEX wcl;
	HINSTANCE hinst = GetModuleHandle(NULL);
	memset(&wcl,0,sizeof(wcl));
	wcl.cbSize			= sizeof(WNDCLASSEX);
	wcl.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcl.lpfnWndProc		= WndProc;
	wcl.cbClsExtra		= 0;
	wcl.cbWndExtra		= 0;
	wcl.hInstance		= hinst;
	wcl.hIcon			= NULL;
	wcl.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wcl.hbrBackground	= (HBRUSH)(COLOR_BTNFACE+1);
	wcl.lpszMenuName	= "";
	wcl.lpszClassName	= "HxChromeWin";
	wcl.hIconSm			= 0;
	RegisterClassEx(&wcl);

	HWND wnd = CreateWindow(wcl.lpszClassName,"HxChrome",WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	
	CefRunMessageLoop();

	return 0;
}