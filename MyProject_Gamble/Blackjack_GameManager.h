/**************************************************//*
	@file	| Blackjack_GameManager.h
	@brief	| ブラックジャックゲームマネージャークラスのhファイル
	@note	| ブラックジャックゲーム全体の管理を行うシングルトンクラス
*//**************************************************/
#pragma once
#include "Singleton.h"
#include <list>
#include "PlayingCard.h"

class CBlackjack_Croupier;
class CBlackjack_Player;

// @brief ブラックジャックゲームマネージャークラス
class CBlackjack_GameManager : public ISingleton<CBlackjack_GameManager>
{
private:
	// @brief コンストラクタ
	CBlackjack_GameManager();

	friend class ISingleton<CBlackjack_GameManager>;

public:
	// @brief デストラクタ
	~CBlackjack_GameManager();

	// @brief 初期化処理
	void Init();

	// @brief 終了処理
	void Uninit();

	// @brief 更新処理
	void Update();

	// @brief プレイ開始処理
	void StartGame(int nPlayerNum);

	// @brief カードを配る
	// @return 配られたカード情報構造体
	CPlayingCard::Info DealCard();

	// @brief プレイヤーの行動が終了したかどうかを取得
	bool IsPlayerActionEnd() { return m_bIsPlayerActionEnd; }

private:

	// @brief リザルトチェック処理
	void ResultCheck();

private:
	// @brief ゲームを進行中かどうか
	bool m_bIsGameProgress = false;

	// @brief プレイヤー人数
	int m_nPlayerNum = 0;

	// @brief トランプのカードリスト
	std::list<CPlayingCard::Info> m_CardList;

	// @brief リザルトオブジェクト
	std::list<CGameObject*> m_ResultObjectList;

	// @brief プレイヤーの行動終了フラグ
	bool m_bIsPlayerActionEnd = false;
};