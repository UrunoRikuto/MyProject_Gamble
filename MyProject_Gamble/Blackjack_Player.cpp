/**************************************************//*
	@file	| Blackjack_Player.cpp
	@brief	| ブラックジャックプレイヤークラスのcppファイル
	@note	| カードの管理やチップの管理をする
*//**************************************************/
#include "Blackjack_Player.h"
#include "Main.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CBlackjack_Player::CBlackjack_Player()
	: m_Cards()
{
}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CBlackjack_Player::~CBlackjack_Player()
{
}

/*****************************************//*
	@brief　	| 初期化処理
*//*****************************************/
void CBlackjack_Player::Init()
{
	// 基底クラスの初期化処理
	CGameObject::Init();

	// カードリストの初期化
	m_Cards.clear();
}

/*****************************************//*
	@brief　	| 更新処理
*//*****************************************/
void CBlackjack_Player::Update()
{
	// カードの位置を設定
	// 開始位置
	DirectX::XMFLOAT3 startPos = DirectX::XMFLOAT3(-15.0f, -30.0f, 0.0f);
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
	@brief　	| プレイヤーにカードを追加する
	@param		| tCardInfo：追加するカード情報構造体
*//*****************************************/
void CBlackjack_Player::AddCard(CPlayingCard::Info tCardInfo)
{
	// トランプカードオブジェクトの生成
	CPlayingCard* pCard = GetScene()->AddGameObject<CPlayingCard>(Tag::GameObject,"PlayingCard");
	// カード情報の設定
	pCard->Setting(tCardInfo.m_eSuit, tCardInfo.m_nNumber);

	// プレイヤーのカードリストに追加
	m_Cards.push_back(pCard);
}