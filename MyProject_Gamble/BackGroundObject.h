/**************************************************//*
	@file	| BackGroundObject.h
	@brief	| 背景オブジェクトクラスのhファイル
	@note	| 背景オブジェクトのゲームオブジェクトを管理する
*//**************************************************/
#pragma once
#include "GameObject.h"

// @brief 背景オブジェクトクラス
class CBackGroundObject : public CGameObject
{
public:
	// @brief コンストラクタ
	CBackGroundObject();
	// @brief デストラクタ
	~CBackGroundObject();
	// @brief 初期化処理
	void Init() override;
};

