#ifndef TCPSENDER_H
#define TCPSENDER_H

#include "RdtSender.h"

constexpr int N = 4;//窗口

class TCPSender : public RdtSender {
private:
	int nextseqnum;//下一发送序列号
	int base;//基号
	int num;//窗口当前已发送序列数
	int count;//冗余ACK计数
	bool waitingState;//等待状态
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