#include "overlay.h"
#include <wrl.h>
#include "skCrypter.h"

//modified ver of https://github.com/iraizo/nvidia-overlay-hijack

static HWND win;
int FOverlay::ScreenHeight = 0;
int FOverlay::ScreenWidth = 0;
/*
Window functions
*/

auto FOverlay::InitWindowStuff() -> void {
	//Set window style
	int i = 0;
	i = (int)GetWindowLongA(win, -20);
	SetWindowLongPtrA(win, -20, (LONG_PTR)(i | 0x20));
	
	//Set transparency
	MARGINS margin;
	UINT opacity_flag, color_key_flag, color_key = 0;
	UINT opacity = 0;
	margin.cyBottomHeight = -1;
	margin.cxLeftWidth = -1;
	margin.cxRightWidth = -1;
	margin.cyTopHeight = -1;
	DwmExtendFrameIntoClientArea(win, &margin);
	opacity_flag = 0x02;
	color_key_flag = 0x01;
	color_key = 0x000000;
	opacity = 0xFF;
	SetLayeredWindowAttributes(win, color_key, opacity, opacity_flag);
	
	//Set top most
	SetWindowPos(win, HWND_TOPMOST, 0, 0, 0, 0, 0x0002 | 0x0001);
}

auto FOverlay::retrieve_window() -> HWND { return win; }

const MARGINS margin = { 0, 0, 0, 0 };
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case (WM_PAINT):
		DwmExtendFrameIntoClientArea(win, &margin);
		break;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void FOverlay::create_window() {
	LPCSTR lpClassName = "overlay";
	LPCSTR lpWindowName = "Evolve Overlayy";

	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = GetModuleHandleA(NULL);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)RGB(0, 0, 0);
	wc.lpszClassName = lpClassName;
	wc.lpszMenuName = lpWindowName;

	RegisterClassEx(&wc);

	win = CreateWindowExA(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT, lpClassName, lpWindowName, WS_POPUP, 0, 0, ScreenWidth, ScreenHeight, NULL, NULL, wc.hInstance, NULL);
	printf(skCrypt("[!] Could not find NVIDIA GeForce Overlay. Creating custom render window!\n"));
}

// Hijacking method down here.
auto FOverlay::window_init() -> BOOL {
	//Find Nvidia window
	win = FindWindow("CEF-OSC-WIDGET", "NVIDIA GeForce Overlay");
	
	//If window is not found, it will create its own window :)
	if (!win) create_window(); else printf(skCrypt("[+] Found NVIDIA GeForce Overlay!\n"));

	if(!win) return FALSE;

	FOverlay::InitWindowStuff();

	ShowWindow(win, SW_SHOWNORMAL);

	return TRUE;
}

/*
Overlay functions
*/

ID2D1Factory* d2d_factory;
ID2D1HwndRenderTarget* tar;
IDWriteFactory* write_factory;
ID2D1SolidColorBrush* brush;
ID2D1SolidColorBrush* red_brush;
ID2D1SolidColorBrush* green_brush;
ID2D1SolidColorBrush* blue_brush;
IDWriteTextFormat* format;
IDWriteTextLayout* layout;

auto FOverlay::d2d_shutdown() -> void {
	// Release
	tar->Release();
	write_factory->Release();
	brush->Release();
	red_brush->Release();
	green_brush->Release();
	blue_brush->Release();
	d2d_factory->Release();
}

auto FOverlay::init_d2d() -> BOOL {
	HRESULT ret;
	RECT rc;

	// Initialize D2D here
	ret = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2d_factory);
	if(FAILED(ret))
		return FALSE;

	ret =
		DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
			(IUnknown**)(&write_factory));
	if(FAILED(ret))
		return FALSE;

	write_factory->CreateTextFormat(
		L"Segoeuil", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL, 13.0, L"en-us", &format);

	GetClientRect(FOverlay::retrieve_window(), &rc);

	ret = d2d_factory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN,
				D2D1_ALPHA_MODE_PREMULTIPLIED)),
		D2D1::HwndRenderTargetProperties(
			FOverlay::retrieve_window(),
			D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)),
		&tar);
	if(FAILED(ret))
		return FALSE;

	tar->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &brush);
	tar->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &red_brush);
	tar->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green), &green_brush);
	tar->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Blue), &blue_brush);

	return TRUE;
}

