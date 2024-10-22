#include "DX12Game.h"


#pragma comment(lib,"winmm.lib")

using namespace DirectX;

void DX12Game::OnInit(HINSTANCE hinst, HWND hwnd)
{
    LoadPipeline();
    LoadAssets();
    InitDirectInput(hinst,hwnd);

    //ChangeFullScreenState();

    LoadSizeDependentResources();
    m_Scene = new Scene();
    m_Scene->Init(m_device.Get(), m_commandQueue.Get(), m_pPool[POOL_TYPE_RES], m_width, m_height);

    ThrowIfFailed(m_inputDevice->Acquire());
}


void DX12Game::LoadAssets()
{
    {
        D3D12_ROOT_SIGNATURE_FLAGS flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
        flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
        flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;


        CD3DX12_ROOT_PARAMETER rootParam[2] = {};
        rootParam[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);

        D3D12_DESCRIPTOR_RANGE range = {};
        range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        range.NumDescriptors = 1;
        range.BaseShaderRegister = 0;
        range.RegisterSpace = 0;
        range.OffsetInDescriptorsFromTableStart = 0;

        rootParam[1].InitAsDescriptorTable(1, &range, D3D12_SHADER_VISIBILITY_ALL);


        D3D12_STATIC_SAMPLER_DESC sampler = CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_POINT);
        sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;


        CD3DX12_ROOT_SIGNATURE_DESC desc = {};
        desc.NumParameters = std::size(rootParam);
        desc.NumStaticSamplers = 1;
        desc.pParameters = rootParam;
        desc.pStaticSamplers = &sampler;
        desc.Flags = flag;

        ComPtr<ID3DBlob> pBlob;
        ComPtr<ID3DBlob> pErrorBlob;

        ThrowIfFailed(D3D12SerializeRootSignature(
            &desc,
            D3D_ROOT_SIGNATURE_VERSION_1_0,
            pBlob.GetAddressOf(),
            pErrorBlob.GetAddressOf()));
        ThrowIfFailed(m_device->CreateRootSignature(
            0,                                              // GPUが複数ある場合のノードマスク
            pBlob->GetBufferPointer(),                      // シリアライズしたデータのポインタ
            pBlob->GetBufferSize(),                         // シリアライズしたデータのサイズ
            IID_PPV_ARGS(m_rootSignature.GetAddressOf()))); // ルートシグニチャ格納先のポインタ
    }

    {
        ComPtr<ID3DBlob> vertexShader;
        ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
        UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        UINT compileFlags = 0;
#endif

        ThrowIfFailed(D3DCompileFromFile(GetAssetFullPath(L"SpriteShader.hlsl").c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
        ThrowIfFailed(D3DCompileFromFile(GetAssetFullPath(L"SpriteShader.hlsl").c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));

        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
        psoDesc.pRootSignature = m_rootSignature.Get();
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable = FALSE;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        psoDesc.SampleDesc.Count = 1;

        // ブレンド状態の設定
        psoDesc.BlendState.AlphaToCoverageEnable = FALSE;
        psoDesc.BlendState.IndependentBlendEnable = FALSE;
        const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
        {
            FALSE,FALSE,
            D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
            D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
            D3D12_LOGIC_OP_NOOP,
            D3D12_COLOR_WRITE_ENABLE_ALL,
        };
        for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i) {
            psoDesc.BlendState.RenderTarget[i] = defaultRenderTargetBlendDesc;
        }

        // ブレンドを有効にする
        psoDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
        psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
        psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;


        ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
    }

    //深度ステンシルバッファの作成
    {
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.NumDescriptors = 24;
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        if (!DescriptorPool::Create(m_device.Get(), &heapDesc, &m_pPool[POOL_TYPE_DSV]))throw std::exception();

        if (!m_DSBuffer.Init(m_device.Get(), m_pPool[POOL_TYPE_DSV], m_width, m_height))throw std::exception();
    }

    
    ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator[m_frameIndex].Get(), m_pipelineState.Get(), IID_PPV_ARGS(&m_commandList)));

    ThrowIfFailed(m_commandList->Close());

    {
        ThrowIfFailed(m_device->CreateFence(m_fenceValue[m_frameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
        m_fenceValue[m_frameIndex]++;
        
        m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (m_fenceEvent == nullptr)
        {
            ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }


        WaitForGpu();
    }


}


void DX12Game::LoadSizeDependentResources()
{
    UpdateViewAndScissor();

    for (UINT n = 0; n < FrameCount; n++)
    {
        m_RenderTargetView[n].OnSizeChanged(m_device.Get(), m_swapChain.Get());
    }
}


void DX12Game::OnDestroy()
{
    m_Scene->Term();
    delete m_Scene;
    m_Scene = nullptr;

    WaitForGpu();

    if (!m_tearingSupport)
    {
        ThrowIfFailed(m_swapChain->SetFullscreenState(FALSE, nullptr));
    }

    CloseHandle(m_fenceEvent);

    ThrowIfFailed(m_inputDevice->Unacquire());
    m_inputDevice->Release();
    m_directInput->Release();
}

//フレーム毎の更新処理
void DX12Game::OnUpdate()
{
    QueryPerformanceFrequency(&mTimeFreq);
    QueryPerformanceCounter(&mTimeStart);

    unsigned char KeyState[0x100] = {0};
    HRESULT hr= m_inputDevice->GetDeviceState(sizeof(KeyState), KeyState);
    if (hr != S_OK) {
        ThrowIfFailed(m_inputDevice->Acquire());
    }
    if (KeyState[DIK_RETURN] & 0x80)
    {
        //ChangeFullScreenState();
    }
    int result= m_Scene->OnUpdate(KeyState);
    if (result == 1) 
    {
        PostQuitMessage(0);
    }
}

void DX12Game::OnRender(HWND hwnd)
{
    PopulateCommandList();

    ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    auto hdc = GetDC(hwnd);
    auto rate = GetDeviceCaps(hdc, VREFRESH);

    ThrowIfFailed(m_swapChain->Present((rate < 65 ? 1 : 2), 0));

    MoveToNextFrame();

    QueryPerformanceCounter(&mTimeEnd);
    auto mFrameTime = static_cast<double>(mTimeEnd.QuadPart - mTimeStart.QuadPart) / static_cast<double>(mTimeFreq.QuadPart);
#ifdef _DEBUG
#ifdef UNICODE
    std::wstringstream stream;
#else
    std::stringstream stream;
#endif
    stream << (1 / mFrameTime) << " FPS" << std::endl;
    OutputDebugString(stream.str().c_str());
#endif 
}

void DX12Game::ChangeFullScreenState()
{
    if (m_tearingSupport)
    {
        Win32Application::ToggleFullscreenWindow();
        LoadSizeDependentResources();
    }
    else
    {
        BOOL fullscreenState;
        ThrowIfFailed(m_swapChain->GetFullscreenState(&fullscreenState, nullptr));
        ThrowIfFailed(m_swapChain->SetFullscreenState(!fullscreenState, nullptr));
        LoadSizeDependentResources();
    }
}

void  DX12Game::OnKeyDown(UINT8 key)
{
    
}

void DX12Game::PopulateCommandList()
{
    
    ThrowIfFailed(m_commandAllocator[m_frameIndex]->Reset());

    
    ThrowIfFailed(m_commandList->Reset(m_commandAllocator[m_frameIndex].Get(), m_pipelineState.Get()));

    
    m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());
    m_commandList->SetDescriptorHeaps(1, m_pPool[POOL_TYPE_RES]->GetHeapAddress());
    m_commandList->SetPipelineState(m_pipelineState.Get());

    m_commandList->RSSetViewports(1, &m_viewport);
    m_commandList->RSSetScissorRects(1, &m_scissorRect);

    
    m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargetView[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));



    m_commandList->OMSetRenderTargets(1, &m_RenderTargetView[m_frameIndex].GetHandleCPU(), FALSE, &m_DSBuffer.GetHandleCPU());

    
    const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
    m_commandList->ClearRenderTargetView(m_RenderTargetView[m_frameIndex].GetHandleCPU(), clearColor, 0, nullptr);
    m_commandList->ClearDepthStencilView(m_DSBuffer.GetHandleCPU(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    m_Scene->DrawSprite(m_commandList.Get());


    
    m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargetView[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

    ThrowIfFailed(m_commandList->Close());
}

void DX12Game::UpdateViewAndScissor()
{
    m_viewport.TopLeftX = 0.0f;
    m_viewport.TopLeftY = 0.0f;
    m_viewport.Width = static_cast<float>(m_width);
    m_viewport.Height = static_cast<float>(m_height);

    m_scissorRect.left = 0;
    m_scissorRect.top = 0;
    m_scissorRect.right = static_cast<LONG>(m_width);
    m_scissorRect.bottom = static_cast<LONG>(m_height);
}
