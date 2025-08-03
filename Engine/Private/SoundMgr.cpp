#include "SoundMgr.h"

//IMPLEMENT_SINGLETON(CSoundMgr)
CSoundMgr::CSoundMgr()
{
	ZeroMemory(&m_pChannelArr, sizeof(FMOD::Channel*) * MAXCHANNEL);
	m_pSystem = nullptr;
}

HRESULT CSoundMgr::Ready_Sound()
{
	// ���带 ����ϴ� ��ǥ��ü�� �����ϴ� �Լ�
	FMOD::System_Create(&m_pSystem);
	// 1. �ý��� ������, 2. ����� ����ä�� �� , �ʱ�ȭ ���) 
	m_pSystem->init(32, FMOD_INIT_NORMAL, nullptr);
	LoadSoundFile();

	return S_OK;
}

// ���� ä�ο� ���� ���� �־ �Ҹ� ��
void CSoundMgr::Play(const TCHAR* pSoundKey, CHANNELID eID, float fVolume)
{
	map<const TCHAR*, FMOD::Sound*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return;

	m_pSystem->playSound(iter->second, nullptr, false, &m_pChannelArr[eID]);
	m_pChannelArr[eID]->setVolume(fVolume);

	m_pChannelArr[eID]->setMode(FMOD_DEFAULT);

	m_pSystem->update();
}


// ���� ä�ο� ���� ���� �־ �Ҹ� ��
void CSoundMgr::LoopPlay(const TCHAR* pSoundKey, CHANNELID eID, float fVolume)
{
	map<const TCHAR*, FMOD::Sound*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return;

	m_pSystem->playSound(iter->second, nullptr, false, &m_pChannelArr[eID]);
	m_pChannelArr[eID]->setVolume(fVolume);

	m_pChannelArr[eID]->setMode(FMOD_LOOP_NORMAL);

	m_pSystem->update();
}


// ���� ä�ο� ���� ����� ����
void CSoundMgr::PlayOnce(const TCHAR* pSoundKey, CHANNELID eID, float fVolume)
{
	map<const TCHAR*, FMOD::Sound*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return;

	// ���� ��� ���� ���尡 �������� Ȯ��
	bool isPlaying;
	m_pChannelArr[eID]->isPlaying(&isPlaying);

	if (isPlaying)
	{
		FMOD::Sound* pCurrentSound;
		m_pChannelArr[eID]->getCurrentSound(&pCurrentSound);

		// ���� ������ �ٷ� return
		if (pCurrentSound == iter->second)
			return;
		// �ٸ� ����� ������ BGM�� ���߱�
		else
			m_pChannelArr[eID]->stop();
	}

	m_pSystem->playSound(iter->second, nullptr, false, &m_pChannelArr[eID]);
	m_pChannelArr[eID]->setVolume(fVolume);

	m_pSystem->update();
}

// ���� �������̸� return �ٸ� �������̸� ������ ���߰� ����
void CSoundMgr::PlayBGM(const TCHAR* pSoundKey, float fVolume)
{
	map<const TCHAR*, FMOD::Sound*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return;

	// ���� ��� ���� ���尡 �������� Ȯ��
	bool isPlaying;
	m_pChannelArr[SOUND_BGM]->isPlaying(&isPlaying);

	if (isPlaying)
	{
		FMOD::Sound* pCurrentSound;
		m_pChannelArr[SOUND_BGM]->getCurrentSound(&pCurrentSound);

		// ���� ������ �ٷ� return
		if (pCurrentSound == iter->second)
			return; 
		// �ٸ� ����� ������ BGM�� ���߱�
		else
			m_pChannelArr[SOUND_BGM]->stop();
		
	}

	m_pSystem->playSound(iter->second, nullptr, false, &m_pChannelArr[SOUND_BGM]);
	m_pChannelArr[SOUND_BGM]->setMode(FMOD_LOOP_NORMAL);
	m_pChannelArr[SOUND_BGM]->setVolume(fVolume);

	m_pSystem->update();
}

