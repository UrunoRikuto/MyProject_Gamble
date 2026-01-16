/**************************************************//*
	@file	| ObjectLoad.cpp
	@brief	| オブジェクトロードクラスのcppファイル
	@note	| オブジェクトのロード、アンロードを行う
*//**************************************************/
#include "ObjectLoad.h"
#include "RendererComponent.h"
#include "Defines.h"

/*****************************************//*
	@brief　	| 全てのオブジェクトをロード
*//****************************************/
void CObjectLoad::LoadAll()
{
	// フェード用テクスチャのロード
    CRendererComponent::Load(RendererKind::Texture, TEXTURE_PATH("Fade.png"), "Fade");

	// トランプカードのテクスチャロード
	CRendererComponent::Load(RendererKind::Texture, TEXTURE_PATH("PlayingCards.png"), "PlayingCards");
	// トランプカードの裏面テクスチャロード
	CRendererComponent::Load(RendererKind::Texture, TEXTURE_PATH("PlayingCard_Back.png"), "PlayingCardBack");
}


/*****************************************//*
	@brief　	| 全てのオブジェクトをアンロード
*//****************************************/
void CObjectLoad::UnLoadAll()
{
    CRendererComponent::UnLoad();
}
