/**************************************************//*
	@file	| Blackjack_GameManager.cpp
	@brief	| ブラックジャックゲームマネージャークラスのcppファイル
	@note	| ブラックジャックゲーム全体の管理を行うシングルトンクラス
*//**************************************************/
#include "Blackjack_GameManager.h"
#include "Main.h"
#include "Blackjack_Croupier.h"
#include "Blackjack_Player.h"
#include "Blackjack_Network.h"
#include <algorithm>
#include <random>
#include "BillboardRenderer.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CBlackjack_GameManager::CBlackjack_GameManager()
	: m_nPlayerNum(0)
	, m_CardList()
	, m_bIsGameProgress(false)
{
}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CBlackjack_GameManager::~CBlackjack_GameManager()
{
}

/*****************************************//*
	@brief　	| 初期化処理
*//*****************************************/
void CBlackjack_GameManager::Init()
{
	for(auto resultObject : m_ResultObjectList)
	{
		if (resultObject)
		{
			resultObject->Destroy();
		}
	}
	m_ResultObjectList.clear();

	// トランプの初期化
	m_CardList.clear();
	for (int suit = 0; suit < 4; ++suit)
	{
		for (int number = 1; number <= 13; ++number)
		{
			CPlayingCard::Info cardInfo;
			cardInfo.m_eSuit = static_cast<CPlayingCard::Suit>(suit);
			cardInfo.m_nNumber = number;
			m_CardList.push_back(cardInfo);
		}
	}

	// カードリストのシャッフル
	std::vector<CPlayingCard::Info> cardVector(m_CardList.begin(), m_CardList.end());
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(cardVector.begin(), cardVector.end(), g);
	m_CardList = std::list<CPlayingCard::Info>(cardVector.begin(), cardVector.end());
}

/*****************************************//*
	@brief　	| 終了処理
*//*****************************************/
void CBlackjack_GameManager::Uninit()
{
}

/*****************************************//*
	@brief　	| 更新処理
*//*****************************************/
void CBlackjack_GameManager::Update()
{
	// リザルトチェック処理
	ResultCheck();
}

/*****************************************//*
	@brief　	| プレイ開始処理
	@param		| nPlayerNum：プレイヤー人数
*//*****************************************/
void CBlackjack_GameManager::StartGame(int nPlayerNum)
{
	// プレイヤー人数の設定
	m_nPlayerNum = nPlayerNum;

	// ゲーム進行中フラグを立てる
	m_bIsGameProgress = true;
}

/*****************************************//*
	@brief　	| カードを配る
	@return		| 配られたカード情報構造体
*//*****************************************/
CPlayingCard::Info CBlackjack_GameManager::DealCard()
{
	// 先頭のカードを取得してリストから削除
	CPlayingCard::Info dealtCard = m_CardList.front();
	m_CardList.pop_front();
	// 配られたカード情報を返す
	return dealtCard;
}

/*****************************************//*
	@brief　	| リザルトチェック処理
*//*****************************************/
void CBlackjack_GameManager::ResultCheck()
{
	// 現在シーンの取得
	CScene* pScene = GetScene();
	// ディーラーの取得
	CBlackjack_Croupier* pCroupier = pScene->GetGameObject<CBlackjack_Croupier>();
	// プレイヤーの取得
	CBlackjack_Player* pPlayer = pScene->GetGameObject<CBlackjack_Player>();

	// プレイヤーの行動終了判定
	m_bIsPlayerActionEnd = !pPlayer->CanAction();

	// --- 結果表示タイミング ---
	// ネットワーク同期の場合: サーバー側の DealerFinished を待つ
	bool canShowResult = false;
	if (g_pNetwork)
	{
		const DealerData& d = g_pNetwork->GetDealerData();
		canShowResult = d.AllPlayersStand && d.DealerFinished;
	}
	else
	{
		// ローカル進行の場合: ディーラーが行動中でないことを待つ
		bool bIsCroupierAction = pCroupier->IsActioning();
		canShowResult = (m_bIsPlayerActionEnd && !bIsCroupierAction);
	}

	// ゲーム中に一回だけ勝敗判定を行う
	if (!(m_ResultObjectList.empty() && canShowResult)) return;

	// プレイヤーの手札の取得
	auto playerCards = pPlayer->GetPlayerCards();

	// 結果表示用オブジェクトの生成
	CGameObject* pPlayerResultObject = pScene->AddGameObject<CGameObject>(Tag::GameObject, "PlayerResultObject");
	pPlayerResultObject->SetPos({ -15.0f, -10.0f,5.0f });
	pPlayerResultObject->SetSize({10.0f,8.0f,1.0f });

	// プレイヤーがバーストしている場合
	if (pPlayer->IsBurst(playerCards))
	{
		pPlayerResultObject->AddComponent<CBillboardRenderer>()->SetKey("Lose");
	}
	else if (pCroupier->IsBurst())
	{
		pPlayerResultObject->AddComponent<CBillboardRenderer>()->SetKey("Win");
	}
	else if (pPlayer->IsBlackjack(playerCards) && !pCroupier->IsBlackjack())
	{
		pPlayerResultObject->AddComponent<CBillboardRenderer>()->SetKey("Win");
	}
	else if (!pPlayer->IsBlackjack(playerCards) && pCroupier->IsBlackjack())
	{
		pPlayerResultObject->AddComponent<CBillboardRenderer>()->SetKey("Lose");
	}
	else if (pCroupier->CalcHandValue() > pPlayer->CalcHandValue(playerCards))
	{
		pPlayerResultObject->AddComponent<CBillboardRenderer>()->SetKey("Lose");
	}
	else if (pCroupier->CalcHandValue() < pPlayer->CalcHandValue(playerCards))
	{
		pPlayerResultObject->AddComponent<CBillboardRenderer>()->SetKey("Win");
	}
	else
	{
		pPlayerResultObject->AddComponent<CBillboardRenderer>()->SetKey("Draw");
	}

	m_ResultObjectList.push_back(pPlayerResultObject);
}
