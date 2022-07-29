#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <random>
#include <stdint.h>
#include "def.h"
#include <list>
#include "Overlay.h"
#include <thread>
#include "Mhyprot/mhyprot.hpp"
#include "skCrypter.h"
#include "Mhyprot/baseadress.h"

// All features set to false for release purposes

bool tracersEnabled = false;
bool aimbotEnabled = false;
bool boxESPEnabled = false;
bool hpESPEnabled = false;
bool thirdPerson = false;
bool glowEnabled = false;
bool noRecoil = false;
bool noSpread = false;
bool HealthSpoof = false;
bool nameESP = false;
bool smoothing = false;
bool lockWhenClose = false;
float smoothingValue = 0.23f; // from 0-1
bool isPredictionAim = false;
bool Triggerbot = false;
bool SniperTriggerbot = false;
bool LockedESP = false;
bool Bhop = false;
bool SpeedMod = false;
bool menu = false;
bool VisibleCheck = false;
bool fovcircle = false;
bool overrideFOV = false;
bool BigHorsey = false;
bool SmallHorsey = false;
bool Emo = false;

float deafultFOV = 130.f;
//seprate Shit
bool justEnabled = true;
int enabledTicks = 0;

struct Vec2
{
public:
	float x;
	float y;
};

void ESPLoop();

uint64_t process_base = 0;

LONG WINAPI SimplestCrashHandler(EXCEPTION_POINTERS* ExceptionInfo)
{
	std::cout << skCrypt("[!!] Crashed at 0x") << ExceptionInfo->ExceptionRecord->ExceptionAddress << skCrypt(", by 0x") << std::hex << ExceptionInfo->ExceptionRecord->ExceptionCode << std::endl;

	return EXCEPTION_EXECUTE_HANDLER;
}

bool rendering = true;
int frame = 0;
FOverlay* g_overlay;

namespace math
{
	// Constants that we need for maths stuff
#define Const_URotation180        32768
#define Const_PI                  3.14159265358979323
#define Const_RadToUnrRot         10430.3783504704527
#define Const_UnrRotToRad         0.00009587379924285
#define Const_URotationToRadians  Const_PI / Const_URotation180 

	int ClampYaw(int angle) {
		static const auto max = Const_URotation180 * 2;

		while(angle > max)
			angle -= max;

		while(angle < 0)
			angle += max;

		return angle;
	}
	int ClampPitch(int angle) {
		if(angle > 16000)
			angle = 16000;

		if(angle < -16000)
			angle = -16000;
		return angle;
	}

	float GetFOV(float Fov)
	{
		return Fov * 10.0f;
	}


	float VectorMagnitude(Vec2 Vec) {
		return sqrt((Vec.x * Vec.x) + (Vec.y * Vec.y));
	}
	void Normalize(Vec2& v)
	{
		float size = VectorMagnitude(v);

		if(!size)
			v.x = v.y = 1;
		else
		{
			v.x /= size;
			v.y /= size;
		}
	}

}

bool MainAddress() {
#define NAMEOF(name) #name // USE FOR DEBUGGING
#define CHECKVAL(_name)		\
	/*printf("%s : 0x%llX\n", NAMEOF(_name), _name.data);*/ \
	if(!_name.IsValid()) { \
		return false;			\
	}							\

	//kinda dirty defines, use for debugging
#define PRINTVAL(_name) printf(skCrypt("%s : 0x%llX\n"), NAMEOF(_name), _name);
#define PRINTVALD(_name) printf(skCrypt("%s : %d\n"), NAMEOF(_name), _name);
#define PRINTVALF(_name) printf(skCrypt("%s : %f\n"), NAMEOF(_name), _name);

#define BREAK(_name) if(!_name.IsValid()) { \
		continue;			\
	}


	return true;
}

float ScreenCenterX;
float ScreenCenterY;

