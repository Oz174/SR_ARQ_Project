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

#include "Node.h"
#include "StatsGenerator.h"

Define_Module(Node);

// static int base = 0;
// static int next_frame_to_send = 0;

// static int control_frame_expected = 0;

std::vector<std::bitset<4>> errorArrayStatsGenerator;
StatsGenerator stats;
// static std::vector<std::string> messageArray;


// static int size_of_Array;
// static int frame_to_print = 1;

void Node::initialize()
{
    // TODO - Generated method body
}

void Node::handleMessage(cMessage *msg)
{
    int receiver_window_size = par("WindowSize").intValue();
    int sender_window_size = par("WindowSize").intValue();
    // TODO - Generated method body
    // casting the message to our custom type
    Message *mmsg = check_and_cast<Message *>(msg);
    // getting to know the role
    if (this->is_sender == -1)
    {
        // first message
        if (((static_cast<std::string>(mmsg->getPayload()) == "input0.txt") && (static_cast<std::string>(this->getName()) == "Node0")) || ((static_cast<std::string>(mmsg->getPayload()) == "input0.txt") && (static_cast<std::string>(this->getName()) == "Node0")))
        {
            // initialize sender variables
            this->is_sender = 1;
            std::string filename = static_cast<std::string>(mmsg->getPayload());
            // read messages
            this->readMessages(filename, this->errorArray, this->messageArray);

            this->readingPrint(this->errorArray[0]); // Coordinator first message


            for(int i=0 ; i<this->errorArray.size(); i++){
                std::bitset<4> errorCode(static_cast<unsigned long>(errorArray[i]));
                errorArrayStatsGenerator.push_back(errorCode);
            }

            stats.setup(errorArrayStatsGenerator,this->messageArray);

            this->sender_max_sequence_number = 2 * sender_window_size - 1;
            for (int i = 0; i < this->messageArray.size(); i++)
            {
                this->sent_sequences.push_back(0);
                this->ACK_sequences.push_back(0);
            }
        }
        else
        {
            // initialize receiver variables
            this->is_sender = 0;
            this->receiver_max_sequence_number = 2 * receiver_window_size - 1;
            for (int i = 0; i < this->receiver_max_sequence_number + 1; i++)
            {
                this->NACK_Sent.push_back(0);
                this->Data_received.push_back(0);
            }
        }
    }

    // main loop

    if (this->is_sender == 1)
    {
        if (mmsg->isSelfMessage()){
            std::string payloadString(mmsg->getPayload());
            std::string msg_index_s = "";
            std::string retransmitted_s = "";
            bool commaPassed = false;
            for(int i=0; i < payloadString.size(); i++){
                if (commaPassed){
                    retransmitted_s += payloadString[i];
                }
                else{
                    if (payloadString[i]==','){
                        commaPassed = true;
                    }
                    else {
                        msg_index_s += payloadString[i];
                    }
                }
            }
            int msg_index = std::stoi(msg_index_s);
            bool retransmitted = (bool)std::stoi(retransmitted_s);
            sendLogic(mmsg,msg_index,retransmitted,true);
            return;
        }
        // message is ACK or NACK
        if (mmsg->getType() == 1 || mmsg->getType() == 2)
        {
            if (mmsg->getType() == 1) // ack
            {

                int ack_no = mmsg->getAck_no();

                controlPrint(mmsg, true, ack_no);

                ack_no = (ack_no == 0) ? 6 : ack_no;

                int ack_index_number = -1;

                // find the index of this
                for (int i = (ack_no - 1); i < ACK_sequences.size(); i += (this->sender_max_sequence_number + 1))
                {
                    if (ACK_sequences[i] == 0)
                    {
                        ack_index_number = i + 1;
                        for (int j = 0; j <= i; j++)
                        {
                            // add the info in the ACK_sequances
                            this->ACK_sequences[j] = 1;
                            // add info in sent_sequences
                            this->sent_sequences[j] = 1;
                        }
                        break;
                    }
                }


                // advance to the next window
                int index_to_advance_to = ack_index_number;
                int last_index = index_to_advance_to + sender_window_size - 1;

                EV << "i will advance to: " << mmsg->getAck_no() << " message is ACK\n";

                EV << "first index: " << index_to_advance_to << " last index " << last_index << "\n";

                if (index_to_advance_to >= this->messageArray.size())
                {
                    // if index to advance to finished messages to send end program

                    return;
                }
                if (last_index >= this->messageArray.size())
                {
                    last_index = this->messageArray.size() - 1;
                }

                this->processFrames(index_to_advance_to, last_index , false);
            }
            else // nack
            {
                // I would have to re-transmit on nack
                int msg_index = -1;
                EV << "Sequence Number: " << mmsg->getAck_no() << " message is NACK\n";
                controlPrint(mmsg, true, mmsg->getAck_no());
                for (int i = (mmsg->getAck_no()); i < ACK_sequences.size(); i += (this->sender_max_sequence_number + 1))
                {
                    if (ACK_sequences[i] == 0)
                    {
                        // etb3tli nack 3 , as long u r not ack then get me the index
                        msg_index = i;

                        break;
                    }
                }

                sendLogic(mmsg, msg_index,true);
            }

            return;
        }


        // sender
        if (!msg->isSelfMessage())
        {
            processFrames(0, sender_window_size - 1, false);
            // processFrames( this->current_end_frame-sender_window_size +1,this->current_end_frame );
        }
    }
    else if (this->is_sender == 0)
    {

        EV << "I received sequence number : " << mmsg->getHeader() << "\n";

        // receiver
        if (mmsg->getHeader() == this->expected_seqence_number)
        {

            // add the message in data received
            if (!this->errorDetection(mmsg))
            {
                this->NACK_Sent[mmsg->getHeader()] = 1;
                this->Data_received[mmsg->getHeader()] = 1;

                // ACK message
                //  Message * ack_message= new Message;
                //  send ACKS for correct messages in order
                int check_sequence_number = this->expected_seqence_number;
                for (int i = this->expected_seqence_number; i < this->receiver_max_sequence_number + receiver_window_size + 1; i++)
                {

                    // we reached the expected sequence number
                    if (Data_received[check_sequence_number] == 0)
                    {
                        this->expected_seqence_number = check_sequence_number;
                        this->send_ACK_or_NACK(new Message, true, check_sequence_number);
                        controlPrint(mmsg, false , check_sequence_number);
                        break;
                    }
                    //if (Data_received[check_sequence_number] == 1 && (i == receiver_window_size - 1 || i == this->receiver_max_sequence_number))
                    if (Data_received[check_sequence_number] == 1 && (i == this->receiver_max_sequence_number))
                    {
                        check_sequence_number + 1 > this->receiver_max_sequence_number ? check_sequence_number = 0 : check_sequence_number++;
                        //this->expected_seqence_number = check_sequence_number;
                        //this->send_ACK_or_NACK(new Message, true, check_sequence_number);
                        //break;
                    }

                    // make index circular
                    check_sequence_number + 1 > this->receiver_max_sequence_number ? check_sequence_number = 0 : check_sequence_number++;
                }

                // reset the already received frames
                int reset_index = this->expected_seqence_number;

                for (int i = 0; i < receiver_window_size; i++)
                {
                    this->NACK_Sent[reset_index] = 0;
                    this->Data_received[reset_index] = 0;
                    --reset_index;
                    if (reset_index == -1)
                    {
                        reset_index = this->receiver_max_sequence_number;
                    }
                }
            }
            else
            {
                if (this->NACK_Sent[mmsg->getHeader()] == 0)
                {
                    this->send_ACK_or_NACK(new Message, false, mmsg->getHeader());
                    controlPrint(mmsg, false , mmsg->getHeader());
                    this->NACK_Sent[mmsg->getHeader()] = 1;
                }
            }
        }
        else if (!(mmsg->getHeader() > ((this->expected_seqence_number + receiver_window_size- 1 ) % (this->sender_max_sequence_number + 1))))
        // this condition handles if sender retransmitted an already acknowledged frame
        {
            // NACK message for currently received frame if it has error

            if (!this->errorDetection(mmsg))
            {
                this->Data_received[mmsg->getHeader()] = 1;
            }
            else
            {
                if (this->NACK_Sent[mmsg->getHeader()] == 0)
                {
                    this->NACK_Sent[mmsg->getHeader()] = 1;
                    this->send_ACK_or_NACK(new Message, false, mmsg->getHeader());

                    controlPrint(mmsg, false , mmsg->getHeader());
                }
            }

            // NACK message for expected frame if out of order (tricky)

            if (this->NACK_Sent[expected_seqence_number] == 0)
            {
                this->NACK_Sent[expected_seqence_number] = 1;
                this->send_ACK_or_NACK(new Message, false, this->expected_seqence_number);
                // tricky controlPrint , assume lost even if delayed until we work around errorCodeType_T using an index ...
                controlPrint(mmsg, false, this->expected_seqence_number);
            }
        }
    }
}

