/**************************************************//*
	@file	| Blackjack_Network.h
	@brief	| ブラックジャックネットワーククラスのヘッダファイル
	@note	| ネットワーク通信に関する処理を行う
*//**************************************************/
#pragma once
#include "json.hpp"
//#include <WinSock2.h>
#include <map>
#include <vector>
#include <string>
#include "PlayingCard.h"

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

#define BUFFER_SIZE (4096)

// 他プレイヤーデータ構造体
struct OtherPlayerData
{
	std::vector<CPlayingCard::Info> Cards; // プレイヤーが持っているカードリスト
	int AnimationNo = 0;
	bool IsActive = false;
};

// ディーラー情報(山札/場札)を保持
struct DealerData
{
	// 山札(デッキ)のカード情報
	std::vector<CPlayingCard::Info> DeckCards;

	// 場に出しているカード情報
	std::vector<CPlayingCard::Info> TableCards;

	// サーバー権威: 全員Standが揃っているか
	bool AllPlayersStand = false;

	// サーバー権威: ディーラー行動が完了したか（AllPlayersStand==true時に有効）
	bool DealerFinished = false;

	bool IsActive = false;

	// クライアント用: 直近に受信したTableCards数（差分/リセット検知の補助）
	int LastTableCount = 0;
};

class CBlackjack_Network;
extern CBlackjack_Network* g_pNetwork;

// ブラックジャックネットワーククラス
class CBlackjack_Network
{
public:
	void Init();
	void Update();
	void Uninit();

	// ディーラー情報の取得（ネットワーク受信値）
	const DealerData& GetDealerData() const { return dealer; }

	// ヒット要求（サーバー権威で1枚配ってもらう）
	void RequestHit();

	//受信したHitResult（最新）を取り出す（1回読んだら消費）
	// outCardに成功時カードが入る
	bool ConsumeHitResult(CPlayingCard::Info& outCard);

	// リセット後の再JoinでInitialCardsを受信したフレームにtrueになり、1回読まれたらfalseになる
	bool ConsumeRoundReset();

private:
    // ネットワーク関連
    int sock = -1;
    struct sockaddr_in server_addr;
    bool isInitialized = false;
    int myUserID = -1;

    // 送受信バッファ
    char recvBuffer[BUFFER_SIZE];

    // 他プレイヤー管理
    std::map<int, OtherPlayerData> otherPlayers;

    // ディーラー情報
    DealerData dealer;
    bool dealerRequested = false;

    // Hit要求/応答
    int hitRequestId =0;
    bool hasPendingHitResult = false;
    CPlayingCard::Info pendingHitCard{};

    // リセット検知後に擬似的に再Joinする
    bool needsRejoin = false;
    float rejoinCooldown =0.0f;
    float RejoinCooldownInterval =0.5f;

    // 初期化・終了
    void InitializeNetwork();
    void CleanupNetwork();

    // 送受信
    void SendJoinRequest();
    void SendPlayerData();
    void ReceiveData();

    // 送信(ディーラー)
    void SendDealerQuery();

    // 受信(ディーラー)
    void HandleDealerState(const nlohmann::json& received);

    // 送信(ヒット要求)
    void SendHitRequest(int requestId);

    // 受信(ヒット結果)
    void HandleHitResult(const nlohmann::json& received);

    // 送信タイマー
    float sendTimer = 0.0f;

    // Join再送用
    float joinTimer = 0.0f;

public:
    // プロパティ
    std::string ServerIP = "127.0.0.1";
    int ServerPort = 50009;
    float SendInterval = 0.05f;  // 送信間隔(秒)

    // Join用（必要ならUI等から変更して使う）
    std::string PlayerName = "Player";
    float JoinRetryInterval = 1.0f; // myUserID未確定時のJoin再送間隔(秒)

    bool roundResetReceived = false;
};

