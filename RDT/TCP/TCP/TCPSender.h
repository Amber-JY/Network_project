#ifndef TCPSENDER_H
#define TCPSENDER_H

#include "RdtSender.h"

constexpr int N = 4;//����

class TCPSender : public RdtSender {
private:
	int nextseqnum;//��һ�������к�
	int base;//����
	int num;//���ڵ�ǰ�ѷ���������
	int count;//����ACK����
	bool waitingState;//�ȴ�״̬
	int current_ack;
	int last_ack;

	Packet packetWaitingAck[N];

public:
	bool send(const Message& message); 
	void receive(const Packet& ackpkt);
	void timeoutHandler(int nextseqnum);
	inline bool getWaitingState() { return waitingState; }

	TCPSender() :num(0), nextseqnum(0), base(0),count(0), waitingState(false)
	{

	}

	~TCPSender()
	{

	}


};



#endif