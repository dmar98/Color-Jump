#pragma once
class WorldServer final : public World
{
public:
	~WorldServer() override;
	void Update() override;
	void LoadLevel() override;
	Character* AddCharacter(int identifier, EColorType color) override;
	Character* AddGhostCharacter(const int identifier, const EColorType color) override;
protected:
	void HandleCollisions() override;
public:
	WorldServer();
};