auto FOverlay::begin_scene() -> void { tar->BeginDraw(); }

auto FOverlay::end_scene() -> void { tar->EndDraw(); }

auto FOverlay::clear_scene() -> void { tar->Clear(); }

auto FOverlay::draw_text_white(int x, int y, const char* str, ...) -> void {
	char buf[4096];
	int len = 0;
	wchar_t b[256];

	// if (!draw) // no need for it.
	//	 return;

	va_list arg_list;
	va_start(arg_list, str);
	vsnprintf(buf, sizeof(buf), str, arg_list);
	va_end(arg_list);

	len = strlen(buf);
	mbstowcs(b, buf, len);

	tar->DrawText(b, len, format, D2D1::RectF(x, y, 1920, 1080), brush,
		D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
}

auto FOverlay::draw_line(int x1, int y1, int x2, int y2, D2D1::ColorF color) -> void {
	auto point1 = D2D1_POINT_2F();
	point1.x = x1;
	point1.y = y1;

	auto point2 = D2D1_POINT_2F();
	point2.x = x2;
	point2.y = y2;

	ID2D1SolidColorBrush* temp;

	tar->CreateSolidColorBrush(color, &temp);
	tar->DrawLine(point1, point2, temp);
	temp->Release();
}

auto FOverlay::DrawtestShit()->void {
	//Microsoft::WRL::ComPtr<ID2D1Effect> directionalBlurEffect;
	//m_d2dContext->CreateEffect(CLSID_D2D1DirectionalBlur, &directionalBlurEffect);

	//directionalBlurEffect->SetInput(0, bitmap);
	//directionalBlurEffect->SetValue(D2D1_DIRECTIONALBLUR_PROP_STANDARD_DEVIATION, 7.0f);

	//m_d2dContext->BeginDraw();
	//m_d2dContext->DrawImage(directionalBlurEffect.Get());
	//m_d2dContext->EndDraw();
}

auto FOverlay::draw_text(int x, int y, D2D1::ColorF color, const char* str, ...) -> void {
	char buf[4096];
	int len = 0;
	wchar_t b[256];

	// if (!draw) // no need for it.
	//	 return;

	va_list arg_list;
	va_start(arg_list, str);
	vsnprintf(buf, sizeof(buf), str, arg_list);
	va_end(arg_list);

	len = strlen(buf);
	mbstowcs(b, buf, len);
	ID2D1SolidColorBrush* temp;

	tar->CreateSolidColorBrush(color, &temp);

	tar->DrawText(b, len, format, D2D1::RectF(x, y, 1920, 1080), temp,
		D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
	temp->Release();
}

auto FOverlay::drawRect(int x, int y, int width, int height, float thickness, D2D1::ColorF color ...)-> void {
	ID2D1SolidColorBrush* coolbrush;
	tar->CreateSolidColorBrush(
		D2D1::ColorF(color),
		&coolbrush
	);
	coolbrush->SetOpacity(0.9f);
	tar->DrawRectangle(D2D1::RectF(x, y, width, height), coolbrush, thickness);
}

auto FOverlay::drawRectBlack(int x, int y, int width, int height, float thickness)-> void {
	ID2D1SolidColorBrush* coolbrush;
	tar->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black),
		&coolbrush
	);
	coolbrush->SetOpacity(0.9f);
	tar->DrawRectangle(D2D1::RectF(x, y, width, height), coolbrush, thickness);
}

