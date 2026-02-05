/**************************************************//*
	@file	| Blackjack_Croupier.cpp
	@brief	| ブラックジャックディーラークラスのcppファイル
	@note	| ブラックジャックディーラーのゲームオブジェクトを管理する
*//**************************************************/
#include "Blackjack_Croupier.h"
#include "Main.h"
#include "Blackjack_GameManager.h"
#include "Blackjack_Player.h"
#include "Blackjack_Network.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CBlackjack_Croupier::CBlackjack_Croupier()
	: m_Cards()
	, m_bCanAction(true)
{
}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CBlackjack_Croupier::~CBlackjack_Croupier()
{
	ClearCards();
}

/*****************************************//*
	@brief　	| 初期化処理
*//*****************************************/
void CBlackjack_Croupier::Init()
{
	// 基底クラスの初期化処理
	CGameObject::Init();

	ClearCards();

	//まだディーラーの手番ではないので「行動中ではない」
	m_bCanAction = false;
}

/*****************************************//*
	@brief　	| 更新処理
*//*****************************************/
void CBlackjack_Croupier::Update()
{
	if (!m_bNetworkSync)
	{
		// 従来のローカル進行
		Action();
	}

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
	std::string cardName = "CroupierCard_";
	switch (tCardInfo.m_eSuit)
	{
	case CPlayingCard::Suit::Spade: cardName += "Spade_"; break;
	case CPlayingCard::Suit::Club: cardName += "Club_"; break;
	case CPlayingCard::Suit::Heart: cardName += "Heart_"; break;
	case CPlayingCard::Suit::Diamond: cardName += "Diamond_"; break;
	}
	cardName += std::to_string(tCardInfo.m_nNumber);
	cardName += "_" + std::to_string(static_cast<int>(m_Cards.size()));

	// カードオブジェクトをシーンに追加
	CPlayingCard* pCard = GetScene()->AddGameObject<CPlayingCard>(Tag::GameObject, cardName);
	pCard->Setting(tCardInfo.m_eSuit, tCardInfo.m_nNumber, bFaceUp);
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
		CBlackjack_Player* player = GetScene()->GetGameObject<CBlackjack_Player>();
		// 行動不可に設定
		player->SetCanAction(false);

		m_Cards[1]->FaceUp(); // 2枚目のカードを表向きにする
	}
}

/*****************************************//*
	@brief　	| 行動処理
*//*****************************************/
void CBlackjack_Croupier::Action()
{
	// ネットワーク同期中はローカルでカードを引かない
	if (m_bNetworkSync) return;

	// プレイヤーが行動終了しているか確認
	if (!CBlackjack_GameManager::GetInstance()->IsPlayerActionEnd())
	{
		// 行動可能なプレイヤーがいる場合、処理を終了
		return;
	}

	// ディーラーの手番開始（行動中）
	m_bCanAction = true;

	if (m_Cards.size() >=2 && m_Cards[1])
	{
		m_Cards[1]->FaceUp(); //2枚目のカードを表向きにする
	}

	// 持っているカードの合計値を計算
	int handValue = CalcHandValue();

	// 合計値が17未満の場合、カードを引く
	if (handValue <17)
	{
		//1フレームで複数枚引かないように、ここでは1枚だけ引いて手番を一旦終了する
		// （次に引くかどうかは、マネージャ側が再度ディーラー手番にしたタイミングで判定する）
		AddCard(CBlackjack_GameManager::GetInstance()->DealCard(), true);
		m_bCanAction = false;
		return;
	}

	//これ以上引かない＝行動終了
	m_bCanAction = false;
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
int CBlackjack_Croupier::CalcHandValue(bool UpFaceOnly)
{
	int totalValue = 0;
	int aceCount = 0;
	// 各カードの値を計算
	for (CPlayingCard* pCard : m_Cards)
	{
		// 表向きのカードのみ計算する場合、裏向きのカードはスキップ
		if (UpFaceOnly && !pCard->IsFaceUp())continue;

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

void CBlackjack_Croupier::ClearCards()
{
	for (auto card : m_Cards)
	{
		if (card)
		{
			card->Destroy();
		}
	}
	m_Cards.clear();
}

void CBlackjack_Croupier::ApplyFromNetwork(const CBlackjack_Network& net)
{
	const DealerData& d = net.GetDealerData();

	// IsActiveでまだディーラー情報が来ていない/無効な場合は何もしない
	if (!d.IsActive) return;

	//変化がない場合は更新しない（無駄なDestroy/AddCardを避ける)
	if (m_LastSyncedTableCards.size() == d.TableCards.size())
	{
		bool same = true;
		for (size_t i =0; i < d.TableCards.size(); ++i)
		{
			const auto& a = m_LastSyncedTableCards[i];
			const auto& b = d.TableCards[i];
			if (a.m_eSuit != b.m_eSuit || a.m_nNumber != b.m_nNumber || a.m_bFaceUp != b.m_bFaceUp)
			{
				same = false;
				break;
			}
		}
		if (same) return;
	}

	// 手札が巻き戻った/リセットされた（数が減った）場合は作り直す
	if (d.TableCards.size() < m_LastSyncedTableCards.size())
	{
		ClearCards();
		for (const auto& info : d.TableCards)
		{
			AddCard(info, info.m_bFaceUp);
		}
		m_LastSyncedTableCards = d.TableCards;
		m_bCanAction = false;
		return;
	}

	//既存分のFaceUp更新（同一位置に同一カードが来る想定）
	const size_t common = (std::min)(m_Cards.size(), d.TableCards.size());
	for (size_t i =0; i < common; ++i)
	{
		CPlayingCard* pCard = m_Cards[i];
		if (!pCard) continue;

		const bool faceUpFromNet = d.TableCards[i].m_bFaceUp;
		if (faceUpFromNet && !pCard->IsFaceUp())
		{
			pCard->FaceUp();
		}
		// FaceDownが必要になったらここに処理を追加（現状APIが無さそうなので省略）
	}

	// 増えた分だけAddCard
	for (size_t i = m_Cards.size(); i < d.TableCards.size(); ++i)
	{
		const auto& info = d.TableCards[i];
		AddCard(info, info.m_bFaceUp);
	}

	m_LastSyncedTableCards = d.TableCards;
	m_bCanAction = false;
}