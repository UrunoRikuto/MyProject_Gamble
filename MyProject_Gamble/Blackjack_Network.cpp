/**************************************************//*
	@file	| Blackjack_Network.cpp
	@brief	| ブラックジャックネットワーククラスのcppファイル
	@note	| ネットワーク通信に関する処理を行う
*//**************************************************/
#include "Blackjack_Network.h"
#include "Main.h"
#include "Blackjack_Player.h"
#include "Defines.h"

//受信したHitResult/InitialCardsを貯める簡易キュー
static std::vector<CPlayingCard::Info> s_hitQueue;

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

	ReceiveData();

	// リセット検知後の再Join（擬似的な再接続）
	if (needsRejoin)
	{
		rejoinCooldown += fDeltaTime;
		if (rejoinCooldown >= RejoinCooldownInterval)
		{
			SendJoinRequest();
			rejoinCooldown =0.0f;
			// Join応答が来るまでneedsRejoinは維持（連打を避けるためクールダウンで間引く）
		}
	}

	if (myUserID == -1)
	{
		joinTimer += fDeltaTime;
		if (joinTimer >= JoinRetryInterval)
		{
			SendJoinRequest();
			joinTimer =0.0f;
		}
		return;
	}

	// Join時にInitialCardsが届くため、ここで初期2Hitは要求しない

	sendTimer += fDeltaTime;
	if (sendTimer >= SendInterval)
	{
		SendPlayerData();
		// サーバーはDealerStateをブロードキャストするため、定期DealerQueryは不要
		//SendDealerQuery();
		sendTimer =0.0f;
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

	// Stand状態（行動可能フラグの逆）
	bool isStand = false;
	if (auto* player = GetScene() ? GetScene()->GetGameObject<CBlackjack_Player>() : nullptr)
	{
		isStand = !player->CanAction();
	}
	sendData["IsStand"] = isStand;

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
	@brief　	| ヒット(Hit)要求送信処理
*//*****************************************/
void CBlackjack_Network::SendHitRequest(int requestId)
{
	if (!isInitialized || myUserID == -1) return;

	nlohmann::json sendData;
	sendData["Type"] = "Hit";
	sendData["UserID"] = myUserID;
	sendData["ProtocolVersion"] =1;
	sendData["RequestID"] = requestId;

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
	// DealerStateはサーバーが決定した場札を配信する想定
	dealer.IsActive = true;

	// --- リセット検知用に旧値を保持 ---
	const int prevTableCount = static_cast<int>(dealer.TableCards.size());

	dealer.DeckCards.clear();
	dealer.TableCards.clear();

	// サーバー判定フラグ
	dealer.AllPlayersStand = (received.contains("AllPlayersStand") && received["AllPlayersStand"].is_boolean())
		? received["AllPlayersStand"].get<bool>()
		: false;
	dealer.DealerFinished = (received.contains("DealerFinished") && received["DealerFinished"].is_boolean())
		? received["DealerFinished"].get<bool>()
		: false;

	auto parseCardArray = [](const nlohmann::json& arr, std::vector<CPlayingCard::Info>& out)
	{
		if (!arr.is_array()) return;
		for (const auto& c : arr)
		{
			if (!c.is_object()) continue;

			CPlayingCard::Info info{};
			if (c.contains("Suit"))
			{
				const std::string suitStr = c["Suit"].get<std::string>();
				if (suitStr == "Spade") info.m_eSuit = CPlayingCard::Suit::Spade;
				else if (suitStr == "Club") info.m_eSuit = CPlayingCard::Suit::Club;
				else if (suitStr == "Heart") info.m_eSuit = CPlayingCard::Suit::Heart;
				else if (suitStr == "Diamond") info.m_eSuit = CPlayingCard::Suit::Diamond;
				else info.m_eSuit = CPlayingCard::Suit::Spade;
			}
			if (c.contains("Number")) info.m_nNumber = c["Number"].get<int>();
			if (c.contains("FaceUp") && c["FaceUp"].is_boolean()) info.m_bFaceUp = c["FaceUp"].get<bool>();
			out.push_back(info);
		}
	};

	// DeckCardsは配信されない想定になった（DeckCountのみ）
	if (received.contains("TableCards")) parseCardArray(received["TableCards"], dealer.TableCards);

	dealer.LastTableCount = static_cast<int>(dealer.TableCards.size());

	//旧実装では「場札が減ったらリセット」と見なして再Joinしていたが、
	//それはタイミング依存で不自然になりやすい。
	// リセットはInitialCards（MyUserID付き）受信を確定トリガーとする。
	(void)prevTableCount;
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

			// --- server welcome / initial cards (Join/Reset配信) ---
			if (received.contains("MyUserID") && received["MyUserID"].is_number_integer())
			{
				// 自分宛のメッセージのみ処理
				const int uid = received["MyUserID"].get<int>();
				if (myUserID != -1 && uid != myUserID)
				{
					continue;
				}

				myUserID = uid;

				if (received.contains("DealerInitialized") && received["DealerInitialized"].is_boolean())
				{
					dealer.IsActive = received["DealerInitialized"].get<bool>();
				}

				if (received.contains("InitialCards") && received["InitialCards"].is_array())
				{
					// ---ここを「ラウンドリセットの確定トリガー」にする ---
					otherPlayers.clear();
					needsRejoin = false;
					rejoinCooldown =0.0f;

					// 新ラウンドの初期手札として扱うため、必ずキューを作り直す
					s_hitQueue.clear();

					for (const auto& c : received["InitialCards"])
					{
						if (!c.is_object()) continue;
						CPlayingCard::Info info{};
						if (c.contains("Suit") && c["Suit"].is_string())
						{
							const std::string suitStr = c["Suit"].get<std::string>();
							if (suitStr == "Spade") info.m_eSuit = CPlayingCard::Suit::Spade;
							else if (suitStr == "Club") info.m_eSuit = CPlayingCard::Suit::Club;
							else if (suitStr == "Heart") info.m_eSuit = CPlayingCard::Suit::Heart;
							else if (suitStr == "Diamond") info.m_eSuit = CPlayingCard::Suit::Diamond;
							else info.m_eSuit = CPlayingCard::Suit::Spade;
						}
						if (c.contains("Number") && c["Number"].is_number_integer())
							info.m_nNumber = c["Number"].get<int>();
						info.m_bFaceUp = !(c.contains("FaceUp") && c["FaceUp"].is_boolean()) ? true : c["FaceUp"].get<bool>();
						s_hitQueue.push_back(info);
					}

					// 「初期手札が届いた」= 新ラウンド開始として通知
					roundResetReceived = true;
				}

				continue;
			}

			const std::string type = (received.contains("Type") && received["Type"].is_string())
				? received["Type"].get<std::string>()
				: std::string();

			if (type == "DealerState")
			{
				HandleDealerState(received);
				continue;
			}

			// ヒット結果
			if (type == "HitResult")
			{
				if (received.contains("UserID") && received["UserID"].is_number_integer())
				{
					const int uid = received["UserID"].get<int>();
					if (uid != myUserID) continue;
				}

				if (received.contains("Card") && received["Card"].is_object())
				{
					const auto& c = received["Card"];
					CPlayingCard::Info info{};
					if (c.contains("Suit") && c["Suit"].is_string())
					{
						const std::string suitStr = c["Suit"].get<std::string>();
						if (suitStr == "Spade") info.m_eSuit = CPlayingCard::Suit::Spade;
						else if (suitStr == "Club") info.m_eSuit = CPlayingCard::Suit::Club;
						else if (suitStr == "Heart") info.m_eSuit = CPlayingCard::Suit::Heart;
						else if (suitStr == "Diamond") info.m_eSuit = CPlayingCard::Suit::Diamond;
						else info.m_eSuit = CPlayingCard::Suit::Spade;
					}
					if (c.contains("Number") && c["Number"].is_number_integer())
						info.m_nNumber = c["Number"].get<int>();
					if (c.contains("FaceUp") && c["FaceUp"].is_boolean())
						info.m_bFaceUp = c["FaceUp"].get<bool>();

					s_hitQueue.push_back(info);
				}
				continue;
			}

			// 他プレイヤーの情報
			if (type == "PlayerState" && received.contains("UserID") && received["UserID"].is_number_integer())
			{
				const int otherUserID = received["UserID"].get<int>();
				if (otherUserID == myUserID) continue;

				OtherPlayerData& p = otherPlayers[otherUserID];
				p.IsActive = true;
				p.Cards.clear();

				if (received.contains("Cards") && received["Cards"].is_array())
				{
					for (const auto& cj : received["Cards"])
					{
						if (!cj.is_object()) continue;
						CPlayingCard::Info info{};
						if (cj.contains("Suit") && cj["Suit"].is_string())
						{
							const std::string suitStr = cj["Suit"].get<std::string>();
							if (suitStr == "Spade") info.m_eSuit = CPlayingCard::Suit::Spade;
							else if (suitStr == "Club") info.m_eSuit = CPlayingCard::Suit::Club;
							else if (suitStr == "Heart") info.m_eSuit = CPlayingCard::Suit::Heart;
							else if (suitStr == "Diamond") info.m_eSuit = CPlayingCard::Suit::Diamond;
							else info.m_eSuit = CPlayingCard::Suit::Spade;
						}
						if (cj.contains("Number") && cj["Number"].is_number_integer())
							info.m_nNumber = cj["Number"].get<int>();
						p.Cards.push_back(info);
					}
				}
				continue;
			}
		}
		catch (const nlohmann::json::parse_error&)
		{
		}
	}
}

bool CBlackjack_Network::ConsumeHitResult(CPlayingCard::Info& outCard)
{
	if (!s_hitQueue.empty())
	{
		outCard = s_hitQueue.front();
		s_hitQueue.erase(s_hitQueue.begin());
		return true;
	}
	return false;
}

bool CBlackjack_Network::ConsumeRoundReset()
{
	if (roundResetReceived)
	{
		roundResetReceived = false;
		return true;
	}
	return false;
}

void CBlackjack_Network::RequestHit()
{
	if (!isInitialized || myUserID == -1) return;
	++hitRequestId;
	SendHitRequest(hitRequestId);
}