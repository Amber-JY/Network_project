#include"Global.h"
#include"stdafx.h"
#include"TCPSender.h"

bool TCPSender::send(const Message& message)
{
	if (this->nextseqnum < this->base + N) {
		this->packetWaitingAck[num].acknum = -1;//����
		this->packetWaitingAck[num].seqnum = this->nextseqnum;
		memcpy(this->packetWaitingAck[num].payload, message.data, sizeof(message.data));
		this->packetWaitingAck[num].checksum = pUtils->calculateCheckSum(this->packetWaitingAck[num]);

		if (this->nextseqnum == this->base) {
			pns->startTimer(SENDER, Configuration::TIME_OUT, this->packetWaitingAck[num].seqnum);
		}
		pUtils->printPacket("���ͷ��������ݣ�", this->packetWaitingAck[num]);
		pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[num]);

		this->nextseqnum++;
		if (this->nextseqnum >= this->base + N) 
			this->waitingState = true;
		this->num++;
		return true;


	}
	else {//��������
		this->waitingState = true;
		return false;
	}
}

void TCPSender::receive(const Packet& ackpkt)
{
	if (this->num > 0) {//��������δȷ�����ݱ�
		int check_sum = pUtils->calculateCheckSum(ackpkt);

		if (check_sum == ackpkt.checksum && ackpkt.acknum >= this->base) {//���У���
			if (ackpkt.acknum == this->packetWaitingAck[0].seqnum) {
				this->count++;
				if (this->count > 3) {//����ACK
					pns->stopTimer(SENDER, this->packetWaitingAck[0].seqnum);
					pUtils->printPacket("����ACK�ش���", this->packetWaitingAck[0]);
					pns->startTimer(SENDER, Configuration::TIME_OUT, this->packetWaitingAck[0].seqnum);
					pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[0]);
					pUtils->printPacket("���ͷ������ݱ���", this->packetWaitingAck[0]);
					this->count = 0;
					return;
				}
			}
			else
				this->count = 1;

			if (this->count == 1) {
				int ack_num = ackpkt.acknum - this->base+1;//��ȷ�ϣ������Թ��ģ�������
				this->base += ack_num;//�ۼ�ȷ��

				pUtils->printPacket("���ͷ����յ�ACK��", ackpkt);
				if (this->base == this->nextseqnum)//���ACKΪ�ļ�ĩβ��������ʱ
					pns->stopTimer(SENDER, this->packetWaitingAck[0].seqnum);
				else {
					pns->stopTimer(SENDER, this->packetWaitingAck[0].seqnum);
					pns->startTimer(SENDER, Configuration::TIME_OUT, this->packetWaitingAck[ack_num].seqnum);
				}
				//���ڻ���
				for (int i = 0; i < this->num - ack_num; i++) {
					this->packetWaitingAck[i] = this->packetWaitingAck[i + ack_num];
				}
				this->num = this->num - ack_num;
				this->waitingState = false;
			}
	
		}
	}
}

void TCPSender::timeoutHandler(int nextseqnum)
{
	pns->stopTimer(SENDER, this->packetWaitingAck[0].seqnum);
	pUtils->printPacket("��ʱ�ش���", this->packetWaitingAck[0]);
	pns->startTimer(SENDER, Configuration::TIME_OUT, this->packetWaitingAck[0].seqnum);
	pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[0]);
}