/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
/* C server code inspired by http://www.linuxhowtos.org/C_C++/socket.htm
/* function to read in dir http://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program*/
/* function to concatinate strings from http://stackoverflow.com/questions/8465006/how-to-concatenate-2-strings-in-c*/

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

char * intToString(int n, char * string){
    char *result = malloc(n);
    sprintf(result, "%d", n);
    return result;
}

/* concatinate strings together 
 * takes two string pointers, concats sencond to the end of the first
 * returns the pointer to the new string
*/
char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

/* write to socket */

/*read from socket*/


/*Creates string with file names from the directory
* takes a string pointer, reads each file from dir, concats to string
* retuns a string with formatted file names
*/
char * createFileNameString(char * fileStr){
    DIR           *d;
    struct dirent *dir;
    char * newLine = "\n";
   
    d = opendir(".");
    if (d)
    {
        //get number of files
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type == DT_REG){
                //concat strings together
                fileStr = concat(fileStr, dir->d_name);
                fileStr = concat(fileStr, newLine);
            }
        }
        closedir(d);
    }
    return fileStr;
}


int main(int argc, char *argv[])
{
    char * fileStr = "\nDirectory Listing From Server:\n"; //holds string with all files
    int stringLengthInt = 0; //to store length of string as int
    char * stringLengthStr; //to store length of string as char *
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n, i;
    char * stringToSend;
    
    

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


    //listen for the new connection and connect
    listen(sockfd,5);
   
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) 
        error("ERROR on accept");
    printf("Client connected\n");

    n = write(newsockfd,"Server Connected\n",17);
    if (n < 0) error("ERROR writing to socket");




    //Sends file names
    fileStr = createFileNameString(fileStr);
    stringLengthInt = strlen(fileStr);
    stringLengthStr = intToString(stringLengthInt, stringLengthStr);
    while( strlen(stringLengthStr) < 10){
        stringLengthStr = concat("0", stringLengthStr);
    }
    //sends length of string to come
    write(newsockfd, stringLengthStr, 10);
    //send file names
    write(newsockfd, fileStr ,stringLengthInt);

    return 0; 
}