// Printing in console and pushing back in the output buffer for file writing
void Node::readingPrint(ErrorCodeType_t errorCode)
{

    std::string node_reading = "At time [" + simTime().str() + "], Node[" + this->getName()[4] + +"], Introducing channel error with code = " + std::bitset<4>(errorCode).to_string();

    outputBuffer.push_back(node_reading);
}
void Node::beforeTransmissionPrint(Message *msg, ErrorCodeType_t input)
{

    // DONE: get the correct node id
    // DONE: get the correct duplicate version
    std::bitset<8> trailer_bits = msg->getTrailer();
    std::bitset<4> code(input);

    std::string lost = "No";

    if (code[2] == 1)
    {
        lost = "Yes";
    }

    float delay = 0; // kda kda fe td

    if (code[0] == 1)
    {
        delay += par("ErrorDelay").doubleValue();
    }

    double pt = par("ProcessingTime").doubleValue();
    std::string line_to_print = "At time [" + simTime().str() + "] Node[" + this->getName()[4] + "] sent frame with seq_num=[" + std::to_string(msg->getHeader()) + "], and payload=[" + msg->getPayload() + "], and trailer =[" + trailer_bits.to_string() + "] ,Lost [" + lost + "], Duplicate [" + std::to_string(msg->getType()) + "], Delay [" + std::to_string(delay) + "].\n";

    std::cout << line_to_print << std::endl;
    outputBuffer.push_back(line_to_print);
}
void Node::controlPrint(Message *msg, bool is_sender , int ack_no)
{
    std::string ack;
    std::string send_or_receive = (is_sender) ? "recived" : "sent";
    if (msg->getType() == 2)
    {
        ack = "NACK";
    }
    else if (msg->getType() == 1)
    {
        ack = "ACK";
    }
    else
    { /*nothing*/
        return;
    }

    std::string line_to_print = "At time [" + simTime().str() + "], Node[" + this->getName()[4] + "]" + send_or_receive + "[" + ack + "] with number[" + std::to_string(ack_no) + "]";


    std::cout << line_to_print << std::endl;
    outputBuffer.push_back(line_to_print);
}
void Node::timeoutPrint(Message *msg)
{

    std::string line_to_print = "Time out event at time [" + simTime().str() + "], at Node [" + this->getName()[4] + "] for frame with seq_num=[" + std::to_string(msg->getHeader()) + "]; \n";
    std::cout << line_to_print << std::endl;
    outputBuffer.push_back(line_to_print);
}

