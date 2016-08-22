//#include <algorithm>

//#include <base/printf.h>
//#include <base/thread.h>
//#include <util/string.h>
//#include <timer_session/connection.h>
//#include <qemusavm_session/connection.h>
//#include <terminal_session/connection.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "genericpack/GenericPacket.h"
#include "genericpack/packettypes.h"

//#include <nic/packet_allocator.h>
#include <sys/types.h>
#include <sys/socket.h>

//#include <lwip/lwipopts.h>
//#include <lwip/sockets.h>
//#include <lwip/api.h>
//#include <lwip/genode.h>
#include "genericpack/types.h"
#include "servocontroller.h"
//#include <base/thread.h>
//#include <base/sleep.h>

void braking(int target,float brakevalue, Servocontroller *servo){

	int brake = (int) (brakevalue * 2000);
	printf("Brake at %d has value %f \r\n",target, brake);
//	printf("Brake at %d has value %f \r\n",target, brakevalue);
	servo->set_target(target, 6000 + brake);

}

void process_simcom_data(int* sockfd, GenericPacket *psend, GenericPacket *precv, Servocontroller *servo)
{

//	printf("Process SimcomData");
	precv->clear();
	//printf("Receiving Packet from SimCom\n"); 
    int  bytes = 0;
    bytes = recv(*sockfd, precv->getPacketPtr(), 4096,0);
	if (bytes < 0){
		printf("Error: Package not received correctly! %i \n", bytes);
//		 return;
	}

	int gear = 42; //pSensorDataIn->curGear;

	float brake, brakeFL, brakeFR , brakeRL , brakeRR  = 42;
	precv->get(&brake, sizeof(float));
	precv->get(&brakeFL, sizeof(float));
	printf("Value of brake is %f \r\n", brake); 
//	printf("Value of float is %d", sizeof(float));
	precv->get(&brakeFR, sizeof(float));
	precv->get(&brakeRL, sizeof(float));
	precv->get(&brakeRR, sizeof(float));

	precv->get(&gear, sizeof(int));
	printf("Gear  %d \r\n", gear);
	
	
	/*
	printf("BrakeFL value is %d\r\n",(int)  brakeFL);
	printf("BrakeFR value is %d\r\n",(int)  brakeFR);
	printf("BrakeRL value is %d\r\n",(int)  brakeRL);
	printf("BrakeRR value is %d\r\n",(int)  brakeRR);
	*/
	
	//	printf("Gear %i \n", gear);

    psend->clear();
	
//	printf("sendpacket cleared");

	psend->setPacketType(packettype_request_gear);

//	printf("Packettype is set");


//	printf("speed = %d", (int)speed);
//	printf("speed (sensordata) = %d", (int) pSensorDataIn->ownSpeed);

//	psend->add(&speed, sizeof(float));

//	printf("Process gear SimcomData");
	psend->add(&brake, sizeof(float));
	psend->add(&brakeFL, sizeof(float));
	psend->add(&brakeFR, sizeof(float));
	psend->add(&brakeRL, sizeof(float));
	psend->add(&brakeRR, sizeof(float));

	psend->add(&gear, sizeof(int));

     	//printf("Sending Packet to SimCom\n");

   bytes = send(*sockfd, psend->getPacketPtr(), psend->getPacketLength(), 0);
   
   //setting servocontroller

   switch (gear){
	  case 1:
		     	servo->set_target(0, 6000);
			servo->set_target(1, 8000);
			break;
	  case 2:
			servo->set_target(0, 8000);
			servo->set_target(1, 6000);
			break;
	  case 3:
			servo->set_target(0, 6400);
			servo->set_target(1, 7600);
			break;
	  case 4:
			servo->set_target(0, 6600);
			servo->set_target(1, 7400);
			break;
	  case 5:
			servo->set_target(0, 6800);
			servo->set_target(1, 7200);
			break;
   //more gears or other stuff to go! values could probably be wrong!
	  default:
			servo->set_target(0, 8000);
			servo->set_target(1, 8000);
			break;
   }
 

   braking(2,brake, servo);
   braking(3,brakeFL, servo);
  /*
   braking(1,brakeFR, servo);
   braking(2,brakeRL, servo);
   braking(3,brakeRR, servo);
  */

   if (bytes < psend->getPacketLength()){
		printf("Error: Package not received correctly!\r\n");
//		 return;
   }



	//pCommandData->gear = gear;
//	printf("Received Gear is %i\r\n", gear);
 }




void con2service(char* servicename , int port , int* _sockfd){

   bool connected = false;
   int count = 0;
   struct sockaddr_in sock_addr, client;
   sock_addr.sin_family = AF_INET;
   sock_addr.sin_port = htons(port);
   sock_addr.sin_addr.s_addr = inet_addr("10.0.2.5");

   while(!connected){
   
		if(count < 3){
		   printf("Create new socket for %s ...\r\n", servicename);
		   count++;
		}
		*_sockfd = socket(AF_INET, SOCK_STREAM, 0 );
		if (*_sockfd < 0) {
			if(count < 3){
			printf("No sim socket available!\r\n");
			printf("This Error is only displayed once! But functions will continue!\r\n");
			count++;
			}
			sleep(2);
			continue;
		}
		
		if (connect(*_sockfd, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) < 0)
		{
			if(count < 3)
			{
				printf("ERROR on binding socket to %s \r\n", servicename);
				printf("This Error is only displayed once! But functions will continue!\r\n");
				count++;
			}
			sleep(2);
			close(*_sockfd);
			continue;
		}

   printf("Connected to %s!\r\n", servicename);
   connected = true;
   }
	printf("Starting communication loop with %s .\r\n", servicename);

   return;
}




int main(int argc, char* argv[])
{
	printf("Hello, BeagleBoneBlack here!\r\n");

	//Generic Packages for the simcom qemusavm connection
	//one for sending one for receiving
	GenericPacket psend(1024);
	GenericPacket precv(1024);

	Servocontroller servo;


	//initialize beagleboneblack socket descriptor
	//for later use in upstart "con2service" and working "proccess_*_data" functions
	int beagleboneblacksockfd;






   servo.set_acceleration(0,0);

//		enum { BUF_SIZE = Nic::Packet_allocator::DEFAULT_PACKET_SIZE * 128 };

   //Initializing NIC Interface
//	lwip_tcpip_init();

		 //Getting IP Address
	  //if you want to use static ip addresses use code below instead of the line above
	  //we assume that this is normally not necessary and you should always use a dhcp server
	  //with static leases to achieve static ip adresses for your component!
		   
			 /*   if(lwip_nic_init(inet_addr("10.0.3.53"), // ip
					 		 inet_addr("255.255.255.0"), // subnet mask
					 	 0, // gw
					 		 BUF_SIZE, BUF_SIZE)) */

/*    if(lwip_nic_init(0, 0, 0, BUF_SIZE, BUF_SIZE))
    {
		printf("We got no IP address!");
		return 0;
	}
*/
    //Sleep to wait for IP assignement
//	sleep(4);
   

   //Starts a socket for the Service with the parameters servicename (only for debugging messages), ip address to connect to, port to connect to
   //and prior created socket as pointer.
    printf("BeagleBoneBlack connection routine fired ...\r\n");
	con2service((char*)"Simcom" , 5000, &beagleboneblacksockfd);


   //never stops data exchange, should be later changed to gracefully shut down the simulation
   //consider to also change it in speeddreams, because a connection close is also not handle there.
	while (1) {
			process_simcom_data(&beagleboneblacksockfd, &psend, &precv, &servo);
		 }

	
	return 0;
}


