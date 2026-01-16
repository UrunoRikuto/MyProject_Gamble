/**************************************************//*
	@file	| CameraGame_Blackjack.h
	@brief	| ブラックジャックゲーム用カメラクラスのhファイル
	@note	| ブラックジャックゲーム用のカメラクラス
*//**************************************************/
#pragma once
#include "Camera.h"

// @brief ブラックジャックゲーム用カメラクラス
class CCameraGame_Blackjack : public CCamera
{
public:
	// @brief コンストラクタ
	CCameraGame_Blackjack();

	// @brief デストラクタ
	~CCameraGame_Blackjack();

	// @brief 更新処理
	void Update() override;
};

