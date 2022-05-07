#pragma once

class PlatformClient : public Platform
{
public:
	PlatformClient(sf::Int8 id, EPlatformType platform_type);
	void AddPlatformPart(PlatformPart* tile) override;
	bool HandlePlayerCollisionAndChangeColor(EColorType color_type) override;
	bool HandlePlayerCollision(EColorType color_type) override;
	void SetType(EPlatformType type) override;

	void Update() override;

protected:
	TexturePtr m_current_texture;

private:
	void SetTextureOnParts(const TexturePtr& texture);
};

