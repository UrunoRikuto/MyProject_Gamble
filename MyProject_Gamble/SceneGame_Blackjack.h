/**************************************************//*
	@file	| SceneGame_Blackjack.h
	@brief	| ブラックジャックゲームシーンクラスのhファイル
	@note	| ブラックジャックゲームシーン内のゲームオブジェクト管理、更新、描画等を行う
			| CSceneを継承
*//**************************************************/
#pragma once
#include "Scene.h"

// @brief ブラックジャックゲームシーンクラス
class CSceneGame_Blackjack : public CScene
{
public:

	// @brief コンストラクタ
	CSceneGame_Blackjack();

	// @brief デストラクタ
	~CSceneGame_Blackjack();

	// @brief 初期化処理
	void Init() override;

	// @brief 更新処理
	void Update() override;

	// @brief 描画処理
	void Draw() override;
};

