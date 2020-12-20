#include"Global.h"
#include"stdafx.h"
#include"TCPSender.h"

bool TCPSender::send(const Message& message)
{
	if (this->nextseqnum < this->base + N) {
		this->packetWaitingAck[num].acknum = -1;//忽略
		this->packetWaitingAck[num].seqnum = this->nextseqnum;
		memcpy(this->packetWaitingAck[num].payload, message.data, sizeof(message.data));
		this->packetWaitingAck[num].checksum = pUtils->calculateCheckSum(this->packetWaitingAck[num]);

		if (this->nextseqnum == this->base) {
			pns->startTimer(SENDER, Configuration::TIME_OUT, this->packetWaitingAck[num].seqnum);
		}
		pUtils->printPacket("发送方发送数据：", this->packetWaitingAck[num]);
		pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[num]);

		this->nextseqnum++;
		if (this->nextseqnum >= this->base + N) 
			this->waitingState = true;
		this->num++;
		return true;


	}
	else {//窗口已满
		this->waitingState = true;
		return false;
	}
}

void TCPSender::receive(const Packet& ackpkt)
{
	if (this->num > 0) {//窗口内有未确认数据报
		int check_sum = pUtils->calculateCheckSum(ackpkt);

		if (check_sum == ackpkt.checksum && ackpkt.acknum >= this->base) {//检查校验和
			if (ackpkt.acknum == this->packetWaitingAck[0].seqnum) {
				this->count++;
				if (this->count > 3) {//冗余ACK
					pns->stopTimer(SENDER, this->packetWaitingAck[0].seqnum);
					pUtils->printPacket("冗余ACK重传：", this->packetWaitingAck[0]);
					pns->startTimer(SENDER, Configuration::TIME_OUT, this->packetWaitingAck[0].seqnum);
					pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[0]);
					pUtils->printPacket("发送方发数据报：", this->packetWaitingAck[0]);
					this->count = 0;
					return;
				}
			}
			else
				this->count = 1;

			if (this->count == 1) {
				int ack_num = ackpkt.acknum - this->base+1;//已确认（包括略过的）分组数
				this->base += ack_num;//累计确认

				pUtils->printPacket("发送方接收到ACK：", ackpkt);
				if (this->base == this->nextseqnum)//如果ACK为文件末尾，结束计时
					pns->stopTimer(SENDER, this->packetWaitingAck[0].seqnum);
				else {
					pns->stopTimer(SENDER, this->packetWaitingAck[0].seqnum);
					pns->startTimer(SENDER, Configuration::TIME_OUT, this->packetWaitingAck[ack_num].seqnum);
				}
				//窗口滑动
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
	pUtils->printPacket("超时重传：", this->packetWaitingAck[0]);
	pns->startTimer(SENDER, Configuration::TIME_OUT, this->packetWaitingAck[0].seqnum);
	pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[0]);
}