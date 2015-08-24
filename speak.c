/******************************************************************************
*
*  File Name........: speak.c
*
*  Description......:
*	Create a process that talks to the listen.c program.  After 
*  connecting, each line of input is sent to listen.
*
*  This program takes two arguments.  The first is the host name on which
*  the listen process is running. (Note: listen must be started first.)
*  The second is the port number on which listen is accepting connections.
*
*  Revision History.:
*
*  When	Who         What
*  09/02/96    vin         Created
*
*****************************************************************************/
//search for 'keep this on'
/*........................ Include Files ....................................*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define LEN	64
#define baseport 65420
#define tries 500

struct sockaddr_in sin, pin, nin;
struct my_data {
	  int pid;
	  int prev;	//this is previous neighbours port to which i need to connect
	  struct sockaddr_in pin;
	  int next;	//this is my port where i will listen for next neighbour
	  int master;
	  }my_data;

	
int p, nacc, n, s, fd_max;
int rc, len, port, mynum, hisnum, nummax;
int temp, temp_socket, counter, batata_size, hops, hops_left;
char host[LEN], str[LEN];
char potato[20], id[2], pid_temp;
//char *batata_vada;
int *batata_vada;
struct hostent *hp, *hpp, *hpn, *hptemp;
struct sockaddr_in incoming;
fd_set read_set;
int fd_list[3];
int yes = 1, rnum, rfd, exit_flag;
time_t t;

void get_init_data(int argc, char* argv0, char *argv1, char *argv2)
{
	/* read host and port number from command line */
  if ( argc != 3 ) 
  {
    fprintf(stderr, "Usage: %s <host-name> <port-number>\n", argv0);
    exit(1);
  }
  
  /* fill in hostent struct */
  hp = gethostbyname(argv1); 
  if ( hp == NULL ) 
  {
    fprintf(stderr, "%s: host not found (%s)\n", argv0, host);
    exit(1);
  }
  port = atoi(argv2);
  my_data.master = port;
}

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

int largest(int s1, int s2, int s3)
{
	if (s1 > s2)
	{
		if (s3 > s1)
			return (s3);
		else return (s1);
	}
	else if (s3 > s2)
		return (s3);
	else return (s2);
}
//sending and receiving large chunks of data////////////////////////////////////
//This code has been referenced from Beej's Guide///////////////////////////////////
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
    /* */
    //n = n - (n % 4);
//     if (n%4 != 0)
//     {
// 	    n = n - n%4 - 4000;
//     }
    /* */
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
	  /* */
    //n = n - (n % 4);
//     if (n%4 != 0)
//     {
// 	    n = n - n%4 - 4000;
//     }
    /* */
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
  int i;
  
	get_init_data(argc, argv[0], argv[1], argv[2]);
  
  s = create_socket();
  
  
  /* set up the address and port */
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  memcpy(&sin.sin_addr, hp->h_addr_list[0], hp->h_length);
  
  /* connect to socket at above addr and port */
  rc = connect(s, (struct sockaddr *)&sin, sizeof(sin));
  if ( rc < 0 ) 
  {
    perror("connect :");
    exit(rc);
  }
  
  len = recv(s, &mynum, sizeof(int), 0);	//receive pid
  if ( len < 0 ) 
  {
    perror("recv:");
    exit(1);
  }
  my_data.pid = mynum;
  printf("Connected as player %d\n", my_data.pid+1);
  
  
  //opening my listening port for next neighbour: from here
