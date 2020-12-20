#include"Global.h"
#include"stdafx.h"
#include"SRReceiver.h"

SRReceiver::SRReceiver() :num(0), base(0) {
	this->Ack.acknum = -1;
	this->Ack.seqnum = -1;
	this->Ack.checksum = 0;
	for (auto& i : this->cache)
		i = 0;
	for (auto& i : this->Ack.payload)
		i = '.';
	this->Ack.checksum = pUtils->calculateCheckSum(this->Ack);
}

void SRReceiver::receive(const Packet& packet) 
{
	if (this->num == 0)//��δ���ܷ��飬cache����
		for (auto& i : this->cache)
			i = 0;
	int check_sum = pUtils->calculateCheckSum(packet);
	if (check_sum == packet.checksum) {//���У���
		bool inrange = false;
		//�ж��Ƿ��ڴ��ڷ�Χ��

		for (int i = 0; i < N / 2; i++) {
			if (packet.seqnum == ((this->base + i) % N)) {
				inrange = true;
				break;
			}
		}

		if (inrange) {//����ڴ�����
			for (int i = 0; i < N / 2; i++) {
				if (packet.seqnum == (this->base + i) % N) {
					this->cache[i] = 1;//cache��Ӧλ�ñ��Ϊ�Ѵ���
					this->RcvPackets[i] = packet;
					//this->num++;
					pUtils->printPacket("���շ��ɹ����գ�", this->RcvPackets[i]);
					this->Ack.acknum = this->RcvPackets[0].seqnum;
					this->Ack.checksum = pUtils->calculateCheckSum(this->Ack);
					pns->sendToNetworkLayer(SENDER, this->Ack);
					pUtils->printPacket("���շ�����ȷ�ϱ��ģ�", this->Ack);
					break;
				}
			}
			if (cache[0] == 1) {
				int ack_num = 0;//��¼��base��ʼ��ȷ�ϵ�packet��
				for (int i = 0; this->cache[i] != 0 && i < N / 2; i++) {
					ack_num++;
				}
				for (int i = 0; i < ack_num; i++) {
					Message msg;
					memcpy(msg.data, this->RcvPackets[i].payload, sizeof(this->RcvPackets[i].payload));
					pns->delivertoAppLayer(RECEIVER, msg);
				}

				//��������
				if (this->num != ack_num) {
					for (int i = 0; i < this->num - ack_num; i++) {
						this->RcvPackets[i] = this->RcvPackets[i + ack_num];
					}//
				}
				

				this->num -= ack_num;
				this->base = (this->base + ack_num) % N;
				for (auto& n : cache)
					n = 0;
			}
			
		}
		else {//���ڴ�����
			pUtils->printPacket("���շ����յ������ⱨ�ģ�", packet);
			this->Ack.acknum = packet.seqnum;
			this->Ack.checksum = pUtils->calculateCheckSum(this->Ack);
			pns->sendToNetworkLayer(SENDER, this->Ack);
		}
	}
	else {
		pUtils->printPacket("���շ�����ʧ�ܣ�����У��ʹ���", packet);
	}
}