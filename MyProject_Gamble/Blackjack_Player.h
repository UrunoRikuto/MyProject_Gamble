/**************************************************//*
	@file	| Blackjack_Player.h
	@brief	| ブラックジャックプレイヤークラスのhファイル
	@note	| カードの管理やチップの管理をする
*//**************************************************/
#pragma once
#include "GameObject.h"
#include "PlayingCard.h"

// @brief ブラックジャックプレイヤークラス
class CBlackjack_Player : public CGameObject
{
public:
	// @brief コンストラクタ
	CBlackjack_Player();

	// @brief デストラクタ
	~CBlackjack_Player();

	// @brief 初期化処理
	void Init() override;

	// @brief 更新処理
	void Update() override;

	// @brief プレイヤーにカードを追加する
	void AddCard(CPlayingCard::Info tCardInfo);

	// @brief プレイヤーが持っているカードリストを取得する
	std::vector<CPlayingCard*>& GetPlayerCards() { return m_Cards; }

private:
	// @brief プレイヤーが持っているカードリスト
	std::vector<CPlayingCard*> m_Cards;
};

