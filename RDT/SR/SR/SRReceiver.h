#ifndef SRRECEIVER_H
#define SRRECEIVER_H
#include "RdtReceiver.h"

//const int N = 8;

class SRReceiver :public RdtReceiver
{
private:
    Packet RcvPackets[N / 2];
    Packet Ack;
    int base;
    int num;//�ѽ��ܵ�packet��
    int cache[N / 2];//����ѽ��ܵ�packet���
public:
    SRReceiver();
    virtual ~SRReceiver() {};

public:
    void receive(const Packet& packet);
};
#endif