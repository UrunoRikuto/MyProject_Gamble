/**************************************************//*
	@file	| Blackjack_Croupier.cpp
	@brief	| ブラックジャックディーラークラスのcppファイル
	@note	| ブラックジャックディーラーのゲームオブジェクトを管理する
*//**************************************************/
#include "Blackjack_Croupier.h"
#include "Main.h"
#include "Blackjack_GameManager.h"
#include "Blackjack_Player.h"

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
	for(auto card : m_Cards)
	{
		if (card)
		{
			card->Destroy();
		}
	}
	m_Cards.clear();
}

/*****************************************//*
	@brief　	| 更新処理
*//*****************************************/
void CBlackjack_Croupier::Update()
{
	// 行動処理
	Action();

	// カードの位置を設定
	AdjustCardPositions();

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

/*****************************************//*
	@brief　	| ディーラーがブラックジャックかどうかの処理
*//*****************************************/
void CBlackjack_Croupier::FirstCheckBlackjack()
{
	// 持っているカードの合計値を計算
	int handValue = CalcHandValue();

	// 合計値が21の場合、ブラックジャック成立
	if (handValue == 21)
	{
		// すべてのプレイヤーを取得
		auto players = GetScene()->GetGameObjects<CBlackjack_Player>();
		for (auto player : players)
		{
			// 行動不可に設定
			player->SetCanAction(false);
		}

		m_Cards[1]->FaceUp(); // 2枚目のカードを表向きにする
	}
}

/*****************************************//*
	@brief　	| 行動処理
*//*****************************************/
void CBlackjack_Croupier::Action()
{
	auto players = GetScene()->GetGameObjects<CBlackjack_Player>();
	// 全てのプレイヤーが行動終了しているか確認
	for (auto player : players)
	{
		if (player->CanAction())
		{
			// 行動可能なプレイヤーがいる場合、処理を終了
			return;
		}
	}

	m_Cards[1]->FaceUp(); // 2枚目のカードを表向きにする

	// 持っているカードの合計値を計算
	int handValue = CalcHandValue();

	// 合計値が17未満の場合、カードを引く
	if (handValue < 17)
	{
		// カードを1枚引く
		AddCard(CBlackjack_GameManager::GetInstance()->DealCard(), true);
	}
}

/*****************************************//*
	@brief　	| カードの位置調整
*//*****************************************/
void CBlackjack_Croupier::AdjustCardPositions()
{
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
}

/*****************************************//*
	@brief　	| 持っているカードの合計値を計算する
	@return		| 合計値
*//*****************************************/
int CBlackjack_Croupier::CalcHandValue()
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
		// エースの場合は11として扱い、エースの枚数をカウント
		if (cardValue == 1)
		{
			cardValue = 11;
			aceCount++;
		}
		totalValue += cardValue;
	}
	// 合計値が21を超えている場合、エースを1として扱う
	while (totalValue > 21 && aceCount > 0)
	{
		totalValue -= 10;
		aceCount--;
	}
	return totalValue;
}