//Inclusão de módulos

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("SecondScriptExample");

int
main (int argc, char *argv[])
{
  bool verbose = true;      // Habilita os componentes de registro de UdpEchoClientApplication e UdpEchoServerApplication.
  uint32_t nCsma = 3;       //número de nós csma

  //habilitando alteração de parâmetros via linha de comando

  CommandLine cmd;
  cmd.AddValue ("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);

  cmd.Parse (argc,argv);

  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

  nCsma = nCsma == 0 ? 1 : nCsma;

//criação de nós  ponto-a-ponto

  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  // Declaramos outro NodeContainer para manter os nós que serão parte da rede em barramento (CSMA).

  NodeContainer csmaNodes;
  csmaNodes.Add (p2pNodes.Get (1));
  csmaNodes.Create (nCsma);  //Um nó da rede funcionará como Gateway interligando as duas redes. 


  //Instancindo um PointToPointHelper e definido os atributos de transmissão e atraso 

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));


  // Isntanciado mais um NetDeviceContainer para gerenciar os dispositivos ponto-a-ponto 

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes); // Instalamos os dispositivos nos nós ponto-a-ponto.

 // Criando o NetDeviceContainer para gerenciar os dispositivos criados pelo nosso CsmaHelper.

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

// Método Install do CsmaHelper para instalar os dispositivos nos nós do csmaNodes NodeContainer

  NetDeviceContainer csmaDevices;
  csmaDevices = csma.Install (csmaNodes);

// Instalar pilha de protocolos da internetem todos os nós. 

  InternetStackHelper stack;
  stack.Install (p2pNodes.Get (0));
  stack.Install (csmaNodes);

 
// atribuir endereços IP para as interfaces de nossos dispositivos

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);

  address.SetBase ("12.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces;
  csmaInterfaces = address.Assign (csmaDevices);


UdpEchoServerHelper echoServer (7);   //Servidor UdpEchoServerHelper e o atributo obrigatório do construtor que é o número da porta.


// Instalando a aplicação ao nó de rede csma

  //ApplicationContainer serverApps = echoServer.Install (csmaNodes.Get (nCsma));
  //serverApps.Start (Seconds (1.0));
  //serverApps.Stop (Seconds (10.0));

  ApplicationContainer serverApps = echoServer.Install (p2pNodes.Get (0));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (25.0));

  //Definindo os atributos do  construtor  cliente 

  //UdpEchoClientHelper echoClient (csmaInterfaces.GetAddress (nCsma), 9);
  //echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  //echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  //echoClient.SetAttribute ("PacketSize", UintegerValue (1024));


 UdpEchoClientHelper echoClient1 (p2pInterfaces.GetAddress (0), 7);
  echoClient1.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient1.SetAttribute ("Interval", TimeValue (Seconds (2.0)));
  echoClient1.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps1 = echoClient1.Install (csmaNodes.Get (nCsma));
  clientApps1.Start (Seconds (2.0));
  clientApps1.Stop (Seconds (25.0));


UdpEchoClientHelper echoClient2 (p2pInterfaces.GetAddress (0), 7);
  echoClient2.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient2.SetAttribute ("Interval", TimeValue (Seconds (2.0)));
  echoClient2.SetAttribute ("PacketSize", UintegerValue (2048));

  //ApplicationContainer clientApps = echoClient.Install (p2pNodes.Get (0));
  //clientApps.Start (Seconds (2.0));
  //clientApps.Stop (Seconds (10.0));
  

  ApplicationContainer clientApps2 = echoClient2.Install (csmaNodes.Get (nCsma));
  clientApps2.Start (Seconds (8.0));
  clientApps2.Stop (Seconds (25.0));


  //Definindo  tabela de rotemanento (Cada nó se comporta como um OSPF)

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  pointToPoint.EnablePcapAll ("second"); //Habilitando o rastreamento pcap P2P
  csma.EnablePcap ("second", csmaDevices.Get (1), true); //Habilitando o rastreamento pcap nś csma
  // csmaDevices.Get (1) seleciona o 1º nó para captura

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
