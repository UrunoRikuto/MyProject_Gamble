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
#include "Blackjack_Network.h"


static int CalcHandValueSimple(const std::vector<CPlayingCard::Info>& cards)
{
	int total =0;
	int aceCount =0;
	for (const auto& c : cards)
	{
		int v = c.m_nNumber;
		if (v >10) v =10;
		if (v ==1) { v =11; ++aceCount; }
		total += v;
	}
	while (total >21 && aceCount >0)
	{
		total -=10;
		--aceCount;
	}
	return total;
}

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

	// 他プレイヤーのUI表示
	DrawOtherPlayersUI_Blackjack();

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

	// フォントの設定（ブラックジャックUIはデフォルトフォントで表示）

	// ウィンドウの開始
	ImGui::Begin("Dealer UI", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
	ImGui::SetWindowFontScale(3.0f);

	// カードの合計値表示
	int nCardScore = pDealer->CalcHandValue(true);

	ImGui::Text("%d", nCardScore);

	ImGui::End();
	// フォントの解除
}

/****************************************//*
	@brief　	| ブラックジャックのプレイヤーUI表示
*//****************************************/
void CImguiSystem::DrawPlayerUI_Blackjack(CScene* In_pScene)
{
	// プレイヤーのポインタ取得
	CBlackjack_Player* pPlayer = In_pScene->GetGameObject<CBlackjack_Player>();

	// ウィンドウの設定
	ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT - 350));
	ImGui::SetNextWindowSize(ImVec2(280, 180));

	ImGui::Begin("Player UI", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
	ImGui::SetWindowFontScale(3.0f);

	int nCardScore = pPlayer->CalcHandValue(pPlayer->GetPlayerCards());
	ImGui::Text("%d", nCardScore);

	ImGui::End();
}

/****************************************//*
	@brief　	| ブラックジャックの操作キー説明UI表示
*//****************************************/
void CImguiSystem::DrawControlUI_Blackjack()
{
	ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH /2.0f +220, SCREEN_HEIGHT -350));
	ImGui::SetNextWindowSize(ImVec2(400,500));

	ImGui::Begin("Control UI", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
	ImGui::SetWindowFontScale(2.0f);
	ImGui::Text("[ Q ]Hit");
	ImGui::Text("[ W ]Stand");
	ImGui::Text("[ E ]Double Down");

	ImGui::End();
}

void CImguiSystem::DrawOtherPlayersUI_Blackjack()
{
	if (!g_pNetwork) return;

	const auto& others = g_pNetwork->GetOtherPlayers();
	if (others.empty()) return;

	ImGui::SetNextWindowPos(ImVec2(20.0f,20.0f));
	ImGui::SetNextWindowSize(ImVec2(240.0f,300.0f));

	ImGui::Begin("Other Players UI", nullptr,
		ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
	ImGui::SetWindowFontScale(2.0f);

	ImGui::Text("Other Players");
	for (const auto& kv : others)
	{
		const int userId = kv.first;
		const OtherPlayerData& p = kv.second;
		if (!p.IsActive) continue;

		const int sum = CalcHandValueSimple(p.Cards);
		ImGui::Text("ID:%d Sum:%d", userId, sum);
	}

	ImGui::End();
}
