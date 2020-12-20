#ifndef GBNRDTSENDER_H
#define GBNRDTSENDER_H

#include "RdtSender.h"

constexpr int N = 4;//����

class GBNRdtSender : public RdtSender {
	private:
		int nextseqnum;//��һ�������к�
		int base;//����
		int num;//���ڵ�ǰ�ѷ���������
		bool waitingState;//�ȴ�״̬

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