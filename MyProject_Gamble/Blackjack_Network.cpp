/**************************************************//*
	@file	| Blackjack_Network.cpp
	@brief	| ブラックジャックネットワーククラスのcppファイル
	@note	| ネットワーク通信に関する処理を行う
*//**************************************************/
#include "Blackjack_Network.h"
#include "Main.h"
#include "Blackjack_Player.h"
#include "Defines.h"

/*****************************************//*
	@brief　	| 初期化処理
*//*****************************************/
void CBlackjack_Network::Init()
{
	InitializeNetwork();
	if (!isInitialized) return;

	// UDPは落ちる前提なので、起動時にJoinを投げる（UserID割当要求）
	SendJoinRequest();
	joinTimer = 0.0f;
}

/*****************************************//*
	@brief　	| 更新処理
*//*****************************************/
void CBlackjack_Network::Update()
{
	if (!isInitialized) return;

	// データ受信
	ReceiveData();

	// UserID未確定ならJoinを再送（UDP取りこぼし対策）
	if (myUserID == -1)
	{
		joinTimer += fDeltaTime;
		if (joinTimer >= JoinRetryInterval)
		{
			SendJoinRequest();
			joinTimer = 0.0f;
		}
		return; // UserID確定するまでは通常送信しない
	}

	// ディーラー状態をまだ要求していなければ1回だけ要求
	if (!dealerRequested)
	{
		SendDealerQuery();
		dealerRequested = true;
	}

	// 定期的にデータ送信
	sendTimer += fDeltaTime;
	if (sendTimer >= SendInterval)
	{
		SendPlayerData();
		sendTimer = 0.0f;
	}
}

/*****************************************//*
	@brief　	| 終了処理
*//*****************************************/
void CBlackjack_Network::Uninit()
{
	CleanupNetwork();
}

/*****************************************//*
	@brief　	| ネットワーク初期化処理
*//*****************************************/
void CBlackjack_Network::InitializeNetwork()
{
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return;
	}
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET)
	{
		WSACleanup();
		return;
	}
	// ノンブロッキングモード
	u_long val = 1;
	ioctlsocket(sock, FIONBIO, &val);
	// サーバーアドレス設定
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(ServerPort);
	server_addr.sin_addr.S_un.S_addr = inet_addr(ServerIP.c_str());
	isInitialized = true;
}

/*****************************************//*
	@brief　	| ネットワーク終了処理
*//*****************************************/
void CBlackjack_Network::CleanupNetwork()
{
	if (sock != INVALID_SOCKET)
	{
		closesocket(sock);
		sock = INVALID_SOCKET;
	}
	WSACleanup();
	isInitialized = false;
}

/*****************************************//*
	@brief　	| 参加要求(Join/Hello)送信処理
*//*****************************************/
void CBlackjack_Network::SendJoinRequest()
{
	if (!isInitialized) return;

	nlohmann::json sendData;
	sendData["Type"] = "Join";
	sendData["Name"] = PlayerName;

	// クライアント/サーバの互換チェック用（運用で便利）
	sendData["ProtocolVersion"] = 1;

	std::string data_str = sendData.dump();

	int server_addr_len = sizeof(server_addr);
	int sent = sendto(sock, data_str.c_str(), static_cast<int>(data_str.length()), 0,
		(struct sockaddr*)&server_addr, server_addr_len);

	if (sent == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		(void)error;
	}
}

/*****************************************//*
	@brief　	| プレイヤーデータ送信処理
*//*****************************************/
void CBlackjack_Network::SendPlayerData()
{
	if (!isInitialized || myUserID == -1) return;

	// JSON作成
	nlohmann::json sendData;
	sendData["Type"] = "PlayerState";
	sendData["UserID"] = myUserID;

	// 所持しているカード情報を送信
	sendData["Cards"] = nlohmann::json::array();
	auto* player = GetScene() ? GetScene()->GetGameObject<CBlackjack_Player>() : nullptr;
	if (player)
	{
		for (const auto* card : player->GetPlayerCards())
		{
			if (!card) continue;
			nlohmann::json cardJson;
			cardJson["Suit"] = card->GetStringSuit();
			cardJson["Number"] = card->GetNumber();
			cardJson["FaceUp"] = card->IsFaceUp();
			sendData["Cards"].push_back(cardJson);
		}
	}

	std::string data_str = sendData.dump();

	//送信
	int server_addr_len = sizeof(server_addr);
	int sent = sendto(sock, data_str.c_str(), static_cast<int>(data_str.length()),0,
		(struct sockaddr*)&server_addr, server_addr_len);

	if (sent == SOCKET_ERROR) {
		int error = WSAGetLastError();
		(void)error;
	}
}

