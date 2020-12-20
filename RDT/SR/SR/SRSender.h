#ifndef SRSENDER_H
#define SRSENDER_H

#include "RdtSender.h"

//constexpr int N = 8;//����

class SRSender : public RdtSender {
private:
	int nextseqnum;//��һ�������к�
	int base;//����
	int num;//���ڵ�ǰ�ѷ���������
	bool waitingState;//�ȴ�״̬

	int cache[N / 2];
	Packet packetWaitingAck[N / 2];

public:
	bool send(const Message& message);
	void receive(const Packet& ackpkt);
	void timeoutHandler(int nextseqnum);
	inline bool getWaitingState() { return waitingState; }

	SRSender() :num(0), nextseqnum(0), base(0), waitingState(false)
	{

	}

	~SRSender()
	{

	}


};



#endif