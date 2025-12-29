#include "ApplicationClass.h"

ApplicationClass::ApplicationClass()
{
}

bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	return true;
}

void ApplicationClass::Shutdown()
{
	return;
}

bool ApplicationClass::Frame()
{
	return true;
}

bool ApplicationClass::Render(float rotation)
{
	return true;
}
