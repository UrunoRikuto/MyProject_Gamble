/**************************************************//*
	@file	| BackGroundObject.cpp
	@brief	| 背景オブジェクトクラスのcppファイル
	@note	| 背景オブジェクトのゲームオブジェクトを管理する
*//**************************************************/
#include "BackGroundObject.h"
#include "Sprite3DRenderer.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CBackGroundObject::CBackGroundObject()
	:CGameObject()
{
	AddComponent<CSprite3DRenderer>();
}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CBackGroundObject::~CBackGroundObject()
{
}

/*****************************************//*
	@brief　	| 初期化処理
*//*****************************************/
void CBackGroundObject::Init()
{
	// 基底クラスの初期化処理
	CGameObject::Init();

	// 描画コンポーネントのパラメータ設定
	m_tParam.m_f3Pos = DirectX::XMFLOAT3(0.0f, 0.0f, -10.0f);
	m_tParam.m_f3Size = DirectX::XMFLOAT3(230, 130, 0.0f);
	m_tParam.m_f3Rotate = DirectX::XMFLOAT3(0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f);
}