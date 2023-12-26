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

#include "StatsGenerator.h"



StatsGenerator::StatsGenerator() {

}

void StatsGenerator::setup(std::vector<std::bitset<4>> errorArray , std::vector<std::string> messageArray){
    // total_messages = messageArray.size(); //initially I have n messages
    std::bitset<4> errorCode;
    std::string tempMsg;
    for(int i=0 ; i < errorArray.size(); i++){
        errorCode = errorArray[i];
        tempMsg = messageArray[i];

        if(errorCode != 0b0000){
            erroneous_messages++;
        }

        //modified
        if(errorCode[3] == 1){
            modified_messages++;
            // if modified , i resend
            total_messages++;
            total_lengths += tempMsg.size()*8;
        }
        // lost
        if(errorCode[2] == 1){
            // if lost , i resend
            lost_messages++;
            total_messages++;
            total_lengths += tempMsg.size()*8;
        }
        //duplicate
        if(errorCode[1] == 1){
            // duplicate means twice , right ?
            total_messages++;
            total_lengths += tempMsg.size()*8;
        }

        total_lengths += tempMsg.size() * 8; // for every letter in message is 8 bits
        total_messages++;
    }

}

StatsGenerator::~StatsGenerator() {
  // Open the file for writing
  std::remove("sims_stats.txt"); // if the file exists , deletes

  std::ofstream outFile("sim_stats.txt");

  // Check if the file was opened successfully
  if (outFile.is_open()) {
    // Write the data member values in the desired format
    outFile << "Loss Rate: " << static_cast<double>(lost_messages) / total_messages << std::endl;
    outFile << "Modification Rate: " << static_cast<double>(modified_messages) / total_messages << std::endl;
    outFile << "Channel Efficiency: " << static_cast<double>(erroneous_messages) / total_messages << std::endl;
    outFile << "Average Length of sent Messages: " << static_cast<double>(total_lengths) / total_messages << std::endl;
    outFile << "Average Time taken: " << time_taken_for_send << std::endl;

    // Close the file
    outFile.close();
  }
}


