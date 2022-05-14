#include "ColorJumpPCH.hpp"

void OutputMemoryBitStream::WriteBits(const uint8_t inData,
                                      const uint32_t inBitCount)
{
	const uint32_t nextBitHead = mBitHead + inBitCount;

	if (nextBitHead > mBitCapacity)
	{
		ReallocBuffer(std::max(mBitCapacity * 2, nextBitHead));
	}

	//calculate the byteOffset into our buffer
	//by dividing the head by 8
	//and the bitOffset by taking the last 3 bits
	const uint32_t byteOffset = mBitHead >> 3;
	const uint32_t bitOffset = mBitHead & 0x7;

	const uint8_t currentMask = static_cast<char>(~(0xff << bitOffset));
	mBuffer[byteOffset] = static_cast<char>((mBuffer[byteOffset] & currentMask) | inData <<
		bitOffset);

	//calculate how many bits were not yet used in
	//our target byte in the buffer
	const uint32_t bitsFreeThisByte = 8 - bitOffset;

	//if we needed more than that, carry to the next byte
	if (bitsFreeThisByte < inBitCount)
	{
		//we need another byte
		mBuffer[byteOffset + 1] = inData >> bitsFreeThisByte;
	}

	mBitHead = nextBitHead;
}

void OutputMemoryBitStream::WriteBits(const void* inData, uint32_t inBitCount)
{
	auto srcByte = static_cast<const char*>(inData);
	//write all the bytes
	while (inBitCount > 8)
	{
		WriteBits(*srcByte, 8);
		++srcByte;
		inBitCount -= 8;
	}
	//write anything left
	if (inBitCount > 0)
	{
		WriteBits(*srcByte, inBitCount);
	}
}


void OutputMemoryBitStream::ReallocBuffer(const uint32_t inNewBitCapacity)
{
	if (mBuffer == nullptr)
	{
		//just need to memset on first allocation
		mBuffer = static_cast<char*>(std::malloc(inNewBitCapacity >> 3));
		memset(mBuffer, 0, inNewBitCapacity >> 3);
	}
	else
	{
		//need to memset, then copy the buffer
		const auto tempBuffer = static_cast<char*>(std::malloc(inNewBitCapacity >> 3));
		memset(tempBuffer, 0, inNewBitCapacity >> 3);
		memcpy(tempBuffer, mBuffer, mBitCapacity >> 3);
		std::free(mBuffer);
		mBuffer = tempBuffer;
	}

	//handle realloc failure
	//...
	mBitCapacity = inNewBitCapacity;
}


void test1()
{
	OutputMemoryBitStream mbs;

	mbs.WriteBits(11, 5);
	mbs.WriteBits(52, 6);
}

void InputMemoryBitStream::ReadBits(uint8_t& outData, uint32_t inBitCount)
{
	const uint32_t byteOffset = mBitHead >> 3;
	const uint32_t bitOffset = mBitHead & 0x7;

	outData = static_cast<uint8_t>(mBuffer[byteOffset]) >> bitOffset;

	const uint32_t bitsFreeThisByte = 8 - bitOffset;
	if (bitsFreeThisByte < inBitCount)
	{
		//we need another byte
		outData |= static_cast<uint8_t>(mBuffer[byteOffset + 1]) << bitsFreeThisByte;
	}

	//don't forget a mask so that we only read the bit we wanted...
	outData &= ~(0x00ff << inBitCount);

	mBitHead += inBitCount;
}

void InputMemoryBitStream::ReadBits(void* outData, uint32_t inBitCount)
{
	auto destByte = static_cast<uint8_t*>(outData);
	//write all the bytes
	while (inBitCount > 8)
	{
		ReadBits(*destByte, 8);
		++destByte;
		inBitCount -= 8;
	}
	//write anything left
	if (inBitCount > 0)
	{
		ReadBits(*destByte, inBitCount);
	}
}