void HackTick() {
	//Inject Notification!!!!
	if (justEnabled) {
		if (frame > 1 && frame < 100) {
			g_overlay->draw_background(775, 7, 1095, 40, 2.f, D2D1::ColorF(D2D1::ColorF(0.15f, 0.15f, 0.15f, 1.0f)));// A Dark Gray
			g_overlay->draw_outline(775, 7, 1095, 40, 2.f, D2D1::ColorF(D2D1::ColorF(0.f, 0.f, 0.40f, 1.f)));//A Dark Blue
			g_overlay->draw_text(850, 13, D2D1::ColorF::White, "Evolve Has Been Attached!");
		} else if (frame > 100) {
			justEnabled = false;
			frame = 0;
		}
	}

	if (MainAddress()) {
		ESPLoop();
	}else
		Sleep(100);
}

void ESPLoop() {
	//Make an ESP or some shit here

	auto running = "";
	auto loopFrame = (frame % 400);
	if(loopFrame < 100)
		running = "|";
	else if(loopFrame < 200)
		running = "/";
	else if(loopFrame < 300)
		running = "--";
	else if(loopFrame < 400)
		running = "\\";
}

static void render(FOverlay* overlay)
{
	while(rendering)
	{
		overlay->begin_scene();

		overlay->clear_scene();
		frame++;

		HackTick();
		if(frame % 60 == 0) { // once a second (given a 60hz updaterate) because the results are cached 
			
		}

		// eject shit
		while (GetAsyncKeyState(VK_END) & 1)
		{
			std::cout << skCrypt("[X] Exiting...");
			g_overlay->clear_screen();
			g_overlay->begin_scene();
			g_overlay->clear_scene();
			g_overlay->end_scene();
			g_overlay->clear_screen();
			g_overlay->begin_scene();
			g_overlay->clear_scene();
			g_overlay->end_scene();
			rendering = false;
		}

		// menu
		if (menu)
		{
			//Visuals
			g_overlay->d_menu_header(300, 5, 470, 30, 2.f);
			g_overlay->d_menu_header2(300, 200, 470, 27, 2.f);
			g_overlay->drawRect(300, 200, 470, 5, 2.5f, D2D1::ColorF(0.14f, 0.14f, 0.14f, 1.0f));
			g_overlay->draw_text(355, 7, D2D1::ColorF::White, skCrypt("Visuals"));
			g_overlay->d_menu_base(300, 200, 470, 30, 2.f);

			//Combat
			g_overlay->d_menu_header(500, 5, 670, 30, 2.f);
			g_overlay->d_menu_header2(500, 200, 670, 27, 2.f);
			g_overlay->drawRect(500, 200, 670, 5, 2.5f, D2D1::ColorF(0.14f, 0.14f, 0.14f, 1.0f));
			g_overlay->draw_text(555, 7, D2D1::ColorF::White, ("Combat"));
			g_overlay->d_menu_base(500, 200, 670, 30, 2.f);

			//Player
			g_overlay->d_menu_header(700, 5, 870, 30, 2.f);
			g_overlay->d_menu_header2(700, 200, 870, 27, 2.f);
			g_overlay->drawRect(700, 200, 870, 5, 2.5f, D2D1::ColorF(0.14f, 0.14f, 0.14f, 1.0f));
			g_overlay->draw_text(760, 7, D2D1::ColorF::White, skCrypt("Player"));
			g_overlay->d_menu_base(700, 200, 870, 30, 2.f);


			//Visual Shit
			g_overlay->draw_text(335, 35, D2D1::ColorF::White, skCrypt("Box ESP"));
			if (boxESPEnabled)
				g_overlay->draw_text(420, 35, D2D1::ColorF::DarkBlue, skCrypt("[F5]"));
			else
				g_overlay->draw_text(420, 35, D2D1::ColorF::White, skCrypt("[F5]"));

			g_overlay->draw_text(320, 55, D2D1::ColorF::White, skCrypt("Glow Outline"));
			if (glowEnabled)
				g_overlay->draw_text(420, 55, D2D1::ColorF::DarkBlue, skCrypt("[F6]"));
			else
				g_overlay->draw_text(420, 55, D2D1::ColorF::White, skCrypt("[F6]"));

			g_overlay->draw_text(325, 75, D2D1::ColorF::White, skCrypt("Health ESP"));
			if (hpESPEnabled)
				g_overlay->draw_text(420, 75, D2D1::ColorF::DarkBlue, skCrypt("[F7]"));
			else
				g_overlay->draw_text(420, 75, D2D1::ColorF::White, skCrypt("[F7]"));

			g_overlay->draw_text(335, 95, D2D1::ColorF::White, skCrypt("Tracers"));
			if (tracersEnabled)
				g_overlay->draw_text(420, 95, D2D1::ColorF::DarkBlue, skCrypt("[F8]"));
			else
				g_overlay->draw_text(420, 95, D2D1::ColorF::White, skCrypt("[F8]"));

			g_overlay->draw_text(330, 115, D2D1::ColorF::White, skCrypt("Name ESP"));
			if (nameESP)
				g_overlay->draw_text(420, 115, D2D1::ColorF::DarkBlue, skCrypt("[F9]"));
			else
				g_overlay->draw_text(420, 115, D2D1::ColorF::White, skCrypt("[F9]"));

			g_overlay->draw_text(330, 135, D2D1::ColorF::White, skCrypt("Draw FOV"));
			if (fovcircle)
				g_overlay->draw_text(410, 135, D2D1::ColorF::DarkBlue, skCrypt("[NUM6]"));
			else
				g_overlay->draw_text(410, 135, D2D1::ColorF::White, skCrypt("[NUM6]"));

			//Combat shit
			g_overlay->draw_text(535, 35, D2D1::ColorF::White, skCrypt("Aimbot"));
			if (aimbotEnabled)
				g_overlay->draw_text(620, 35, D2D1::ColorF::DarkBlue, skCrypt("[F1]"));
			else
				g_overlay->draw_text(620, 35, D2D1::ColorF::White, skCrypt("[F1]"));

			g_overlay->draw_text(525, 55, D2D1::ColorF::White, skCrypt("Triggerbot"));
			if (Triggerbot)
				g_overlay->draw_text(620, 55, D2D1::ColorF::DarkBlue, skCrypt("[F2]"));
			else
				g_overlay->draw_text(620, 55, D2D1::ColorF::White, skCrypt("[F2]"));

			g_overlay->draw_text(525, 75, D2D1::ColorF::White, skCrypt("Slower TB"));
			if (SniperTriggerbot)
				g_overlay->draw_text(620, 75, D2D1::ColorF::DarkBlue, skCrypt("[F3]"));
			else
				g_overlay->draw_text(620, 75, D2D1::ColorF::White, skCrypt("[F3]"));

			g_overlay->draw_text(510, 95, D2D1::ColorF::White, skCrypt("Locked On ESP"));
			if (LockedESP)
				g_overlay->draw_text(620, 95, D2D1::ColorF::DarkBlue, skCrypt("[F4]"));
			else
				g_overlay->draw_text(620, 95, D2D1::ColorF::White, skCrypt("[F4]"));

			g_overlay->draw_text(510, 115, D2D1::ColorF::White, skCrypt("Predict Aimbot"));
			if (isPredictionAim)
				g_overlay->draw_text(620, 115, D2D1::ColorF::DarkBlue, skCrypt("[NUM4]"));
			else
				g_overlay->draw_text(620, 115, D2D1::ColorF::White, skCrypt("[NUM4]"));
			
			g_overlay->draw_text(510, 135, D2D1::ColorF::White, skCrypt("Aim Smoothing"));
			if (smoothing)
				g_overlay->draw_text(620, 135, D2D1::ColorF::DarkBlue, skCrypt("[NUM5]"));
			else
				g_overlay->draw_text(620, 135, D2D1::ColorF::White, skCrypt("[NUM5]"));
			

			//Player shit
			g_overlay->draw_text(730, 35, D2D1::ColorF::White, skCrypt("Bunnyhop"));
			if (Bhop)
				g_overlay->draw_text(820, 35, D2D1::ColorF::DarkBlue, skCrypt("[F10]"));
			else
				g_overlay->draw_text(820, 35, D2D1::ColorF::White, skCrypt("[F10]"));

			g_overlay->draw_text(720, 55, D2D1::ColorF::White, skCrypt("Third Person"));
			if (thirdPerson)
				g_overlay->draw_text(820, 55, D2D1::ColorF::DarkBlue, skCrypt("[F12]"));
			else
				g_overlay->draw_text(820, 55, D2D1::ColorF::White, skCrypt("[F12]"));
			
			g_overlay->draw_text(730, 75, D2D1::ColorF::White, skCrypt("No Recoil"));
			if (noRecoil)
				g_overlay->draw_text(815, 75, D2D1::ColorF::DarkBlue, skCrypt("[NUM1]"));
			else
				g_overlay->draw_text(815, 75, D2D1::ColorF::White, skCrypt("[NUM1]"));
			
			g_overlay->draw_text(730, 95, D2D1::ColorF::White, skCrypt("No Spread"));
			if (noSpread)
				g_overlay->draw_text(815, 95, D2D1::ColorF::DarkBlue, skCrypt("[NUM2]"));
			else
				g_overlay->draw_text(815, 95, D2D1::ColorF::White, skCrypt("[NUM2]"));
			
			g_overlay->draw_text(720, 115, D2D1::ColorF::White, skCrypt("Health Spoof"));
			if (HealthSpoof)
				g_overlay->draw_text(815, 115, D2D1::ColorF::DarkBlue, skCrypt("[NUM3]"));
			else
				g_overlay->draw_text(815, 115, D2D1::ColorF::White, skCrypt("[NUM3]"));

			g_overlay->draw_text(725, 135, D2D1::ColorF::White, skCrypt("Big Horse"));
			if (BigHorsey)
				g_overlay->draw_text(815, 135, D2D1::ColorF::DarkBlue, skCrypt("[NUM7]"));
			else
				g_overlay->draw_text(815, 135, D2D1::ColorF::White, skCrypt("[NUM7]"));

			g_overlay->draw_text(725, 155, D2D1::ColorF::White, skCrypt("Small Horse"));
			if (SmallHorsey)
				g_overlay->draw_text(815, 155, D2D1::ColorF::DarkBlue, skCrypt("[NUM8]"));
			else
				g_overlay->draw_text(815, 155, D2D1::ColorF::White, skCrypt("[NUM8]"));

			g_overlay->draw_text(745, 175, D2D1::ColorF::White, skCrypt("Emo"));
			if (Emo)
				g_overlay->draw_text(815, 175, D2D1::ColorF::DarkBlue, skCrypt("[NUM9]"));
			else
				g_overlay->draw_text(815, 175, D2D1::ColorF::White, skCrypt("[NUM9]"));
		}

		// other menu test
		// expirement with this when you can
		
		auto running = "";
		auto loopFrame = (frame % 100);
		if (loopFrame < 25)
			running = "|";
		else if (loopFrame < 50)
			running = "/";
		else if (loopFrame < 75)
			running = "--";
		else if (loopFrame < 100)
			running = "\\";

		while (GetAsyncKeyState(VK_INSERT) & 1)
			menu = !menu;
		while (GetAsyncKeyState(VK_F1) & 1)
			aimbotEnabled = !aimbotEnabled;

		while (GetAsyncKeyState(VK_F8) & 1)
			tracersEnabled = !tracersEnabled;

		while (GetAsyncKeyState(VK_F5) & 1)
			boxESPEnabled = !boxESPEnabled;

		while (GetAsyncKeyState(VK_F6) & 1)
			glowEnabled = !glowEnabled;

		while (GetAsyncKeyState(VK_F7) & 1)
			hpESPEnabled = !hpESPEnabled;

		while (GetAsyncKeyState(VK_F9) & 1)
			nameESP = !nameESP;

		while (GetAsyncKeyState(VK_F2) & 1)
			Triggerbot = !Triggerbot;

		while (GetAsyncKeyState(VK_F3) & 1)
			SniperTriggerbot = !SniperTriggerbot;

		while (GetAsyncKeyState(VK_F4) & 1)
			LockedESP = !LockedESP;

		while (GetAsyncKeyState(VK_F10) & 1)
			Bhop = !Bhop;

		while (GetAsyncKeyState(VK_F12) & 1)
			thirdPerson = !thirdPerson;

		while (GetAsyncKeyState(VK_NUMPAD1) & 1)
			noRecoil = !noRecoil;

		while (GetAsyncKeyState(VK_NUMPAD2) & 1)
			noSpread = !noSpread;

		while (GetAsyncKeyState(VK_NUMPAD3) & 1)
			HealthSpoof = !HealthSpoof;

		while (GetAsyncKeyState(VK_NUMPAD4) & 1)
			isPredictionAim = !isPredictionAim;

		while (GetAsyncKeyState(VK_NUMPAD5) & 1)
			smoothing = !smoothing;

		while (GetAsyncKeyState(VK_NUMPAD6) & 1)
			fovcircle = !fovcircle;

		while (GetAsyncKeyState(VK_NUMPAD7) & 1)
			BigHorsey = !BigHorsey;

		while (GetAsyncKeyState(VK_NUMPAD8) & 1)
			SmallHorsey = !SmallHorsey;

		while (GetAsyncKeyState(VK_NUMPAD9) & 1)
			Emo = !Emo;

		if (Bhop)
		{
			while (true)
			{
				keybd_event(VK_SPACE, 0,
					KEYEVENTF_EXTENDEDKEY | 0,
					0);

				keybd_event(VK_SPACE, 2,
					KEYEVENTF_KEYUP | 2,
					2);
				break;
			}
		}

		if (BigHorsey) {	
		}

		if (SmallHorsey) {
		}

		if (Emo) {
		}
		
		if (fovcircle) g_overlay->draw_circle();

		//Watermark at top left :)
		 g_overlay->d_menu_header(5, 5, 220, 75, 2.3f);
		 g_overlay->d_menu_header2(5, 20, 220, 25, 2.3f);
		 g_overlay->d_menu_base(5, 22, 220, 75, 2.3f);
		 g_overlay->drawRect(5, 5, 220, 75, 2.3f, D2D1::ColorF(0.14f, 0.14f, 0.14f, 1.0f));
		 g_overlay->draw_text(9.5, 4, D2D1::ColorF::White, skCrypt("Evolve - Minecraft"));
		 g_overlay->draw_text(9.5, 27, D2D1::ColorF::DarkBlue, skCrypt("Cheat is running... %s"), running);
		 g_overlay->draw_text(9.5, 48, D2D1::ColorF::White, skCrypt("Menu key:"));
		 g_overlay->d_menu_header(80, 50, 150, 65, 2.3f);
		 g_overlay->draw_text(91, 49, D2D1::ColorF::White, skCrypt("INSERT"));
		
		//if minecraft dissapeared in the process list, just exit the cheat :)
		 uint64_t process_id = GetProcessId(skCrypt("Minecraft.Windows.exe"));
		 if (!process_id) {
			 std::cout << skCrypt("[X] Exiting...");
			 g_overlay->clear_screen();
			 g_overlay->begin_scene();
			 g_overlay->clear_scene();
			 g_overlay->end_scene();
			 g_overlay->clear_screen();
			 g_overlay->begin_scene();
			 g_overlay->clear_scene();
			 g_overlay->end_scene();
			 rendering = false;
		 }

		overlay->end_scene();
	}
}

