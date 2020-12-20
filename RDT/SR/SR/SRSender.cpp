#include"Global.h"
#include"stdafx.h"
#include"SRSender.h"

bool SRSender::send(const Message& message) {

	if (this->num < (N / 2)) {//窗口有空位
		if (this->num == 0) {
			for (int i = 0; i < (N / 2); i++) {
				this->cache[i] = 0;//初始化cache
			}
		}
		this->waitingState = false;
		this->packetWaitingAck[num].acknum = -1; //忽略该字段
		this->packetWaitingAck[num].seqnum = this->nextseqnum;
		this->packetWaitingAck[num].checksum = 0;
		memcpy(this->packetWaitingAck[num].payload, message.data, sizeof(message.data));//拷贝数据

		this->packetWaitingAck[num].checksum = pUtils->calculateCheckSum(this->packetWaitingAck[num]);//计算校验和
		pUtils->printPacket("发送方发送报文", this->packetWaitingAck[num]);

		pns->startTimer(SENDER, Configuration::TIME_OUT, this->packetWaitingAck[num].seqnum);//每个分组都分配一个定时器
		pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[num]);
		this->num++;//报文数目加一

		if (this->num == (N/2)) {
			cout << "窗口已满" << endl;
			this->waitingState = true;//窗口已满
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
	int flag = 0;//记录收到的序号
	int check_sum = pUtils->calculateCheckSum(ackpkt);
	if (check_sum == ackpkt.checksum) {//确认校验和
		for (int i = 0; i < this->num; i++) {//查找收到ack对应的分组
			if (ackpkt.acknum == this->packetWaitingAck[i].seqnum) {
				cache[i] = 1;
				flag = i;
				inrange = true;
				break;
			}
		}

		if (inrange) {
			if (cache[0] == 1) {//ack等于基序号
				pUtils->printPacket("收到窗口首位ACK：", ackpkt);
				pns->stopTimer(SENDER, this->packetWaitingAck[flag].seqnum);
				int AckNum = 0;
				for (int i = 0; i < (N / 2); i++) {//查找已确认分组数
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
						printf("窗口未确认的第%d位分组是seq %c\n", i, this->packetWaitingAck[i].payload[0]);
					}
					for (int i = 0; i < N / 2; i++)
						this->cache[i] = 0;
				}
				//将已确认分组腾空的部分清零
				else 
					for (int i = this->num - AckNum; i < N / 2; i++) {
						cache[i] = 0;
					}
				this->num -= AckNum;
				this->base = (this->base + AckNum) % N;
				this->waitingState = false;

			}
			else {
				pUtils->printPacket("收到非窗口首位ACK：", ackpkt);
				pns->stopTimer(SENDER, this->packetWaitingAck[flag].seqnum);
			}
		}
		else {
			pns->stopTimer(SENDER, ackpkt.acknum);
			pUtils->printPacket("发送方收到窗口外的ACK：", ackpkt);
		}
	}
	else {
		pUtils->printPacket("ACK校验和出错：", ackpkt);
	}
}

void SRSender::timeoutHandler(int seq) {
	int i = 0;
	for (; this->packetWaitingAck[i].seqnum!=seq; i++);

	pns->stopTimer(SENDER, seq);
 	pUtils->printPacket("超时重发", this->packetWaitingAck[i]);
	pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[i]);
	pns->startTimer(SENDER, Configuration::TIME_OUT, seq);
}
