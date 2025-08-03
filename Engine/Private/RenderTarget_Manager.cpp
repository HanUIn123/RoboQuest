#include "RenderTarget_Manager.h"
#include "RenderTarget.h"


CRenderTarget_Manager::CRenderTarget_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : m_pDevice{_pDevice}
    , m_pContext{_pContext}
{
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pDevice);
}

HRESULT CRenderTarget_Manager::Initialize()
{
    return S_OK;
}

HRESULT CRenderTarget_Manager::Add_RenderTarget(const _wstring& _strRenderTargetTag, _uint _iWidth, _uint _iHeight, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor)
{
    if (nullptr != Find_RenderTarget(_strRenderTargetTag))
        return E_FAIL;

    CRenderTarget* pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, _iWidth, _iHeight, _ePixelFormat, _vClearColor);
    if (nullptr == pRenderTarget)
        return E_FAIL;

    m_mapRenderTargets.emplace(_strRenderTargetTag, pRenderTarget);

    return S_OK;
}

HRESULT CRenderTarget_Manager::Add_MRT(const _wstring& _strMRTTag, const _wstring& _strRenderTargetTag)
{
    // Add_RenderTarget 에 넣어놧던 렌타뷰들을 다시 검색해서, 따로 여기서 또 별도의 벡터 컨테이너에 담아주는 역할임.
    // 멤버에 해야 값이 유지 되지 않나요? 하는데 보면 Find_MRT 라는 벡터 반환형 그쪽의 map에다가 담아 두고 찾아서 반환해주고 있네.
    // 그래서 지금  vector<CRenderTarget*>*          pVecMRT = Find_MRT(_strMRTTag); 이게, map 에 담아놨던 그거를 다시 담아줘서 유지가 되는 식임.

    CRenderTarget* pRenderTarget = Find_RenderTarget(_strRenderTargetTag);

    if (nullptr == pRenderTarget)
        return E_FAIL;
    
    // 멀티 렌타뷰 용 map 컨테이너에서 second 인 벡터를 찾아서 반환해주는 Find_MRT 자체가 vector 반환타입이니 
    // 이게 pVecMRT 라는 지역벡터 포인터임;; 
    // 결국 pVecMRT 이새끼한테 푸쉬백 한다는게, 멀티 렌타뷰용 map 컨테이너의 second 인(렌타뷰 담은 vector)에 채워진다는 소리?
    vector<CRenderTarget*>*          pVecMRT = Find_MRT(_strMRTTag);

    // 첨에 Add_MRT 가 호출되면서 만약에 Find_MRT 를 통해서 찾은게 없어? 그러면 null 이니까, 그 때 m_mapMRTs에다가 하나씩 저장을 해두는거임.
    // 있으면? 찾은 렌타뷰 벡터에다, 원래의 기존 렌타뷰 찾아놓은거를 넣어주는거임;; 

    if (nullptr == pVecMRT)
    {
        vector<CRenderTarget*>              vecMRT;

        // RenderTarget 은 최대 8개임. 
        vecMRT.reserve(8);
        vecMRT.push_back(pRenderTarget);

        // Find_MRT 를 통해서 찾은 기존 렌타뷰 없을 때, 멀티 렌타뷰 용 Map container 에다가 기존 렌타뷰 담긴 vecMRT 를 넣어줌;; 
        // 그럼 멀티 렌타뷰용 맵의 사이즈가 하나 늘었고.. 
        m_mapMRTs.emplace(_strMRTTag, vecMRT);
    }
    else
    {
        // Find_MRT 를 통해서 찾은 기존 렌타뷰가 있어, 기존 께 잇단말이지, 그럼 그냥 멀티 렌타뷰는 늘어나지는 않고, 
        // 일반적으로 멀티 렌타뷰 용 find 에서 발견한 렌타뷰가 벡터에 담김.
        // 그럼 멀티 렌타뷰용 map 컨테이너 가 담고 있는 벡터에 기존 렌타뷰가 들어간다는 소리임. 
        pVecMRT->push_back(pRenderTarget);
    }

    // MRT 에 쓰려는 렌더타겟들과 기존의,m_mapRenderTargets 에 쓰였던 렌더타겟들과, 이제 공유되는 느낌이라서, Add_Ref 해줘야한다. 
    Safe_AddRef(pRenderTarget);

    return S_OK;
}

