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
    // TODO Auto-generated constructor stub
    // statistics part
      lost_messages = 0;
      modified_messages = 0;
      erroneous_messages = 0;
      total_messages = 0;
      total_lengths = 0;
      time_taken_for_send = 0.0; // (TD+ ED) for each message / final sim_time
}

StatsGenerator::~StatsGenerator() {
  // Open the file for writing
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
