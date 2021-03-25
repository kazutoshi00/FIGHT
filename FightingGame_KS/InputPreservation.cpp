#include "InputPreservation.h"
#include "input.h"
#include "debugproc.h"
#include "Texture.h"
#include "polygon.h"

#define INPUT_BUFFER_FRAME (50)
#define INPUT_GRACE_INTERVAL (10)	// ���͗P�\�t���[��(�R�}���h����������ɂ́A���ꂼ��̓��͂̊Ԋu�����̒l�ȓ��ɂ��Ȃ���΂Ȃ�Ȃ�)
#define INPUT_DIRECTION_TEX ("data/texture/InputDirection.png")

const int com[][7] = {
{ 2,3,6 ,NULL,NULL,NULL,NULL,},
{ 2,1,4 ,NULL,NULL,NULL,NULL,},
{ 6,2,3 ,NULL,NULL,NULL,NULL,},
{ 4,2,1 ,NULL,NULL,NULL,NULL,},
{ 4,1,2,3,6 ,NULL,NULL,},
{ 6,3,2,1,4 ,NULL,NULL,},
{ 2,3,6,2,3,6,NULL,},
{ 2,1,4,2,1,4,NULL,}, };

const char comname[][256] = {
	{"��޳"},
	{"��Ϸ"},
	{"���ح�"},
	{"�ެ����ح�"},
	{"ֶ�"},
	{"�ެ�ֶ�"},
	{"�ݸ���޳"},
	{"�ݸ���Ϸ"},
};

CInputPreservation::CInputPreservation(bool usecommand[COMMAND_MAX])
{
	ID3D11Device* pDevice = GetDevice();
	CreateTextureFromFile(pDevice,					// �f�o�C�X�ւ̃|�C���^
		INPUT_DIRECTION_TEX,		// �t�@�C���̖��O
		&m_pdebTex);	// �ǂݍ��ރ������[
	
	for (int i = 0; i < 20; i++)
	{
		m_debDirection[i] = CenterMiddle;
	
	}
	m_bDispDeb = true;
	regist = false;
	for (int i = 0; i < COMMAND_MAX; i++)
	{
		for (int j = 0; j < COMMAND_LENGTH_MAX; j++)
		{
			m_commandList[i].InDir[j] = (InputDirection)com[i][j];
		}
		strcpy(m_commandList[i].name,comname[i]);
		m_commandList[i].priority = (i / 2) + 1;
		m_activeCommand[i].active = false;
	}
	m_NowUseKey[UB_Punch1] = VK_J;
	m_NowUseKey[UB_Punch2] = VK_I;
	m_NowUseKey[UB_Punch3] = VK_9;
	m_NowUseKey[UB_Kick1] = VK_K;
	m_NowUseKey[UB_Kick2] = VK_O;
	m_NowUseKey[UB_Kick3] = VK_0;
	m_NowUseKey[UB_Up] = VK_W;
	m_NowUseKey[UB_Down] = VK_S;
	m_NowUseKey[UB_Left] = VK_A;
	m_NowUseKey[UB_Right] = VK_D;

	// �L�[���͏��ۑ�����N���A����
	for (int i = 0; i < PRESERVATION_FRAME; i++)
	{
		m_saveKeystats[i].progressFrame = 0;
		m_saveKeystats[i].bUse = false;
	}

	SetUseCommand(usecommand);
}


CInputPreservation::~CInputPreservation()
{
	SAFE_RELEASE(m_pdebTex);
}

