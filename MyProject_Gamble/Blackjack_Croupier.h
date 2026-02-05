/**************************************************//*
	@file	| Blackjack_Croupier.h
	@brief	| ブラックジャックディーラークラスのhファイル
	@note	| ブラックジャックディーラーのゲームオブジェクトを管理する
*//**************************************************/
#pragma once
#include "GameObject.h"
#include "PlayingCard.h"
#include <vector>

class CBlackjack_Network;

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

	// @brief ディーラーがブラックジャックかどうかの処理
	void FirstCheckBlackjack();

	// @brief 持っているカードの合計値を計算する
	// @brief UpFaceOnly：trueなら表向きのカードのみ計算、falseなら全てのカードを計算
	int CalcHandValue(bool UpFaceOnly = false);

	// @brief 行動中かどうかを取得
	bool IsActioning() { return m_bCanAction; }

	// @brief 手札の合計値がバーストしているかどうかを取得
	bool IsBurst()
	{
		return CalcHandValue() > 21;
	}

	// @brief 手札の合計値がブラックジャック(21)かどうかを取得
	bool IsBlackjack()
	{
		return CalcHandValue() == 21;
	}

	// ネットワーク同期：Update内でローカルAI(Action)を走らせない
	void SetNetworkSync(bool enable) { m_bNetworkSync = enable; }

	// ネットワーク同期：現在のDealerDataに合わせて手札を再構築/更新
	void ApplyFromNetwork(const CBlackjack_Network& net);

private:

	// @brief 行動処理
	void Action();

	// @brief カードの位置調整
	void AdjustCardPositions();

	// ネットワーク用：手札を全破棄して0枚にする
	void ClearCards();

private:

	// @brief ディーラーが持っているカードリスト
	std::vector<CPlayingCard*> m_Cards;

	// @brief プレイヤーが行動可能かどうか
	bool m_bCanAction = true;

	// ネットワーク同期フラグ
	bool m_bNetworkSync = false;

	// ネットワーク同期: 前回適用したディーラー場札（差分判定用）
	std::vector<CPlayingCard::Info> m_LastSyncedTableCards;
};

