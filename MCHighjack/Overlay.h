#define _CRT_SECURE_NO_WARNINGS

#ifndef FOverlay_H
#define FOverlay_H

#include <windows.h>
#include <stdio.h>
#include <dwmapi.h> 
#include <d2d1.h>
#include <dwrite.h>
#include <effects.h>
#pragma comment(lib, "Dwrite")
#pragma comment(lib, "Dwmapi.lib") 
#pragma comment(lib, "d2d1.lib")

#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

class FOverlay
{
public:
	static int ScreenHeight;
	static int ScreenWidth;
	void create_window();
	auto init_d2d()->BOOL;
	auto end_scene()-> void;
	auto window_init()->BOOL;
	auto begin_scene()-> void;
	auto clear_scene()-> void;
	auto DrawtestShit()->void;
	auto d2d_shutdown()-> void;
	auto clear_screen()-> void;
	auto draw_circle() -> void;
	auto retrieve_window()->HWND;
	auto InitWindowStuff()-> void;
	auto draw_box(D2D1_RECT_F rect, D2D1::ColorF color) -> void;
	auto draw_text_red(int x, int y, const char* str, ...)-> void;
	auto draw_text_blue(int x, int y, const char* str, ...)-> void;
	auto draw_text_white(int x, int y, const char* str, ...)-> void;
	auto draw_text_green(int x, int y, const char* str, ...)-> void;
	auto draw_line(int x1, int y1, int x2, int y2, D2D1::ColorF color) -> void;
	auto drawRectRed(int x, int y, int width, int height, float thickness)-> void;
	auto d_menu_base(int x, int y, int width, int height, float thickness)-> void;
	auto drawRectBlue(int x, int y, int width, int height, float thickness)-> void;
	auto draw_text(int x, int y, D2D1::ColorF color, const char* str, ...) -> void;
	auto drawBoxBlack(int x, int y, int width, int height, float thickness)-> void;
	auto drawRectGreen(int x, int y, int width, int height, float thickness)-> void;
	auto d_menu_header(int x, int y, int width, int height, float thickness)-> void;
	auto drawRectBlack(int x, int y, int width, int height, float thickness)-> void;
	auto d_menu_header2(int x, int y, int width, int height, float thickness)-> void;
	auto d_menu_outline(int x, int y, int width, int height, float thickness)-> void;
	auto draw_box(int x, int y, float width, float height, D2D1::ColorF color) -> void;
	auto ProgressBar(float x, float y, float w, float h, int value, int v_max, int barColor) -> void;
	auto active(int x, int y, int width, int height, float thickness, D2D1::ColorF color ...)-> void;
	auto drawRect(int x, int y, int width, int height, float thickness, D2D1::ColorF color ...)-> void;
	auto inactive(int x, int y, int width, int height, float thickness, D2D1::ColorF color ...)-> void;
	auto draw_redbox(int x, int y, int width, int height, float thickness, D2D1::ColorF color ...)-> void;
	auto draw_outline(int x, int y, int width, int height, float thickness, D2D1::ColorF color ...)-> void;
	auto draw_background(int x, int y, int width, int height, float thickness, D2D1::ColorF color ...)-> void;
};

#endif