void CInputPreservation::Update()
{
	for (int i = 0; i < PRESERVATION_FRAME; i++)
	{
		if (++(m_saveKeystats[i].progressFrame) > PRESERVATION_FRAME)
		{
			m_saveKeystats[i].bUse = false;
		}
	}
	regist = false;
	for (int i = 0; i < PRESERVATION_FRAME; i++)
	{
		if (m_saveKeystats[i].bUse)continue;
		m_saveKeystats[i].bUse = true;
		m_saveKeystats[i].progressFrame = 0;
		for (int j = 0; j < UB_MAX; j++)
		{
			m_saveKeystats[i].saveKeyStats[j] = GetKeyPress((int)m_NowUseKey[j]);
		}
		m_saveKeystats[i].InDir = CalcInputDirection(m_saveKeystats[i].saveKeyStats);
		PrintDebugProc("NowCommand:%d\n", (int)m_saveKeystats[i].InDir);
		break;
	}
	for (int i = 0; i < COMMAND_MAX; i++)
	{
		m_activeCommand[i].active = false;
	}

	for (int i = INPUT_BUFFER_FRAME; i >= 0; i--)
	{
		SearchActiveCommand(i);
	}
	nowCommand = FinalCommandCalc();
	//PrintDebugProc("NowAttack:%s\n", nowCommandName);
}
void CInputPreservation::Draw()
{
	if (!m_bDispDeb)return;
	SetZBuffer(false);
	SetCullMode(CULLMODE_CCW);	// �w�ʃJ�����O(����`���Ȃ�)
	for (int i = 0; i < 20; i++)
	{
		if (m_debDirection[i] == CenterMiddle)
		{
			break;
		}
		float angle = 0.0f;
		switch (m_debDirection[i])
		{
		case LeftBottom:
			angle = 225.0f;
			break;
		case CenterBottom:
			angle = 270.0f;
			break;
		case RightBottom:
			angle = 315.0f;
			break;
		case LeftMiddle:
			angle = 180.0f;
			break;
		case RightMiddle:
			angle = 0.0f;
			break;
		case LeftTop:
			angle = 135.0f; 
			break;
		case CenterTop:
			angle = 90.0f; 
			break;
		case RightTop:
			angle = 45.0f;
			break;
		}
		SetPolygonSize(20.0f, 20.0f);
		SetPolygonColor(1.0f, 1.0f, 1.0f);
		SetPolygonPos(-400.0f, 300.0f - i * 21.0f);
		SetPolygonAngle(angle);
		SetPolygonAlpha(1.0f);
		SetPolygonTexture(m_pdebTex);
		SetPolygonFrameSize(1, 1);	// �ǂݍ��񂾉摜�̕�����
		SetPolygonUV(1.0f, 1.0f);
		UpdatePolygon();
		DrawPolygon(GetDeviceContext());

	}
	SetCullMode(CULLMODE_CW);	// �O�ʃJ�����O(�\��`���Ȃ�)
	SetZWrite(true);
}

//********************************************************
// ���݂̃L�[���͂���A���o�[�Ƃ������̓��͕������v�Z����
//********************************************************
InputDirection CInputPreservation::CalcInputDirection(bool* pks)
{
	int x = 0, y = 0;
	InputDirection ret = CenterMiddle;
	// ���ꂼ��̓��͏󋵂�����͕�����]��
	if (pks[UB_Left])
	{
		x = -1;
		if (pks[UB_Right])
		{// ���������͐^��
			x = 0;
		}
	}
	else if (pks[UB_Right])
	{
		x = 1;
	}
	// �㉺�͏���������͂���Ă�����A�����炪�D�悳���
	if (pks[UB_Up])
	{
		y = 1;
	}
	else if (pks[UB_Down])
	{
		y = -1;
	}

	if (y < 0)
	{
		if (x < 0) { ret = LeftBottom; }
		else if (x > 0) { ret = RightBottom; }
		else { ret = CenterBottom; }
	}
	else if (y > 0)
	{
		if (x < 0) { ret = LeftTop; }
		else if (x > 0) { ret = RightTop; }
		else { ret = CenterTop; }
	}
	else
	{
		if (x < 0) { ret = LeftMiddle; }
		else if (x > 0) { ret = RightMiddle; }
		else { ret = CenterMiddle; }
	}
	if (!regist)
	{
		for (int i = 0; i < 20; i++)
		{
			if (m_debDirection[i] == CenterMiddle)
			{
				if (ret != CenterMiddle && m_debDirection[i - 1] != ret)
				{
					m_debDirection[i] = ret;
					regist = true;
					break;
				}
			}
		}
	}
	if (!regist)
	{
		if (ret != CenterMiddle && m_debDirection[19] != ret)
		{
			for (int i = 0; i < 19; i++)
			{
				m_debDirection[i] = m_debDirection[i + 1];
			}
			m_debDirection[19] = ret;
		}
	}
	return ret;
}

