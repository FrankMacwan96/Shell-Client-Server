#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h> 
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>


int main(int argc, char *argv[])
{
  srand(time(NULL));
  signal(SIGINT, SIG_IGN);
  char message[100];
  int server, portNumber;
  socklen_t len;
  struct sockaddr_in servAdd;     // server socket address
  
 if(argc != 3){
    printf("Call model: %s <IP Address> <Port Number>\n", argv[0]);
    exit(0);
  }

  if ((server = socket(AF_INET, SOCK_STREAM, 0)) < 0){
     fprintf(stderr, "Cannot create socket\n");
     exit(1);
  }

  servAdd.sin_family = AF_INET;
  sscanf(argv[2], "%d", &portNumber);
  servAdd.sin_port = htons((uint16_t)portNumber);

  if(inet_pton(AF_INET, argv[1], &servAdd.sin_addr) < 0){
  fprintf(stderr, " inet_pton() has failed\n");
  exit(2);
  }

 if(connect(server, (struct sockaddr *) &servAdd, sizeof(servAdd))<0){
  fprintf(stderr, "connect() has failed, exiting\n");
  exit(3);
 }


  
  char output[10240];
  char command[1024];
  while(1)
  {
	
    printf("Enter Command or type 'quit' to exit\n");
	
	if(!fgets(command, 1024, stdin))
			break;
	size_t len = strlen(command);
	if (command[len - 1] == '\n')
			command[len - 1] = '\0'; 
		
	if(strcmp(command, "quit") == 0){
		close(server);
		exit(5);
	}
	
    write(server, command, sizeof(command));
	int x;
	read(server,&x,sizeof(int));
	
    int fd1 = open("cmdoutput.txt",O_RDONLY,0777);
	
	
	memset(output, 0, sizeof(output));
    int n1 = read(fd1,output,sizeof(output));
	printf("Output:\n");    
    
	write(1,output,n1);
	close(fd1);
	remove("cmdoutput.txt");
	//if (read(server, output, sizeof(output))<0){
    //  fprintf(stderr, "read() error\n");
    //  exit(3); 
    //}
    //printf("%s\n",output);
	
   }

  exit(0);
}
