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

#include <utility>
#include <vector>
#include <queue>
#include <algorithm>
#include <iostream>
#include "ns3/assert.h"
#include "ns3/fatal-error.h"
#include "ns3/log.h"
#include "ns3/node-list.h"
#include "ns3/ipv4.h"
#include "ns3/ipv4-routing-protocol.h"
#include "ns3/ipv4-list-routing.h"





using namespace std;
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Campus Network");


/*
5 main network/hub : lab, lib, office, hostel, department connected using mesh topology

LAB
 --Electrical Department
    --2 sub nodes
 --CSE Deaprtment
    --LAN/CSMA with 5 nodes
 --Mechanical Department
    --1 sub node

LIB
 --Floor 1
    --LAN with 5 nodes
 --Floor 2
    --LAN with 5 nodes

OFFICE
 --Admin block
    --2 sub nodes
 --Academic block
    --1 LAN with 5 nodes
 --SAC
    --2 sub nodes

Hostel
 --4 hostels
    -- each with a CSMA connection

Department
 --6 departments 
    --each with 2 sub node
 --1 department
    --1 LAN connection
*/


int main(int argc,char *argv[]){

//general starting for verbose mode    
    CommandLine cmd;
    Time::SetResolution (Time::NS);
    LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    LogComponentEnable ("Ipv4GlobalRouting", LOG_LEVEL_INFO);
    // LogComponentEnable ("PacketSink", LOG_LEVEL_ALL);

    Config::SetDefault ("ns3::OnOffApplication::PacketSize", UintegerValue (250));
    Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue ("4Mbps"));
    cmd.Parse(argc,argv);

    //General node vector containing all nodes
    vector<NodeContainer>  allNodes;
    
    // All node link in a vector
    vector<NetDeviceContainer> connection;   

    //vector containing all the ipv4 containers
    vector<Ipv4InterfaceContainer> InfCon;




//Connectors
    //p2p for central strucutre
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate",StringValue("80Mbps"));
    p2p.SetChannelAttribute("Delay",TimeValue(NanoSeconds(7000)));

    //p2p for sub node of central structure
    PointToPointHelper p2p_department;
    p2p_department.SetDeviceAttribute("DataRate",StringValue("10Mbps"));
    p2p_department.SetChannelAttribute("Delay",TimeValue(MilliSeconds(0.1)));

    //CSMA for sub node of central network
    CsmaHelper csma;
    csma.SetChannelAttribute ("DataRate",StringValue("10Mbps")); //setting data transfer rate
    csma.SetChannelAttribute ("Delay", TimeValue(MilliSeconds (0.1))); //setting time delay



//Creating Nodes
    //creating 5 main nodes/servers :- Labs,Lib,Offices,Hostel,Departments repectively
    NodeContainer centre;  
    centre.Create(5); allNodes.push_back(centre);

  //Creating 3 Lab nodes/ router
    NodeContainer labs;
    labs.Create(3);  allNodes.push_back(labs);
    //Creating 2 sub nodes for lab node 0/electrical department
    NodeContainer elec_lab;
    elec_lab.Create(2); allNodes.push_back(elec_lab);
    //Creasting lan for cse lab
    NodeContainer cse_lab;
    cse_lab.Create(5); allNodes.push_back(cse_lab);
    //Creating a node for mech department
    NodeContainer mech_lab;
    mech_lab.Create(1); allNodes.push_back(mech_lab);

   //creating 2 lan networks for each floor of library
   NodeContainer lib_floor1,lib_floor2;
   lib_floor1.Create(5);
   lib_floor2.Create(5);
   allNodes.push_back(lib_floor1); allNodes.push_back(lib_floor2);

  //Creating 3 offices :- admin block, academic block, DSW
    NodeContainer offices;
    offices.Create(3); allNodes.push_back(offices);
    //Creating subnodes of 3 offices
    NodeContainer SAC,admin,acad;
    SAC.Create(2); admin.Create(2); acad.Create(5);
    allNodes.push_back(SAC); allNodes.push_back(admin); allNodes.push_back(acad);

  //Creating 4 hostels/student residence area
    NodeContainer hostels;
    hostels.Create(4); allNodes.push_back(hostels);
    //Creating subnodes of each hostel
    NodeContainer jasper,amber,emrald,diamond;
    jasper.Create(5); amber.Create(5); emrald.Create(5); diamond.Create(5);
    allNodes.push_back(jasper); allNodes.push_back(amber); allNodes.push_back(emrald); allNodes.push_back(diamond);

  //Creating 7 departments hub
    NodeContainer departments;
    departments.Create(7); allNodes.push_back(departments);
    //Creating 7 department
    NodeContainer cse,ece,me,pe,mnc,ce,ee;
    cse.Create(2); ece.Create(2); me.Create(2); mnc.Create(2); pe.Create(2); ce.Create(2); ee.Create(2);
    allNodes.push_back(cse); allNodes.push_back(ece); allNodes.push_back(me);
    allNodes.push_back(mnc); allNodes.push_back(ce); allNodes.push_back(pe);  allNodes.push_back(ee);




