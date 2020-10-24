/* -- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include<time.h>
#include<bits/stdc++.h>

using namespace std;
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ScratchSimulator");

int 
main (int argc, char *argv[])
{
    CommandLine cmd;
    Time::SetResolution (Time::NS);
    LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

    uint32_t n1 = 4;
    uint32_t n2 = 4;

    cmd.AddValue("n1", "Number of LAN 1 nodes", n1);
    cmd.AddValue("n2", "Number of LAN 2 nodes", n2);

    cmd.Parse(argc, argv);
    
    //For the first network
    NodeContainer csma1_nodes;

    //For the second network
    NodeContainer csma2_nodes;

    //For router node
    NodeContainer router_node;

    //creating nodes
    csma1_nodes.Create(n1);
    csma2_nodes.Create(n2);
    router_node.Create(2);

    //creating a csma network to connect all nodes of one side
    CsmaHelper csma1;
    csma1.SetChannelAttribute ("DataRate",StringValue("10Mbps")); //setting data transfer rate
    csma1.SetChannelAttribute ("Delay", TimeValue(NanoSeconds (6560))); //setting time delay
    csma1_nodes.Add(router_node.Get(0)); //connecting csma1 to router
    NetDeviceContainer csma1_device;
    csma1_device = csma1.Install(csma1_nodes); //attaching CsmaNetDevice to all LAN 1 nodes

    //doing same for LAN 2
    CsmaHelper csma2;
    csma2.SetChannelAttribute ("DataRate",StringValue("10Mbps")); //setting data transfer rate
    csma2.SetChannelAttribute ("Delay", TimeValue(NanoSeconds (6560))); //setting time delay
    csma2_nodes.Add(router_node.Get(1)); //connecting csma2  to router
    NetDeviceContainer csma2_device;
    csma2_device = csma2.Install(csma2_nodes); //attaching CsmaNetDevice to all LAN 1 nodes

    //connecting 2 routers using point to point protocol
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate",StringValue("10Mbps"));
    pointToPoint.SetChannelAttribute("Delay",StringValue("1ms"));

    NetDeviceContainer routerDevice;
    routerDevice = pointToPoint.Install(router_node);


    srand(time(0));
    int start = rand()%4; 
    int finish = rand()%4;
    //cout<<start<<" "<<finish;

    InternetStackHelper stack;
    stack.Install(csma1_nodes);
    stack.Install(csma2_nodes);

    //assigning address to lan 1 nodes
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0","255.255.255.0");
    Ipv4InterfaceContainer lan1interface;
    lan1interface = address.Assign(csma1_device);

    //assgning address to lan2 nodes
    address.SetBase("10.1.2.0","255.255.255.0");
    Ipv4InterfaceContainer lan2interface;
    lan2interface = address.Assign(csma2_device);

    //assigning address to routers
    address.SetBase("10.1.3.0","255.255.255.0");
    Ipv4InterfaceContainer routerInterface;
    routerInterface = address.Assign(routerDevice);

    //installING a UdpEchoServer on all nodes of LAN2
    
    UdpEchoServerHelper echoServer (9);
    ApplicationContainer serverApps = echoServer.Install (csma2_nodes);
    serverApps.Start (Seconds (0));
    serverApps.Stop (Seconds (20));

    //createing UdpEchoClients in all LAN1 nodes.
    UdpEchoClientHelper echoClient (lan2interface.GetAddress (finish), 9);
    echoClient.SetAttribute ("MaxPackets", UintegerValue (5));
    echoClient.SetAttribute ("Interval", TimeValue (MilliSeconds (2000)));
    echoClient.SetAttribute ("PacketSize", UintegerValue (2048));

    
    //installing UdpEchoClient on a node in lan1 nodes
    NodeContainer clientNode (csma1_nodes.Get(start));
    ApplicationContainer clientapp = echoClient.Install(clientNode);
    clientapp.Start(Seconds(0.001));
    clientapp.Stop(Seconds(20));

    //For routers to be able to forward packets, they need to have routing rules.
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();  




    csma1.EnablePcap("lan1", csma1_device);
    csma2.EnablePcap("lan2", csma2_device);
    pointToPoint.EnablePcapAll("routers");
    pointToPoint.EnableAscii("ascii-p2p", router_node);

/*
    AnimationInterface anim ("animation1.xml");
    anim.SetConstantPosition(router_node.Get(0),30,14);
    anim.SetConstantPosition(router_node.Get(0),60,14);

*/
    Simulator::Stop(Seconds(20));
    Simulator::Run ();
    Simulator::Destroy ();
}
