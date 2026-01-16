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

	// @brief トランプのカード情報構造体
	struct Info
	{
		Suit m_eSuit;	// カードのスート
		int m_nNumber;				// カードの数字
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
	Suit GetSuit() { return m_tCardInfo.m_eSuit; }

	// @brief カードの数字取得
	int GetNumber() { return m_tCardInfo.m_nNumber; }

	// @brief カードの向きを表にする
	void FaceUp();

private:
	bool m_bIsFaceUp = true; // カードが表向きかどうか

	Info m_tCardInfo;	// カード情報構造体

};
