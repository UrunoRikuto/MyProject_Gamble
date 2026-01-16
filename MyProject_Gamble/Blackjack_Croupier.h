/**************************************************//*
	@file	| Blackjack_Croupier.h
	@brief	| ブラックジャックディーラークラスのhファイル
	@note	| ブラックジャックディーラーのゲームオブジェクトを管理する
*//**************************************************/
#pragma once
#include "GameObject.h"
#include "PlayingCard.h"

// @brief ブラックジャックディーラークラス
class CBlackjack_Croupier : public CGameObject
{
public:
	// @brief コンストラクタ
	CBlackjack_Croupier();

	// @brief デストラクタ
	~CBlackjack_Croupier();

	// @brief 初期化処理
	void Init() override;

	// @brief 更新処理
	void Update() override;

	// @brief ディーラーにカードを追加する
	void AddCard(CPlayingCard::Info tCardInfo, bool m_bFaceUp = true);

	// @brief ディーラーが持っているカードリストを取得する
	std::vector<CPlayingCard*>& GetCroupierCards() { return m_Cards; }

private:
	// @brief ディーラーが持っているカードリスト
	std::vector<CPlayingCard*> m_Cards;
};

