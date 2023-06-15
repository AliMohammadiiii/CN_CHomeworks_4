#include <iostream>

class TCPConnection {
private:
    int CWND;     
    int ssthresh; 
    int RTT;      
    bool inSlowStart;       
    bool inCongestionAvoidance;  
    bool inFastRecovery;    

public:
    TCPConnection(int initialCWND, int initialSSThresh, int initialRTT = 0) {
        CWND = initialCWND;
        ssthresh = initialSSThresh;
        RTT = initialRTT;
        inSlowStart = true;
        inCongestionAvoidance = false;
        inFastRecovery = false;
    }
    
    void SendData() {
        std::cout << "Sending data with CWND = " << CWND << " and ssthresh = " << ssthresh << std::endl;
    }

    void onPacketLoss() {
        CWND /= 2;
        ssthresh = CWND;
        if (inSlowStart) {
            inFastRecovery = true;
        } else if (inCongestionAvoidance) {
            inFastRecovery = true;
        }
    }

    void onRTTUpdate(int updatedRTT) {
        RTT = updatedRTT;
        if (inSlowStart) {
            CWND += 1; // it can *2 and other +1
            if (CWND >= ssthresh) {
                inSlowStart = false;
                inCongestionAvoidance = true;
            }
        } else if (inCongestionAvoidance) {
            CWND += 1 / CWND; // it can +1 and other *2
        } else if (inFastRecovery) {
            inFastRecovery = false;
            inCongestionAvoidance = true;
        }
    }

    int getCWND()
    {
        return CWND;
    }

    int getSsthresh()
    {
        return ssthresh;
    }
    int getRTT()
    {
        return RTT;
    }
    bool isInSlowmode()
    {
        return inSlowStart;
    }
    bool isInFastRecovery()
    {
        return inFastRecovery;
    }
};

int main() {
    TCPConnection conn1(10, 100);
    TCPConnection conn2(20, 200);

    conn1.SendData();
    conn1.onRTTUpdate(55);
    conn2.SendData();
    conn2.onPacketLoss();
    conn2.onRTTUpdate(70);

    return 0;
}
