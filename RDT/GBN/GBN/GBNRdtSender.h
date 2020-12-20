#ifndef GBNRDTSENDER_H
#define GBNRDTSENDER_H

#include "RdtSender.h"

constexpr int N = 4;//窗口

class GBNRdtSender : public RdtSender {
	private:
		int nextseqnum;//下一发送序列号
		int base;//基号
		int num;//窗口当前已发送序列数
		bool waitingState;//等待状态

		Packet packetWaitingAck[N];

	public:
		bool send(const Message& message);
		void receive(const Packet& ackpkt);
		void timeoutHandler(int nextseqnum);
		bool getWaitingState() { return waitingState; }

		GBNRdtSender() :num(0), nextseqnum(1), base(1), waitingState(false)
		{

		}

		~GBNRdtSender()
		{

		}


};



#endif