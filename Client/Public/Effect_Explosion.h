#pragma once
#include "Client_Defines.h"
#include "BlendObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CEffect_Explosion final : public CBlendObject
{
private:
	CEffect_Explosion(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CEffect_Explosion(const CEffect_Explosion& _Prototype);
	virtual ~CEffect_Explosion() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* _pArg) override;
	virtual void				Priority_Update(_float _fTimeDelta) override;
	virtual void				Update(_float _fTimeDelta) override;
	virtual void				Late_Update(_float _fTimeDelta) override;
	virtual HRESULT				Render() override;


	void						Setting_BillBoard();
private:
	CShader*					m_pShaderCom = { nullptr };
	CTexture*					m_pTextureCom = { nullptr };
	CVIBuffer_Rect*				m_pVIBufferCom = { nullptr };
	_float						m_fFrame = {};

	_float						m_fGlobalTime = {};
public:
	HRESULT						Ready_Components();


public:
	static CEffect_Explosion*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*		Clone(void* _pArg) override;
	virtual void				Free() override;
};
END