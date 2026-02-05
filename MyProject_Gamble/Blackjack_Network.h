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

	bool IsActive = false;
};

// ブラックジャックネットワーククラス
class CBlackjack_Network
{
public:
	void Init();
	void Update();
	void Uninit();

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
};

