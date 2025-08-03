#pragma once
#include "Base.h"

BEGIN(Engine)

// abstract : Engine������ Level�� �θ� Ŭ����, �ڽ�Ŭ�������� Client������Ʈ���� ������ ����.
// Engine_DLL �߰��ؼ�, Client�� �����ְڴٴ� ����� ������.
class ENGINE_DLL CLevel abstract : public CBase
{
protected: 
    CLevel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual ~CLevel() = default;

public:
    virtual HRESULT Initialize();
    virtual void Update(_float _fTimeDelta);
    virtual HRESULT Render();
protected:
    ID3D11Device*                   m_pDevice = { nullptr };
    ID3D11DeviceContext*            m_pContext = { nullptr };
    class CGameInstance*            m_pGameInstance = { nullptr };

public:
    virtual void Free() override;
};
END

