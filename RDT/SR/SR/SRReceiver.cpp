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
	if (this->num == 0)//若未接受分组，cache清零
		for (auto& i : this->cache)
			i = 0;
	int check_sum = pUtils->calculateCheckSum(packet);
	if (check_sum == packet.checksum) {//检查校验和
		bool inrange = false;
		//判断是否在窗口范围内

		for (int i = 0; i < N / 2; i++) {
			if (packet.seqnum == ((this->base + i) % N)) {
				inrange = true;
				break;
			}
		}

		if (inrange) {//如果在窗口内
			for (int i = 0; i < N / 2; i++) {
				if (packet.seqnum == (this->base + i) % N) {
					this->cache[i] = 1;//cache对应位置标记为已储存
					this->RcvPackets[i] = packet;
					//this->num++;
					pUtils->printPacket("接收方成功接收：", this->RcvPackets[i]);
					this->Ack.acknum = this->RcvPackets[0].seqnum;
					this->Ack.checksum = pUtils->calculateCheckSum(this->Ack);
					pns->sendToNetworkLayer(SENDER, this->Ack);
					pUtils->printPacket("接收方发送确认报文：", this->Ack);
					break;
				}
			}
			if (cache[0] == 1) {
				int ack_num = 0;//记录从base开始已确认的packet数
				for (int i = 0; this->cache[i] != 0 && i < N / 2; i++) {
					ack_num++;
				}
				for (int i = 0; i < ack_num; i++) {
					Message msg;
					memcpy(msg.data, this->RcvPackets[i].payload, sizeof(this->RcvPackets[i].payload));
					pns->delivertoAppLayer(RECEIVER, msg);
				}

				//调整窗口
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
		else {//不在窗口内
			pUtils->printPacket("接收方接收到窗口外报文：", packet);
			this->Ack.acknum = packet.seqnum;
			this->Ack.checksum = pUtils->calculateCheckSum(this->Ack);
			pns->sendToNetworkLayer(SENDER, this->Ack);
		}
	}
	else {
		pUtils->printPacket("接收方接受失败，分组校验和错误：", packet);
	}
}