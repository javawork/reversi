#pragma once

namespace google
{
	namespace protobuf
	{
		class Message;
	}
}

class CSendPacket
{
public:
	CSendPacket(void);
	~CSendPacket(void);

	void Init();
	void SetID(USHORT Id);
	bool PackProtoBuf(::google::protobuf::Message *pMsg);

	const char* GetPacket();
	int GetPacketSize();

protected:
	char m_Buffer[MAX_PACKET_SIZE];
};


inline void CSendPacket::SetID(USHORT Id)
{
	*(USHORT*)(m_Buffer+Protocol_Position_Of_Id) = Id;
}

inline const char* CSendPacket::GetPacket()
{
	return m_Buffer;
}

inline int CSendPacket::GetPacketSize()
{
	return (*(USHORT*)m_Buffer)+Protocol_Header_Total_Size;
}
