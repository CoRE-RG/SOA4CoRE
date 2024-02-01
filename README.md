# SOA4CoRE
SOA4CoRE (Service-Oriented Architecture for Communication over Realtime Ethernet) is an open-source extension for the event-based simulation of service-oriented communication in real-time Ethernet networks in the [OMNEST/OMNeT++](https://omnetpp.org/) simulation system and replaces the deprecated [SOQoSMW Framework](https://github.com/CoRE-RG/SOQoSMW). It is based on the [INET-Framework](https://inet.omnetpp.org/), providing internet technologies, the [CoRE4INET Framework](https://github.com/CoRE-RG/CoRE4INET), providing real-time Ethernet protocols such as AS6802, AVB and TSN, and the SignalsAndGateway framework (https://github.com/CoRE-RG/SignalsAndGateways) providing gateways to legacy bus systems.
SOA4CoRE provides service-oriented middleware components, service applications and service endpoints for avb, tcp and udp. It was created by the [CoRE (Communication over Realtime Ethernet)](https://core-researchgroup.de/) at the [HAW-Hamburg (Hamburg University of Applied Sciences)](https://www.haw-hamburg.de/english.html).

<img src="/doc/images/soa4core.png" alt="SOA4CoRE Environment" width="75%">

## News
* Switch to OMNeT++ 6.0.2 and INET 3.8.3
* Continuous integration is now enabled.
* Model release including service-oriented middleware components, service applications and service endpoints for avb, tcp and udp. 
* Name changed from SOQoSMW to SOA4CoRE.
* Model extended to support AUTOSAR SOME/IP with Service Discovery and SOME/IP service endpoints.

## References
If you would like to reference this project please use the following [publication](https://doi.org/10.29007/w71t):

 >Cakir, Mehmet and Häckel, Timo and Reider, Sandra and Meyer, Philipp and Korf, Franz and Schmidt, Thomas C. A QoS Aware Approach to Service-Oriented Communication in Future Automotive Networks, 2019 IEEE Vehicular Networking Conference (VNC), IEEE, Dec., 2019


## Quick Start
1. Download OMNeT++ 6.0.2
    * [https://omnetpp.org/download](https://omnetpp.org/download)
2. Install OMNeT++
    * [https://doc.omnetpp.org/omnetpp/InstallGuide.pdf](https://doc.omnetpp.org/omnetpp/InstallGuide.pdf)
3. Get INET framework 3.8.3
    * [https://inet.omnetpp.org/Download.html](https://inet.omnetpp.org/Download.html)
    * Disable the feature 'visualization' and all dependent features (causes build issues)
4. Get CoRE framework
    * GitHub: Clone [CoRE4INET](https://github.com/CoRE-RG/CoRE4INET), [FiCo4OMNeT](https://github.com/CoRE-RG/FiCo4OMNeT), [SignalsAndGateways](https://github.com/CoRE-RG/SignalsAndGateways), and this repository and import them in OMNEST/OMNeT++
5. Working with the framework
    * See the documentation in [doc/](/doc)
    * Start the examples in the framework


## Continuous Integration

The build state of the master branch is monitored:
* Building:
<a><img src="https://jenkins.core-rg.de/buildStatus/icon?job=SOA4CoRE/SOA4CoRE"></a>
* Tests:
<a><img src="https://jenkins.core-rg.de/buildStatus/icon?job=SOA4CoRE/SOA4CoRE_tests"></a>

<table>
  <tr>
    <th></th>
    <th>Ubuntu 22.04</th>
    <th>Windows 11</th>
  </tr>
  <tr>
    <td>Building</td>
    <td><img src="https://jenkins.core-rg.de/buildStatus/icon?job=SOA4CoRE/SOA4CoRE/Nodes=Ubuntu_22.04"></td>
    <td><img src="https://jenkins.core-rg.de/buildStatus/icon?job=SOA4CoRE/SOA4CoRE/Nodes=Windows_11"></td>
  </tr>
  <tr>
    <td>Tests</td>
    <td><img src="https://jenkins.core-rg.de/buildStatus/icon?job=SOA4CoRE/SOA4CoRE_tests/Nodes=Ubuntu_22.04"></td>
    <td><img src="https://jenkins.core-rg.de/buildStatus/icon?job=SOA4CoRE/SOA4CoRE_tests/Nodes=Windows_11"></td>
  </tr>
</table>


## Further Information

### Installation
Please see [INSTALL](/INSTALL)

### Documentation
Please see [doc/](/doc)

### Multicast and IGMP in INET 3.6.6
At the moment of this writing the INET framework does not support multicast route updates in routers using the IGMP (see [inet-framework issue #80](https://github.com/inet-framework/inet/issues/80)).
We did a very basic implementation that reacts to IGMP messages to add and remove routes dynamically in routers.
You can download this version from [our fork of the inet-framework](https://github.com/CoRE-RG/inet/tree/feature/DynamicIGMPRoutes) and checkout the branch ```feature/DynamicIGMPRoutes```
The patch is also located at ```SOA4CoRE/patches/inet-igmp-patch.diff``` and can be applied with git. 

## IMPORTANT
The SOA4CoRE model is under continuous development: new parts are added, bugs are corrected, and so on. We cannot assert that the implementation will work fully according to the specifications. YOU ARE RESPONSIBLE YOURSELF TO MAKE SURE THAT THE MODELS YOU USE IN YOUR SIMULATIONS WORK CORRECTLY, AND YOU'RE GETTING VALID RESULTS.