void exiting() {
	g_overlay->draw_text(9.5, 27, D2D1::ColorF(0.14f, 0.14f, 0.14f, 1.0f), skCrypt("Cheat stopped"));
	std::cout << skCrypt("Exiting");
	g_overlay->clear_screen();
	g_overlay->begin_scene();
	g_overlay->clear_scene();
	g_overlay->end_scene();
	rendering = false;
}

static void _init(FOverlay* overlay)
{
	// Initialize the window
	if (!overlay->window_init())
		return;

	// D2D Failed to initialize?
	if(!overlay->init_d2d())
		return;

	// render loop
	std::thread r(render, overlay);

	r.join(); // threading

	overlay->d2d_shutdown();

	return;
}

int main()
{
	SetConsoleTitle(skCrypt("Surge Console"));
	SetUnhandledExceptionFilter(SimplestCrashHandler);
	//initTrace();

	system(skCrypt("sc stop mhyprot2")); // RELOAD DRIVER JUST IN CASE
	system(skCrypt("CLS")); // CLEAR

	printf(skCrypt("[!] Welcome to the Surge Loader."));
	Sleep(1500);
	std::cout << "\x1B[2J\x1B[H";
	printf(skCrypt("[!] Please do not close this window while cheat is running."));
	Sleep(1500);
	std::cout << "\x1B[2J\x1B[H";

	auto process_name = skCrypt("Minecraft.Windows.exe");
	auto process_id = GetProcessId(process_name);
	if (!process_id){
		printf(skCrypt("[!] Minecraft was not found in the processes list.\n"), process_name);
		Sleep(2000);
		return -1;
	}

	printf(skCrypt("[+] Minecraft process found!\n"));
	Sleep(1000);
	std::cout << "\x1B[2J\x1B[H";

	//
	// initialize its service, etc
	//
	if (!mhyprot::init())
	{
		printf(skCrypt("[X] Cheat loading failed.\n"));
		return -1;
	}

	if (!mhyprot::driver_impl::driver_init(
		false, // print debug
		false // print seedmap
	))
	{
		printf(skCrypt("[X] Cheat loading failed.\n"));
		mhyprot::unload();
		return -1;
	}
	process_base = GetProcessBase(process_id);
	if (!process_base) {
		printf(skCrypt("[X] Cheat loading failed.\n"));
		mhyprot::unload();
		return -1;
	}

	//printf("[+] Game Base is 0x%llX\n", process_base);
	IMAGE_DOS_HEADER dos_header = read<IMAGE_DOS_HEADER>(process_base);
	printf(skCrypt("[+] Found Game Header!\n"));
	Sleep(1000);
	std::cout << "\x1B[2J\x1B[H";
	printf(skCrypt("[+] Successfully Attached Cheat. Have fun!\n"));
	if (dos_header.e_magic != 0x5A4D) 
		printf(skCrypt("[!] Game Header Initilization Timed Out.\n"));
		Sleep(1000);

	MainAddress();

	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = (HWND)FindWindowA(nullptr, (LPCSTR)"Minecraft");
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	HDC monitor = GetDC(hDesktop);

	int current = GetDeviceCaps(monitor, VERTRES);
	int total = GetDeviceCaps(monitor, DESKTOPVERTRES);

	FOverlay::ScreenWidth = (desktop.right - desktop.left) * total / current;
	FOverlay::ScreenHeight = (desktop.bottom - desktop.top) * total / current;

	ScreenCenterX = FOverlay::ScreenWidth / 2.f;
	ScreenCenterY = FOverlay::ScreenHeight / 2.f;
	g_overlay = { 0 };
	_init(g_overlay);
}