// Get current directory ...
// el beeda el awel wla el far5a ?
std::string Node::getCurrentDirectory()
{
    char buff[FILENAME_MAX]; // create string buffer to hold path
    _getcwd(buff, FILENAME_MAX);
    std::string current_working_dir(buff);
    return current_working_dir;
}

// Reading from file phase
void Node::readMessages(std::string &fileName,
                        std::vector<ErrorCodeType_t> &errorArray,
                        std::vector<std::string> &messageArray)
{

    // Open the file
    std::ifstream node_file;

    node_file.open(getCurrentDirectory() + "\\" + fileName, std::ios::in);

    // Return if file was not opened
    if (!node_file.is_open())
    {
        std::cerr << "[NODE] Error opening file." << std::endl;
        return;
    }
    else
    {
        std::string tmp_errorcodeBinary;

        std::string tmp_line = "";
        std::string tmp_msg = "";

        while (std::getline(node_file, tmp_line))
        {
            // start at the beginning file (line 0)
            std::stringstream s_stream(tmp_line);
            // read the errorcode
            s_stream >> tmp_errorcodeBinary;
            // until newline , read all
            std::getline(s_stream, tmp_msg, '\n');

            // Push back into the vectors
            // errorCode -> bistset
            std::bitset<4> tmp_bits(tmp_errorcodeBinary);
            // for each message I need to know its error code
            errorArray.push_back(
                static_cast<ErrorCodeType_t>(tmp_bits.to_ulong()));

            messageArray.push_back(tmp_msg);
            tmp_msg.clear();
            s_stream.clear();
        }
        node_file.close();
    }

    return;
}

