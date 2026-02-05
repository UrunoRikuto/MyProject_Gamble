/**************************************************//*
	@file	| PlayingCard.h
	@brief	| トランプカードクラスのhファイル
	@note	| トランプカードのゲームオブジェクトを管理する
*//**************************************************/
#pragma once
#include "GameObject.h"

// @brief トランプカードクラス
class CPlayingCard : public CGameObject
{
public:
	enum class Suit
	{
		Spade,	// スペード
		Club,	// クラブ
		Heart,	// ハート
		Diamond,// ダイヤ
	};
	std::string GetStringSuit() const
	{
		switch (m_tCardInfo.m_eSuit)
		{
		case Suit::Spade:
			return "Spade";
		case Suit::Club:
			return "Club";
		case Suit::Heart:
			return "Heart";
		case Suit::Diamond:
			return "Diamond";
		default:
			return "Unknown";
		}
	}

	// @brief トランプのカード情報構造体
	struct Info
	{
		Suit m_eSuit;	// カードのスート
		int m_nNumber;				// カードの数字
		bool m_bFaceUp = true; // 表向きかどうか（ネットワーク同期用）
	};

public:
	// @brief コンストラクタ
	CPlayingCard();

	// @brief デストラクタ
	~CPlayingCard();

	// @brief 初期化処理
	void Init() override;

	// @brief 更新処理
	void Update() override;

	// @brief インスペクター表示処理
	int Inspecter(bool isEnd = true) override;

	// @brief カード情報設定
	void Setting(Suit eSuit, int nNumber, bool bFaceUp = true);
	
	// @brief カードのスート取得
	Suit GetSuit() const { return m_tCardInfo.m_eSuit; }

	// @brief カードの数字取得
	int GetNumber() const { return m_tCardInfo.m_nNumber; }

	// @brief カードの向きを表にする
	void FaceUp();
	// @brief カードの向きが表かどうか取得
	bool IsFaceUp() const { return m_bIsFaceUp; }

private:
	bool m_bIsFaceUp = true; // カードが表向きかどうか

	Info m_tCardInfo;	// カード情報構造体

};