// 	n = socket(AF_INET, SOCK_STREAM, 0);
//   if ( n < 0 ) 
//   {
//     perror("socket:");
//     exit(n);
//   }

	n = create_socket();
  
  gethostname(host, sizeof host);	
  hpn = gethostbyname(host);
  if ( hpn == NULL ) 
  {
    fprintf(stderr, "%s: host not found (%s)\n", argv[0], host);
    exit(1);
  }
  //printf("here1: %d\n", my_data.pid);
  
	//this part trying- start
	temp = baseport;
	rc = -1;
  //counter = tries;
  while ((rc < 0) && (temp < 65500))
  {
	  nin.sin_family = AF_INET;
	  nin.sin_port = htons(temp);		//my_data.next is my local port to which i will bind and listen for the next player
	  memcpy(&nin.sin_addr, hpn->h_addr_list[0], hpn->h_length);
	  
	  
	  /* bind socket s to address sin */
	  //printf("here2: %d\t port: %d\n", my_data.pid, temp);
	  rc = bind(n, (struct sockaddr *)&nin, sizeof(nin));
	  //printf ("rc = %d\n", rc);
	  if ( rc < 0 ) 
	  {
	    //perror("bind :");
			temp++;
	  }
	}
	if (temp >= 65500)//rc < 0)
	{
		printf("Player #%d: Unable to bind- Exiting\n", my_data.pid);
		exit(rc);
	}
	//this part trying- end
  
  //printf("here3: %d\n", my_data.pid);
  
	my_data.next = temp;
	len = send(s, &temp, sizeof(int), 0);	//send port where i will receive
  if (len < 0) 
  {
    perror("send :");
    exit(1);
  }
  
  
  //printf ("Player %d waiting to receive neighbours port\n", my_data.pid);
  len = recv(s, &temp, sizeof(int), 0);		//receive port where i can send to neighbour
  if ( len < 0 ) 
  {
    perror("recv :");
    exit(1);
  }
  my_data.prev = temp;
  
  len = recv(s, &pin, sizeof(struct sockaddr_in), 0);		//receive address of previous neighbour
  if ( len < 0 ) 
  {
    perror("recv :");
    exit(1);
  }
  pin.sin_port = htons(my_data.prev);
  memcpy(&(my_data.pin), &pin, sizeof(struct sockaddr_in));
  
  //printf("\n Player #%d at your service\n\tDark Lord resides at %d\n\tI listen at %d\n\tI send from %d\n", my_data.pid, my_data.master, my_data.prev, my_data.next);
  
  
  
  
  //printf("Player #%d awaiting lord\n", my_data.pid);
  
  if (1)
  {
	  len = send(s, &my_data.pid, sizeof(int), 0);	//send notification to lord that process is ready/
	  if (len < 0) 
	  {
	    perror("send :");
	    exit(1);
	  }
	}
	
	for (i = 0; i < 2; i++)
	{
		len = recv(s, &temp, sizeof(int), 0);		//receive accept/connect order
	  if ( len < 0 ) 
	  {
	    perror("recv :");
	    exit(1);
	  }
	  if (temp == 0)
	  {
		  //the other guy will connect to me
		  //printf("Player %d: received accept signal from lord\n", my_data.pid);
	    rc = listen(n, 3);
		  if ( rc < 0 ) 
		  {
		    perror("listen :");
		    exit(rc);
		  }
//		  printf(">> I,  Player %d await my next players orders...Connected to %s;\n", my_data.pid, hptemp->h_name);
		  nacc = accept(n, (struct sockaddr *)&incoming, &len);
		  if ( nacc < 0 ) 
		  {
		    perror("accept :");
		    exit(rc);
		  }
		  //printf("Player %d: accept activated\n", my_data.pid);

		  
				//printf("I,  Player %d have received the word of  from younger Player\n", my_data.pid);
//		  printf(">> I,  Player %d have received the word of  from Player %d...Connected to %s;\n", my_data.pid, nprev.pid, hptemp->h_name);
		  
		}
		else if (temp == 1)
		{
		  //if (my_data.pid == 0)
		  	//printf("received connect signal- waiting for neighbour to connect\n");
			
			//connecting to prev neighours port: from here
		 	//will need to change 2 lines below- the hostname of the prev neighbour should be in hp
		 	//printf("Player %d: received connect signal from lord\n", my_data.pid);
// 	    gethostname(host, sizeof host);
// 		 	hpp = gethostbyname(host); 
// 		  if ( hpp == NULL ) 
// 		  {
// 		    fprintf(stderr, "%s: host not found (%s)\n", argv[0], host);
// 		    exit(1);
// 		  }
		  p = create_socket();
		    
		  /* set up the address and port */
// 		  pin.sin_family = AF_INET;
// 		  pin.sin_port = htons(my_data.prev);
//		  memcpy(&pin.sin_addr, my_data.hpp->h_addr_list[0], my_data.hpp->h_length);
		  
		  /* connect to socket at above addr and port */
		  counter = tries;
		  rc = -1;
		  while ((rc < 0) && (counter > 0))
		  {
			  //printf("\tPlayer %d trying to connect\n", my_data.pid);
			  rc = connect(p, (struct sockaddr *)&pin, sizeof(pin));
			  if ( rc < 0 ) 
			  {
// 			    printf("Player #%d:\n", my_data.pid);
// 				  perror("connect :");
			    //exit(rc);
			  }
			  counter--;
	  	}
	  	if ((rc < 0) && (counter == 0))
	  	{
		  	printf("Player %d: I have failed to connect after trying repeatedly: exiting\n", my_data.pid);
		  	exit(rc);
	  	}
		  
		  len = send(s, &rc, sizeof(int), 0);	//send notification to lord that process is connected and ready to accept
		  if (len < 0) 
		  {
		    perror("send :");
		    exit(1);
		  }
		  
	  }
			
			
	}
	
	len = recv(s, &nummax, sizeof(int), 0);		//receive total number of players
  if ( len < 0 ) 
  {
    perror("recv :");
    exit(1);
  }
	len = recv(s, &hops, sizeof(int), 0);		//receive final go signal with total number of hops
  if ( len < 0 ) 
  {
    perror("recv :");
    exit(1);
  }
	//printf("Player %d: total hops = %d\n", my_data.pid, hops);
	hops_left = hops-1;
  
  	
