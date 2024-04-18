#pragma once

#include "stdafx.h"
#include "D3D12Application.h"

class DX12Game : public D3D12Application 
{
public:
	using D3D12Application::D3D12Application;
	virtual void OnUpdate();
	virtual void OnRender();
private:

};