#include "RoboCatClientPCH.hpp"

std::unique_ptr<TextureManager> TextureManager::sInstance;

void TextureManager::StaticInit()
{
	sInstance.reset(new TextureManager());
}

TextureManager::TextureManager()
{
	CacheTexture("cat", "../Assets/cat.png");
	CacheTexture("mouse", "../Assets/mouse.png");
	CacheTexture("yarn", "../Assets/yarn.png");
	CacheTexture("background", "../Assets/Media/Textures/TitleScreen.png");
	CacheTexture("button", "../Assets/Media/Textures/Buttons.png");
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
