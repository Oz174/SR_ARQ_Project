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

#ifndef STATSGENERATOR_H_
#define STATSGENERATOR_H_

#include <fstream>
#include <vector>
#include <string>
#include <bitset>
#include <iostream>


class StatsGenerator {
public:

    int lost_messages = 0;
    int modified_messages = 0;
    int erroneous_messages = 0;
    int total_messages = 0;
    int total_lengths = 0;
    double time_taken_for_send = 0.0;



    StatsGenerator();
    void setup(std::vector<std::bitset<4>> errorArray , std::vector<std::string> messageArray);

  virtual ~StatsGenerator();
};

#endif /* STATSGENERATOR_H_ */
