#pragma once
#include "Base.h"

BEGIN(Engine)

class CTimer : public CBase
{
private:
    CTimer();
    virtual ~CTimer() = default;

public:
    _float                  Get_TimeDelta() const { return m_fTimeDelta * m_fTimeScale; }

    void                    Set_TimeDelta(_float _fTimeScale) { m_fTimeScale = _fTimeScale; }

public:
    HRESULT                 Ready_Timer();
    void                    Update_Timer();

private:
    LARGE_INTEGER           m_FrameTime = {};
    LARGE_INTEGER           m_FixTime = {};
    LARGE_INTEGER           m_LastTime = {};
    LARGE_INTEGER           m_CpuTick = {};

    _float                  m_fTimeDelta = {};
    _float                  m_fTimeScale = { 1.0f };


public:
    static CTimer*          Create();
    virtual void            Free();
};

END

