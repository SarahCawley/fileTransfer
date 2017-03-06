/* Sarah Cawley 3/12/2017, Intro to computer networks
 * Program 2 "Design and implement a simple file transfer system"
 * C server code inspired by http://www.linuxhowtos.org/C_C++/socket.htm
 * function to read in dir http://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program
 * function to concatinate strings from http://stackoverflow.com/questions/8465006/how-to-concatenate-2-strings-in-c
*/

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

/* Turns and int into a string
 * takes an int an an array to save the stringafied int into
 * returns the int as a string
*/
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
    char *result = malloc(strlen(s1)+strlen(s2)+1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

/* reads in option from client (-l to list files in dir, -g to transer file)
 * takes the socket number
 * returns 1 if -l and 2 if -g
*/
int readOptionFromClient(int sockfd){
    char option[3];
    int n;

    bzero(option, 3);
    n = read(sockfd,option, 2);
    if (n < 0) 
         error("ERROR reading from socket");

    if(strcmp(option, "-l")  == 0){
        return 1;
    }
    else if(strcmp(option, "-g" ) == 0){
        return 2;
    }
    else{
        printf("\n%s is not a valid option. Please try again\n", option);
        return 3;
    }
}

/* reads file name from socket
 * takes the socket number
 * returns the string of the filename
*/
char * readFileName(int sockfd, char * fileName){
    char buffer[50];
    int n;

    bzero(buffer,50);
    n = read(sockfd,buffer, 50);
    if (n < 0) 
         error("ERROR reading from socket");
    fileName = malloc(strlen(buffer) +1);
    strcpy(fileName, buffer);
    return fileName;
}

/* verifies file is in dir
 * takes file name
 * returns 1 if verified, 2 if un verified
*/
int verifyFileName(int sockfd, char * fileName){
    int verified = 0;
    int n;
    DIR           *d;
    struct dirent *dir;   
    d = opendir(".");
    if (d)
    {
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type == DT_REG){
                //check file name
                if(strcmp(fileName, dir->d_name) == 0){
                    printf("The file has a match\n");
                    n = write(sockfd,"1",1);
                    if (n < 0) error("ERROR writing to socket");
                    return 1;
                }
                
            }
        }
        closedir(d);
        printf("%s is not a valid file name\n", fileName);
        n = write(sockfd,"0",1);
        if (n < 0) error("ERROR writing to socket");
    }
    return 2;

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
        //get files
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
    int n, i, verified;
    char * stringToSend; //string to sent to client
    char * fileName; //filename from client
    int option = 0; //if -l or -g
    
    

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

    while(1){

        //listen for the new connection and connect
        listen(sockfd,5);
       
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) 
            error("ERROR on accept");
        printf("Client connected\n");

        n = write(newsockfd,"Server Connected\n",17);
        if (n < 0) error("ERROR writing to socket");
        
        //get option from client (-l or -g)
        option = readOptionFromClient(newsockfd);

        //if listing files
        if(option == 1){
            //Sends file names
            fileStr = createFileNameString(fileStr);
            stringLengthInt = strlen(fileStr);
            stringLengthStr = intToString(stringLengthInt, stringLengthStr);
            while( strlen(stringLengthStr) < 10){
                stringLengthStr = concat("0", stringLengthStr);
            }
            printf("Sending directory listing to client\n");
        
            //TODO MOVE WRITE INTO A FUNCTION
            //sends length of string to come
            write(newsockfd, stringLengthStr, 10);
            //send file names
            write(newsockfd, fileStr ,stringLengthInt);
        }

        //send file
        else if( option == 2){
            //read in file name
            fileName = readFileName(newsockfd, fileName);
            //verify file name 1 verfied, 2 no match
            verified = verifyFileName(newsockfd, fileName);
            //send file
        }
    }

    return 0; 
}