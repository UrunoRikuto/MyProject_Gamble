/**************************************************//*
	@file	| ImguiSystem.h
	@brief	| Imguiシステムクラスのhファイル
	@note	| Imguiの初期化、更新、描画等を行う
			| シングルトンパターンで作成
*//**************************************************/
#pragma once
#include "GameObject.h"
#include "imgui.h"
#include <map>

constexpr ImVec2 ce_f2InspecterSize = ImVec2(250, 30);
// デバックフォントサイズ
constexpr float ce_fDebugFontSize = 20.0f;
// インゲームフォントサイズ
constexpr float ce_fInGameFontSize = 70.0f;

class CScene;

// @brief Imguiシステムクラス
class CImguiSystem
{
private:

	// @brief コンストラクタ
	CImguiSystem();

public:
	// デフォルトコンストラクタ禁止
	CImguiSystem(const CImguiSystem&) = delete;
	// 代入演算子禁止
	CImguiSystem& operator=(const CImguiSystem&) = delete;

	// @brief デストラクタ
	~CImguiSystem();

	// @brief インスタンスの取得
	static CImguiSystem* GetInstance();

	// @brief インスタンスの解放
	static void ReleaseInstance();

	// @brief 初期化処理
	void Init();

	// @brief 終了処理
	void Uninit();

	// @brief 更新処理
	void Update();

	// @brief 描画処理
	void Draw();

	// @brief 更新処理を管理するフラグの取得
	// @return true:更新処理を行う false:更新処理を止める
	bool IsUpdate() { return m_bUpdate; }
private:
	// @brief フォントロード関数
	void LoadFont(ImGuiIO& io, float In_fFontSize);

	// @brief フォント取得関数
	ImFont* GetFont(float In_fFontSize) { return m_FontMap[In_fFontSize]; }
private:
	// @brief 階層表示
	void DrawHierarchy();

	// @brief カメラのパラメータ表示
	void DrawCameraParam();

	// @brief 更新を止めるチェックボックス表示
	void DrawUpdateTick();

	// @brief 当たり判定を表示するチェックボックス表示
	void DrawCollision();

	// @brief フレームレート表示
	void DrawFPS();

private:// ブラックジャック UI
	// @brief ゲームUI表示
	void DrawGameUI_Blackjack();
	
	// @brief ディーラーUI表示
	void DrawDealerUI_Blackjack(CScene* In_pScene);

	// @brief プレイヤーUI表示
	void DrawPlayerUI_Blackjack(CScene* In_pScene);

	// @brief 操作キー説明UI表示
	void DrawControlUI_Blackjack();

private:
	// @brief インスタンス
	static CImguiSystem* m_pInstance;

	// @brief 選択しているゲームオブジェクト
	CGameObject* m_pGameObject;

	// @brief 更新処理を管理する
	// @note true:更新処理を行う false:更新処理を止める
	bool m_bUpdate;

	// @brief 当たり判定の表示フラグ
	// @note true:当たり判定を表示する false:当たり判定を表示しない
	bool m_bCollisionDraw;

	// @brief フォントマップ
	std::map<float, ImFont*> m_FontMap;
};

