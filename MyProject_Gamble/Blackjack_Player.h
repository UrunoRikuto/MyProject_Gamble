/**************************************************//*
	@file	| Blackjack_Player.h
	@brief	| ブラックジャックプレイヤークラスのhファイル
	@note	| カードの管理やチップの管理をする
*//**************************************************/
#pragma once
#include "GameObject.h"
#include "PlayingCard.h"

constexpr DirectX::XMFLOAT3 ce_f3PlayerCardStartPos = DirectX::XMFLOAT3(-15.0f, -30.0f, 0.0f); // プレイヤーのカード開始位置

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

	// @brief プレイヤーが行動可能かどうかを取得する
	bool CanAction() { return m_bCanAction; }
	// @brief プレイヤーが行動可能かどうかを設定する
	void SetCanAction(bool bCanAction) { m_bCanAction = bCanAction; }

	// @brief 持っているカードの合計値を計算する
	int CalcHandValue(std::vector<CPlayingCard*> cardlist);

	// @brief 手札の合計値がバーストしているかどうかを取得する
	bool IsBurst(std::vector<CPlayingCard*> cardlist)
	{
		return CalcHandValue(cardlist) > 21;
	}

	// @brief 手札の合計値がブラックジャック(21)かどうかを取得する
	bool IsBlackjack(std::vector<CPlayingCard*> cardlist)
	{
		return CalcHandValue(cardlist) == 21;
	}

	// @brief 手札をクリアする（カードオブジェクトも破棄）
	void ClearHand();

private:
	// @brief 行動処理
	void Action();

	// @brief カードの位置調整
	void AdjustCardPositions();

private:
	// @brief プレイヤーが持っているカードリスト
	std::vector<CPlayingCard*> m_Cards;

	// @brief プレイヤーが行動可能かどうか
	bool m_bCanAction = true;
};

