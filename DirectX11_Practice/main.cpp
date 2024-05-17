////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////
#include "systemclass.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,	_In_ LPSTR lpCmdLine, 	_In_ int nShowCmd
)
{
	SystemClass* System;
	bool result;

	// 새로운 개체 생성.
	System = new SystemClass;

	// System 개체 초기화 후 실행.
	result = System->Initialize();
	if (result)
	{
		System->Run();
	}

	// System 개체 종료 후 메모리 해제
	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}