auto FOverlay::drawRectRed(int x, int y, int width, int height, float thickness)-> void {
	ID2D1SolidColorBrush* coolbrush;
	tar->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Red, 1.0f),
		&coolbrush
	);
	tar->DrawRectangle(D2D1::RectF(x, y, width, height), coolbrush, thickness);
}
auto FOverlay::drawRectGreen(int x, int y, int width, int height, float thickness)-> void {
	ID2D1SolidColorBrush* coolbrush;
	tar->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Green, 1.0f),
		&coolbrush
	);
	tar->DrawRectangle(D2D1::RectF(x, y, width, height), coolbrush, thickness);
}
auto FOverlay::drawRectBlue(int x, int y, int width, int height, float thickness)-> void {
	ID2D1SolidColorBrush* coolbrush;
	tar->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Blue, 1.0f),
		&coolbrush
	);
	tar->DrawRectangle(D2D1::RectF(x, y, width, height), coolbrush, thickness);
}

auto FOverlay::draw_box(int x, int y, float width, float height, D2D1::ColorF color) -> void {
	auto rect = D2D1_RECT_F();
	rect.bottom = y - height / 2;
	rect.top = y + height / 2;
	rect.right = x + width / 2;
	rect.left = x - width / 2;

	draw_box(rect, color);
}

auto FOverlay::draw_circle() -> void {
	int ScreenHeight = 0;
	int ScreenWidth = 0;

	float ScreenCenterX;
	float ScreenCenterY;
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	HDC monitor = GetDC(hDesktop);

	int current = GetDeviceCaps(monitor, VERTRES);
	int total = GetDeviceCaps(monitor, DESKTOPVERTRES);

	ScreenWidth = (desktop.right - desktop.left) * total / current;
	ScreenHeight = (desktop.bottom - desktop.top) * total / current;

	ScreenCenterX = ScreenWidth / 2.f;
	ScreenCenterY = ScreenHeight / 2.f;
	D2D1_ELLIPSE ellipse = D2D1::Ellipse(
		D2D1::Point2F(ScreenCenterX, ScreenCenterY),
		370,
		370
	);
	ID2D1SolidColorBrush* temp;
	tar->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::White, 0.7f),
		&temp
	);
	tar->DrawEllipse(ellipse, temp);
}

auto FOverlay::draw_box(D2D1_RECT_F rect, D2D1::ColorF color) -> void {
	ID2D1SolidColorBrush* temp;

	tar->CreateSolidColorBrush(color, &temp);

	tar->FillRectangle(rect, temp);
	temp->Release();
}

auto FOverlay::ProgressBar(float x, float y, float w, float h, int value, int v_max, int barColor) -> void {
	auto rect = D2D1_RECT_F();
	rect.bottom = y - h / 2;
	rect.top = y + h / 2;
	rect.right = x + w / 2;
	rect.left = x - w / 2;

	draw_box(rect, barColor);
}

auto FOverlay::draw_redbox(int x, int y, int width, int height, float thickness, D2D1::ColorF color ...)-> void {
	ID2D1SolidColorBrush* coolbrush;
	tar->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black, 1.0f),
		&coolbrush
	);
	coolbrush->SetOpacity(0.45f);
	tar->FillRectangle(D2D1::RectF(x, y, width, height), coolbrush);
}

auto FOverlay::active(int x, int y, int width, int height, float thickness, D2D1::ColorF color ...)-> void {
	ID2D1SolidColorBrush* coolbrush;
	tar->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Green, 1.0f),
		&coolbrush
	);
	coolbrush->SetOpacity(0.6f);
	tar->FillRectangle(D2D1::RectF(x, y, width, height), coolbrush);
}

auto FOverlay::inactive(int x, int y, int width, int height, float thickness, D2D1::ColorF color ...)-> void {
	ID2D1SolidColorBrush* coolbrush;
	tar->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Red, 1.0f),
		&coolbrush
	);
	coolbrush->SetOpacity(0.6f);
	tar->FillRectangle(D2D1::RectF(x, y, width, height), coolbrush);
}

auto FOverlay::drawBoxBlack(int x, int y, int width, int height, float thickness)-> void {
	ID2D1SolidColorBrush* coolbrush;
	tar->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black, 1.0f),
		&coolbrush
	);
	coolbrush->SetOpacity(0.6f);
	tar->FillRectangle(D2D1::RectF(x, y, width, height), coolbrush);
}

