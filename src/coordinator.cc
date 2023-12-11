//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "coordinator.h"

enum M_type {
    Data, Ack, Nack
};

Define_Module(Coordinator);

void Coordinator::initialize()
{
    // TODO - Generated method body
    // TODO - Generated method body

    //TESTING READ COORDINATOR

    std::string fp = "coordinator.txt";
    int nodeID = 532;
    float start_t = 0;
    readCoordinatorFile(fp, nodeID, start_t);
    // Send first message to the starting node to indicate it will be sender

    Message *mmsg = new Message();

    switch (nodeID) {
    case 0:
        mmsg->setPayload("input0.txt");
        sendDelayed(mmsg, start_t, "out0");
        break;
    case 1:
        mmsg->setPayload("input1.txt");
        sendDelayed(mmsg, start_t, "out1");
        break;
    }
}

void Coordinator::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}

void Coordinator::readCoordinatorFile(std::string &fileName, int &nodeID,
        float &startTime_sec) {
    // Open the file
    std::ifstream coordinator_file;
    coordinator_file.open(getCurrentDirectory() + "\\" + fileName, std::ios::in);
    // Return if file was not opened
    if (!coordinator_file.is_open()) {
        std::cerr << "[COORDINATOR] Error opening file." << std::endl;
        return;
    } else {
        // Read the 2 numbers from file
        // NodeID is the first number, then start time in seconds
        coordinator_file >> nodeID >> startTime_sec;
        coordinator_file.close();
    }
}

std::string Coordinator::getCurrentDirectory() {
    char buff[FILENAME_MAX]; //create string buffer to hold path
    _getcwd(buff, FILENAME_MAX);
    std::string current_working_dir(buff);
    return current_working_dir;
}

