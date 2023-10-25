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

package soa4core.manager.someip;

import soa4core.manager.base.Manager;

//
// The SOME/IP service manager module creates, finds and removes local SOME/IP services.
// As this module manages all local services it also manages the service discovery for these services @see(~SomeIpSD). 
//
// Excerpts are from the AUTOSAR Standard: AUTOSAR, "SOME/IP Service Discovery Protocol Specification", in release "AUTOSAR FO R21-11", Nov. 2021.
//
// @author Mehmet Mueller and Timo Haeckel for HAW Hamburg
//
simple SomeIpManager extends Manager
{
    parameters:
        @class(SOA4CoRE::SomeIpManager);
        @display("i=block/join");
        
        // The signal which is emitted when a requested service is found
        @signal[findResultSignal];
        // The signal which is emitted when a service is subscribed
        @signal[subscribeSignal];
        // The signal which is emitted when a subscription has been acknowledged
        @signal[subscribeAckSignal];
        
        // [PRS_SOMEIPSD_00395] For each Service Instance the Service Discovery shall have at least these three phases in regard to sending entries:
        // Initial Wait Phase, Repetition Phase, and Main Phase    
            
        // Initial Wait Phase:
        // - Starts when the client/server app is up and the network is up [PRS_SOMEIPSD_00397], [PRS_SOMEIPSD_00133]
		// - Wait for the "INITIAL_DELAY" after entering the initial wait phase for the service, before sending the first messages for the Service Instance [PRS_SOMEIPSD_00399]
		// - Wait time =  random between minimum and maximum defined as INITIAL_DELAY [PRS_SOMEIPSD_00400] [PRS_SOMEIPSD_00401]
		// - if a ClientService or ServerSerivce enters the Initial Wait Phase, they shall use an individual calculated random value within the Initial Wait Phase. [PRS_SOMEIPSD_00804]
		// - After sending the first message the Repetition Phase of this Service Instance/these Service Instances is entered. [PRS_SOMEIPSD_00404]
		// - If a FindService Entry is received within the Initial Wait Phase for this Server Service Instance, it shall be ignored. [PRS_SOMEIPSD_00839]
        double initialDelayMin @unit(s) = default(0s);
        double initialDelayMax @unit(s) = default(0s);
        volatile double initialDelay @unit(s) = default(uniform(initialDelayMin, initialDelayMax)); // setting this directly will override the initial delay min and max values
        // Example: 	
        // 1. Wait for random_delay in Range(initialDelayMin, initialDelayMax)
		// 2. Send message (Find Service and Offer Service entries)
		        
        // Repetition Phase:
     	// - wait in the Repetitions Phase based on REPETITIONS_BASE_DELAY [PRS_SOMEIPSD_00405]
	 	// - After each message sent in the Repetition Phase the delay is doubled [PRS_SOMEIPSD_00406]
	 	// - send out only up to REPETITIONS_MAX entries during the Repetition Phase [PRS_SOMEIPSD_00407]
	 	// - Sending Find entries shall be stopped after receiving the corresponding Offer entries --> go to Main Phase [PRS_SOMEIPSD_00408]
	 	// - If REPETITIONS_MAX is set to 0, the Repetition Phase shall be skipped [PRS_SOMEIPSD_00409]
 		// - After the Repetition Phase the Main Phase is being entered for a Service Instance. [PRS_SOMEIPSD_00410]
        int repetitionsMax = default(0); // 0 => no repition phase
        double repititionBaseDelay @unit(s) = default(1s);
        // Example: 
        // 	1. Wait 2^0 * repititionBaseDelay
        //	2. Send message (Find Service and Offer Service entries)
        // 	3. Wait 2^1 * repititionBaseDelay
        //	4. Send message (Find Service and Offer Service entries)
        //	...
        //	X. Wait 2^(repetitionsMax-1) * repititionBaseDelay
        //	Y. Send message (Find Service and Offer Service entries)
        
        // Main Phase:
        // 	- Wait 1*CYCLIC_OFFER_DELAY before sending the first offer entry message [PRS_SOMEIPSD_00411]
        //  - Offer Messages shall be sent cyclically if a CYCLIC_OFFER_DELAY is configured [PRS_SOMEIPSD_00412]
        //  - For Find entries (Find Service and Find Eventgroup) no cyclic messages are allowed in Main Phase [PRS_SOMEIPSD_00415]
        //  - Subscribe EventGroup Entries shall be triggered by Offer entries, which are sent cyclically  [PRS_SOMEIPSD_00582]
        double cyclicOfferDelay @unit(s) = default(0s);
        // Example: 
		// 1. Wait CYCLIC_OFFER_DELAY
		// 2. Send message (Offer Service entries)
		// ... 
}		
