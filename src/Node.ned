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

package sr_arq_project;

//
// TODO auto-generated module
//
simple Node
{
        parameters:
        double ProcessingTime = default(0.5);
        double DuplicationDelay = default(0.1);
        double TransmissionDelay = default(1.0);
        double ErrorDelay = default(4.0);
        int WindowSize = default(3);
        double TimeoutInterval = default(10);
    // Node either receives from the other node , or from the coordinater 
    gates:
        output out; // to the other node
        input in; //from the other node
        
        input coordinator_gate; //from the coordinator
        
}