//*******************************************************************
// ���݊i�[����Ă�����͕�������A�R�}���h���������Ă��邩�𒲂ׂ�
//*******************************************************************
void CInputPreservation::SearchActiveCommand(int frame)
{
	bool fin[COMMAND_MAX];	// �e��R�}���h�̔��肪�I���������ǂ���
	int count[COMMAND_MAX];	// �e��R�}���h�̉��ڂ܂œ��͂��������Ă��邩
	int intervaltime[COMMAND_MAX];	// ���͕������Ȃ���P�\����
	int fincount = 0;	// ���͔��肪�I��������
	for (int i = 0; i < COMMAND_MAX; i++)
	{
		// �g��Ȃ��R�}���h�͏I��点�Ă���
		if (m_bUseCommand[i])
		{
			fin[i] = false;
		}
		else
		{
			fin[i] = true;
			fincount++;
		}
		count[i] = 0;
		intervaltime[i] = frame;
	}
	for (int i = frame;fincount < COMMAND_MAX ||  i >= 0; i--)
	{
		int idx = 0;
		for (; idx < PRESERVATION_FRAME; idx++)
		{
			if (m_saveKeystats[idx].progressFrame == i)
			{
				break;
			}
		}
		for (int j = 0; j < COMMAND_MAX; j++)
		{
			if (fin[j])continue;
			intervaltime[j]--;
			if (intervaltime[j] < 0)
			{
				fincount++;
				fin[j] = true;
				continue;
			}
			if (m_saveKeystats[idx].InDir == m_commandList[j].InDir[count[j]])
			{
				count[j]++;
				intervaltime[j] = INPUT_GRACE_INTERVAL;
				if (m_commandList[j].InDir[count[j]] == NULL)
				{// �R�}���h���������Ă�����
					fincount++;
					fin[j] = true;
					m_activeCommand[j].active = true;
					m_activeCommand[j].progressFrame = i;
				}
			}
		}
	}
}
//*******************************************************
// �������Ă���R�}���h����A�D��x�̍������̂Ɍ��肷��
//*******************************************************
CommandKind CInputPreservation::FinalCommandCalc()
{
	int selectIndex = -1;
	for (int i = 0; i < COMMAND_MAX; i++)
	{
		if (!m_activeCommand[i].active) { continue; }
		if (selectIndex < 0) { selectIndex = i; continue; }	// ��ڂȂ�i�[���Ď���
		if(m_commandList[i].priority > m_commandList[selectIndex].priority)
		{	// �D��x�������Ȃ�i�[���Ď���
			selectIndex = i; 
			continue; 
		}
		else if (m_commandList[i].priority == m_commandList[selectIndex].priority)
		{	// �D��x���ꏏ�Ȃ�A�Ō�ɗL���ɂȂ����R�}���h���D��
			if (m_activeCommand[i].progressFrame < m_activeCommand[selectIndex].progressFrame)
			{	// �o�߃t���[�����������ق����x���ɗL���ɂȂ��Ă���
				selectIndex = i;
			}

		}
	}
	if (selectIndex >= 0)
	{
		strcpy(nowCommandName, m_commandList[selectIndex].name);
		return (CommandKind)selectIndex;
	}
	else
	{
		strcpy(nowCommandName, "");
	}
	return COMMAND_MAX;
}
//******************************************************
// �L�����N�^�[�Ŏg���R�}���h���Z�b�g����(bool �^�z��)
//******************************************************
void CInputPreservation::SetUseCommand(bool UseComomand[COMMAND_MAX])
{
	for (int i = 0; i < COMMAND_MAX; i++)
	{
		m_bUseCommand[i] = UseComomand[i];
	}
}

//******************************************************
// �O������̃R�}���h�̎擾
//******************************************************
CommandKind CInputPreservation::GetCommand()
{
	return nowCommand;
}

//******************************************************
// �ݒ肳��Ă���L�[�̎擾
//******************************************************
const char* CInputPreservation::GetUseKey()
{
	return m_NowUseKey;
}

void CInputPreservation::SetDispDeb(bool bDisp)
{
	m_bDispDeb = bDisp;
}