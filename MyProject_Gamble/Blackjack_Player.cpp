/**************************************************//*
	@file	| Blackjack_Player.cpp
	@brief	| ブラックジャックプレイヤークラスのcppファイル
	@note	| カードの管理やチップの管理をする
*//**************************************************/
#include "Blackjack_Player.h"
#include "Main.h"
#include "Input.h"
#include "Blackjack_GameManager.h"

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
	@brief　	| 初期化処理
*//*****************************************/
void CBlackjack_Player::Init()
{
	// 基底クラスの初期化処理
	CGameObject::Init();

	// カードリストの初期化
	for(auto card : m_Cards)
	{
		if (card)
		{
			card->Destroy();
		}
	}
	m_Cards.clear();
	for (auto card : m_SplitCards)
	{
		if (card)
		{
			card->Destroy();
		}
	}
	m_SplitCards.clear();

	// 行動可能フラグを初期化
	m_bCanAction = true;
}

/*****************************************//*
	@brief　	| 更新処理
*//*****************************************/
void CBlackjack_Player::Update()
{
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
	// トランプカードオブジェクトの生成
	CPlayingCard* pCard = GetScene()->AddGameObject<CPlayingCard>(Tag::GameObject,"PlayingCard");
	// カード情報の設定
	pCard->Setting(tCardInfo.m_eSuit, tCardInfo.m_nNumber);

	// プレイヤーのカードリストに追加
	m_Cards.push_back(pCard);
}

/*****************************************//*
	@brief　	| 行動処理
*//*****************************************/
void CBlackjack_Player::Action()
{
	// 行動可能かどうか確認
	if(!m_bCanAction)return;

	// ヒット
	if(IsKeyTrigger('Q'))
	{
		// カードを1枚引く
		AddCard(CBlackjack_GameManager::GetInstance()->DealCard());
	}
	// スタンド
	else if(IsKeyTrigger('W'))
	{
		// 手番終了
		m_bCanAction = false;
	}
	// ダブルダウン
	else if(IsKeyTrigger('E'))
	{
		// チップを倍にしてカードを1枚引く
		AddCard(CBlackjack_GameManager::GetInstance()->DealCard());
		// 手番終了
		m_bCanAction = false;
	}
	// スプリット
	// 同じ数字のカードが2枚あるか確認
	else if(m_Cards.size() == 2 &&
		m_Cards[0]->GetNumber() == m_Cards[1]->GetNumber())
	{
		if (IsKeyTrigger('R'))
		{
			// スプリット用のカードリストに2枚目のカードを移動
			m_SplitCards.push_back(m_Cards[1]);
			m_Cards.pop_back();
			// それぞれの手札に1枚ずつカードを引く
			AddCard(CBlackjack_GameManager::GetInstance()->DealCard());
			m_SplitCards.push_back(GetScene()->AddGameObject<CPlayingCard>(Tag::GameObject, "PlayingCard"));
			m_SplitCards.back()->Setting(
				m_Cards[0]->GetSuit(),
				m_Cards[0]->GetNumber()
			);
			m_SplitCards.back()->Setting(
				CBlackjack_GameManager::GetInstance()->DealCard().m_eSuit,
				CBlackjack_GameManager::GetInstance()->DealCard().m_nNumber
			);
		}

	}

	// バーストしているか確認
	if (CalcHandValue() >= 21)
	{
		m_bCanAction = false;
	}
}


/*****************************************//*
	@brief　	| カードの位置調整
*//*****************************************/
void CBlackjack_Player::AdjustCardPositions()
{
	// スプリットしていない場合
	if (m_SplitCards.empty())
	{
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
	}
	else
	{
		// スプリットしている場合の位置調整処理
		// 一組目の開始位置
		DirectX::XMFLOAT3 startPos1 = DirectX::XMFLOAT3(-50.0f, -30.0f, 0.0f);
		// 二組目の開始位置
		DirectX::XMFLOAT3 startPos2 = DirectX::XMFLOAT3(50.0f, -30.0f, 0.0f);
		// カード間の間隔
		DirectX::XMFLOAT3 offset = DirectX::XMFLOAT3(15.0f, 0.0f, 0.0f);
		// 一組目のカード位置調整
		for (size_t i = 0; i < m_Cards.size(); ++i)
		{
			CPlayingCard* pCard = m_Cards[i];
			if (pCard)
			{
				// 位置の計算
				DirectX::XMFLOAT3 cardPos = DirectX::XMFLOAT3(
					startPos1.x + offset.x * static_cast<float>(i),
					startPos1.y + offset.y * static_cast<float>(i),
					startPos1.z + offset.z * static_cast<float>(i)
				);
				// カードの位置を設定
				pCard->SetPos(cardPos);
			}
		}
		// 二組目のカード位置調整
		for (size_t i = 0; i < m_SplitCards.size(); ++i)
		{
			CPlayingCard* pCard = m_SplitCards[i];
			if (pCard)
			{
				// 位置の計算
				DirectX::XMFLOAT3 cardPos = DirectX::XMFLOAT3(
					startPos2.x + offset.x * static_cast<float>(i),
					startPos2.y + offset.y * static_cast<float>(i),
					startPos2.z + offset.z * static_cast<float>(i)
				);
				// カードの位置を設定
				pCard->SetPos(cardPos);
			}
		}
	}
}

/*****************************************//*
	@brief　	| 持っているカードの合計値を計算する
	@return		| 合計値
*//*****************************************/
int CBlackjack_Player::CalcHandValue()
{
	int totalValue = 0;
	int aceCount = 0;
	// 各カードの値を計算
	for (CPlayingCard* pCard : m_Cards)
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