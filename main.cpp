#include <iostream>
#include <cstdlib>
#include <string>
#include <iostream>
#include <cstdlib>
#include <string>
#include <ctime>
#include <cmath>

using namespace std;

const float PACKET_LOSS_RATE = 0.3;
const int SIMULATED_PACKETS = 100;

const string RED = "\x1B[31m";
const string GREEN = "\x1B[32m";
const string RESET = "\033[0m";
const string YELLOW = "\x1B[33m";
const string BLUE = "\x1B[34m";

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

    // Add BBR-specific parameters
    double btlBwEstimate;
    double minRTT;
    double cycleStart;
    double gainCycle;
    double cwndGain;

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

        // Initialize BBR-specific parameters
        btlBwEstimate = 1.0;
        minRTT = INFINITY;
        cycleStart = 0.0;
        gainCycle = 1.0;
        cwndGain = 2.0;
    }

    void simulate() {
        if (enableLogging)
            cout << BLUE + "Start simulation.\t\t" << " cwnd: " << cwnd << " ssthresh: " << ssthresh << RESET << endl;

        sendData(1);
    }

private:
    void handleSlowStart() {
        cwnd *= 2;

        if (enableLogging)
            cout << BLUE + "Slow start mode.\t\t" << " cwnd: " << cwnd << " ssthresh: " << ssthresh << RESET << endl; 
    }

    void handleCongestionAvoidance() {
        cwnd += 1;

        if (enableLogging)
            cout << BLUE + "Congestion avoidance mode.\t" << " cwnd: " << cwnd << " ssthresh: " << ssthresh << RESET << endl; 
    }

    void handleFastRecovery() {
        if (algorithm == "Reno") {
            cwnd = cwnd / 2;
            ssthresh = cwnd;
        }

        if (enableLogging)
            cout << BLUE + "Fast recovery mode.\t\t"  << " cwnd: " << cwnd << " ssthresh: " << ssthresh << RESET << endl; 
    }

    void onPacketLost() {
        ssthresh = cwnd / 2;
        cwnd = 1;

        if (enableLogging)
            cout << BLUE + "Timeout.\t\t\t" << " cwnd: " << cwnd << " ssthresh: " << ssthresh << RESET << endl; 
    }

    void handleBBRSlowStart() {
        cwnd *= 2;

        if (enableLogging)
            cout << BLUE + "BBR Slow start mode.\t\t" << " cwnd: " << cwnd << " ssthresh: " << ssthresh << RESET << endl;
    }

    void handleBBRCongestionAvoidance() {
        cwnd += cwndGain;

        if (enableLogging)
            cout << BLUE + "BBR Congestion avoidance mode.\t" << " cwnd: " << cwnd << " ssthresh: " << ssthresh << RESET << endl;
    }

    void updateBBRParameters() {
        double currentRTT = getCurrentRTT();
        if (currentRTT < minRTT) {
            minRTT = currentRTT;
        }
        double targetBtlBw = cwnd / minRTT;
        double btlBwDelta = targetBtlBw - btlBwEstimate;
        double gain = gainCycle * cwndGain;

        if (btlBwDelta > 0) {
            btlBwEstimate += btlBwDelta / ceil(btlBwEstimate / gain);
        } else {
            btlBwEstimate += btlBwDelta / ceil(-btlBwEstimate / gain);
        }
    }

    double getCurrentRTT() {
        return minRTT + rand() % 10;
    }

    void sendData(int i) {
        if (i > SIMULATED_PACKETS) {
            cout << YELLOW <<  "Simulation finished." << " total packets: " << totalPackets << " performance: " << SIMULATED_PACKETS / (float)totalPackets << " cwnd: " << cwnd << RESET << endl;
            return;
        }

        if (enableLogging)
            cout << "Packet " << i << " sent." << " ";

        lastSentPacket = i;
        totalPackets++;

        if (rand() % 100 < PACKET_LOSS_RATE * 100) {
            if (enableLogging)
                cout << RED << "Packet " << i << " lost." << RESET << endl;
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
                cout << GREEN << "Acknowledgement " << ackedPacket << " received." << RESET << endl;

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
                if (algorithm == "Reno") {
                    if (cwnd < ssthresh) {
                        handleSlowStart();
                    }
                    else {
                        handleCongestionAvoidance();
                    }
                }
                else if (algorithm == "newReno") {
                    if (cwnd < ssthresh) {
                        handleSlowStart();
                    }
                    else {
                        handleCongestionAvoidance();
                    }
                }
                else if (algorithm == "BBR") {
                    updateBBRParameters();
                    if (cwnd < btlBwEstimate) {
                        handleBBRSlowStart();
                    }
                    else {
                        handleBBRCongestionAvoidance();
                    }
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

    cout << "BBR:" << endl;
    TCPConnection bbr(4, 16, "BBR", false);
    bbr.simulate();

    return 0;
}