//////Preparing bata vada///////////////////////////////////////
	//batata_vada = (char*) malloc (hops + 1);
	batata_vada = 0;
	batata_vada = (int*) malloc ((hops) * sizeof(int));
	//printf ("batata_vada size = %d\n", (hops+1) * sizeof(int));
  if (!batata_vada) 
  {
	  printf("Couldn't allocate memory for potato: Exiting\n");
  	exit (1);
	}

//   for (counter = 0; counter < batata_size; counter++)
//     batata_vada[counter] = 'o';
//   batata_vada[batata_size]='\0';
/////////////////////////////////////////////////

//Sending confirmation that tiffin box is ready/////////////////////////////////////
	counter = tries;
	rc = -1;
	while ((rc < 0) && (counter > 0))
	{
		rc = send(s, &hops, sizeof(int), 0);	//send notification to lord that the tiffin box is ready
	  if (rc < 0) 
	  {
	    perror("send :");
	    exit(1);
	  }
	  if ( rc < 0 ) 
	  {
	    //printf("Player #%d:\n", my_data.pid);
		  perror("Send: Error sending confirmation :");
	    //exit(rc);
	  }
	  counter--;
	}
	if ((rc < 0) && (counter == 0))
	{
  	printf("Player %d: Master doesn't know I have succeeded in allocating memory: Exiting\n", my_data.pid);
  	exit(rc);
	}
///////////////////////////////////////////////////////////////////////////////////////	

