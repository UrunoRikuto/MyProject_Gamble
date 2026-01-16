/**************************************************//*
	@file	| Blackjack_GameManager.cpp
	@brief	| ブラックジャックゲームマネージャークラスのcppファイル
	@note	| ブラックジャックゲーム全体の管理を行うシングルトンクラス
*//**************************************************/
#include "Blackjack_GameManager.h"
#include <algorithm>
#include <random>

/*****************************************//*
	@brief　	| コンストラクタ
*//*****************************************/
CBlackjack_GameManager::CBlackjack_GameManager()
	: m_nPlayerNum(0)
	, m_CardList()
	, m_bIsGameProgress(false)
{
}

/*****************************************//*
	@brief　	| デストラクタ
*//*****************************************/
CBlackjack_GameManager::~CBlackjack_GameManager()
{
}

/*****************************************//*
	@brief　	| 初期化処理
*//*****************************************/
void CBlackjack_GameManager::Init()
{
	// トランプの初期化
	m_CardList.clear();
	for (int suit = 0; suit < 4; ++suit)
	{
		for (int number = 1; number <= 13; ++number)
		{
			CPlayingCard::Info cardInfo;
			cardInfo.m_eSuit = static_cast<CPlayingCard::Suit>(suit);
			cardInfo.m_nNumber = number;
			m_CardList.push_back(cardInfo);
		}
	}

	// カードリストのシャッフル
	std::vector<CPlayingCard::Info> cardVector(m_CardList.begin(), m_CardList.end());
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(cardVector.begin(), cardVector.end(), g);
	m_CardList = std::list<CPlayingCard::Info>(cardVector.begin(), cardVector.end());
}

/*****************************************//*
	@brief　	| 終了処理
*//*****************************************/
void CBlackjack_GameManager::Uninit()
{
}

/*****************************************//*
	@brief　	| 更新処理
*//*****************************************/
void CBlackjack_GameManager::Update()
{
}

/*****************************************//*
	@brief　	| プレイ開始処理
	@param		| nPlayerNum：プレイヤー人数
*//*****************************************/
void CBlackjack_GameManager::StartGame(int nPlayerNum)
{
	// プレイヤー人数の設定
	m_nPlayerNum = nPlayerNum;

	// ゲーム進行中フラグを立てる
	m_bIsGameProgress = true;
}

/*****************************************//*
	@brief　	| カードを配る
	@return		| 配られたカード情報構造体
*//*****************************************/
CPlayingCard::Info CBlackjack_GameManager::DealCard()
{
	// 先頭のカードを取得してリストから削除
	CPlayingCard::Info dealtCard = m_CardList.front();
	m_CardList.pop_front();
	// 配られたカード情報を返す
	return dealtCard;
}
