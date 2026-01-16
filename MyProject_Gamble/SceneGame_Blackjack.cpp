/**************************************************//*
	@file	| SceneGame_Blackjack.cpp
	@brief	| ブラックジャックゲームシーンクラスのcppファイル
	@note	| ブラックジャックゲームシーン内のゲームオブジェクト管理、更新、描画等を行う
			| CSceneを継承
*//**************************************************/
#include "SceneGame_Blackjack.h"
#include "Camera.h"
#include "Input.h"
#include "PlayingCard.h"
#include "Blackjack_GameManager.h"
#include "Blackjack_Player.h"
#include "Blackjack_Croupier.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CSceneGame_Blackjack::CSceneGame_Blackjack()
{
}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CSceneGame_Blackjack::~CSceneGame_Blackjack()
{
}

/*****************************************//*
	@brief　	| 初期化処理
*//*****************************************/
void CSceneGame_Blackjack::Init()
{
	// 基底クラスの初期化処理
	CScene::Init();

	// カメラの種類をブラックジャック用カメラに設定
	CCamera::GetInstance()->SetCameraKind(CameraKind::CAM_GAME_BLACKJACK);

	// ブラックジャックのプレイヤーとディーラーの生成
	std::list<CBlackjack_Player*> pPlayers;
	pPlayers.push_back(AddGameObject<CBlackjack_Player>(Tag::GameObject, "BlackjackPlayer"));
	CBlackjack_Croupier* pCroupier = AddGameObject<CBlackjack_Croupier>(Tag::GameObject, "BlackjackCroupier");

	// ブラックジャックゲームマネージャーの初期化
	CBlackjack_GameManager* pGameManager = CBlackjack_GameManager::GetInstance();
	pGameManager->Init();
	pGameManager->StartGame(1);

	// 最初のカードを配る
	for (int i = 0; i < 2; ++i)
	{
		// プレイヤーにカードを配る
		CPlayingCard::Info playerCardInfo = pGameManager->DealCard();
		for(auto player : pPlayers)
		{
			player->AddCard(playerCardInfo);
		}
		// ディーラーにカードを配る
		CPlayingCard::Info croupierCardInfo = pGameManager->DealCard();
		pCroupier->AddCard(croupierCardInfo, i < 1);
	}

	pCroupier->FirstCheckBlackjack();

}

/*****************************************//*
	@brief　	| 更新処理
*//*****************************************/
void CSceneGame_Blackjack::Update()
{
	// 基底クラスの更新処理
	CScene::Update();

	if (IsKeyTrigger(VK_DELETE))
	{
		// プレイヤーのカードをクリア
		auto players = GetGameObjects<CBlackjack_Player>();
		for (auto player : players)
		{
			player->Init();
		}

		// ディーラーのカードをクリア
		CBlackjack_Croupier* croupier = GetGameObject<CBlackjack_Croupier>();
		croupier->Init();

		CBlackjack_GameManager* pGameManager = CBlackjack_GameManager::GetInstance();
		pGameManager->Init();
		pGameManager->StartGame(1);

		// 最初のカードを配る
		for (int i = 0; i < 2; ++i)
		{
			// プレイヤーにカードを配る
			CPlayingCard::Info playerCardInfo = pGameManager->DealCard();
			for(auto player : players)
			{
				player->AddCard(playerCardInfo);
			}
			// ディーラーにカードを配る
			CPlayingCard::Info croupierCardInfo = pGameManager->DealCard();
			croupier->AddCard(croupierCardInfo, i < 1);
		}
	}
}

/*****************************************//*
	@brief　	| 描画処理
*//*****************************************/
void CSceneGame_Blackjack::Draw()
{
	// 基底クラスの描画処理
	CScene::Draw();
}