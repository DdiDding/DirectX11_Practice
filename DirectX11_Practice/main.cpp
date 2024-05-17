////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////
#include "systemclass.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,	_In_ LPSTR lpCmdLine, 	_In_ int nShowCmd
)
{
	SystemClass* System;
	bool result;

	// ���ο� ��ü ����.
	System = new SystemClass;

	// System ��ü �ʱ�ȭ �� ����.
	result = System->Initialize();
	if (result)
	{
		System->Run();
	}

	// System ��ü ���� �� �޸� ����
	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}