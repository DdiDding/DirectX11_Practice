#pragma once
#include <fbxsdk.h>

class FbxTool
{
public:

	FbxTool();
	~FbxTool() = default;

private:

	void Initialize();

	bool Load(const char* fileName);

	void Shotdown();

private:

	FbxManager* m_manager;
};
