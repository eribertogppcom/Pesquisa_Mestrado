//Inclusão de blibiotecas 

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;  //Agrupa todas as declarações relacionadas ao projeto em um escopo fora do global

 
 //Habilitando os componentes de log 

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int
main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);
  
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);  //definindo o tipo de log 
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);


  // Criando dois nós 

  NodeContainer nodes;  //  Forma conveniente de criar,gerenciar e acessar qualquer objeto Node que queremos para executar a simulação.
  nodes.Create (2);     // Método Create sobre o objeto nodes, atribuindo dois nós

  
//Definindo assistente de topologia 

// Definindo o tipo de canal  e seus atributos
  PointToPointHelper pointToPoint; 
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps")); 
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer devices;  //Agrupando objetos criados
  devices = pointToPoint.Install (nodes);  //Instalando os pontos na rede

  // Instalando a pilha de protocolos TCP/IP nos nós
  InternetStackHelper stack;
  stack.Install (nodes);

//Atribuição de IP aos nós e as interfaces
  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");

  Ipv4InterfaceContainer interfaces = address.Assign (devices);


  // Criando uma aplicação  (servidor) que vai ouvir todas as mensagens na porta 9
 
  UdpEchoServerHelper echoServer (9); //Definindo o assitente para facilitar os demais passos da aplicação, assim como define a porta

  // Instanciando a aplicação ao server para o 1º nó
  ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));
  serverApps.Start (Seconds (1.0));  //tempo de início da aplicação
  serverApps.Stop (Seconds (10.0));  //tempo final da aplicação


 // Criando uma aplicação  (servidor) que vai ouvir todas as mensagens na porta 9

  UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9); // Nó 1 se conectando ao servidor na porta 9
    
  // Configurando pacotes para nó 1

  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  UdpEchoClientHelper echoClient2 (interfaces.GetAddress (1), 9); // Nó 2  se conectando ao servidor na porta 9

// Configurando pacotes para nó 2
  echoClient2.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient2.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient2.SetAttribute ("PacketSize", UintegerValue (2048));

  // Instalando a aplicacao nó 1 da rede
  ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));

 // Instalando a aplicacao nó 2 da rede

  ApplicationContainer clientApps2 = echoClient2.Install (nodes.Get(0));
  clientApps2.Start(Seconds (2.0));
  clientApps2.Stop(Seconds  (10.0));

// Executando simulação

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}