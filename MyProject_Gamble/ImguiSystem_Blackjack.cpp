/**************************************************//*
	@file	| ImguiSystem_Blackjack.cpp
	@brief	| ブラックジャック用Imguiシステムクラスのcppファイル
	@note	| ブラックジャック用のImguiUI表示を行う
*//**************************************************/
#include "ImguiSystem.h"
#include "Main.h"
#include "SceneGame_Blackjack.h"
#include "Blackjack_Croupier.h"
#include "Blackjack_Player.h"


/****************************************//*
	@brief　	| ブラックジャックのゲームUI表示
*//****************************************/
void CImguiSystem::DrawGameUI_Blackjack()
{
	CScene* pScene = GetScene();

	// ディーラーのUI表示
	DrawDealerUI_Blackjack(pScene);

	// プレイヤーのUI表示
	DrawPlayerUI_Blackjack(pScene);

	// 操作キー説明UI表示
	DrawControlUI_Blackjack();

}

/****************************************//*
	@brief　	| ブラックジャックのディーラーUI表示
*//****************************************/
void CImguiSystem::DrawDealerUI_Blackjack(CScene* In_pScene)
{
	// ディーラーのポインタ取得
	auto pDealer = In_pScene->GetGameObject<CBlackjack_Croupier>();

	// ウィンドウの設定
	ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH / 2.0f, 0));
	ImGui::SetNextWindowSize(ImVec2(280, 180));

	// フォントの設定
	ImGui::PushFont(GetFont(ce_fInGameFontSize));

	// ウィンドウの開始
	ImGui::Begin("Dealer UI", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);

	// カードの合計値表示
	int nCardScore = pDealer->CalcHandValue(true);

	ImGui::Text("%d", nCardScore);

	ImGui::End();
	// フォントの解除
	ImGui::PopFont();
}

/****************************************//*
	@brief　	| ブラックジャックのプレイヤーUI表示
*//****************************************/
void CImguiSystem::DrawPlayerUI_Blackjack(CScene* In_pScene)
{
	// プレイヤーのポインタ取得
	auto pPlayers = In_pScene->GetGameObjects<CBlackjack_Player>();
	

	for (auto pPlayer : pPlayers)
	{
		if (pPlayer->IsCurrentSplitHand())
		{
			// メインカードの合計値
			// ウィンドウの設定
			ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH / 2.0f + 200, SCREEN_HEIGHT - 350));
			ImGui::SetNextWindowSize(ImVec2(280, 180));
			// フォントの設定
			ImGui::PushFont(GetFont(ce_fInGameFontSize));
			// ウィンドウの開始
			ImGui::Begin("Player UI Main Hand", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
			// カードの合計値表示
			int nCardScore = pPlayer->CalcHandValue(pPlayer->GetPlayerCards());
			ImGui::Text("%d", nCardScore);
			ImGui::End();
			// フォントの解除
			ImGui::PopFont();

			// スプリットカードの合計値
			// ウィンドウの設定
			ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH / 2.0f - 200, SCREEN_HEIGHT - 350));
			ImGui::SetNextWindowSize(ImVec2(280, 180));
			// フォントの設定
			ImGui::PushFont(GetFont(ce_fInGameFontSize));
			// ウィンドウの開始
			ImGui::Begin("Player UI Split Hand", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
			// カードの合計値表示
			int nSplitCardScore = pPlayer->CalcHandValue(pPlayer->GetSplitCards());
			ImGui::Text("%d", nSplitCardScore);
			ImGui::End();
			// フォントの解除
			ImGui::PopFont();

		}
		else
		{
			// ウィンドウの設定
			ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT - 350));
			ImGui::SetNextWindowSize(ImVec2(280, 180));

			// フォントの設定
			ImGui::PushFont(GetFont(ce_fInGameFontSize));

			// ウィンドウの開始
			ImGui::Begin("Player UI", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
			
			// カードの合計値表示
			int nCardScore = pPlayer->CalcHandValue(pPlayer->GetPlayerCards());
			ImGui::Text("%d", nCardScore);

			ImGui::End();
			// フォントの解除
			ImGui::PopFont();
		}
	}
	
}

/****************************************//*
	@brief　	| ブラックジャックの操作キー説明UI表示
*//****************************************/
void CImguiSystem::DrawControlUI_Blackjack()
{
}
