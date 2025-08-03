#pragma once
#include "Client_Defines.h"
#include "EffectPartObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)
class CFire_Frame final : public CEffectPartObject
{
public:
	struct FIRE_EFFECT_DESC : public CEffectPartObject::EFFECT_PARTOBJECT_DESC
	{

	};
public:
	void Set_Activate_Effect(_bool _bActivate) { m_bActivate = _bActivate; }
	void Set_Position(_vector _vCurrentPos);
	//void Hold_Position(_vector _vCurrentPos);

	_bool IsExplosion_Finished() { return m_bFinish; }
public:
	void Set_CombineMatrix(_float4x4* _pCombineMatrix) { m_CombinedWorldMatrix = *_pCombineMatrix; }

private:
	CFire_Frame(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CFire_Frame(const CFire_Frame& _Prototype);
	virtual ~CFire_Frame() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* _pArg) override;
	virtual void				Priority_Update(_float _fTimeDelta) override;
	virtual void				Update(_float _fTimeDelta) override;
	virtual void				Late_Update(_float _fTimeDelta) override;
	virtual HRESULT				Render() override;

	void						Setting_BillBoard();
	void						DeActivate_Effect();

	//void						Set_Position(_vector _vPosition);


private:
	CShader*					m_pShaderCom = { nullptr };
	CTexture*					m_pTextureCom = { nullptr };
	CVIBuffer_Rect*				m_pVIBufferCom = { nullptr };
	_float						m_fFrame = {};
	_bool						m_bActivate = { false };
	_bool						m_bFinish = { false };

public:
	HRESULT						Ready_Component();

public:
	static CFire_Frame*			Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*		Clone(void* _pArg) override;
	virtual void				Free() override;
};
END
