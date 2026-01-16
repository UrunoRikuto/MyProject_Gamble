/**************************************************//*
	@file	| CameraGame_Blackjack.cpp
	@brief	| ブラックジャックゲーム用カメラクラスのcppファイル
	@note	| ブラックジャックゲーム用のカメラを管理する
*//**************************************************/
#include "CameraGame_Blackjack.h"

constexpr float ce_fRadXZ = 0.0f;
constexpr float ce_fRadY = DirectX::XMConvertToRadians(0.0f);
constexpr float ce_fRadius = 100.0f;

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CCameraGame_Blackjack::CCameraGame_Blackjack()
	: CCamera()
{
}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CCameraGame_Blackjack::~CCameraGame_Blackjack()
{
}

/*****************************************//*
	@brief　	| 更新処理
*//*****************************************/
void CCameraGame_Blackjack::Update()
{
	// カメラの位置計算
	m_f3Pos.x = ce_fRadius * cosf(ce_fRadY) * sinf(ce_fRadXZ);
	m_f3Pos.y = ce_fRadius * sinf(ce_fRadY);
	m_f3Pos.z = ce_fRadius * cosf(ce_fRadY) * cosf(ce_fRadXZ);

	// カメラの注視点を原点に設定
	m_f3Look = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
}