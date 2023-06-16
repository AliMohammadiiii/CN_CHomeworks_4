#include <iostream>
#include <cstdlib>
#include <string>

using namespace std;

const float PACKET_LOSS_RATE = 0.3;
const int SIMULATED_PACKETS = 1000;

const string RED = "\x1B[31m";
const string GREEN = "\x1B[32m";
const string RESET = "\033[0m";

class TCPConnection {
private:
    int cwnd;
    int ssthresh;
    int dupACKs;
    int totalPackets;
    int lastAckedPacket;
    int lastSentPacket;
    string algorithm;
    bool packets[SIMULATED_PACKETS] = {false};
    bool enableLogging;

public:
    TCPConnection(int cwnd = 4, int ssthresh = 16, string algorithm = "Reno", bool enableLogging = false) {
        this->cwnd = cwnd;
        this->ssthresh = ssthresh;
        this->algorithm = algorithm;
        this->enableLogging = enableLogging;
        dupACKs = 0;
        totalPackets = 0;
        lastSentPacket = 0;
        lastAckedPacket = 0;
        srand(time(NULL));
    }

    void simulate() {
        if (enableLogging)
            cout << RED + "Start simulation.\t\t" << " cwnd: " << cwnd << " ssthresh: " << ssthresh << RESET << endl;

        sendData(1);
    }

private:
    void handleSlowStart() {
        cwnd *= 2;

        if (enableLogging)
            cout << RED + "Slow start mode.\t\t" << " cwnd: " << cwnd << " ssthresh: " << ssthresh << RESET << endl; 
    }

    void handleCongestionAvoidance() {
        cwnd += 1;

        if (enableLogging)
            cout << RED + "Congestion avoidance mode.\t" << " cwnd: " << cwnd << " ssthresh: " << ssthresh << RESET << endl; 
    }

    void handleFastRecovery() {
        if (algorithm == "Reno") {
            cwnd = cwnd / 2;
            ssthresh = cwnd;
        }

        if (enableLogging)
            cout << RED + "Fast recovery mode.\t\t"  << " cwnd: " << cwnd << " ssthresh: " << ssthresh << RESET << endl; 
    }

    void onPacketLost() {
        ssthresh = cwnd / 2;
        cwnd = 1;

        if (enableLogging)
            cout << RED + "Timeout.\t\t\t" << " cwnd: " << cwnd << " ssthresh: " << ssthresh << RESET << endl; 
    }

    void sendData(int i) {
        if (i > SIMULATED_PACKETS) {
            cout << GREEN <<  "Simulation finished." << " total packets: " << totalPackets << " performance: " << SIMULATED_PACKETS / (float)totalPackets << RESET << endl;
            return;
        }

        if (enableLogging)
            cout << "Packet " << i << " sent." << " ";

        lastSentPacket = i;
        totalPackets++;

        if (rand() % 100 < PACKET_LOSS_RATE * 100) {
            if (enableLogging)
                cout << "Packet " << i << " lost." << endl;
        }
        else {
            packets[i - 1] = true;
            int ackedPacket;
            for (ackedPacket = 0; ackedPacket < SIMULATED_PACKETS; ackedPacket++) {
                if (packets[ackedPacket] == false) {
                    break;
                }
            }
            
            if (enableLogging)
                cout << "Acknowledgement " << ackedPacket << " received." << endl;

            if (ackedPacket == lastAckedPacket) {
                dupACKs++;
                if (dupACKs == 2) {
                    handleFastRecovery();
                    sendData(lastAckedPacket + 1);
                    return;
                }
            }
            else {
                dupACKs = 0;
                lastAckedPacket = ackedPacket;
                if (cwnd < ssthresh) {
                    handleSlowStart();
                }
                else {
                    handleCongestionAvoidance();
                }
                sendData(lastAckedPacket + 1);
                return;
            }
        }

        if (lastSentPacket - lastAckedPacket < cwnd) {
            sendData(lastSentPacket + 1);
        }
        else {
            onPacketLost();
            sendData(lastAckedPacket + 1);
        }
    }
};

int main() {
    cout << "Reno:" << endl;
    TCPConnection reno(4, 16, "Reno", false);
    reno.simulate();

    cout << "NewReno:" << endl;    
    TCPConnection newReno(4, 16, "newReno", false);
    newReno.simulate();

    return 0;
}