//Connecting all p2p nodes
    // connecting all the central nodes
    for(int i=0;i<5;i++){
        for(int j=i+1;j<5;j++){
            connection.push_back(p2p.Install(centre.Get(i),centre.Get(j)));
        }
    }

    //connecting all p2p nodes in LAB
    connection.push_back(p2p_department.Install(centre.Get(0),labs.Get(0)));
    connection.push_back(p2p_department.Install(centre.Get(0),labs.Get(1)));
    connection.push_back(p2p_department.Install(centre.Get(0),labs.Get(2)));
    connection.push_back(p2p_department.Install(elec_lab.Get(0),labs.Get(0)));
    connection.push_back(p2p_department.Install(elec_lab.Get(1),labs.Get(0)));
    connection.push_back(p2p_department.Install(mech_lab.Get(0),labs.Get(2)));
    //connectiong all p2p nodes in office
    connection.push_back(p2p_department.Install(centre.Get(2),offices.Get(0)));
    connection.push_back(p2p_department.Install(centre.Get(2),offices.Get(1)));
    connection.push_back(p2p_department.Install(centre.Get(2),offices.Get(2)));
    connection.push_back(p2p_department.Install(SAC.Get(0),offices.Get(0)));
    connection.push_back(p2p_department.Install(SAC.Get(1),offices.Get(0)));
    connection.push_back(p2p_department.Install(admin.Get(0),offices.Get(0)));
    connection.push_back(p2p_department.Install(admin.Get(1),offices.Get(0)));
    //connecting all p2p nodes is departments
    connection.push_back(p2p_department.Install(centre.Get(4),departments.Get(0)));
    connection.push_back(p2p_department.Install(centre.Get(4),departments.Get(1)));
    connection.push_back(p2p_department.Install(centre.Get(4),departments.Get(2)));
    connection.push_back(p2p_department.Install(centre.Get(4),departments.Get(3)));
    connection.push_back(p2p_department.Install(centre.Get(4),departments.Get(4)));
    connection.push_back(p2p_department.Install(centre.Get(4),departments.Get(5)));
    connection.push_back(p2p_department.Install(centre.Get(4),departments.Get(6)));
    connection.push_back(p2p_department.Install(cse.Get(0),departments.Get(0)));
    connection.push_back(p2p_department.Install(ece.Get(0),departments.Get(1)));
    connection.push_back(p2p_department.Install(ece.Get(1),departments.Get(1)));
    connection.push_back(p2p_department.Install(pe.Get(0),departments.Get(2)));
    connection.push_back(p2p_department.Install(pe.Get(1),departments.Get(2)));
    connection.push_back(p2p_department.Install(me.Get(0),departments.Get(3)));
    connection.push_back(p2p_department.Install(me.Get(1),departments.Get(3)));
    connection.push_back(p2p_department.Install(ce.Get(0),departments.Get(4)));
    connection.push_back(p2p_department.Install(ce.Get(1),departments.Get(4)));
    connection.push_back(p2p_department.Install(ee.Get(0),departments.Get(5)));
    connection.push_back(p2p_department.Install(ee.Get(1),departments.Get(5)));
    connection.push_back(p2p_department.Install(mnc.Get(0),departments.Get(6)));
    connection.push_back(p2p_department.Install(mnc.Get(1),departments.Get(6)));
    //connecting all csma nodes
    connection.push_back(csma.Install(cse_lab));
    connection.push_back(csma.Install(acad));
    connection.push_back(csma.Install(lib_floor1));
    connection.push_back(csma.Install(lib_floor2));
    connection.push_back(csma.Install(jasper));
    connection.push_back(csma.Install(amber));
    connection.push_back(csma.Install(emrald));
    connection.push_back(csma.Install(diamond));
    connection.push_back(csma.Install(cse));
    //connecting csma with other network
    connection.push_back(p2p_department.Install(cse_lab.Get(0),labs.Get(1)));
    connection.push_back(p2p_department.Install(acad.Get(0),offices.Get(2)));
    connection.push_back(p2p_department.Install(lib_floor1.Get(0),centre.Get(1)));
    connection.push_back(p2p_department.Install(lib_floor2.Get(0),centre.Get(1)));
    connection.push_back(p2p_department.Install(jasper.Get(0),hostels.Get(0)));
    connection.push_back(p2p_department.Install(amber.Get(0),hostels.Get(0)));
    connection.push_back(p2p_department.Install(emrald.Get(0),hostels.Get(0)));
    connection.push_back(p2p_department.Install(diamond.Get(0),hostels.Get(0)));
    connection.push_back(p2p_department.Install(cse.Get(0),departments.Get(0)));

    long unsigned int i;

