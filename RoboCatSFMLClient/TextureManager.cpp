#include "RoboCatClientPCH.hpp"

std::unique_ptr<TextureManager> TextureManager::sInstance;

void TextureManager::StaticInit()
{
	sInstance.reset(new TextureManager());
}

TextureManager::TextureManager()
{
	CacheTexture("background", "../Assets/Media/Textures/TitleScreen.png");
	CacheTexture("button", "../Assets/Media/Textures/Buttons.png");
	CacheTexture("LevelTileSet", "../Assets/Media/Textures/spritesheet.png");
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
