#pragma once
inline uint32_t ConvertToFixed(const float inNumber, const float inMin, const float inPrecision)
{
	return static_cast<int>((inNumber - inMin) / inPrecision);
}

inline float ConvertFromFixed(const uint32_t inNumber, const float inMin, const float inPrecision)
{
	return static_cast<float>(inNumber) * inPrecision + inMin;
}


class OutputMemoryBitStream
{
public:
	OutputMemoryBitStream() :
		mBuffer(nullptr),
		mBitHead(0),
		mBitCapacity(0)
	{
		ReallocBuffer(1500 * 8);
	}

	~OutputMemoryBitStream() { std::free(mBuffer); }

	void WriteBits(uint8_t inData, uint32_t inBitCount);
	void WriteBits(const void* inData, uint32_t inBitCount);

	const char* GetBufferPtr() const { return mBuffer; }
	uint32_t GetBitLength() const { return mBitHead; }
	uint32_t GetByteLength() const { return (mBitHead + 7) >> 3; }

	void WriteBytes(const void* inData, uint32_t inByteCount)
	{
		WriteBits(inData, inByteCount << 3);
	}

	/*
	void Write( uint32_t inData, uint32_t inBitCount = 32 )	{ WriteBits( &inData, inBitCount ); }
	void Write( int inData, uint32_t inBitCount = 32 )		{ WriteBits( &inData, inBitCount ); }
	void Write( float inData )								{ WriteBits( &inData, 32 ); }

	void Write( uint16_t inData, uint32_t inBitCount = 16 )	{ WriteBits( &inData, inBitCount ); }
	void Write( int16_t inData, uint32_t inBitCount = 16 )	{ WriteBits( &inData, inBitCount ); }

	void Write( uint8_t inData, uint32_t inBitCount = 8 )	{ WriteBits( &inData, inBitCount ); }
	*/

	template <typename T>
	void Write(T inData, uint32_t inBitCount = sizeof(T) * 8)
	{
		static_assert(std::is_arithmetic_v<T> ||
			std::is_enum_v<T>,
			"Generic Write only supports primitive data types");
		WriteBits(&inData, inBitCount);
	}

	void Write(const bool inData) { WriteBits(&inData, 1); }

	void Write(const std::string& inString)
	{
		const uint32_t elementCount = inString.size();
		Write(elementCount, 4);
		for (const auto& element : inString)
		{
			Write(element);
		}
	}

private:
	void ReallocBuffer(uint32_t inNewBitCapacity);

	char* mBuffer;
	uint32_t mBitHead;
	uint32_t mBitCapacity;
};

class InputMemoryBitStream
{
public:
	InputMemoryBitStream(char* inBuffer, uint32_t inBitCount) :
		mBuffer(inBuffer),
		mBitHead(0),
		mBitCapacity(inBitCount),
		mIsBufferOwner(false)
	{
	}

	InputMemoryBitStream(const InputMemoryBitStream& inOther) :
		mBitHead(inOther.mBitHead),
		mBitCapacity(inOther.mBitCapacity),
		mIsBufferOwner(true)
	{
		//allocate buffer of right size
		const int byteCount = mBitCapacity / 8;
		mBuffer = static_cast<char*>(malloc(byteCount));
		//copy
		memcpy(mBuffer, inOther.mBuffer, byteCount);
	}

	~InputMemoryBitStream() { if (mIsBufferOwner) { free(mBuffer); } }

	const char* GetBufferPtr() const { return mBuffer; }
	uint32_t GetRemainingBitCount() const { return mBitCapacity - mBitHead; }

	void ReadBits(uint8_t& outData, uint32_t inBitCount);
	void ReadBits(void* outData, uint32_t inBitCount);

	void ReadBytes(void* outData, uint32_t inByteCount) { ReadBits(outData, inByteCount << 3); }

	template <typename T>
	void Read(T& inData, uint32_t inBitCount = sizeof(T) * 8)
	{
		static_assert(std::is_arithmetic_v<T> ||
			std::is_enum_v<T>,
			"Generic Read only supports primitive data types");
		ReadBits(&inData, inBitCount);
	}

	void Read(uint32_t& outData, uint32_t inBitCount = 32) { ReadBits(&outData, inBitCount); }
	void Read(int& outData, uint32_t inBitCount = 32) { ReadBits(&outData, inBitCount); }
	void Read(float& outData) { ReadBits(&outData, 32); }

	void Read(uint16_t& outData, uint32_t inBitCount = 16) { ReadBits(&outData, inBitCount); }
	void Read(int16_t& outData, uint32_t inBitCount = 16) { ReadBits(&outData, inBitCount); }

	void Read(uint8_t& outData, uint32_t inBitCount = 8) { ReadBits(&outData, inBitCount); }
	void Read(bool& outData) { ReadBits(&outData, 1); }

	void ResetToCapacity(uint32_t inByteCapacity)
	{
		mBitCapacity = inByteCapacity << 3;
		mBitHead = 0;
	}


	void Read(std::string& inString)
	{
		uint32_t elementCount = 0;
		Read(elementCount, 4);
		inString.resize(elementCount);
		for (auto& element : inString)
		{
			Read(element);
		}
	}

private:
	char* mBuffer;
	uint32_t mBitHead;
	uint32_t mBitCapacity;
	bool mIsBufferOwner;
};
