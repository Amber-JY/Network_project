#include"Global.h"
#include"stdafx.h"
#include"GBNRdtSender.h"

bool GBNRdtSender::send(const Message& message) {
	if (this->waitingState) { //���ͷ����ڵȴ�ȷ��״̬
		cout << "���ͷ��ȴ���" << endl;
		return false;
	}
	
	if (this->nextseqnum < this->base + N) {
		this->waitingState = false;
		this->packetWaitingAck[num].acknum = -1; //���Ը��ֶ�
		this->packetWaitingAck[num].seqnum = this->nextseqnum;
		this->packetWaitingAck[num].checksum = 0;
		memcpy(this->packetWaitingAck[num].payload, message.data, sizeof(message.data));//��������

		this->packetWaitingAck[num].checksum = pUtils->calculateCheckSum(this->packetWaitingAck[num]);//����У���
		pUtils->printPacket("���ͷ����ͱ���", this->packetWaitingAck[num]);
		if (this->base == this->nextseqnum)
			pns->startTimer(SENDER, Configuration::TIME_OUT, this->packetWaitingAck[num].seqnum);
		pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[num]);
		this->num++;
		if (this->num >= N) {
			cout << "��������" << endl;
			this->waitingState = true;//��������
		}
		this->nextseqnum++;
		return true;

	}
	else {
		this->waitingState == true;
		return false;
	}
	
}

void GBNRdtSender::receive(const Packet& ackpkt) {
	if (this->num > 0) {
		int checksum = pUtils->calculateCheckSum(ackpkt);
		if (checksum == ackpkt.checksum && ackpkt.acknum >= this->base) {
			int pkt_acked = ackpkt.acknum - this->base + 1;//��ȷ�ϵķ�����

			this->base += pkt_acked;

			pUtils->printPacket("���ͷ�ȷ��", ackpkt);

			if (this->base == this->nextseqnum)
				pns->stopTimer(SENDER, this->packetWaitingAck[0].seqnum);//��ʱ����
			else {
				pns->stopTimer(SENDER, this->packetWaitingAck[0].seqnum);
				pns->startTimer(SENDER, Configuration::TIME_OUT, this->packetWaitingAck[pkt_acked].seqnum);
			}

			//����δ�������ݻ���
			for (int i = 0; i < num - pkt_acked; i++) {
				this->packetWaitingAck[i] = this->packetWaitingAck[i+ pkt_acked];
				printf("the %d member in the windows is seq%d. \n", i, this->packetWaitingAck[i].seqnum);
			}

			//�����ڷ�����Ŀ���¼���
			this->num -= pkt_acked;
			this->waitingState = false;
		}
	}
}

void GBNRdtSender::timeoutHandler(int nextseqnum) {
	pUtils->printPacket("��ʱ�ط�", this->packetWaitingAck[0]);
	pns->stopTimer(SENDER, this->packetWaitingAck[0].seqnum);
	pns->startTimer(SENDER, Configuration::TIME_OUT, this->packetWaitingAck[0].seqnum);//
	for (int i = 0; i < this->num; i++) {
		pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[i]);
	}
}