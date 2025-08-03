#pragma once
#include "Client_Defines.h"
#include "EffectPartObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)
class CBoss_Explosion final : public CEffectPartObject
{
public:
	struct BOSS_EXPLO_DESC : public CEffectPartObject::EFFECT_PARTOBJECT_DESC
	{

	};

public:
	void Set_Activate_Effect(_bool _bActivate) { m_bActivate = _bActivate; }

public:
	void Set_CombineMatrix(_float4x4* _pCombineMatrix) { m_CombinedWorldMatrix = *_pCombineMatrix; }

private:
	CBoss_Explosion(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CBoss_Explosion(const CBoss_Explosion& _Prototype);
	virtual ~CBoss_Explosion() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* _pArg) override;
	virtual void				Priority_Update(_float _fTimeDelta) override;
	virtual void				Update(_float _fTimeDelta) override;
	virtual void				Late_Update(_float _fTimeDelta) override;
	virtual HRESULT				Render() override;

	void						Setting_BillBoard();
	void						DeActivate_Effect();

private:
	CShader*					m_pShaderCom = { nullptr };
	CTexture*					m_pTextureCom = { nullptr };
	CVIBuffer_Rect*				m_pVIBufferCom = { nullptr };
	_float						m_fFrame = {};
	_bool						m_bActivate = { false };

public:
	HRESULT						Ready_Component();

public:
	static CBoss_Explosion*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*		Clone(void* _pArg) override;
	virtual void				Free() override;
};
END
