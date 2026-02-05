/**************************************************//*
	@file	| Blackjack_Player.cpp
	@brief	| ブラックジャックプレイヤークラスのcppファイル
	@note	| カードの管理やチップの管理をする
*//**************************************************/
#include "Blackjack_Player.h"
#include "Main.h"
#include "Input.h"
#include "Blackjack_GameManager.h"
#include "Oparation.h"
#include "Blackjack_Network.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CBlackjack_Player::CBlackjack_Player()
	: m_Cards()
	, m_bCanAction(true)
{
}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CBlackjack_Player::~CBlackjack_Player()
{
}

/*****************************************//*
	@brief　	| 手札クリア処理
*//*****************************************/
void CBlackjack_Player::ClearHand()
{
	for (auto* card : m_Cards)
	{
		if (card) card->Destroy();
	}
	m_Cards.clear();
	m_bCanAction = true;
}

/*****************************************//*
	@brief　	| 初期化処理
*//*****************************************/
void CBlackjack_Player::Init()
{
	// 基底クラスの初期化処理
	CGameObject::Init();

	ClearHand();
}

/*****************************************//*
	@brief　	| 更新処理
*//*****************************************/
void CBlackjack_Player::Update()
{
	// ネットワークからラウンドリセット通知が来たら手札をクリア
	if (g_pNetwork && g_pNetwork->ConsumeRoundReset()) //1回だけtrue
	{
		ClearHand();
	}

	// 行動処理
	Action();

	// カードの位置を設定
	AdjustCardPositions();

	// 基底クラスの更新処理
	CGameObject::Update();
}

/*****************************************//*
	@brief　	| プレイヤーにカードを追加する
	@param		| tCardInfo：追加するカード情報構造体
*//*****************************************/
void CBlackjack_Player::AddCard(CPlayingCard::Info tCardInfo)
{
	//既に同じカードが末尾にある場合（ネットワーク再送・取り込み重複など）
	if (!m_Cards.empty())
	{
		CPlayingCard* last = m_Cards.back();
		if (last && last->GetSuit() == tCardInfo.m_eSuit && last->GetNumber() == tCardInfo.m_nNumber)
		{
			return;
		}
	}

	// トランプカードオブジェクトの生成
	std::string cardName = "PlayerCard_";
	switch (tCardInfo.m_eSuit)
	{
	case CPlayingCard::Suit::Spade: cardName += "Spade_"; break;
	case CPlayingCard::Suit::Club: cardName += "Club_"; break;
	case CPlayingCard::Suit::Heart: cardName += "Heart_"; break;
	case CPlayingCard::Suit::Diamond: cardName += "Diamond_"; break;
	}
	cardName += std::to_string(tCardInfo.m_nNumber);
	cardName += "_" + std::to_string(static_cast<int>(m_Cards.size()));

	CPlayingCard* pCard = GetScene()->AddGameObject<CPlayingCard>(Tag::GameObject, cardName);

	// ネットワーク同期用: Info内のFaceUpを反映（デフォルトtrue）
	pCard->Setting(tCardInfo.m_eSuit, tCardInfo.m_nNumber, tCardInfo.m_bFaceUp);

	m_Cards.push_back(pCard);
}

/*****************************************//*
	@brief　	| 行動処理
*//*****************************************/
void CBlackjack_Player::Action()
{
	// 行動可能かどうか確認
	if (!m_bCanAction) return;

	// ヒット（サーバー権威）
	if (IsKeyTrigger('Q'))
	{
		if (g_pNetwork)
		{
			g_pNetwork->RequestHit();
		}
	}
	// スタンド（行動終了）
	else if (IsKeyTrigger('W'))
	{
		m_bCanAction = false;
	}
	// ダブルダウン（1枚だけ引いて行動終了）
	else if (IsKeyTrigger('E'))
	{
		if (g_pNetwork)
		{
			g_pNetwork->RequestHit();
		}
		m_bCanAction = false;
	}

	// バーストまたは21以上で行動終了
	if (CalcHandValue(m_Cards) >= 21)
	{
		m_bCanAction = false;
	}
}


/*****************************************//*
	@brief　	| カードの位置調整
*//*****************************************/
void CBlackjack_Player::AdjustCardPositions()
{
	// プレイヤー自身の位置を基準にする
	const DirectX::XMFLOAT3 base = this->GetPos();
	const DirectX::XMFLOAT3 start = DirectX::XMFLOAT3(
		base.x + ce_f3PlayerCardStartPos.x,
		base.y + ce_f3PlayerCardStartPos.y,
		base.z + ce_f3PlayerCardStartPos.z);

	// カード間の間隔
	const float xStep =15.0f;
	const float zStep = -0.01f; // 重なりちらつき回避

	for (size_t i =0; i < m_Cards.size(); ++i)
	{
		CPlayingCard* pCard = m_Cards[i];
		if (!pCard) continue;

		DirectX::XMFLOAT3 cardPos = DirectX::XMFLOAT3(
			start.x + xStep * static_cast<float>(i),
			start.y,
			start.z + zStep * static_cast<float>(i));

		pCard->SetPos(cardPos);
	}
}

/*****************************************//*
	@brief　	| 持っているカードの合計値を計算する
	@return		| 合計値
*//*****************************************/
int CBlackjack_Player::CalcHandValue(std::vector<CPlayingCard*> cardlist)
{
	int totalValue = 0;
	int aceCount = 0;
	// 各カードの値を計算
	for (CPlayingCard* pCard : cardlist)
	{
		int cardValue = pCard->GetNumber();
		// 10以上のカードは10として扱う
		if (cardValue > 10)
		{
			cardValue = 10;
		}
		// エースの場合は11として扱い、後で調整する
		else if (cardValue == 1)
		{
			cardValue = 11;
			aceCount++;
		}
		totalValue += cardValue;
	}
	// 合計値が21を超えている場合、エースを1として扱うように調整
	while (totalValue > 21 && aceCount > 0)
	{
		totalValue -= 10; // エースを11から1に変更
		aceCount--;
	}
	return totalValue;
}
