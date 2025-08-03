#pragma once
#include "Base.h"

BEGIN(Engine)

// abstract : Engine에서의 Level은 부모 클래스, 자식클래스들은 Client프로젝트에서 생성할 것임.
// Engine_DLL 추가해서, Client에 보여주겠다는 양식을 취하자.
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