HRESULT CRenderTarget_Manager::Begin_MRT(const _wstring& _strMRTTag, _bool _bIsClear)
{
    // context 로부터 Get하는 함수를 가져오는데 이 떄의 함수는, 기존의 백버퍼의 렌더타겟뷰와, 기존의 스텐실버퍼임. 
    m_pContext->OMGetRenderTargets(1, &m_pBackRTV, &m_pOriginalDSV);

    vector<CRenderTarget*>* pVecMRT = Find_MRT(_strMRTTag);
    if (nullptr == pVecMRT)
        return E_FAIL;

    // RenderTarget 초기화 작업 ㄱㄱ 최대 8개인 렌타뷰를 담아줄 배열을 이렇게 초기화 작업해놓는 거임.
    ID3D11RenderTargetView* pRTVs[8] = { nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr };

    _uint           iNumRenderTargets = { 0 };

    // 순회하면서 렌타뷰 배열에, Get_RTV() 를 통해 렌타뷰 가져온 정보를 담아주자.
    for (auto& pRenderTarget : *pVecMRT)
    {
        if (true == _bIsClear)
        {
            // 이걸 해줘야 잔상이 안남음. 
            pRenderTarget->Clear();
        }

        pRTVs[iNumRenderTargets++] = pRenderTarget->Get_RTV();
    }

    // constext 의, OMSetRenderTargets 함수로 세팅해주는데 이 떄 파라미터로, 렌타뷰 갯수와, 그것들을 담은 배열을 그냥 통으로 보내주자. 파라미터
    // 2인자 요구 자료형 보면 ID3D11RenderTargetView* const* 라서 이중포인터란말임? 그래서 pRTVS 의 첫 주소 그냥 줘버리셈(배열은 이름과 같지?)ㅇㅇ
    // 3인자 때 뎁스스텐실이 필요하다 그래서 맨 위에 이거 하기 전에 그냥 context 로부터 OMGetRenderTargets해가지고, 기존의 스텐실 버퍼 꺼내와서 그냥 맥여주자. 
    m_pContext->OMSetRenderTargets(iNumRenderTargets, pRTVs, m_pOriginalDSV);

    return S_OK;
}

HRESULT CRenderTarget_Manager::End_MRT()
{
    // 동시 바인딩이 끝나면 바로 End 호출하는데 여기서 Context의 Set함수로 렌타뷰를 다시 기존의 백버퍼와 뎁스텐실로 ㄱㄱ
    m_pContext->OMSetRenderTargets(1, &m_pBackRTV, m_pOriginalDSV);

    // 위 begin 에서     m_pContext->OMGetRenderTargets(1, &m_pBackRTV, &m_pOriginalDSV);
    // 하면 계속 호출되면서 내부적으로 누수 존나 많이 뜸(AddRef 존나하는중). 그래서 해제 해줘야함;; 
    Safe_Release(m_pOriginalDSV);
    Safe_Release(m_pBackRTV);

    return S_OK;
}

HRESULT CRenderTarget_Manager::Copy_TextureResource(const _wstring& _strRenderTargetTag, ID3D11Texture2D* _pTexture2D)
{
    CRenderTarget* pRenderTarget = Find_RenderTarget(_strRenderTargetTag);
    if (nullptr == pRenderTarget)
        return E_FAIL;

    return pRenderTarget->Copy_TextureResource(_pTexture2D);
}

HRESULT CRenderTarget_Manager::Bind_ShaderResource(const _wstring& _strRenderTargetTag, CShader* _pShader, const _char* _pConstantName)
{
    CRenderTarget* pRenderTarget = Find_RenderTarget(_strRenderTargetTag);

    if (nullptr == pRenderTarget)
        return E_FAIL;

    return pRenderTarget->Bind_ShaderResource(_pShader, _pConstantName);
}

HRESULT CRenderTarget_Manager::Ready_RenderTarget_DebugMode(const _wstring& _strRenderTargetTag, _float _fX, _float _fY, _float _fSizeX, _float _fSizeY)
{
    // 찾아온 RenderTarget.
    CRenderTarget* pRenderTarget = Find_RenderTarget(_strRenderTargetTag);

    if (nullptr == pRenderTarget)
        return E_FAIL;

    return pRenderTarget->Ready_DebugMode(_fX, _fY, _fSizeX, _fSizeY);
}

// _strMRTTag 의 그룹에 있는, 예를 들면 GameObject 이런 것들을 _pShader 로 구동해서, _pVIBuffer 로 그린다~
HRESULT CRenderTarget_Manager::Render_RenderTarget_DebugMode(const _wstring& _strMRTTag, CShader* _pShader, CVIBuffer_Rect* _pVIBuffer)
{
    vector<CRenderTarget*>* pVecMRTs = Find_MRT(_strMRTTag);
    if (nullptr == pVecMRTs)
        return E_FAIL;

    for (auto& pRenderTarget : *pVecMRTs)
    {
        if (nullptr != pRenderTarget)
            pRenderTarget->Render_DebugMode(_pShader, _pVIBuffer);
    }

    return S_OK;
}

CRenderTarget* CRenderTarget_Manager::Find_RenderTarget(const _wstring& _strRenderTargetTag)
{
    auto iter = m_mapRenderTargets.find(_strRenderTargetTag);

    if (iter == m_mapRenderTargets.end())
        return nullptr;

    return iter->second;
}

vector<class CRenderTarget*>* CRenderTarget_Manager::Find_MRT(const _wstring& _strMRTTag)
{
    auto iter = m_mapMRTs.find(_strMRTTag);

    if (iter == m_mapMRTs.end())
        return nullptr;

    return &iter->second;
}

CRenderTarget_Manager* CRenderTarget_Manager::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CRenderTarget_Manager* pInstance = new CRenderTarget_Manager(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CRenderTarget_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRenderTarget_Manager::Free()
{
    __super::Free();

    for (auto& Pair : m_mapMRTs)
    {
        for (auto& pRenderTarget : Pair.second)
            Safe_Release(pRenderTarget);
        Pair.second.clear();
    }
    m_mapMRTs.clear();

    for (auto& Pair : m_mapRenderTargets)
        Safe_Release(Pair.second);
    m_mapRenderTargets.clear();


    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
