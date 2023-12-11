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

#ifndef __SR_ARQ_PROJECT_COORDINATOR_H_
#define __SR_ARQ_PROJECT_COORDINATOR_H_

#include <omnetpp.h>
#include <string>
#include <fstream>
#include <vector>
#include "Message_m.h"

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class Coordinator : public cSimpleModule
{
    private:
    // No One needs to know about this function , right ?
    std::string getCurrentDirectory();
    void readCoordinatorFile(std::string &fileName, int &nodeID,
            float &startTime_sec);

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

#endif