// Writing to File phase

// 1 - Open the File (pretty much intuitive , right ? )
// med eidak ya tayel mn el shora3 w efta7 el bab


// 2- Squeeze the buffer to write in output.txt
// buffer : ana sponge-bob
void Node::writeToFile()
{
    std::remove("output.txt"); // if the file exists , deletes

    // std::ofstream outputFile("output.txt");

    outputFile.open("output.txt", std::ios::out | std::ios::trunc);

    for (auto it : outputBuffer)
    {
        outputFile << it << std::endl;
    }
    outputFile.close();
}

// I definitely need a therapist because I started talking to myself alot !
// horror part starts when self starts replying (AKA : voices inside my head)
// Ok , getting real now , this is to avoid using noisy channel (any other than ideal channel) by sending to oneself
// So , if you wanna delay , delay by calling these two messages with delay time
void Node::selfMessageDelay(Message *msg, double delay,bool retransmitted)
{
    cancelEvent(msg);

    //    scheduleAt(simTime() + delay, msg);
    // sending to other node
    std::bitset<4> tmp_bits(errorArray[msg->getHeader()]); // 0b0100 --> LSB is 0
    // change header to sequence number
    // FIXME: modulus problem
    msg->setHeader(msg->getHeader() % (this->sender_max_sequence_number + 1));
    // check if lost
    if (tmp_bits[Loss] != 1 || retransmitted)
    {
        sendDelayed(msg, delay, "out");
        beforeTransmissionPrint(msg, errorArray[msg->getHeader()]);
    }
}
void Node::selfMessageDuplicate(Message *msg, double delay)
{
    double duplicationDelay = par("DuplicationDelay").doubleValue();

    Message *duplicatedMessage = new Message(*msg);
    msg->setType(1);
    duplicatedMessage->setType(2);

    selfMessageDelay(msg, delay,0);
    selfMessageDelay(duplicatedMessage, delay + duplicationDelay,0);
    beforeTransmissionPrint(msg, errorArray[msg->getHeader()]);
}

// It's time for the data link to do its job

// 1- Calculate Checksum before setting the trailer of the message
char Node::calculateChecksum(std::string &payload)
{
    char checksumbyte = 0;
    int payloadSize = payload.size();
    for (int i = 0; i < payloadSize; ++i)
    {

        checksumbyte = (checksumbyte ^ payload[i]);
    }

    return checksumbyte;
}

// 2- Modifying the message (aka : 7ot el error mn 3andak abl ma tb3t)

void Node::modifyMessage(std::string &payload)
{
    int bitIdx = rand() % 8; // 3dd el bits l kol 7arf
    int byteIdx = rand() % payload.length();
    payload[byteIdx] ^= (1 << bitIdx);
    // ABC : 01000001 01000010 01000011  , bitIdx = 4 , byteIndex : B
    //                00010000
    // res: 01000001  01010010 01000011  //AbC
}

// 3- Finally .... Framing
void Node::framingByteStuffing(Message *mptr, std::string &payload, int seq,
                               bool modificationFlag)
{
    int updateSize = 2; // including the 2 dollar signs below , this to update the length in statsGenerator

    std::string modified = "$"; // flag fl awel
    int payloadSize = payload.size();

    for (int i = 0; i < payloadSize; i++)
    {
        char c = payload[i];

        if (c == '$')
        {
            modified += "/$"; // escape b4 accidental flag
            updateSize++; // ana bzwd wa7d bas 34an ana already 7asabt el accidental dollar sign fl setup func
        }
        else if (c == '/')
        {
            modified += "//"; // escape b4 accidental escape
            updateSize++;
        }
        else
        {
            modified += payload[i]; // else
        }
    }
    modified += "$"; // flag fl a5r
    stats.updateMsgLength(updateSize);

    char checksum = calculateChecksum(modified);
    if (modificationFlag == 1)
        modifyMessage(modified);



    mptr->setPayload(modified.c_str());
    mptr->setHeader(seq);
    mptr->setTrailer(checksum);
    mptr->setType(MsgType_t::Data);

}

