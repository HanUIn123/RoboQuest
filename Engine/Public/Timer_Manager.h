#pragma once
#include "Base.h"

BEGIN(Engine)

class CTimer;
class CTimer_Manager : public CBase
{
private:
    CTimer_Manager();
    virtual ~CTimer_Manager() = default;

public:
    _float                              Get_TimeDelta(const _wstring& _strTimerTag);
    void                                Set_TimeDelta(const _wstring& _strTimerTag, _float _fTimeScale);
public:
    HRESULT                             Add_Timer(const _wstring& _strTimerTag);
    void                                Compute_TimeDelta(const _wstring& _strTimerTag);

private:
    CTimer*                             Find_Timer(const _wstring& _strTimerTag);
    // can use Class CTimer* instead Class CTimer in front of Class CTiemr_Manager...
private:
    map<const _wstring, CTimer*>        m_Timers;

public:
    static CTimer_Manager*              Create();
    virtual void                        Free();
};

END

