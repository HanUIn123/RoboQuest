#include "../Public/GameObject.h"
#include "GameInstance.h"

CGameObject::CGameObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : m_pDevice(_pDevice)
    , m_pContext(_pContext)
    , m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

CGameObject::CGameObject(const CGameObject& _Prototype)
    : m_pDevice(_Prototype.m_pDevice)
    , m_pContext(_Prototype.m_pContext)
    , m_pGameInstance(_Prototype.m_pGameInstance)
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CGameObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGameObject::Initialize(void* _pArg)
{
    GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(_pArg);

    // ��������� �����س���(Component �� �ڽ� transform) transform ������Ʈ, 
    m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == m_pTransformCom)
        return E_FAIL;

    if (FAILED(m_pTransformCom->Initialize(_pArg)))
        return E_FAIL;




    m_mapComponent.emplace(g_strTransformTag, m_pTransformCom);
    Safe_AddRef(m_pTransformCom);

    return S_OK;
}

void CGameObject::Priority_Update(_float _fTimeDelta)
{
}

void CGameObject::Update(_float _fTimeDelta)
{
}

void CGameObject::Late_Update(_float _fTimeDelta)
{
}

HRESULT CGameObject::Render()
{
    return S_OK;
}

CComponent* CGameObject::Find_Component(const _wstring& _strComponentTag)
{
    auto iter = m_mapComponent.find(_strComponentTag);

    if (iter == m_mapComponent.end())
        return nullptr;

    return iter->second;
}

HRESULT CGameObject::Add_Component(_uint _iPrototypeLevelIndex, const _wstring& _strPrototypeTag, const _wstring& _strComponentTag, CComponent** _ppOut, void* _pArg)
{
    // ���� ������ ���� Ű���� ������ ���� ���� ��ü�� �����Ѵ�!
    // �� ����� �纻��ü�� pComponent ������ ��Ƽ�, map �����̳ʿ� ����� value������ ���� ����  
    CComponent* pComponent = dynamic_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::TYPE_COMPONENT, _iPrototypeLevelIndex, _strPrototypeTag, _pArg));
    if (pComponent == nullptr)
        return E_FAIL;

    // nullptr�� �ƴ϶��, �ߺ��� �ִٶ�� ���̹Ƿ�, e_fail!
    if (nullptr != Find_Component(_strComponentTag))
        return E_FAIL;

    // key & value
    m_mapComponent.emplace(_strComponentTag, pComponent);

    // �ּҸ� �޾ƿ;� �ϹǷ� ������������ �� ( ������ ) 
    *_ppOut = pComponent;

    Safe_AddRef(pComponent);

    return S_OK;
}

void CGameObject::Free()
{
    __super::Free();

    for (auto& Pair : m_mapComponent)
        Safe_Release(Pair.second);
    m_mapComponent.clear();

    Safe_Release(m_pTransformCom);
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
