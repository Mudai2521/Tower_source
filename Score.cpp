#include "Score.h"

using namespace DirectX;

Score::Score() :
	m_CharactorState{ CharactorState(
		XMFLOAT2(32.0f, 64.0f),
		0.0f,
		XMFLOAT2(0.0f, 0.0f)),
		CharactorState(
		XMFLOAT2(64.0f, 64.0f),
		0.0f,
		XMFLOAT2(0.0f, 0.0f)) }
{
}

Score::~Score()
{
	Term();
}

bool Score::Init(ID3D12Device* pDevice, ID3D12CommandQueue* pQueue, DescriptorPool* pPool, UINT width, UINT height)
{
	if (!m_spritedata.Init(L"Sprite/Number.dds", pDevice, pQueue, pPool, width, height, ScoreDigitMax+3, ScoreDigitMax+3))throw std::exception();
	if (!m_spritedata.AddSprite(L"Sprite/m.dds", pDevice, pQueue, pPool))throw std::exception();
	if (!m_spritedata.AddSprite(L"Sprite/NumberBG.dds", pDevice, pQueue, pPool))throw std::exception();
	m_width = width;
	m_height = height;

	scoreMarginY = m_height / 6;

	m_CharactorState[0].Trans = XMFLOAT2(m_width- scoreMarginX - m_CharactorState[1].Scale.x - m_CharactorState[0].Scale.x / 2- charaInterval, m_height - scoreMarginY);
	m_CharactorState[1].Trans = XMFLOAT2(m_width - scoreMarginX - m_CharactorState[1].Scale.x / 2, m_height -scoreMarginY);

	return true;
}

void Score::Term()
{
	m_spritedata.Term();
}

void Score::SetScore(UINT score) 
{
	ScoreNumArray.clear();
	while (true)
	{
		if (score / 10 > 0)
		{
			ScoreNumArray.push_back(score % 10);
			score /= 10;
		} else 
		{
			ScoreNumArray.push_back(score % 10);
			break;
		}
	
	}
}

void Score::DrawSprite(ID3D12GraphicsCommandList* pCmdList, UINT Score)
{
	SetScore(Score);

	float BoxTransX = (m_CharactorState[0].Trans.x - m_CharactorState[0].Scale.x * (float(ScoreNumArray.size()) - 1.0f + 0.5f) - (ScoreNumArray.size() - 1) * charaInterval)
		+ (m_CharactorState[1].Trans.x + m_CharactorState[1].Scale.x / 2);
	BoxTransX /= 2.0f;
	float BoxScaleX = (m_CharactorState[0].Trans.x - m_CharactorState[0].Scale.x * (float(ScoreNumArray.size()) - 1.0f + 0.5f) - (ScoreNumArray.size() - 1) * charaInterval)
		- (m_CharactorState[1].Trans.x + m_CharactorState[1].Scale.x / 2);
	BoxScaleX = abs(BoxScaleX);
	float BoxScalePlus = 20.0f;
	m_spritedata.SetSpriteSheet(1, 1, 1, 1, 1);
	m_spritedata.SetWorldMatrix(XMFLOAT2(BoxScaleX + BoxScalePlus, m_CharactorState[0].Scale.y+ BoxScalePlus), 0.0f,
		XMFLOAT2(BoxTransX, m_CharactorState[0].Trans.y), 1);
	m_spritedata.Draw(pCmdList, 1, 2, 1);

	m_spritedata.SetWorldMatrix(m_CharactorState[1].Scale, m_CharactorState[1].Rotate, m_CharactorState[1].Trans, 0);
	m_spritedata.Draw(pCmdList, 0,1,0);

	for (int i = 0; i < ScoreNumArray.size(); i++) 
	{
		m_spritedata.SetSpriteSheet(10, 1, ScoreNumArray[i]+1, 1, i + 3);
		m_spritedata.SetWorldMatrix(m_CharactorState[0].Scale, m_CharactorState[0].Rotate,
			XMFLOAT2(m_CharactorState[0].Trans.x- m_CharactorState[0].Scale.x*i-i* charaInterval, m_CharactorState[0].Trans.y), i + 3);
		m_spritedata.Draw(pCmdList, i + 3,0, i + 3);
	}

	
}