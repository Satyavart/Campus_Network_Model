#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/netanim-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include <time.h>
#include <bits/stdc++.h>

using namespace std;
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Complex Star Topology");

int main(int argc,char *argv[]){
    
    CommandLine cmd;
    Time::SetResolution (Time::NS);
    LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

    uint32_t n=3; 

    cmd.AddValue("n","Number of nodes in star topology",n);
    cmd.Parse(argc,argv);

    //Creating a router node
    NodeContainer router;
    router.Create(1);

    //Creating host node
    NodeContainer host;
    host.Create(1);

    //Definig p2p attributes
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate",StringValue("5Mbps"));
    p2p.SetChannelAttribute("Delay",StringValue("1ms"));

    //Making 2star topology nodes
    PointToPointStarHelper star1(n,p2p);
    PointToPointStarHelper star2(n,p2p);

    //Install internet stack
    //Aggregate implementations of the ns3::Ipv4, ns3::Ipv6, ns3::Udp, and ns3::Tcp classes onto the provided node
    InternetStackHelper internet;
    internet.Install(host);
    internet.Install(router);
    star1.InstallStack(internet);
    star2.InstallStack(internet);

    //connecting star 1 with router ..... say subnet 1
    NodeContainer star1_router;
    star1_router.Add(star1.GetHub());
    star1_router.Add(router.Get(0));

    //connecting star 2 with router ..... say subnet 2
    NodeContainer star2_router;
    star2_router.Add(router.Get(0));
    star2_router.Add(star2.GetHub());

    //Connecting router and host ..... say subnet 3
    NodeContainer router_host;
    router_host.Add(router.Get(0));
    router_host.Add(host.Get(0));
        
    //p2p attributes for router and host connection
    PointToPointHelper p2p_to_host;
    p2p_to_host.SetDeviceAttribute("DataRate",StringValue("10Mbps"));
    p2p_to_host.SetChannelAttribute("Delay",StringValue("2ms"));

    //Assigning ip to subnet 1
    NetDeviceContainer subnet1Devices;
    subnet1Devices = p2p.Install(star1_router);
    Ipv4AddressHelper subnet1Address;
    subnet1Address.SetBase("12.1.1.0","255.255.255.0");
    Ipv4InterfaceContainer subnet1interface;
    subnet1interface = subnet1Address.Assign(subnet1Devices);

    //Assigning ip to subnet 2
    NetDeviceContainer subnet2Devices;
    subnet2Devices = p2p.Install(star2_router);
    Ipv4AddressHelper subnet2Address;
    subnet2Address.SetBase("13.1.1.0","255.255.255.0");
    Ipv4InterfaceContainer subnet2interface;
    subnet2interface = subnet2Address.Assign(subnet2Devices);

    //Assigning ip to subnet 3
    NetDeviceContainer subnet3Devices;
    subnet3Devices = p2p_to_host.Install(router_host);
    Ipv4AddressHelper subnet3Address;
    subnet3Address.SetBase("14.1.1.0","255.255.255.0");
    Ipv4InterfaceContainer subnet3interface;
    subnet3interface = subnet3Address.Assign(subnet3Devices);

    //Assigning IPs to 2 star topology
    star1.AssignIpv4Addresses(Ipv4AddressHelper("10.1.1.0","255.255.255.0"));
    star2.AssignIpv4Addresses(Ipv4AddressHelper("11.2.1.0","255.255.255.0"));

    //Installing server application on host
    int port = 9009;
    UdpEchoServerHelper echoServer(port);
    ApplicationContainer serverApp = echoServer.Install(router_host.Get(1));

    //Staring server app
    serverApp.Start(Seconds(0.01));
    serverApp.Stop(Seconds(20));

    //Installing client application 1
    UdpEchoClientHelper echoClient1 (subnet3interface.GetAddress(1),port);
    echoClient1.SetAttribute("MaxPackets",UintegerValue(5));
    echoClient1.SetAttribute("Interval",TimeValue(Seconds(1)));
    echoClient1.SetAttribute("PacketSize",UintegerValue(1024));

    //Installing client application 2
    UdpEchoClientHelper echoClient2 (subnet3interface.GetAddress(1),port+6);
    echoClient2.SetAttribute("MaxPackets",UintegerValue(5));
    echoClient2.SetAttribute("Interval",TimeValue(Seconds(1)));
    echoClient2.SetAttribute("PacketSize",UintegerValue(1024));

    //Installing client app on 2 nodes of two diff star topology
    ApplicationContainer clientapp1 = echoClient1.Install(star1.GetSpokeNode(1));
    ApplicationContainer clientapp2 = echoClient2.Install(star2.GetSpokeNode(2));

    //Starting Client app of Star 1
    clientapp1.Start(Seconds(1));
    clientapp1.Stop(Seconds(20));

    //Starting client app of star2
    clientapp2.Start(Seconds(2));
    clientapp2.Stop(Seconds(20));

    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    //p2p.EnablePcapAll("star");
 
    //saving animation to xml file
    AnimationInterface anim ("animation3.xml");

    //setting position of nodes
    anim.SetConstantPosition(star1.GetSpokeNode(0),10,20);
    anim.SetConstantPosition(star1.GetSpokeNode(1),10,40);
    anim.SetConstantPosition(star1.GetSpokeNode(2),10,60);
    anim.SetConstantPosition(star1.GetHub(),25,40);
    anim.SetConstantPosition(router.Get(0),40,40);
    anim.SetConstantPosition(host.Get(0),40,15);
    anim.SetConstantPosition(star2.GetSpokeNode(0),70,20);
    anim.SetConstantPosition(star2.GetSpokeNode(1),70,40);
    anim.SetConstantPosition(star2.GetSpokeNode(2),70,60);
    anim.SetConstantPosition(star2.GetHub(),55,40);

    NS_LOG_INFO ("Run Simulation.");
    Simulator::Stop(Seconds(20));
    Simulator::Run ();
    Simulator::Destroy ();
    NS_LOG_INFO ("Done.");
}
