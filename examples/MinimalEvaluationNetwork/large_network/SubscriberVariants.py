import os
import sys
SIMULATION = True
NETWORK_PATH = '/home/mehkir/omnetpp-5.1.1/workspace/soa4core/examples/MinimalEvaluationNetwork/large_network'
TAB = '    '
SUBSCRIBER_NODE_COUNT = 10
SERVICES_COUNT = 10
if SIMULATION:
    cd_command = 'cd '+NETWORK_PATH
    cd_ret = os.system(cd_command)
    if cd_ret is not 0:
        print('cd error')
        sys.exit(-1)
for nodes in range(1, SUBSCRIBER_NODE_COUNT+1):
    servicesXmlPublishersStringBuffer = list()
    # Begin services.xml
    servicesXmlPublishersStringBuffer.append('<services>')
    for publisherIndex in range(0, 10):
        servicesXmlPublishersStringBuffer.append(TAB + '<service name="service{}Publisher" node="Publisher"/>'.format(publisherIndex))
    servicesXmlPublishersStringBuffer.append('\n')
    # Begin subscriber.ini
    subscriberStringBuffer = list()
    subscriberStringBuffer.append('[General]')
    subscriberStringBuffer.append('network = large_network')
    subscriberStringBuffer.append('\n')
    subscriberStringBuffer.append('**.Subscriber[*].networkLayer.ip.filters = xmldoc(\"rtIpConfig.xml\", \"/rtIpConfig/node[@id=\'Subscriber\']\")')
    subscriberStringBuffer.append('\n')
    subscriberStringBuffer.append('**.numSubNodes = {}'.format(nodes))
    subscriberStringBuffer.append('\n')
    subscriberStringBuffer.append('# Mac addresses')
    # Add subscriber nodes to subscriber.ini
    for nodeIndex in range(0, nodes):
        subscriberStringBuffer.append('**.Subscriber[{0}].phy[*].mac.address = \"A1-00-00-00-00-0{0}\"'.format(nodeIndex))
    subscriberStringBuffer.append('# Local addresses')
    for nodeIndex in range(0, nodes):
        subscriberStringBuffer.append('**.Subscriber[{0}].localAddress = \"Subscriber[{0}]\"'.format(nodeIndex))
    subscriberStringBuffer.append('\n')
    # Add subscriber services
    subscriberStringBuffer.append('# Services')
    for services in range(1, SERVICES_COUNT+1):
        servicesStringBuffer = list()
        servicesStringBuffer.append('**.Subscriber[*].numServices = {}'.format(services))
        servicesStringBuffer.append('\n')
        servicesStringBuffer.append('**.Subscriber[*].services[*].typename = \"Subscriber\"')
        servicesStringBuffer.append('**.Subscriber[*].services[*].qosGroup = \"STD_UDP\"')
        servicesStringBuffer.append('\n')
        for serviceIndex in range(0, services):
            servicesStringBuffer.append('**.Subscriber[*].services[{0}].publisherName = \"service{0}Publisher\"'.format(serviceIndex))
            servicesStringBuffer.append('**.Subscriber[*].services[{0}].subscriberName = \"service{0}Subscriber\"'.format(serviceIndex))
            servicesStringBuffer.append('**.Subscriber[*].services[{0}].applicationTCPPort = 317{0}'.format(serviceIndex))
            servicesStringBuffer.append('**.Subscriber[*].services[{0}].applicationUDPPort = 317{0}'.format(serviceIndex))
            servicesStringBuffer.append('\n')
        # Add subscriber nodes with services in xml
        servicesXmlSubscriberStringBuffer = list()
        for nodeIndex in range(0, nodes):
            for serviceIndex in range(0, services):
                servicesXmlSubscriberStringBuffer.append(TAB + '<service name=\"service{}Subscriber\" node=\"Subscriber[{}]\"/>'.format(serviceIndex, nodeIndex))
            servicesXmlSubscriberStringBuffer.append('\n')
        servicesXmlSubscriberStringBuffer.append('</services>')
        if SIMULATION:
            ini_filename = 'Subscriber.ini'
            xml_filename = 'services.xml'
        else:
            ini_filename = 'results Nodes: {}, Services: {}.ini'.format(nodes, services)
            xml_filename = ini_filename[:-4]+'.xml'
        ini_fileString = '{}'.format('\n'.join(subscriberStringBuffer+servicesStringBuffer))
        xml_fileString = '{}'.format('\n'.join(servicesXmlPublishersStringBuffer+servicesXmlSubscriberStringBuffer))
        with open(ini_filename, 'w') as subscriber_ini:
            subscriber_ini.write('{}\n'.format(ini_fileString))
        with open(xml_filename, 'w') as services_xml:
            services_xml.write('{}\n'.format(xml_fileString))
        if SIMULATION:
            opp_run_ret = os.system('opp_run -r 0 -m -u Cmdenv -n ../..:../../../simulations:../../../src/soa4core:../../../../SignalsAndGateways/examples:../../../../SignalsAndGateways/simulations:../../../../SignalsAndGateways/src:../../../../CoRE4INET/examples:../../../../CoRE4INET/simulations:../../../../CoRE4INET/src:../../../../inet/examples:../../../../inet/src:../../../../inet/tutorials:../../../../FiCo4OMNeT/examples:../../../../FiCo4OMNeT/examples_andl:../../../../FiCo4OMNeT/simulations:../../../../FiCo4OMNeT/src --image-path=../../../../CoRE4INET/images -l ../../../src/soa4core -l ../../../../SignalsAndGateways/src/SignalsAndGateways -l ../../../../CoRE4INET/src/CoRE4INET -l ../../../../inet/src/INET -l ../../../../FiCo4OMNeT/src/FiCo4OMNeT --sim-time-limit=10ms omnetpp.ini')
            if opp_run_ret is not 0:
                print('opp_run error')
                sys.exit(-1)
            results_filename = '\"results_Nodes_{} Services_{}/\"'.format(nodes, services)
            print(results_filename)
            cp_command = 'mv results/ '+results_filename
            cp_ret = os.system(cp_command)
            if cp_ret is not 0:
                print('cp error')
                sys.exit(-1)