/*****************************************//*
	@brief　	| ディーラー状態要求(DealerQuery)送信処理
*//*****************************************/
void CBlackjack_Network::SendDealerQuery()
{
	if (!isInitialized || myUserID == -1) return;

	nlohmann::json sendData;
	sendData["Type"] = "DealerQuery";
	sendData["UserID"] = myUserID;
	sendData["ProtocolVersion"] =1;

	const std::string data_str = sendData.dump();
	const int server_addr_len = sizeof(server_addr);

	int sent = sendto(sock, data_str.c_str(), static_cast<int>(data_str.length()),0,
		(struct sockaddr*)&server_addr, server_addr_len);

	if (sent == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		(void)error;
	}
}

/*****************************************//*
	@brief　	| 受信したディーラー状態データの処理
*//*****************************************/
void CBlackjack_Network::HandleDealerState(const nlohmann::json& received)
{
	// DealerStateはサーバーが決定した山札/場札を配信する想定
	dealer.IsActive = true;

	dealer.DeckCards.clear();
	dealer.TableCards.clear();

	auto parseCardArray = [](const nlohmann::json& arr, std::vector<CPlayingCard::Info>& out)
	{
		if (!arr.is_array()) return;
		for (const auto& c : arr)
		{
			if (!c.is_object()) continue;

			CPlayingCard::Info info{};
			// Infoは (Suit, Number)なので JSONは "Suit"(string)/"Number"(int) を想定
			if (c.contains("Suit"))
			{
				const std::string suitStr = c["Suit"].get<std::string>();
				if (suitStr == "Spade") info.m_eSuit = CPlayingCard::Suit::Spade;
				else if (suitStr == "Club") info.m_eSuit = CPlayingCard::Suit::Club;
				else if (suitStr == "Heart") info.m_eSuit = CPlayingCard::Suit::Heart;
				else if (suitStr == "Diamond") info.m_eSuit = CPlayingCard::Suit::Diamond;
				else info.m_eSuit = CPlayingCard::Suit::Spade; // フォールバック
			}
			if (c.contains("Number")) info.m_nNumber = c["Number"].get<int>();

			out.push_back(info);
		}
	};

	if (received.contains("DeckCards")) parseCardArray(received["DeckCards"], dealer.DeckCards);
	if (received.contains("TableCards")) parseCardArray(received["TableCards"], dealer.TableCards);
}

/*****************************************//*
	@brief　	| データ受信処理
*//*****************************************/
void CBlackjack_Network::ReceiveData()
{
	if (!isInitialized) return;

	while (true)
	{
		int recv_size = recvfrom(sock, recvBuffer, BUFFER_SIZE -1,0, nullptr, nullptr);

		if (recv_size <=0)
		{
			int error = WSAGetLastError();
			if (error != WSAEWOULDBLOCK)
			{
			}
			break;
		}

		recvBuffer[recv_size] = '\0';

		try
		{
			nlohmann::json received = nlohmann::json::parse(recvBuffer);

			// 初回接続時のUserID割り当て
			if (received.contains("MyUserID"))
			{
				myUserID = received["MyUserID"];
				continue;
			}

			// ディーラー状態
			if (received.contains("Type") && received["Type"].is_string() && received["Type"].get<std::string>() == "DealerState")
			{
				HandleDealerState(received);
				continue;
			}

			// 他プレイヤーの情報
			if (received.contains("UserID"))
			{
				int otherUserID = received["UserID"];

				// 自分自身は無視
				if (otherUserID == myUserID) continue;
			}
		}
		catch (const nlohmann::json::parse_error&)
		{
		}
	}
}