// msh sa2altny so2al ? agaweb b2a sa3adtak
// I will have to check errors somewhere while receiving
bool Node::errorDetection(Message *msg)
{

    std::vector<std::bitset<8>> vbitset;

    std::string PayLoad = msg->getPayload();

    for (int i = 0; i < PayLoad.size(); i++)
    {
        vbitset.push_back(PayLoad[i]);
    }
    std::bitset<8> trailer_bits = msg->getTrailer();
    std::bitset<8> check(0);

    for (int i = 0; i < vbitset.size(); i++)
    {
        check = check ^ vbitset[i];
    }

    check = check ^ trailer_bits;

    if (check == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

// use the parameter here (USE THE FORCE)
void Node::sendDelayedMsg(Message *msg)
{
    msg->setType(0);

    sendDelayed(msg, par("TransmissionDelay").doubleValue(), "out_gate");
}

void Node::sendLogic(Message *msg, int msg_index , bool retransmitted, bool isSelfMessage, int extraDelay)
{
    // 3ayz a-check 3l error code l kol message (in each line from file) abl ma ab3t
    if (isSelfMessage) {
        std::bitset<4> tmp_bits(this->errorArray[msg_index]); // 0b0100 --> LSB is 0
        if (!retransmitted){
            framingByteStuffing(msg, this->messageArray[msg_index], msg_index, tmp_bits[Modification]);
        }
        else{
            framingByteStuffing(msg, this->messageArray[msg_index], msg_index, 0);
        }

        double delay_time =
            (tmp_bits[Delay] == 1) ? par("ErrorDelay").doubleValue() : 0;

        // delay_time += par("ProcessingTime").doubleValue() + par("TransmissionDelay").doubleValue();
        delay_time += par("TransmissionDelay").doubleValue();
        // we need to sleep with processing time 0.5 before calling sendMessageDelay
        if (tmp_bits[Dup] == 1 && !retransmitted)
        {
            selfMessageDuplicate(msg, delay_time);
        }
        else
        {
            selfMessageDelay(msg, delay_time, retransmitted);
        }
    }
    else {
        readingPrint(this->errorArray[msg_index]); // I might need to comment it
        std::string req_parameters = std::to_string(msg_index) + "," + std::to_string((int)retransmitted);
        Message* dummy = new Message();
        dummy->setPayload(req_parameters.c_str());
        scheduleAt(simTime()+par("ProcessingTime").doubleValue()+extraDelay,dummy);
    }


}

// processing frames in sender
void Node::processFrames(int start_index, int end_index, bool retransmitted)
{
    //    if (start_index > end_index)
    //    {
    //        for (int i = start_index; i <= this->sender_max_sequence_number; i++)
    //        {
    //            Message* msg = new Message;
    //            this->sendLogic(msg, i);
    //        }
    //
    //        for (int i = 0; i <= end_index; i++)
    //        {
    //            Message* msg = new Message;
    //            this->sendLogic(msg, i);
    //        }
    //
    //
    //    }
    //    else
    {
        if (start_index == end_index && (this->sent_sequences[start_index] == 0))
        {
            Message *msg = new Message;
            if (!messageArray[start_index].empty())
            {
                EV << "Message " << start_index << " : " << this->messageArray[start_index] << "\n";
                this->sendLogic(msg, start_index,retransmitted,false);
            }
        }
        double counter = 0;
        for (int i = start_index; i <= end_index; i++)
        {
            if (this->sent_sequences[i] == 0)
            {

                this->sent_sequences[i] = 1;
                Message *msg = new Message;
                if (!messageArray[i].empty())
                {
                    EV << "Message " << i << " : " << this->messageArray[i] << "\n";
                    this->sendLogic(msg, i,retransmitted,false,0.5*counter);
                    counter++;
                }
            }
        }
    }
}

// response from receiver

void Node::send_ACK_or_NACK(Message *msg, bool is_ack, int seq_number)
{
    msg->setAck_no(seq_number);
    if (is_ack)
    {
        // set as ACK
        msg->setType(1);
    }
    else
    {
        // set as NACK
        msg->setType(2);

    }
    double delay = par("ProcessingTime").doubleValue() + par("TransmissionDelay").doubleValue();


    sendDelayed(msg, delay, "out");
}

void Node::finish(){
    writeToFile();
}
