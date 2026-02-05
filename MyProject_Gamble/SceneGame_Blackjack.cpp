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
#include "BackGroundObject.h"
#include "Sprite3DRenderer.h"
#include "Blackjack_Network.h"

CBlackjack_Network* g_pNetwork;

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CSceneGame_Blackjack::CSceneGame_Blackjack()
{
	g_pNetwork = new CBlackjack_Network();
	g_pNetwork->Init();
}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CSceneGame_Blackjack::~CSceneGame_Blackjack()
{
	g_pNetwork->Uninit();
	delete g_pNetwork;
	g_pNetwork = nullptr;
}

/*****************************************//*
	@brief　	| 初期化処理
*//*****************************************/
void CSceneGame_Blackjack::Init()
{
	// 基底クラスの初期化処理
	CScene::Init();

	// 背景オブジェクトの生成
	CBackGroundObject*pBackGround = AddGameObject<CBackGroundObject>(Tag::GameObject, "BlackjackBackGround");
	pBackGround->GetComponent<CSprite3DRenderer>()->SetKey("BlackjackTable");

	// カメラの種類をブラックジャック用カメラに設定
	CCamera::GetInstance()->SetCameraKind(CameraKind::CAM_GAME_BLACKJACK);

	// ブラックジャックのプレイヤーとディーラーの生成
	CBlackjack_Player* pPlayer = AddGameObject<CBlackjack_Player>(Tag::GameObject, "BlackjackPlayer");
	CBlackjack_Croupier* pCroupier = AddGameObject<CBlackjack_Croupier>(Tag::GameObject, "BlackjackCroupier");

	// ブラックジャックゲームマネージャーの初期化（ローカル配布はしない）
	CBlackjack_GameManager* pGameManager = CBlackjack_GameManager::GetInstance();
	pGameManager->Init();
	pGameManager->StartGame(1);

	// 初期手札はNetwork側（Join確定後）で2枚要求する
}

/*****************************************//*
	@brief　	| 更新処理
*//*****************************************/
void CSceneGame_Blackjack::Update()
{
	// 基底クラスの更新処理
	CScene::Update();

	CBlackjack_GameManager* pGameManager = CBlackjack_GameManager::GetInstance();

	if (IsKeyTrigger(VK_DELETE))
	{
		// プレイヤーのカードをクリア
		if (auto* player = GetGameObject<CBlackjack_Player>())
			player->Init();

		// ディーラーのカードをクリア
		if (auto* croupier = GetGameObject<CBlackjack_Croupier>())
			croupier->Init();

		pGameManager->Init();
		pGameManager->StartGame(1);

		// 初期配布はNetwork側に統一（ここでは管理しない）
	}

	// ブラックジャックゲームマネージャーの更新
	pGameManager->Update();

	// ネットワークの更新
	g_pNetwork->Update();

	// --- ラウンドリセット反映（初期手札を入れる前に必ずクリア） ---
	if (g_pNetwork->ConsumeRoundReset())
	{
		if (auto* player = GetGameObject<CBlackjack_Player>())
			player->ClearHand();
	}

	// Hit結果を反映（Network側が初期2枚含めて供給）
	CPlayingCard::Info hitCard{};
	while (g_pNetwork->ConsumeHitResult(hitCard))
	{
		if (auto* player = GetGameObject<CBlackjack_Player>())
			player->AddCard(hitCard);
	}

	// ネットワーク受信したディーラー状態を反映
	if (auto* croupier = GetGameObject<CBlackjack_Croupier>())
	{
		croupier->SetNetworkSync(true);
		croupier->ApplyFromNetwork(*g_pNetwork);
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