#pragma once
//#include "Base.h"

#include "Shader.h"
#include "Texture.h"
#include "Transform.h"

#include "VIBuffer_Rect.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_MapToolTerrain.h"
#include "VIBuffer_Cube.h"
#include "SkyBoxCube.h"
#include "VIBuffer_Point_Instancing.h"
#include "Rectangle_Cube.h"
#include "VIBuffer_VerRect.h"
#include "TrailBuffer.h"


#include "Model.h"
#include "Navigation.h"
#include "Collider.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

// 이 Prototype_Manager에서 복제한 객체들을, Object_Manager에 담아두고 작업하겠다.
BEGIN(Engine)
class CPrototype_Manager final : public CBase
{
private:
    CPrototype_Manager();
    virtual ~CPrototype_Manager() = default;
    
public:
    HRESULT Initialize(_uint _iNumLevels);
    HRESULT Add_Prototype(_uint _iLevelIndex, const _wstring& _strPrototypeTag, CBase* _pPrototype);

    // 무슨 객체 타입인지 (gameobject / component 인지 ) 나눠서, 찾은 원형객체의 클론을 CBase 부모타입 클래스 형식으로 return 한다.->GameObject로 형변환 해줄 것임.
    // HRESULT Add_GameObject_To_Layer(_uint _iProtoLevelIndex, const _wstring& _strProtoTag, _uint _iLevelIndex, const _wstring& _strLayerTag, void* _pArg = nullptr);
    CBase* Clone_Prototype(PROTOTYPE _eType, _uint _iLevelIndex, const _wstring& _strPrototypeTag, void* _pArg);
    void Clear(_uint _iLevelIndex);


private:
    _uint       m_iNumLevels = {};

    map<const _wstring, CBase*>* m_mapPrototype = { nullptr };
    typedef map<const _wstring, CBase*> PROTOTYPES;

private:
    CBase* Find_Prototype(_uint _iLevelIndex, const _wstring& _strPrototypeTag);

public:
    static CPrototype_Manager* Create(_uint _iNumLevels);
    virtual void Free() override;

};

END