#ifndef SRSENDER_H
#define SRSENDER_H

#include "RdtSender.h"

//constexpr int N = 8;//窗口

class SRSender : public RdtSender {
private:
	int nextseqnum;//下一发送序列号
	int base;//基号
	int num;//窗口当前已发送序列数
	bool waitingState;//等待状态

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