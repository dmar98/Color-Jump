#include "ColorJumpClientPCH.hpp"

std::unique_ptr<TextureManager> TextureManager::sInstance;

void TextureManager::StaticInit()
{
	sInstance.reset(new TextureManager());
}

TextureManager::TextureManager()
{
	//Menu Textures
	CacheTexture("background", "../Assets/Media/Textures/TitleScreen.png");
	CacheTexture("button", "../Assets/Media/Textures/Buttons.png");
	CacheTexture("Tutorial 1", "../Assets/Media/Textures/Tutorial_Color_Collisions.png");
	CacheTexture("Tutorial 2", "../Assets/Media/Textures/Tutorial_Impact_Platforms.png");
	CacheTexture("Tutorial 3", "../Assets/Media/Textures/Tutorial_Checkpoints.png");

	//Level Textures
	CacheTexture("LevelTileSet", "../Assets/Media/Textures/spritesheet.png");
	CacheTexture("HImpactBlankPlatform", "../Assets/Media/Textures/BlankPlatform.png");
	CacheTexture("HImpactRedPlatform", "../Assets/Media/Textures/RedImpactPlatform.png");
	CacheTexture("HImpactBluePlatform", "../Assets/Media/Textures/BlueImpactPlatform.png");

	CacheTexture("VImpactBlankPlatform", "../Assets/Media/Textures/VBlankPlatform.png");
	CacheTexture("VNormalPlatform", "../Assets/Media/Textures/VNormalPlatform.png");
	CacheTexture("VImpactRedPlatform", "../Assets/Media/Textures/VRedImpactPlatform.png");
	CacheTexture("VImpactBluePlatform", "../Assets/Media/Textures/VBlueImpactPlatform.png");

	CacheTexture("ActivatedCheckpointPlatform", "../Assets/Media/Textures/CheckpointPlatformActivated.png");
}

TexturePtr TextureManager::GetTexture(const string& inTextureName)
{
	return mNameToTextureMap[inTextureName];
}

bool TextureManager::CacheTexture(const string& inName, const char* inFileName)
{
	const TexturePtr newTexture(new sf::Texture());
	if (!newTexture->loadFromFile(inFileName))
	{
		return false;
	}

	mNameToTextureMap[inName] = newTexture;

	return true;
}
