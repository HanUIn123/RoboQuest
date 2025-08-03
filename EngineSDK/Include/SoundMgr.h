#pragma once

#include "Base.h"
#include "Engine_Define.h"
#include "Engine_Struct.h"


BEGIN(Engine)

class /*ENGINE_DLL*/ CSoundMgr : public CBase
{
    //DECLARE_SINGLETON(CSoundMgr)
private:
	CSoundMgr();
	virtual ~CSoundMgr() = default;

public:
    HRESULT Ready_Sound();

public:
    void Play(const TCHAR* pSoundKey, CHANNELID eID, float fVolume);
    void LoopPlay(const TCHAR* pSoundKey, CHANNELID eID, float fVolume);
    void PlayOnce(const TCHAR* pSoundKey, CHANNELID eID, float fVolume);
    void PlayBGM(const TCHAR* pSoundKey, float fVolume);
    void PlaySelfish(const TCHAR* pSoundKey, CHANNELID eID, float fVolume);
    void PlayTimid(const TCHAR* pSoundKey, CHANNELID eID, float fVolume);

    void StopSound(CHANNELID eID);
    void StopAll();
    void SetChannelVolume(CHANNELID eID, float fVolume);

private:
    void LoadSoundFile();

private:
	map<const TCHAR*, FMOD::Sound*> m_mapSound;
	FMOD::Channel* m_pChannelArr[MAXCHANNEL];
	FMOD::System* m_pSystem;

public:
    static CSoundMgr*          Create();
	virtual void		Free() override;

};

END