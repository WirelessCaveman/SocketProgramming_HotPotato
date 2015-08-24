/******************************************************************************
*
*  File Name........: listen.c
*
*  Description......:
*	Creates a program that establishes a passive socket.  The socket 
*  accepts connection from the speak.c program.  While the connection is
*  open, listen will print the characters that come down the socket.
*
*  Listen takes a single argument, the port number of the socket.  Choose
*  a number that isn't assigned.  Invoke the speak.c program with the 
*  same port number.
*
*  Revision History.:
*
*  When	Who         What
*  09/02/96    vin         Created
*
*****************************************************************************/

/*........................ Include Files ....................................*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
// #include<list>
// #include<vector>
// #include<algorithm>
// using namespace std;

struct player {
	int pid; 
	struct sockaddr_in address;
	int prev;
	int next;
	int fd;
	};

struct player *players;

int *batata_vada;
int hops;
int temp_socket;
int yes = 1;
time_t t;

int create_socket(void)
{
	/* use address family INET and STREAMing sockets (TCP) */
	 
  //this code has been lifted from Beej's guide.
  if ((temp_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
  {
      perror("socket: ");
      exit(1);
  }

//   // lose the pesky "address already in use" error message
//   if (setsockopt(temp_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) 
//   {
//       
//       perror("setsockopt");
//       exit(1);
//   }
  return (temp_socket);
}	

//sending and receiving large chunks of data////////////////////////////////////
//This code has been lifted from Beej's Guide///////////////////////////////////
int sendall(int fd, void *buf, int len)
{
  int total = 0;        // how many bytes we've sent
  int bytesleft = len; // how many we have left to send
  int n;

  while(total < len) 
  {
    n = send(fd, (buf+total), bytesleft, 0);
    //if (n == -1) { break; }
    if (n < 0 || n == 0) { break; }
    total += n;
    bytesleft = bytesleft - n;
  }

  len = total; // return number actually sent here

  //return n==-1?-1:0; // return -1 on failure, 0 on success
  return (n);
}  

int receiveall(int fd, void *buf, int len)
{
	int total = 0;
	int bytesleft = len;
	int n;
	
	while (total < len)
	{
		n = recv(fd, (buf+total), bytesleft, 0);		//receive accept/connect order
	  //if ( n < 0 ) {break;}
	  if (n < 0 || n == 0) { break; }
	  total = total + n;
	  bytesleft = bytesleft - n;
  }
  len = total;
  //return n == -1? -1:0;
  return (n);
}
//////////////////////////////////////////////////////////////////////////////////
 
main (int argc, void *argv[])
{
  char buf[512];
  int temp, temp2, counter;
  char host[64];
  socklen_t len;
  int s, p, fd_max;
  int rc, port, rnum;
  int i, num, first; // numtemp;
  struct hostent *hp, *ihp;
  struct sockaddr_in sin, incoming, temp_addr;
  struct player temp_player;
  char potato[] = "here", basket[100];
  fd_set read_set;
  
//  char string[10];
  
  /* read port number from command line */
  if (argc != 4) 
  {
    fprintf(stderr, "Usage: %s <port-number>\n", argv[0]);
    exit(1);
  }
  port = atoi(argv[1]);
  if (port < 65420 || port > 65500)
  {
	  printf("Invalid port number: Port should be > 65420 and < 65500\n");
	  exit (1);
  }
  
  /* fill in hostent struct for self */
  gethostname(host, sizeof host);
  hp = gethostbyname(host);
  if ( hp == NULL ) 
  {
    fprintf(stderr, "%s: host not found (%s)\n", argv[0], host);
    exit(1);
  }
  
  /* use address family INET and STREAMing sockets (TCP) */
  s = create_socket();
  
  
	//Getting Players ready////////////////////////////////////////
	//printf("the dark lord sees all\n");
  num = atoi(argv[2]);
  if (num < 2)
  {
	  printf("minimum two players required to play\n");
	  exit (1);
  }
  players = (struct player*) malloc (sizeof(struct player) * num);
  

//Preparing bata_vada //////////////////////////
	hops = atoi(argv[3]);
  if (hops < 0)
  {
	  printf("Illegal number of hops\n");
	  exit (1);
  }
  batata_vada = (int*) malloc (sizeof(int) * (hops));
  if (batata_vada == NULL)
  {
	  printf("Error allocating memory\n"); 
  	exit (1);
	}
	//this is the first print statement as required
	printf("Potato Master on %s\nPlayers = %d\nHops = %d\n", host, num, hops);
  
  for (counter = 0; counter < (hops); counter++)
    batata_vada[counter] = -1;
  //batata_vada[hops]='\0';
  //  while (1) {
//  for (; num > 0; num--)
/////////////////////////////////////////////////
  
  /* set up the address and port */
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  memcpy(&sin.sin_addr, hp->h_addr_list[0], hp->h_length);
  
  /* bind socket s to address sin */
  rc = bind(s, (struct sockaddr *)&sin, sizeof(sin));
  if ( rc < 0 ) 
  {
    perror("bind:");
    exit(rc);
  }
  
//  printf("the dark lord sees all\n");
  
  rc = listen(s, 100);
  if ( rc < 0 ) 
  {
    perror("listen:");
    exit(rc);
  }
  

	for (i = 0; i < num; i++)
  {
    len = sizeof(sin);
    //printf ("here1\n");
    fflush(stdout);
    memset(&temp_addr, 0, sizeof(struct sockaddr_in));
    
    p = accept(s, (struct sockaddr *)&incoming, &len);
    if ( p < 0 ) 
    {
      perror("bind:");
      exit(rc);
    }
    //printf ("accepted here1\n");
    fflush(stdout);
   	players[i].pid = i;
//   	players[i].address = incoming;
   	players[i].fd = p;
//   	players[i] = temp_player;
		memcpy(&(players[i].address), &incoming, sizeof(struct sockaddr_in)); 
    ihp = gethostbyaddr((char *)&incoming.sin_addr, sizeof(struct in_addr), AF_INET);
//    players[i].address = ihp;
    //printf(">> Connected to %s; connection #%d\n", ihp->h_name, i);
    printf("player %d is on %s\n", (i+1), ihp->h_name);
    fflush(stdout);
    
//     if (i == 0)
//     	first = p;
         
    len = send(p, &i, sizeof(int), 0);	//send pid
//    printf ("here2");
    if (len != sizeof(int)) 
    {
      perror("send");
      exit(1);
    }
    
    
    len = recv(p, &temp, sizeof(int), 0);		//receive port where process (its local port) will accept from next neighbour
    if (len != sizeof(int)) 
    {
			perror("recv:");
			exit(1);
    }
    players[i].next = temp;
    
   
    if (i > 0)
    {
		  temp = players[i-1].next;		//next of (i-1) is prev of i
		  players[i].prev = temp;
	    len = send(p, &temp, sizeof(int), 0);		//send port where process will communicate with previous neighbour
	    if (len != sizeof(int)) 
	    {
	      perror("send");
	      exit(1);
	    }
//	    ihp = players[i-1].address;
			memcpy(&temp_addr, &players[i-1].address, sizeof(struct sockaddr_in));
	    len = send(p, &temp_addr, sizeof(struct sockaddr_in), 0);		//send port where process will communicate with previous neighbour
	    if (len < 0) 
	    {
	      perror("send");
	      exit(1);
	    }
    }
    if (i == (num -1)) 
    {
	    temp = players[i].next;
	    players[0].prev = temp;
	    len = send(players[0].fd, &temp, sizeof(int), 0);
	    if (len != sizeof(int)) 
	    {
	      perror("send");
	      exit(1);
	    }
//	    ihp = players[i].address;
	    memcpy(&temp_addr, &players[i].address, sizeof(struct sockaddr_in));
			len = send(players[0].fd, &temp_addr, sizeof(struct sockaddr_in), 0);		//send port where process will communicate with previous neighbour
	    if (len < 0) 
	    {
	      perror("send");
	      exit(1);
	    }
    }
    
  }
//   for (i = 0; i < num; i++)
//   {
// 	  printf ("\n Player #%d:\n\tpid: %d\n\tconnects through port: %d\n\taccepts port: %d\n", i, players[i].pid, players[i].prev, players[i].next);
//   }
  
  
  for (i = 0; i < num; i++)
  {
	  len = recv(players[i].fd, &temp, sizeof(int), 0);		//receive signal from all Players when ready
	  if (len != sizeof(int)) 
	  {
			perror("recv:");
			exit(1);
	  }
	  //printf("Take no hostages #%d!!!\n", temp);
	}
	fflush(stdout);
  
  for (i = 0; i < num; i++)
  {	  
	  temp = 0;
	  len = send(players[i].fd, &temp, sizeof(int), 0);		//send accept signal
    if (len != sizeof(int)) 
    {
      perror("send");
      exit(1);
    }
    //printf ("sent accept signal to Player #%d\n", i);
    fflush(stdout);
    
   
    if (i == (num - 1))
	  	temp2 = 0;
	  else temp2 = i + 1;
// 		if (i == (num -1))
// 			i = -1;
	  	
    temp = 1;
	  len = send(players[temp2].fd, &temp, sizeof(int), 0);		//send connect signal
    if (len != sizeof(int)) 
    {
      perror("send");
      exit(1);
    }
    
    len = recv(players[temp2].fd, &temp, sizeof(int), 0);		//receive signal from minion that he has connected and is ready to now accept
	  if (len == -1) 
	  {
			perror("recv:");
			exit(1);
	  }
	  //printf ("\nPlayer %d connected to Player #%d\n", temp2, i);
    fflush(stdout);    
  }
  
  //printf("******************************\n**************************\n***************************\n*******RING FORMED****************\n****************\n*********************\n");
  
  //printf("hops = %d\trandom sockfd = %d\n", hops, players[rnum].fd);
  hops = atoi(argv[3]);
  //printf("sending mumber of hops as %d\n", hops);
  //sleep(10);
  if (hops <= 0)
  {
	  printf("Potato not sent\n");
  	exit (0);
	}
   
//  temp = 69; //send total number of hops from here
//	temp = (hops + 1) * (sizeof(char));
  
	for (i = 0; i < num; i++)
  {
	  len = send(players[i].fd, &num, sizeof(int), 0);		//send number of players
    if (len != sizeof(int)) 
    {
      perror("send hops error :");
      exit(1);
    }
	  len = send(players[i].fd, &hops, sizeof(int), 0);		//send number of hops as start signal
    if (len != sizeof(int)) 
    {
      perror("send hops error :");
      exit(1);
    }
  }
  
  //Receive notification that all players know the size of the batata_vada and have the tiffin box ready////
  for (i = 0; i < num; i++)
  {
	  len = recv(players[i].fd, &temp, sizeof(int), 0);		//receive signal from minion that the tiffin box is ready
	  if (len == -1) 
	  {
			perror("recv:");
			exit(1);
	  }
	  //printf("Minion %d has tiffin ready\n", i);
  }
  //sleep(2);
  //SENDING OUT BATATA_VADA//////////////////////////
  srand((unsigned int)hops * ((int)time(&t)));
  rnum = rand()%num;
  printf("All players present, sending potato to player %d\n", rnum + 1);
  fflush(stdout);
  //len = sendall(players[0].fd, batata_vada, (hops+1) * (sizeof(int)));
  len = send(players[rnum].fd, batata_vada, (hops) * (sizeof(int)), 0);
  if (len == -1) 
  {
    perror("send potato error :");
    exit(1);
  }
 ////////////////////////////////////////////////////// ///
  
  //////////////////////////////////////////////////////////////////////////////
  //do this after sending out the potato- this will receive the potato
//   FD_ZERO(&read_set);    // clear the master and temp sets
//   fd_max = 0;
//   for (i = 0; i < num; i++)
//   {
// 	  if (fd_max < players[i].fd)
// 	  	fd_max = players[i].fd;
// 	  FD_SET(players[i].fd, &read_set);  
//   }
  
  while (1) 
  {
  	FD_ZERO(&read_set);    // clear the master and temp sets
	  fd_max = 0;
	  for (i = 0; i < num; i++)
	  {
		  if (fd_max < players[i].fd)
		  	fd_max = players[i].fd;
		  FD_SET(players[i].fd, &read_set);  
	  }
	  temp = (select(fd_max+1, &read_set, NULL, NULL, NULL));
  	if (temp == -1)
  	{
	  	perror("select");
      exit(1);
    }
    if (temp > 0)
    	break;
  }
  
  for (i = 0; i < num; i++)
  {
	  if (FD_ISSET(players[i].fd, &read_set)) //this one has received
	  {
		  //if ((len = recv(players[i].fd, batata_vada, (hops + 1) * (sizeof(char)), 0)) <= 0) 
		  //if ((len = recv(players[i].fd, batata_vada, (hops+1) * (sizeof(int)), 0)) <= 0) 
		  if ((len = receiveall(players[i].fd, batata_vada, (hops) * (sizeof(int)))) <= 0)
		  {
	      // got error or connection closed by client
	      if (len == 0) 
	      {
	      	// connection closed
	      	printf("Player %d has fallen- Exiting\n", i);
	      	break;
	      } 
	      else 
	      {
	      	perror("Recv");
	      	break;
	      }
	      close(players[i].fd); // bye!
//	      FD_CLR(players[i].fd, &master); // remove from master set
      }
      else	//len is greater than 0 and hence this guy has successfully received
      {
	      //printf("My Player %d is it and my quest for the potato has ended: %s\n", i, batata_vada);
	      //printf("My Player %d is it and my quest for the potato has ended: ", i);
	      printf("Trace of potato:\n");
    		for (counter = 0; counter < (hops); counter++)
	      {
		      printf("%d", batata_vada[counter]);
		      if (counter < (hops - 1)) printf(",");
	      }
	      printf("\n");
      }
    }
  }
//////////////////////////////////////////////////////////////////////////////

//Telling Minions to go boil their heads//////////////////////////////////////
	batata_vada[0] = -69;
	for (counter = 0; counter < num; counter++)
	{
		//len = send(players[counter].fd, batata_vada, (hops+1) * (sizeof(int)), 0);		//send close signal
		len = sendall(players[counter].fd, batata_vada, (hops) * (sizeof(int)));		//send close signal
	  if (len == -1) 
	  {
	    perror("send: send error for close signal: ");
	    exit(1);
	  }
	  //printf("sent exit order to Player %d\n", counter);
	  len = recv(players[counter].fd, &temp, sizeof(int), 0);		//receive confirmation that minion has gone down
	  if (len == -1) 
	  {
			perror("recv:");
			exit(1);
	  }
	  //printf("Player %d has been terminated\n", counter);
  }
  
  exit(0);
}

/*........................ end of listen.c ..................................*/
