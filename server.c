#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <wait.h>
#include <fcntl.h>

static int client_count=0;
int executeCommand(char *command){
	char *args[20] = {}; 
    args[0] = strtok(command, " ");
    int counter = 0;
    
	while (args[counter] != NULL){
    	counter++;
    	args[counter] = strtok(NULL, " ");
    }
	
	char output[1024];
	
	int fd = open("cmdoutput.txt",O_CREAT | O_RDWR | O_APPEND,0777);
	if(fd==-1){
		perror("File Problem");
		exit(1);
	}
	//temp = dup(1);
	//dup2(client, 1 );
	freopen("cmdoutput.txt","a",stdout);
	/*if ((execvp(args[0], args)) < 0){
			dup2(temp, 1);
    		printf("-%s: Command not recognized.\n", args[0]);
			exit(1);
	}
	dup2(temp, 1);
	printf("Command Executed/n");
	close(temp);*/
	int child = fork();

    if (child == 0){
    	if ((execvp(args[0], args)) < 0){
    		printf("-%s: Command not recognized.\n", args[0]);
			exit(1);
		}
    	exit(0);
	}
	else if (child > 0){
		int status;  
		waitpid(child, &status, 0);
		int exitStatus;
		if(WIFEXITED(status)){
			exitStatus = WEXITSTATUS(status);
			//printf("Child %d exited with status %d\n", child, exitStatus);
			return exitStatus;
		}
		else{
			close(fd);
			return 1;
		}
		//dup2(temp,1);
		//int x = 1;
		//write(client,&x,sizeof(int));
		//fflush(stdout);
		//printf("Command Executed\n");
		//close(temp);
	}
}
void serviceClient(int client, int client_number)
{
	
	char command[1024];
	int temp;
	int l;
  while(1)
  {
    
    if(!read(client, command, 1024))
    {
      //printf("Client %d has left\n",client_number);
      close(client);
      exit(5);
    }
	printf("Command: %s\n", command);
	//printf("%s\n", command);
    
	char *commands[10];
  		char *token = strtok(command, ";" );
  		int i = 0;
  		while (token != NULL){
    			commands[i] = token;
    			i++;
    			token = strtok(NULL, ";" );
  		}
  		int numOfCommands = i; 

  		i = 0;
  
		while (i < numOfCommands){
			char *a_token = strtok(commands[i],"&&");
			int count = 0;
			char *t_command[10];
			int status;
			while(a_token != NULL){
				t_command[count] = a_token;
				count++;
				a_token = strtok(NULL, "&&");
			}
			if(count > 1){
				status = executeCommand(t_command[0]);
				if(status == 0){
					executeCommand(t_command[1]);
				}
			}
			else{
				char *o_token = strtok(commands[i],"||");
				count = 0;
				while(o_token != NULL){
					t_command[count] = o_token;
					count++;
					o_token = strtok(NULL, "||");
				}
				if(count > 1){
					status = executeCommand(t_command[0]);
					if(status == 1){
						executeCommand(t_command[1]);
					}
				}
				else{
					executeCommand(commands[i]);
				}
			}
    		i++;
  		}
		int x = 1;
		write(client,&x,sizeof(int));
  }
    exit(1); 
}

int main(int argc, char *argv[]){  

  srand(time(NULL));
  pid_t p;
  int n;
  int sd, client, portNumber;
  socklen_t len;
  char *currentTime;
  time_t currentUnixTime;

  struct sockaddr_in servAdd;     // server socket address
  
 if(argc != 2){
    printf("Call model: %s <Port Number>\n", argv[0]);
    exit(0);
  }

  sd = socket(AF_INET, SOCK_STREAM, 0);
  servAdd.sin_family = AF_INET;
  servAdd.sin_addr.s_addr = htonl(INADDR_ANY);
  sscanf(argv[1], "%d", &portNumber);
  servAdd.sin_port = htons((uint16_t)portNumber);
  
  bind(sd, (struct sockaddr *) &servAdd, sizeof(servAdd));
  listen(sd, 5);

  while(1){
    client = accept(sd, NULL, NULL);
    client_count++;
    printf("Client %d joined\n",client_count);
    if(!fork())
    {
      serviceClient(client, client_count);
    }    
  }
    close(client);  
}