//Pass around the potato///////////////////////////////////////////////////////////////// 
	
	while (1)
	{
		fd_list[0] = s;
	  fd_list[1] = p;
	  fd_list[2] = nacc;
	  FD_ZERO(&read_set);    // clear the master and temp sets
	  fd_max = 0;
	  exit_flag = 0;
	//  hops_left = -69999; //do not clear this guy
	//   for (i = 0; i < num; i++)
	//   {
	// 	  if (fd_max < players[i].fd)
	// 	  	fd_max = players[i].fd;
	// 	  FD_SET(players[i].fd, &read_set);  
	//   }
	
		fd_max = largest(s, p, nacc);
		FD_SET(s, &read_set);
		FD_SET(p, &read_set);
		FD_SET(nacc, &read_set);
	  
	  while (1) 
	  {
	  	temp = (select(fd_max+1, &read_set, NULL, NULL, NULL));
	  	if (temp == -1)
	  	{
		  	perror("select: error: Players exiting at select: Ring broken");
	      exit(1);
	    }
	    if (temp > 0)
	    	break;
	  }
	  
	  for (i = 0; i < 3; i++)
	  {
		  if (FD_ISSET(fd_list[i], &read_set)) //this one has received
		  {
			  //printf("Player %d: Batata is mine. Received from source %d\t", my_data.pid, i);
				  
			  if (i != 0)
			  {
				  len = recv(fd_list[i], &hops_left, sizeof(int), 0);
				  if ( len < 0 ) 
				  {
				    perror("recv : error while receiving hops left: ");
				    exit(1);
				  }
				  if (len == 0)
				  {
					  exit_flag = 1;
		      	//printf("Player #%d: Bretheren %d has fallen into the darkness\n", my_data.pid, i);
					  break;
				  }
				  //printf("Player %d: %d hops left\n", my_data.pid, hops_left);	//keep this on
				  fflush(stdout);
				  
				  temp = 69;
				  len = send(fd_list[i], &temp, sizeof(int), 0);	//send confirmation that hops_left is received
				  if (len < 0) 
				  {
				    perror("send :");
				    exit(1);
				  }
				}
			  //len = recv(fd_list[i], batata_vada, (hops + 1) * (sizeof(char)), 0);
			  memset(batata_vada, -1, (hops) * (sizeof(char)));
			  //len = recv(fd_list[i], batata_vada, (hops+1) * (sizeof(int)), 0);
			  len = receiveall(fd_list[i], batata_vada, (hops) * (sizeof(int)));
			  if (len <= 0) 
			  {
		      // got error or connection closed by client
		      if (len == 0) // connection closed
		      {
		      	exit_flag = 1;
		      	//printf("Player #%d: Bretheren %d has been messily devoured\n", my_data.pid, i);
		      } 
		      else 
		      {
		      	perror("recv");
		      }
		      close(fd_list[i]); // bye!
	//	      FD_CLR(fd_list[i], &master); // remove from master set
	      }
	      else	//len is greater than 0 and hence this guy has successfully received
	      {
		      break;	//break from for loop
	      }
	      
			}
	  }//end of for loop
	  if (exit_flag == 1)	//received notification that another process was messily devoured- I don't do anything but wait for my own signal of doom
	  {
		  //printf("Player %d: Greived I am at the demise of my bretheren %d\n", my_data.pid, i);
		  continue;
	  }
		  
	  if (batata_vada[0] == -69)
    {
      //printf("Player #%d: Master needs me no more: exiting\n", my_data.pid);
      close(p);
      //len = -69;
      rc = send(s, &i, sizeof(int), 0);	//sending confirmation to Dark Lord that previous connection is closed and I am shutting down
		  if (rc < 0) 
		  {
		    perror("send : error while sending exit confirmation:");
		    exit(1);
		  }
      close(s);
      exit(1);
    }
	  if (hops_left < 0)
	  {
		  if (hops_left < -1) printf("Player %d: SOMETHING IS WRONG- negative number of hops: exiting\n", my_data.pid);
		  exit (0);
	  }
	  if (hops_left == 0)
	  {
		  //sprintf(&pid_temp, "%d", my_data.pid);
		  //batata_vada[hops - hops_left] = pid_temp;
		  mynum = my_data.pid + 1;
		  batata_vada[hops - hops_left - 1] = mynum;
		  hops_left--;
		  //printf ("Player %d: last hop. Batata vada- %s\n", my_data.pid, batata_vada);
		  //printf ("Player %d: last hop. Batata vada is: ", my_data.pid);//instead of this really nice line, dr.freeh wants:
		  printf("I'm it\n");
// 		  for (counter = 0; counter < (hops+1); counter++)
//       {
// 	      printf("%d ", batata_vada[counter]);
//       }
//       printf("\n");
		  //len = send(s, batata_vada, (hops + 1) * (sizeof(char)), 0);	//send batata vada to master
		  //len = send(s, batata_vada, (hops+1) * (sizeof(int)), 0);	//send batata vada to master
		  len = sendall(s, batata_vada, (hops) * (sizeof(int)));
		  if (len < 0) 
		  {
		    perror("send : error sending potato to master: ");
		    exit(1);
		  }
		  //exit(1);
	  }
	  else
	  {
//		  sprintf(&pid_temp, "%d", my_data.pid);
//		  printf("Player %d: char conversion: %c\n", my_data.pid, pid_temp);
//		  batata_vada[hops - hops_left] = pid_temp;
// 			if (hops_left < 1000)
//  		  	printf("Player #%d: dumping my data into batata_vada[%d]\n", my_data.pid, hops-hops_left);
			mynum = my_data.pid + 1;
			batata_vada[hops - hops_left - 1] = mynum;
			srand((unsigned int)hops_left * ((int)time(&t)));
		  rnum = (rand()%2);
		  hops_left--;
		  //len = send(nacc, &hops_left, sizeof(int), 0);	//send hops left to next player
		  
		  mynum = my_data.pid + 1; 
		  if (rnum == 0)
		  {
			  if (mynum == 1) hisnum = nummax;
			  else hisnum = mynum - 1;
			  printf("Sending potato to %d\n", hisnum);//keep this on
		  	rfd = p;
	  	}
	  	else
	  	{
		  	if (mynum == nummax) hisnum = 1;
		  	else hisnum = mynum + 1;
		  	printf("Sending potato to %d\n", hisnum); //keep this on
		  	rfd = nacc;
	  	}
	  	len = send(rfd, &hops_left, sizeof(int), 0);	//send hops left to next/previous player
		  if (len < 0) 
		  {
		    perror("send :");
		    exit(1);
		  }
		  len = recv(rfd, &temp, sizeof(int), 0);
		  if ( len < 0 || temp != 69) 
		  {
		    perror("recv : error while receiving confirmation after hops left: ");
		    exit(1);
		  }
		  //fflush(stdout);
		  //if (hops_left < 1000)	printf("Player #%d: batata_vada[%d] = %d\n", my_data.pid, hops-hops_left-1, batata_vada[hops-hops_left-1]);
		  //len = send(rfd, batata_vada, (hops + 1) * (sizeof(char)), 0);	//send batata vada to next/previous Player
		  //len = send(rfd, batata_vada, (hops+1) * (sizeof(int)), 0);	//send batata vada to next/previous Player
		  len = sendall(rfd, batata_vada, (hops) * (sizeof(int)));
		  if (len < 0) 
		  {
		    perror("send : error sending batata_vada to next Player: ");
		    exit(1);
		  }
	  }
  }//end of while (1) loop

/////////////////////////////////////////////////////////////////////////////////////////  
 
  
 
//  close(s);
//  close(p);
//  close(n);
  exit(0);
}



/*........................ end of speak.c ...................................*/
