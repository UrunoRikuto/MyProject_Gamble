/**************************************************//*
	@file	| Blackjack_Croupier.cpp
	@brief	| ブラックジャックディーラークラスのcppファイル
	@note	| ブラックジャックディーラーのゲームオブジェクトを管理する
*//**************************************************/
#include "Blackjack_Croupier.h"
#include "Main.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CBlackjack_Croupier::CBlackjack_Croupier()
	: m_Cards()
{
}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CBlackjack_Croupier::~CBlackjack_Croupier()
{
}

/*****************************************//*
	@brief　	| 初期化処理
*//*****************************************/
void CBlackjack_Croupier::Init()
{
	// 基底クラスの初期化処理
	CGameObject::Init();

	// カードリストの初期化
	m_Cards.clear();
}

/*****************************************//*
	@brief　	| 更新処理
*//*****************************************/
void CBlackjack_Croupier::Update()
{
	// カードの位置を設定
	// 開始位置
	DirectX::XMFLOAT3 startPos = DirectX::XMFLOAT3(-15.0f, 30.0f, 0.0f);
	// カード間の間隔
	DirectX::XMFLOAT3 offset = DirectX::XMFLOAT3(15.0f, 0.0f, 0.0f);
	for (size_t i = 0; i < m_Cards.size(); ++i)
	{
		CPlayingCard* pCard = m_Cards[i];
		if (pCard)
		{
			// 位置の計算
			DirectX::XMFLOAT3 cardPos = DirectX::XMFLOAT3(
				startPos.x + offset.x * static_cast<float>(i),
				startPos.y + offset.y * static_cast<float>(i),
				startPos.z + offset.z * static_cast<float>(i)
			);
			// カードの位置を設定
			pCard->SetPos(cardPos);
		}
	}
	// 基底クラスの更新処理
	CGameObject::Update();
}

/*****************************************//*
	@brief　	| ディーラーにカードを追加する
	@param		| tCardInfo：追加するカード情報構造体
*//*****************************************/
void CBlackjack_Croupier::AddCard(CPlayingCard::Info tCardInfo, bool bFaceUp)
{
	// トランプカードオブジェクトの生成
	CPlayingCard* pCard = GetScene()->AddGameObject<CPlayingCard>(Tag::GameObject, "PlayingCard");
	pCard->Setting(tCardInfo.m_eSuit, tCardInfo.m_nNumber, bFaceUp);
	// カードリストに追加
	m_Cards.push_back(pCard);
}