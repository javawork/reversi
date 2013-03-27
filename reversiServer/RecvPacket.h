#pragma once
class CRecvPacket
{
public:
	CRecvPacket(void);
	~CRecvPacket(void);

	void Init();

	char* GetHeaderBuffer();		//size header buffer pointer
	char* GetBodyBuffer();			//body buffer buffer pointer
	USHORT GetBodySize();			//body size
	USHORT GetId();

	bool CheckRecvHeader();		//check recv header


protected:
	char m_Buffer[MAX_PACKET_SIZE];
};

inline char* CRecvPacket::GetHeaderBuffer()
{
	return m_Buffer;
}

inline char* CRecvPacket::GetBodyBuffer()
{
	return m_Buffer+Protocol_Header_Total_Size;
}

inline USHORT CRecvPacket::GetBodySize()
{
	return *(USHORT*)m_Buffer;
}

inline USHORT CRecvPacket::GetId()
{
	return *(USHORT*)(m_Buffer+Protocol_Position_Of_Id);
}


