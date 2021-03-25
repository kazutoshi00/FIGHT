#pragma once
#include "input.h"

#define PRESERVATION_FRAME (300)
#define COMMAND_LENGTH_MAX (7)	// コマンド成立に必要な入力の個数の最大(最後にNULLを格納するので、その分も含む)

enum UseBottun
{
	UB_Punch1,
	UB_Punch2,
	UB_Punch3,
	UB_Kick1,
	UB_Kick2,
	UB_Kick3,

	UB_Up,
	UB_Down,
	UB_Right,
	UB_Left,

	UB_MAX

};

enum InputDirection
{
	LeftBottom = 1,
	CenterBottom,
	RightBottom,
	LeftMiddle,
	CenterMiddle,
	RightMiddle,
	LeftTop,
	CenterTop,
	RightTop,
};

enum CommandKind
{
	COMMAND_HADOU,
	COMMAND_REVHADOU,
	COMMAND_SHOURYU,
	COMMAND_REVSHOURYU,
	COMMAND_YOGA,
	COMMAND_REVYOGA,
	COMMAND_SHINKUHADOU,
	COMMAND_REVSHINKUHADOU,

	COMMAND_MAX
};

class CInputPreservation
{
private:
	struct KeyStats
	{
		bool saveKeyStats[UB_MAX];
		int progressFrame;
		bool bUse;
		InputDirection InDir;
	};
	char m_NowUseKey[UB_MAX];
	KeyStats m_saveKeystats[PRESERVATION_FRAME];

	struct CommandList
	{
		InputDirection InDir[COMMAND_LENGTH_MAX];
		char name[256];
		int priority;
	};
	CommandList m_commandList[COMMAND_MAX];	// 各種コマンドのデータ：入力方向配列、名前、優先度
	
	struct Active
	{
		bool active;
		int progressFrame;
	};
	Active m_activeCommand[COMMAND_MAX];	// コマンドが成立しているかどうか、成立から何フレーム経過しているか
	bool m_bUseCommand[COMMAND_MAX];	// コマンドがキャラクターで使用されているかどうか

	char nowCommandName[256];
	CommandKind nowCommand;

	InputDirection CalcInputDirection(bool*);
	void SearchActiveCommand(int frame);
	CommandKind FinalCommandCalc();

	ID3D11ShaderResourceView* m_pdebTex;
	InputDirection m_debDirection[20];
	bool m_bDispDeb;
	bool regist;


public:
	CommandKind GetCommand();
	const char* GetUseKey();
	CInputPreservation(bool UseComomand[COMMAND_MAX]);	// キャラクターで使うコマンドをセットする(bool 型配列)
	void SetUseCommand(bool UseComomand[COMMAND_MAX]);	// キャラクターで使うコマンドをセットする(bool 型配列)
	~CInputPreservation();
	void Update();
	void Draw();
	void SetDispDeb(bool);
};

