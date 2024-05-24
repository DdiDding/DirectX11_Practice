#include "InputClass.h"

void InputClass::Initialize()
{
	// 키의 값을 누르지 않은 뜻인 false로 모두 초기화
	for (int i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}

	return;
}


void InputClass::KeyDown(unsigned int input)
{
	// If a key is pressed then save that state in the key array.
	m_keys[input] = true;
	return;
}


void InputClass::KeyUp(unsigned int input)
{
	// If a key is released then clear that state in the key array.
	m_keys[input] = false;
	return;
}


bool InputClass::IsKeyDown(unsigned int key)
{
	// 키가 눌렸는지, 눌리지 않았는지의 상태를 반환한다.
	return m_keys[key];
}