auto FOverlay::d_menu_header(int x, int y, int width, int height, float thickness)-> void {
	ID2D1SolidColorBrush* coolbrush;
	tar->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF(0.10f, 0.10f, 0.10f, 1.0f)),
		&coolbrush
	);
	tar->FillRectangle(D2D1::RectF(x, y, width, height), coolbrush);
}

auto FOverlay::d_menu_header2(int x, int y, int width, int height, float thickness)-> void {
	ID2D1SolidColorBrush* coolbrush;
	tar->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF(0.15f, 0.15f, 0.15f, 1.0f)),
		&coolbrush
	);
	tar->FillRectangle(D2D1::RectF(x, y, width, height), coolbrush);
}

auto FOverlay::d_menu_base(int x, int y, int width, int height, float thickness)-> void {
	ID2D1SolidColorBrush* coolbrush;
	tar->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF(0.11f, 0.11f, 0.11f, 1.0f)),
		&coolbrush
	);
	tar->FillRectangle(D2D1::RectF(x, y, width, height), coolbrush);
}

auto FOverlay::d_menu_outline(int x, int y, int width, int height, float thickness)-> void {
	ID2D1SolidColorBrush* coolbrush;
	tar->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF(0.10f, 0.10f, 0.10f, 1.0f)),
		&coolbrush
	);
	tar->DrawRectangle(D2D1::RectF(x, y, width, height), coolbrush, thickness);
}

auto FOverlay::draw_background(int x, int y, int width, int height, float thickness, D2D1::ColorF color ...)-> void {
	ID2D1SolidColorBrush* coolbrush;
	tar->CreateSolidColorBrush(color, &coolbrush);
	tar->FillRectangle(D2D1::RectF(x, y, width, height), coolbrush);
}

auto FOverlay::draw_outline(int x, int y, int width, int height, float thickness, D2D1::ColorF color ...)-> void {
	ID2D1SolidColorBrush* coolbrush;
	tar->CreateSolidColorBrush(color, &coolbrush);
	tar->DrawRectangle(D2D1::RectF(x, y, width, height), coolbrush, thickness);
}

auto FOverlay::draw_text_red(int x, int y, const char* str, ...) -> void {
	char buf[4096];
	int len = 0;
	wchar_t b[256];

	// if (!draw) // no need for it.
	//	 return;

	va_list arg_list;
	va_start(arg_list, str);
	vsnprintf(buf, sizeof(buf), str, arg_list);
	va_end(arg_list);

	len = strlen(buf);
	mbstowcs(b, buf, len);

	tar->DrawText(b, len, format, D2D1::RectF(x, y, 1920, 1080), red_brush,
		D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
}

auto FOverlay::draw_text_green(int x, int y, const char* str, ...) -> void {
	char buf[4096];
	int len = 0;
	wchar_t b[256];

	// if (!draw) // no need for it.
	//	 return;

	va_list arg_list;
	va_start(arg_list, str);
	vsnprintf(buf, sizeof(buf), str, arg_list);
	va_end(arg_list);

	len = strlen(buf);
	mbstowcs(b, buf, len);

	tar->DrawText(b, len, format, D2D1::RectF(x, y, 1920, 1080), green_brush,
		D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
}

auto FOverlay::draw_text_blue(int x, int y, const char* str, ...) -> void {
	char buf[4096];
	int len = 0;
	wchar_t b[256];

	// if (!draw) // no need for it.
	//	 return;

	va_list arg_list;
	va_start(arg_list, str);
	vsnprintf(buf, sizeof(buf), str, arg_list);
	va_end(arg_list);

	len = strlen(buf);
	mbstowcs(b, buf, len);

	tar->DrawText(b, len, format, D2D1::RectF(x, y, 1920, 1080), blue_brush,
		D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
}

auto FOverlay::clear_screen() -> void {
	FOverlay::begin_scene();
	FOverlay::clear_scene();
	FOverlay::end_scene();
}
