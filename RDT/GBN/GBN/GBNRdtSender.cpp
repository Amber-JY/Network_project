#include"Global.h"
#include"stdafx.h"
#include"GBNRdtSender.h"

bool GBNRdtSender::send(const Message& message) {
	if (this->waitingState) { //发送方处于等待确认状态
		cout << "发送方等待中" << endl;
		return false;
	}
	
	if (this->nextseqnum < this->base + N) {
		this->waitingState = false;
		this->packetWaitingAck[num].acknum = -1; //忽略该字段
		this->packetWaitingAck[num].seqnum = this->nextseqnum;
		this->packetWaitingAck[num].checksum = 0;
		memcpy(this->packetWaitingAck[num].payload, message.data, sizeof(message.data));//拷贝数据

		this->packetWaitingAck[num].checksum = pUtils->calculateCheckSum(this->packetWaitingAck[num]);//计算校验和
		pUtils->printPacket("发送方发送报文", this->packetWaitingAck[num]);
		if (this->base == this->nextseqnum)
			pns->startTimer(SENDER, Configuration::TIME_OUT, this->packetWaitingAck[num].seqnum);
		pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[num]);
		this->num++;
		if (this->num >= N) {
			cout << "窗口已满" << endl;
			this->waitingState = true;//窗口已满
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
			int pkt_acked = ackpkt.acknum - this->base + 1;//已确认的分组数

			this->base += pkt_acked;

			pUtils->printPacket("发送方确认", ackpkt);

			if (this->base == this->nextseqnum)
				pns->stopTimer(SENDER, this->packetWaitingAck[0].seqnum);//计时结束
			else {
				pns->stopTimer(SENDER, this->packetWaitingAck[0].seqnum);
				pns->startTimer(SENDER, Configuration::TIME_OUT, this->packetWaitingAck[pkt_acked].seqnum);
			}

			//窗口未发送内容滑动
			for (int i = 0; i < num - pkt_acked; i++) {
				this->packetWaitingAck[i] = this->packetWaitingAck[i+ pkt_acked];
				printf("the %d member in the windows is seq%d. \n", i, this->packetWaitingAck[i].seqnum);
			}

			//窗口内分组数目重新计算
			this->num -= pkt_acked;
			this->waitingState = false;
		}
	}
}

void GBNRdtSender::timeoutHandler(int nextseqnum) {
	pUtils->printPacket("超时重发", this->packetWaitingAck[0]);
	pns->stopTimer(SENDER, this->packetWaitingAck[0].seqnum);
	pns->startTimer(SENDER, Configuration::TIME_OUT, this->packetWaitingAck[0].seqnum);//
	for (int i = 0; i < this->num; i++) {
		pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[i]);
	}
}