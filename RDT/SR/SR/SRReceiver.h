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
    int num;//已接受的packet数
    int cache[N / 2];//标记已接受的packet序号
public:
    SRReceiver();
    virtual ~SRReceiver() {};

public:
    void receive(const Packet& packet);
};
#endif