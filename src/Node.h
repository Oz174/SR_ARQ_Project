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

#ifndef __SR_ARQ_PROJECT_NODE_H_
#define __SR_ARQ_PROJECT_NODE_H_

#include <omnetpp.h>
#include <iostream>
#include <bitset>
#include <string>
#include <sstream>
#include <fstream>
#include <direct.h>
#include <time.h>
#include "Message_m.h"
#include "StatsGenerator.h"
#include <vector>

#define Delay 0
#define Dup 1
#define Loss 2
#define Modification 3

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class Node : public cSimpleModule
{

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
  public:

        static std::fstream outputFile;
        static std::vector<std::string> outputBuffer;
        /**
         * @enum
         * @brief   Defines error codes that can occur in a message
         *
         */
        typedef enum {
            // [Modification, Loss, Duplication, Delay]
            ErrorCodeType_NoError = 0b0000, /**< No error */
            ErrorCodeType_Delay = 0b0001, /**< Delay only */
            ErrorCodeType_Duplication = 0b0010, /**< Duplication only*/
            ErrorCodeType_Dup_Delay = 0b0011, /**< Duplication & delay */
            ErrorCodeType_Loss = 0b0100, /**< Loss only */
            ErrorCodeType_LossDelay = 0b0101, /**< Loss & delay */
            ErrorCodeType_LossDup = 0b0110, /**< Loss & duplication */
            ErrorCodeType_LossDupDelay = 0b0111,/**< Loss, duplication & delay */
            ErrorCodeType_Modification = 0b1000,/**< Modification only */
            ErrorCodeType_ModDelay = 0b1001, /**< Modification & delay */
            ErrorCodeType_ModDup = 0b1010, /**< Modification & duplication*/
            ErrorCodeType_ModDupDelay = 0b1011, /**< Modification, duplication & delay */
            ErrorCodeType_ModLoss = 0b1100, /**< Modification & loss */
            ErrorCodeType_ModLossDelay = 0b1101,/**< Modification, loss, & delay */
            ErrorCodeType_ModLossDup = 0b1110, /**< Modification, loss & duplication */
            ErrorCodeType_AllErrors = 0b1111, /**< All 4 errors */

        } ErrorCodeType_t;

        // this enum for assigning which node is which by coordinator
        typedef enum {
            NodeType_Sender, NodeType_Receiver
        } NodeType_t;

        // this enum was for the sake of the switch case to easy handle the flow of SR (you might not need it)
        typedef enum {
            Data = 0, ACK = 1, NACK = 2, To_Send = 3, timeout = 4, timeout_print = 5
        } MsgType_t;

        //DONE
        void readMessages(std::string &fileName,
                std::vector<ErrorCodeType_t> &errorArray,
                std::vector<std::string> &messageArray);
        //DONE
        void modifyMessage(std::string &payload);

        //DONE
        char calculateChecksum(std::string &payload);
        void framingByteStuffing(Message *mptr, std::string &payload, int seq,
                bool modifiedFlag);


  private:


      // Gets current working directory of the application (DONE)
      std::string getCurrentDirectory();
      // Determines whether the node is sender or receiver
      NodeType_t nodeType = NodeType_Receiver;
      bool errorDetection(Message *msg);
      //DONE
      void readingPrint(ErrorCodeType_t errorCode);
      void beforeTransmissionPrint(Message *msg, ErrorCodeType_t input);
      void controlPrint(Message *msg, bool lost);
      void timeoutPrint(Message *msg);
      //DONE
      void selfMessageDelay(Message *msg, double delay);
      void selfMessageDuplicate(Message *msg, double delay);
      //DONE
      void sendDelayedMsg(Message *msg);
      void sendLogic(Message *msg, int msg_index);
      //DONE
      static void openOutputFile();
      void writeToFile();
      //processing frames
      void processFrames(int start_index,int end_index);
      //response from the receiver
      void send_ACK_or_NACK (Message *msg,bool is_ack, int seq_number);
      // private data members
      int is_sender = -1;
      std::vector<ErrorCodeType_t> errorArray;
      std::vector<std::string> messageArray;
      //Sender window
      int sender_window_size= 3;
      int current_end_frame=3-1;
      int receiver_window_size=3;
      int receiver_end_frame=3-1;
      int expected_seqence_number=0;
      int receiver_window_index=0;
      std::vector<int> NACK_Sent;
      std::vector<int> Data_received;




};

std::fstream Node::outputFile = nullptr;
std::vector<std::string> Node::outputBuffer = { };
#endif
