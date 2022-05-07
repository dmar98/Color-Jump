#pragma once

class PlayerDataManager
{
public:
	static bool StaticInit();
	static std::unique_ptr<PlayerDataManager> sInstance;

	struct PlayerData
	{
		PlayerData() : m_player_name("Handsome Player"), m_ip_address("127.0.0.1")
		{
		}

		std::string m_player_name;
		std::string m_ip_address;
	};

	void Save() const;
	void Load();
	PlayerData& GetData();
	void SetIp(const string& string);
	std::string GetIP();
	void SetName(const string& string);
	std::string GetName();

private:
	PlayerDataManager();
	PlayerData m_player_data;
};