// ���� ä�ο��� ������̸� ������ ���߰� ������ ���
// �̱����� �÷��� ����
void CSoundMgr::PlaySelfish(const TCHAR* pSoundKey, CHANNELID eID, float fVolume)
{
	map<const TCHAR*, FMOD::Sound*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return;

	bool isPlaying = false;
	if (m_pChannelArr[eID])
	{
		m_pChannelArr[eID]->isPlaying(&isPlaying);
		if (isPlaying)
		{
			m_pChannelArr[eID]->stop();
		}
	}

	m_pSystem->playSound(iter->second, nullptr, false, &m_pChannelArr[eID]);
	m_pChannelArr[eID]->setVolume(fVolume);

	m_pSystem->update();
}

// ���� ä�ο��� ����ǰ� ������ �ٸ� ���忩�� return
// �ҽ��� �÷���
void CSoundMgr::PlayTimid(const TCHAR* pSoundKey, CHANNELID eID, float fVolume)
{
	map<const TCHAR*, FMOD::Sound*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return;

	bool isPlaying = false;
	if (m_pChannelArr[eID])
	{
		m_pChannelArr[eID]->isPlaying(&isPlaying);
		if (isPlaying)
		{
			return;
		}
	}

	m_pSystem->playSound(iter->second, nullptr, false, &m_pChannelArr[eID]);
	m_pChannelArr[eID]->setVolume(fVolume);

	m_pSystem->update();
}

void CSoundMgr::StopSound(CHANNELID eID)
{
	if (m_pChannelArr[eID])
	{
		m_pChannelArr[eID]->stop();
	}
}

void CSoundMgr::StopAll()
{
	for (int i = 0; i < MAXCHANNEL; ++i)
	{
		if (m_pChannelArr[i])
		{
			m_pChannelArr[i]->stop();
		}
	}
}

void CSoundMgr::SetChannelVolume(CHANNELID eID, float fVolume)
{
	if (m_pChannelArr[eID])
	{
		m_pChannelArr[eID]->setVolume(fVolume);
	}
	m_pSystem->update();
}

void CSoundMgr::LoadSoundFile()
{
	// _finddata_t : <io.h>���� �����ϸ� ���� ������ �����ϴ� ����ü
	_finddata_t fd;

	// _findfirst : <io.h>���� �����ϸ� ����ڰ� ������ ��� ������ ���� ù ��° ������ ã�� �Լ�
	intptr_t handle = _findfirst("../Bin/Resources/Sound/*.*", &fd);

	if (handle == -1)
		return;

	int iResult = 0;

	char szCurPath[128] = "../Bin/Resources/Sound/";	 // ��� ���
	char szFullPath[128] = "";

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);

		// "../Sound/" + "Success.wav"
		strcat_s(szFullPath, fd.name);
		// "../Sound/Success.wav"

		FMOD::Sound* pSound = nullptr;

		FMOD_RESULT eRes = m_pSystem->createSound(szFullPath, FMOD_DEFAULT, 0, &pSound);

		if (eRes == FMOD_OK)
		{
			int iLength = strlen(fd.name) + 1;

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

			// �ƽ�Ű �ڵ� ���ڿ��� �����ڵ� ���ڿ��� ��ȯ�����ִ� �Լ�
			MultiByteToWideChar(CP_ACP, 0, fd.name, -1, pSoundKey, static_cast<int>(iLength));

			m_mapSound.emplace(pSoundKey, pSound);
		}
		//_findnext : <io.h>���� �����ϸ� ���� ��ġ�� ������ ã�� �Լ�, ���̻� ���ٸ� -1�� ����
		iResult = _findnext(handle, &fd);
	}

	m_pSystem->update();

	_findclose(handle);
}

CSoundMgr* CSoundMgr::Create()
{
	CSoundMgr* pInstance = new CSoundMgr();

	if (FAILED(pInstance->Ready_Sound()))
	{
		MSG_BOX("Failed to Created : SoundMgr");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSoundMgr::Free()
{
	__super::Free();

	for (auto& Mypair : m_mapSound)
	{
		delete[] Mypair.first;
		Mypair.second->release();
	}
	m_mapSound.clear();

	m_pSystem->close();
	m_pSystem->release();
}
