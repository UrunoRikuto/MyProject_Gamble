/**************************************************//*
	@file	| PlayingCard.cpp
	@brief	| トランプカードクラスのcppファイル
	@note	| トランプカードのゲームオブジェクトを管理する
*//**************************************************/
#include "PlayingCard.h"
#include "Sprite3DRenderer.h"
#include "BillboardRenderer.h"
#include "ImguiSystem.h"

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CPlayingCard::CPlayingCard()	
	:CGameObject()
	, m_tCardInfo{ Suit::Spade, 1 }
{
	AddComponent<CBillboardRenderer>();
}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CPlayingCard::~CPlayingCard()
{
}

/*****************************************//*
	@brief　	| 初期化処理
*//*****************************************/
void CPlayingCard::Init()
{
	// 基底クラスの初期化処理
	CGameObject::Init();

	// スプライト3Dレンダラーコンポーネントの取得
	CBillboardRenderer* pSprite3DRenderer = GetComponent<CBillboardRenderer>();
	// テクスチャキーの設定
	pSprite3DRenderer->SetKey("PlayingCards");


	m_tParam.m_f3Size = DirectX::XMFLOAT3(20.0f, 28.0f, 0.0f);
	m_tParam.m_f3Rotate = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
}

/*****************************************//*
	@brief　	| 更新処理
*//*****************************************/
void CPlayingCard::Update()
{
	// 基底クラスの更新処理
	CGameObject::Update();
}

/*****************************************//*
	@brief　	| インスペクター表示処理
	@param		| isEnd：true:ImGuiのEnd()を呼ぶ false:呼ばない
	@return		| 表示した項目数
*//*****************************************/
int CPlayingCard::Inspecter(bool isEnd)
{
	int nItemCount = CGameObject::Inspecter(false);

	// スートの表示
	const char* pSuitList[] = { "Spade", "Club", "Heart", "Diamond" };
	int nSuitIndex = static_cast<int>(m_tCardInfo.m_eSuit);

	ImGui::Text("Suit:");
	ImGui::SameLine();
	ImGui::Text(pSuitList[nSuitIndex]);

	ImGui::Text("Number: %d", m_tCardInfo.m_nNumber);

	if(ImGui::CollapsingHeader("[Select]"))
	{
		ImGui::Text("Suit:");
		ImGui::SameLine();
		if (ImGui::Button("Spade"))m_tCardInfo.m_eSuit = Suit::Spade;
		ImGui::SameLine();
		if (ImGui::Button("Club"))m_tCardInfo.m_eSuit = Suit::Club;
		ImGui::SameLine();
		if (ImGui::Button("Heart"))m_tCardInfo.m_eSuit = Suit::Heart;
		ImGui::SameLine();
		if (ImGui::Button("Diamond"))m_tCardInfo.m_eSuit = Suit::Diamond;

		ImGui::Text("\n");
		ImGui::Text("Number:");
		for (int i = 1; i <= 13; i++)
		{
			if (ImGui::Button(std::to_string(i).c_str()))
			{
				m_tCardInfo.m_nNumber = i;
			}
			if (i % 7 != 0)
			{
				ImGui::SameLine();
			}
		}

		ImGui::Text("\n");
		ImGui::Text("Now Face:");
		ImGui::SameLine();
		if (m_bIsFaceUp)
		{
			ImGui::Text("Face Up");
		}
		else
		{
			ImGui::Text("Face Down");
		}

		if (ImGui::Button("Flip Card"))
		{
			m_bIsFaceUp = !m_bIsFaceUp;
		}

		Setting(m_tCardInfo.m_eSuit, m_tCardInfo.m_nNumber, m_bIsFaceUp);
	}


	if (isEnd)
	{
		ImGui::End();
	}

	return nItemCount;
}

/*****************************************//*
	@brief　	| カード情報設定
	@param		| eSuit：スート
	@param		| nNumber：ナンバー
*//*****************************************/
void CPlayingCard::Setting(Suit eSuit, int nNumber, bool bFaceUp)
{
	// スートとナンバーを保存
	m_tCardInfo = { eSuit, nNumber };

	// 表向きかどうかを保存
	m_bIsFaceUp = bFaceUp;

	CBillboardRenderer* pRenderer = GetComponent<CBillboardRenderer>();
	if (m_bIsFaceUp)
	{
		pRenderer->SetKey("PlayingCards");

		// トランプカード1枚分のUVサイズを設定
		m_tParam.m_f2UVSize = DirectX::XMFLOAT2(1.0f / 13.0f, 1.0f / 4.0f);

		// トランプのナンバーのUV座標を設定
		m_tParam.m_f2UVPos = DirectX::XMFLOAT2(m_tParam.m_f2UVSize.x * (m_tCardInfo.m_nNumber - 1), m_tParam.m_f2UVSize.y * static_cast<int>(m_tCardInfo.m_eSuit));
	}
	else
	{
		pRenderer->SetKey("PlayingCardBack");

		// UVサイズをカード全体に設定
		m_tParam.m_f2UVSize = DirectX::XMFLOAT2(1.0f, 1.0f);

		// UV座標を(0,0)に設定
		m_tParam.m_f2UVPos = DirectX::XMFLOAT2(0.0f, 0.0f);
	}
}

/*****************************************//*
	@brief　	| カードの向きを反転
*//*****************************************/
void CPlayingCard::FlipCard()
{
	m_bIsFaceUp = !m_bIsFaceUp;


	CBillboardRenderer* pRenderer = GetComponent<CBillboardRenderer>();
	if (m_bIsFaceUp)
	{
		pRenderer->SetKey("PlayingCards");

		// トランプカード1枚分のUVサイズを設定
		m_tParam.m_f2UVSize = DirectX::XMFLOAT2(1.0f / 13.0f, 1.0f / 4.0f);

		// トランプのナンバーのUV座標を設定
		m_tParam.m_f2UVPos = DirectX::XMFLOAT2(m_tParam.m_f2UVSize.x * (m_tCardInfo.m_nNumber - 1), m_tParam.m_f2UVSize.y * static_cast<int>(m_tCardInfo.m_eSuit));
	}
	else
	{
		pRenderer->SetKey("PlayingCardBack");

		// UVサイズをカード全体に設定
		m_tParam.m_f2UVSize = DirectX::XMFLOAT2(1.0f, 1.0f);

		// UV座標を(0,0)に設定
		m_tParam.m_f2UVPos = DirectX::XMFLOAT2(0.0f, 0.0f);
	}
}
