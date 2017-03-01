/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
/* C server code inspired by http://www.linuxhowtos.org/C_C++/socket.htm
/* function to read in dir http://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h> 

void error(const char *msg)
{
    perror(msg);
    exit(1);
}


/* write to socket */

/*read from socket*/

/*list files in dir*/
void listFiles(){
    int numFiles;
    int n;
    DIR           *d;
    struct dirent *dir;
    d = opendir(".");
    if (d)
    {
    //get number of files
    while ((dir = readdir(d)) != NULL) {
        if (dir->d_type == DT_REG){
            //printf("%s\n", dir->d_name);
            numFiles++;
        }
    }
    //send number of files to client

    //send file names to client

    closedir(d);
    }
}

int main(int argc, char *argv[])
{
    
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    /*listFiles variables*/
    DIR           *d;
    struct dirent *dir;
    int numFiles =0;
    char numFilesStr[10];

    /*print hostname hostname*/
    char hostname[128];
    gethostname(hostname, sizeof hostname);
    printf("Server name: %s\n", hostname);

    //check port number provided
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    //create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));


    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
        sizeof(serv_addr)) < 0) 
        error("ERROR on binding");


    //listen for the new connection
    listen(sockfd,5);
   
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    
    if (newsockfd < 0) 
        error("ERROR on accept");
    printf("Client connected\n");


    


    n = write(newsockfd,"Server Connected",16);
    if (n < 0) error("ERROR writing to socket");

//listFiles();
/*listFiles*/
d = opendir(".");
if (d)
{
//get number of files
while ((dir = readdir(d)) != NULL) {
    if (dir->d_type == DT_REG){
        //printf("%s\n", dir->d_name);
        numFiles++;
    }
}
//send number of files to client
sprintf(numFilesStr, "%d", numFiles);
printf("There are %d numFiles and %s numFilesStr\n", numFiles, numFilesStr );
n = write(newsockfd, numFilesStr, 1);
if (n < 0) error("ERROR writing to socket");

//send file names to client

closedir(d);
}

/*end listFiles*/

    bzero(buffer,256);
    
    n = read(newsockfd,buffer,255);

    if (n < 0) error("ERROR reading from socket");
  



    printf("Here is the message: %s\n",buffer);
    
    close(newsockfd);
    close(sockfd);
    return 0; 
}