#include <iostream>
#include <vector>
#include <string.h>
using namespace std;

class TCPconnectionReno
{
private:
    int cwnd;
    int ssthresh;
    int rtt;
    void increasecwnd();
public:
    TCPconnectionReno(int _cwnd = 1, int _ssthresh = 65535, int init_rtt = 0);
    ~TCPconnectionReno();
    void onPacketLoss();
    void onRTTUpdate(int new_rtt);
    int getCwnd()
    {
        return cwnd;
    }
    int getSsthresh()
    {
        return ssthresh;
    }
};

TCPconnectionReno::TCPconnectionReno(int _cwnd = 1, int _ssthresh = 65535, int init_rtt = 0) : 
    cwnd(_cwnd),
    ssthresh(_ssthresh),
    rtt(init_rtt)
{
}

void TCPconnectionReno::onPacketLoss()
{
    cwnd = ssthresh;
}

void TCPconnectionReno::increasecwnd()
{
    if (cwnd < ssthresh)
        cwnd *= 2;
    else
        cwnd += 1;
}

void TCPconnectionReno::onRTTUpdate(int new_rtt)
{
    rtt = new_rtt;
    increasecwnd();   
}

int main()
{

}
