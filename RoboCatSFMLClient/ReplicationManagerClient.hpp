#pragma once
class ReplicationManagerClient
{
public:
	static void Read(InputMemoryBitStream& inInputStream);

private:
	static void ReadAndDoCreateAction(InputMemoryBitStream& inInputStream, int inNetworkId);
	static void ReadAndDoUpdateAction(InputMemoryBitStream& inInputStream, int inNetworkId);
	static void ReadAndDoDestroyAction(int inNetworkId);
};

