#pragma once

class WorldClient : public World
{
public:
	WorldClient();

	void LoadLevel() override;
	void Update() override;

	void SetCamera(sf::View camera);
	sf::View& GetCamera();

protected:
	void HandleCollisions() override;

private:
	sf::View m_camera;
	Platform* m_checkpoint;
	/*Character* m_client_player;
	Character* m_team_mate;*/
	std::function<void()> m_lose_callback;
};

