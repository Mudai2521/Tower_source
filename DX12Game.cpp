#include "DX12Game.h"

using namespace DirectX;

void DX12Game::OnInit()
{
    LoadPipeline();
    LoadAssets();

    //ChangeFullScreenState();

    LoadSizeDependentResources();

    m_Scene.Init(m_device.Get(), m_commandQueue.Get(), m_pPool[POOL_TYPE_RES], m_width, m_height);
}

// Load the sample assets.
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


        D3D12_STATIC_SAMPLER_DESC sampler = CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);

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
            0, // GPUが複数ある場合のノードマスク（今回は1個しか無い想定なので0）
            pBlob->GetBufferPointer(), // シリアライズしたデータのポインタ
            pBlob->GetBufferSize(), // シリアライズしたデータのサイズ
            IID_PPV_ARGS(m_rootSignature.GetAddressOf()))); // ルートシグニチャ格納先のポインタ
    }

    // Create the pipeline state, which includes compiling and loading shaders.
    {
        ComPtr<ID3DBlob> vertexShader;
        ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
        // Enable better shader debugging with the graphics debugging tools.
        UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        UINT compileFlags = 0;
#endif

        ThrowIfFailed(D3DCompileFromFile(GetAssetFullPath(L"SpriteShader.hlsl").c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
        ThrowIfFailed(D3DCompileFromFile(GetAssetFullPath(L"SpriteShader.hlsl").c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));

        // Define the vertex input layout.
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        // Describe and create the graphics pipeline state object (PSO).
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
        psoDesc.pRootSignature = m_rootSignature.Get();
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable = FALSE;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count = 1;
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

    // Create the command list.
    ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator[m_frameIndex].Get(), m_pipelineState.Get(), IID_PPV_ARGS(&m_commandList)));

    // Command lists are created in the recording state, but there is nothing
    // to record yet. The main loop expects it to be closed, so close it now.
    ThrowIfFailed(m_commandList->Close());


    // Create synchronization objects and wait until assets have been uploaded to the GPU.
    {
        ThrowIfFailed(m_device->CreateFence(m_fenceValue[m_frameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
        m_fenceValue[m_frameIndex]++;

        // Create an event handle to use for frame synchronization.
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

    // Create frame resources.
    {
        // Create a RTV for each frame.
        for (UINT n = 0; n < FrameCount; n++)
        {
            m_RenderTargetView[n].OnSizeChanged(m_device.Get(),m_swapChain.Get());
        }

        //DSV作成
        //m_DSBuffer.OnSizeChanged(m_device.Get(), m_width, m_height);
    }

    // This is where you would create/resize intermediate render targets, depth stencils, or other resources
    // dependent on the window size.
}


void DX12Game::OnDestroy()
{

    WaitForGpu();

    if (!m_tearingSupport)
    {
        // Fullscreen state should always be false before exiting the app.
        ThrowIfFailed(m_swapChain->SetFullscreenState(FALSE, nullptr));
    }

    CloseHandle(m_fenceEvent);
}

//フレーム毎の更新処理
void DX12Game::OnUpdate()
{
    m_Scene.OnUpdate();
}

void DX12Game::OnRender(HWND hwnd)
{
    // Record all the commands we need to render the scene into the command list.
    PopulateCommandList();

    // Execute the command list.
    ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    auto hdc = GetDC(hwnd);
    auto rate = GetDeviceCaps(hdc, VREFRESH);

    // Present the frame.
    ThrowIfFailed(m_swapChain->Present((rate < 65 ? 1 : 2), 0));

    MoveToNextFrame();
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
    switch (key)
    {
        
    case VK_RETURN:
    {
        ChangeFullScreenState();
    }
    
    break;
    }

    m_Scene.OnKeyDown(key);
}

void DX12Game::PopulateCommandList()
{
    // Command list allocators can only be reset when the associated 
    // command lists have finished execution on the GPU; apps should use 
    // fences to determine GPU execution progress.
    ThrowIfFailed(m_commandAllocator[m_frameIndex]->Reset());

    // However, when ExecuteCommandList() is called on a particular command 
    // list, that command list can then be reset at any time and must be before 
    // re-recording.
    ThrowIfFailed(m_commandList->Reset(m_commandAllocator[m_frameIndex].Get(), m_pipelineState.Get()));

    // Set necessary state.
    m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());
    m_commandList->SetDescriptorHeaps(1, m_pPool[POOL_TYPE_RES]->GetHeapAddress());
    m_commandList->SetPipelineState(m_pipelineState.Get());

    

    
    m_commandList->RSSetViewports(1, &m_viewport);
    m_commandList->RSSetScissorRects(1, &m_scissorRect);

    // Indicate that the back buffer will be used as a render target.
    m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargetView[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));



    m_commandList->OMSetRenderTargets(1, &m_RenderTargetView[m_frameIndex].GetHandleCPU(), FALSE, &m_DSBuffer.GetHandleCPU());

    // Record commands.
    const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
    m_commandList->ClearRenderTargetView(m_RenderTargetView[m_frameIndex].GetHandleCPU(), clearColor, 0, nullptr);
    m_commandList->ClearDepthStencilView(m_DSBuffer.GetHandleCPU(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    m_Scene.DrawSprite(m_commandList.Get());


    // Indicate that the back buffer will now be used to present.
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
