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
    // Add_RenderTarget �� �־�J�� ��Ÿ����� �ٽ� �˻��ؼ�, ���� ���⼭ �� ������ ���� �����̳ʿ� ����ִ� ������.
    // ����� �ؾ� ���� ���� ���� �ʳ���? �ϴµ� ���� Find_MRT ��� ���� ��ȯ�� ������ map���ٰ� ��� �ΰ� ã�Ƽ� ��ȯ���ְ� �ֳ�.
    // �׷��� ����  vector<CRenderTarget*>*          pVecMRT = Find_MRT(_strMRTTag); �̰�, map �� ��Ƴ��� �װŸ� �ٽ� ����༭ ������ �Ǵ� ����.

    CRenderTarget* pRenderTarget = Find_RenderTarget(_strRenderTargetTag);

    if (nullptr == pRenderTarget)
        return E_FAIL;
    
    // ��Ƽ ��Ÿ�� �� map �����̳ʿ��� second �� ���͸� ã�Ƽ� ��ȯ���ִ� Find_MRT ��ü�� vector ��ȯŸ���̴� 
    // �̰� pVecMRT ��� �������� ��������;; 
    // �ᱹ pVecMRT �̻������� Ǫ���� �Ѵٴ°�, ��Ƽ ��Ÿ��� map �����̳��� second ��(��Ÿ�� ���� vector)�� ä�����ٴ� �Ҹ�?
    vector<CRenderTarget*>*          pVecMRT = Find_MRT(_strMRTTag);

    // ÷�� Add_MRT �� ȣ��Ǹ鼭 ���࿡ Find_MRT �� ���ؼ� ã���� ����? �׷��� null �̴ϱ�, �� �� m_mapMRTs���ٰ� �ϳ��� ������ �صδ°���.
    // ������? ã�� ��Ÿ�� ���Ϳ���, ������ ���� ��Ÿ�� ã�Ƴ����Ÿ� �־��ִ°���;; 

    if (nullptr == pVecMRT)
    {
        vector<CRenderTarget*>              vecMRT;

        // RenderTarget �� �ִ� 8����. 
        vecMRT.reserve(8);
        vecMRT.push_back(pRenderTarget);

        // Find_MRT �� ���ؼ� ã�� ���� ��Ÿ�� ���� ��, ��Ƽ ��Ÿ�� �� Map container ���ٰ� ���� ��Ÿ�� ��� vecMRT �� �־���;; 
        // �׷� ��Ƽ ��Ÿ��� ���� ����� �ϳ� �þ���.. 
        m_mapMRTs.emplace(_strMRTTag, vecMRT);
    }
    else
    {
        // Find_MRT �� ���ؼ� ã�� ���� ��Ÿ�䰡 �־�, ���� �� �մܸ�����, �׷� �׳� ��Ƽ ��Ÿ��� �þ���� �ʰ�, 
        // �Ϲ������� ��Ƽ ��Ÿ�� �� find ���� �߰��� ��Ÿ�䰡 ���Ϳ� ���.
        // �׷� ��Ƽ ��Ÿ��� map �����̳� �� ��� �ִ� ���Ϳ� ���� ��Ÿ�䰡 ���ٴ� �Ҹ���. 
        pVecMRT->push_back(pRenderTarget);
    }

    // MRT �� ������ ����Ÿ�ٵ�� ������,m_mapRenderTargets �� ������ ����Ÿ�ٵ��, ���� �����Ǵ� �����̶�, Add_Ref ������Ѵ�. 
    Safe_AddRef(pRenderTarget);

    return S_OK;
}

HRESULT CRenderTarget_Manager::Begin_MRT(const _wstring& _strMRTTag, _bool _bIsClear)
{
    // context �κ��� Get�ϴ� �Լ��� �������µ� �� ���� �Լ���, ������ ������� ����Ÿ�ٺ��, ������ ���ٽǹ�����. 
    m_pContext->OMGetRenderTargets(1, &m_pBackRTV, &m_pOriginalDSV);

    vector<CRenderTarget*>* pVecMRT = Find_MRT(_strMRTTag);
    if (nullptr == pVecMRT)
        return E_FAIL;

    // RenderTarget �ʱ�ȭ �۾� ���� �ִ� 8���� ��Ÿ�並 ����� �迭�� �̷��� �ʱ�ȭ �۾��س��� ����.
    ID3D11RenderTargetView* pRTVs[8] = { nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr };

    _uint           iNumRenderTargets = { 0 };

    // ��ȸ�ϸ鼭 ��Ÿ�� �迭��, Get_RTV() �� ���� ��Ÿ�� ������ ������ �������.
    for (auto& pRenderTarget : *pVecMRT)
    {
        if (true == _bIsClear)
        {
            // �̰� ����� �ܻ��� �ȳ���. 
            pRenderTarget->Clear();
        }

        pRTVs[iNumRenderTargets++] = pRenderTarget->Get_RTV();
    }

    // constext ��, OMSetRenderTargets �Լ��� �������ִµ� �� �� �Ķ���ͷ�, ��Ÿ�� ������, �װ͵��� ���� �迭�� �׳� ������ ��������. �Ķ����
    // 2���� �䱸 �ڷ��� ���� ID3D11RenderTargetView* const* �� ���������Ͷ�����? �׷��� pRTVS �� ù �ּ� �׳� �������(�迭�� �̸��� ����?)����
    // 3���� �� �������ٽ��� �ʿ��ϴ� �׷��� �� ���� �̰� �ϱ� ���� �׳� context �κ��� OMGetRenderTargets�ذ�����, ������ ���ٽ� ���� �����ͼ� �׳� �ƿ�����. 
    m_pContext->OMSetRenderTargets(iNumRenderTargets, pRTVs, m_pOriginalDSV);

    return S_OK;
}

HRESULT CRenderTarget_Manager::End_MRT()
{
    // ���� ���ε��� ������ �ٷ� End ȣ���ϴµ� ���⼭ Context�� Set�Լ��� ��Ÿ�並 �ٽ� ������ ����ۿ� �����ٽǷ� ����
    m_pContext->OMSetRenderTargets(1, &m_pBackRTV, m_pOriginalDSV);

    // �� begin ����     m_pContext->OMGetRenderTargets(1, &m_pBackRTV, &m_pOriginalDSV);
    // �ϸ� ��� ȣ��Ǹ鼭 ���������� ���� ���� ���� ��(AddRef �����ϴ���). �׷��� ���� �������;; 
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
    // ã�ƿ� RenderTarget.
    CRenderTarget* pRenderTarget = Find_RenderTarget(_strRenderTargetTag);

    if (nullptr == pRenderTarget)
        return E_FAIL;

    return pRenderTarget->Ready_DebugMode(_fX, _fY, _fSizeX, _fSizeY);
}

// _strMRTTag �� �׷쿡 �ִ�, ���� ��� GameObject �̷� �͵��� _pShader �� �����ؼ�, _pVIBuffer �� �׸���~
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
