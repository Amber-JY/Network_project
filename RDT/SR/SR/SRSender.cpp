#include"Global.h"
#include"stdafx.h"
#include"SRSender.h"

bool SRSender::send(const Message& message) {

	if (this->num < (N / 2)) {//�����п�λ
		if (this->num == 0) {
			for (int i = 0; i < (N / 2); i++) {
				this->cache[i] = 0;//��ʼ��cache
			}
		}
		this->waitingState = false;
		this->packetWaitingAck[num].acknum = -1; //���Ը��ֶ�
		this->packetWaitingAck[num].seqnum = this->nextseqnum;
		this->packetWaitingAck[num].checksum = 0;
		memcpy(this->packetWaitingAck[num].payload, message.data, sizeof(message.data));//��������

		this->packetWaitingAck[num].checksum = pUtils->calculateCheckSum(this->packetWaitingAck[num]);//����У���
		pUtils->printPacket("���ͷ����ͱ���", this->packetWaitingAck[num]);

		pns->startTimer(SENDER, Configuration::TIME_OUT, this->packetWaitingAck[num].seqnum);//ÿ�����鶼����һ����ʱ��
		pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[num]);
		this->num++;//������Ŀ��һ

		if (this->num == (N/2)) {
			cout << "��������" << endl;
			this->waitingState = true;//��������
		}
		this->nextseqnum = (this->nextseqnum + 1) % N;
		return true;

	}
	else {
		this->waitingState = true;
		return false;
	}

}

void SRSender::receive(const Packet& ackpkt) {
	bool inrange = false;
	int flag = 0;//��¼�յ������
	int check_sum = pUtils->calculateCheckSum(ackpkt);
	if (check_sum == ackpkt.checksum) {//ȷ��У���
		for (int i = 0; i < this->num; i++) {//�����յ�ack��Ӧ�ķ���
			if (ackpkt.acknum == this->packetWaitingAck[i].seqnum) {
				cache[i] = 1;
				flag = i;
				inrange = true;
				break;
			}
		}

		if (inrange) {
			if (cache[0] == 1) {//ack���ڻ����
				pUtils->printPacket("�յ�������λACK��", ackpkt);
				pns->stopTimer(SENDER, this->packetWaitingAck[flag].seqnum);
				int AckNum = 0;
				for (int i = 0; i < (N / 2); i++) {//������ȷ�Ϸ�����
					if (this->cache[i] != 0) {
						AckNum++;
					}
					else
						break;
				}
				if (AckNum != this->num) {
					for (int i = 0; i < this->num - AckNum; i++) {
						this->packetWaitingAck[i] = this->packetWaitingAck[i + AckNum];
						this->cache[i] = this->cache[i + AckNum];
						printf("����δȷ�ϵĵ�%dλ������seq %c\n", i, this->packetWaitingAck[i].payload[0]);
					}
					for (int i = 0; i < N / 2; i++)
						this->cache[i] = 0;
				}
				//����ȷ�Ϸ����ڿյĲ�������
				else 
					for (int i = this->num - AckNum; i < N / 2; i++) {
						cache[i] = 0;
					}
				this->num -= AckNum;
				this->base = (this->base + AckNum) % N;
				this->waitingState = false;

			}
			else {
				pUtils->printPacket("�յ��Ǵ�����λACK��", ackpkt);
				pns->stopTimer(SENDER, this->packetWaitingAck[flag].seqnum);
			}
		}
		else {
			pns->stopTimer(SENDER, ackpkt.acknum);
			pUtils->printPacket("���ͷ��յ��������ACK��", ackpkt);
		}
	}
	else {
		pUtils->printPacket("ACKУ��ͳ���", ackpkt);
	}
}

void SRSender::timeoutHandler(int seq) {
	int i = 0;
	for (; this->packetWaitingAck[i].seqnum!=seq; i++);

	pns->stopTimer(SENDER, seq);
 	pUtils->printPacket("��ʱ�ط�", this->packetWaitingAck[i]);
	pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[i]);
	pns->startTimer(SENDER, Configuration::TIME_OUT, seq);
}