//Install internet stack
    //Aggregate implementations of the ns3::Ipv4, ns3::Ipv6, ns3::Udp, and ns3::Tcp classes onto the provided node
    InternetStackHelper stack1;
    for(i=0;i<allNodes.size();i++){
        stack1.Install(allNodes[i]);
    }

    
    //assiging ip to central structure
    Ipv4AddressHelper address;
    for(i=0;i<connection.size();i++){
        string ip="20.10.";
        ip+=((char)'0'+i+1);
        ip+=".0";
        address.SetBase(ip.c_str(),"255.255.255.0");
        InfCon.push_back(address.Assign(connection[i]));
    }

    //set routing table
    
   Config::SetDefault ("ns3::Ipv4GlobalRouting::RandomEcmpRouting",BooleanValue (true));
   Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

//TCP packet transfer 
    //Installing TCP server to all nodes
    Address sinkLocalAddress (InetSocketAddress (Ipv4Address::GetAny (), 10));
    PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", sinkLocalAddress);
    ApplicationContainer sinkApp = sinkHelper.Install (centre);
    sinkApp.Start (Seconds (1.0));
    sinkApp.Stop (Seconds (50.0));
    
    srand(time(0));
    int s2 = rand()%10;
    int c1 = rand()%allNodes.size();
    int c2 = rand()%allNodes[c1].GetN();

    OnOffHelper clientHelper ("ns3::TcpSocketFactory", Address ());
    clientHelper.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
    clientHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
    ApplicationContainer clientApps;
    AddressValue remoteAddress(InetSocketAddress (InfCon[s2].GetAddress (0), 10));
    clientHelper.SetAttribute ("Remote",remoteAddress);
    clientApps=clientHelper.Install (allNodes[c1].Get (c2));
    clientApps.Start (Seconds (2.0));
    clientApps.Stop (Seconds (5.0));
      
//UDP packet tranfer
//Installing server application on host
    int port = 9009;
    UdpEchoServerHelper echoServer(port);
    ApplicationContainer serverApp = echoServer.Install(centre.Get(0));

    //Staring server app
    serverApp.Start(Seconds(0.01));
    serverApp.Stop(Seconds(20));

    //Installing client application
    UdpEchoClientHelper echoClient2 (InfCon[0].GetAddress(0),port);
    echoClient2.SetAttribute("MaxPackets",UintegerValue(5));
    echoClient2.SetAttribute("Interval",TimeValue(Seconds(1)));
    echoClient2.SetAttribute("PacketSize",UintegerValue(1024));

    //Installing client app
    ApplicationContainer clientapp2 = echoClient2.Install(lib_floor1.Get(0));

    //Starting client app 
    clientapp2.Start(Seconds(2));
    clientapp2.Stop(Seconds(20));

    AsciiTraceHelper ascii;
    p2p.EnableAsciiAll (ascii.CreateFileStream ("Project_2.tr"));
    p2p_department.EnableAsciiAll (ascii.CreateFileStream ("Project_2.tr"));
    csma.EnableAsciiAll (ascii.CreateFileStream ("Project_2.tr"));


    p2p_department.EnablePcapAll("abc");
    AnimationInterface anim("Campus-Network-Prototype.xml");
    anim.EnablePacketMetadata();
    NS_LOG_INFO ("Run Simulation.");
    Simulator::Stop(Seconds(50));
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
