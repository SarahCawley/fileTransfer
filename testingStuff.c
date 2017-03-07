/* Sarah Cawley 3/12/2017, Intro to computer networks
 * Program 2 "Design and implement a simple file transfer system"
 * C server code inspired by http://www.linuxhowtos.org/C_C++/socket.htm
 * function to read in dir http://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program
 * function to concatinate strings from http://stackoverflow.com/questions/8465006/how-to-concatenate-2-strings-in-c
 * function to read the file and send in the socket http://stackoverflow.com/questions/2014033/send-and-receive-a-file-in-socket-programming-in-linux-with-c-c-gcc-g
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

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

/* creats a string from and integer of strlen. To be sent to size buffer correctly
 * takes a string to find length of
 * returns a stringafied int that is 3 digits long
*/
char * getLengthOfStringAsString(char * string){
    int stringLengthInt; //length of string in int
    char * stringLengthStr; //stringafiled int, for instance "253"
    stringLengthInt = strlen(string);
    stringLengthStr = intToString(stringLengthInt, stringLengthStr);
    //zero pads the string to be 3 digits long
    while( strlen(stringLengthStr) < 3){
        stringLengthStr = concat("0", stringLengthStr);
    }
    return stringLengthStr;
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
    int fp = 0; //file pointer to file


    fp = open(fileName, O_RDONLY);
    if (fp < 1) /*cant open file*/{
        printf("can't find file %s\n", fileName);
        write(sockfd, "0", 1);
        return 0;
    }
    else {
        printf("found and send file %s\n", fileName);
        write(sockfd, "1", 1);
        return fp;
    }
}

void sendFile(char * input_file, int transferSocket, int fp){
    char buffer[512];
    int bytes_read;
    int i =1; 
    char * bytes_read_str;
    while (1) {
        // Read data into buffer.  We may not have enough to fill up buffer, so we
        // store how many bytes were actually read in bytes_read.
        bytes_read = read( (int)fp, buffer, sizeof(buffer));
        if (bytes_read == 0) // We're done reading from the file
            break;

        if (bytes_read < 0) {
            // handle errors
        }

        // writeToSocket will return how many bytes were written. p keeps
        // track of where in the buffer we are, while we decrement bytes_read
        // to keep track of how many bytes are left to write.
        void *p = buffer;
        while (bytes_read > 0) {
            bytes_read_str =  intToString(bytes_read, bytes_read_str);
            while( strlen(bytes_read_str) < 3){
                bytes_read_str = concat("0", bytes_read_str);
            }
            write(transferSocket, bytes_read_str, 3);
            int bytes_written = write(transferSocket, p, bytes_read);

            if (bytes_written <= 0) {
                // handle errors
            }
            bytes_read -= bytes_written;
            p += bytes_written;
        }
    }
}

/* sends information to client
 * takes the socket int and the string to send
 * gets lenth of string
 * Sends on message to client with length of string
 * sends second message to client with string
*/
int writeToSocket(int sockfd, char * string){
    int stringLengthInt = 0;
    char * stringLengthStr;
    int n;

    //get int length of string
    stringLengthInt = strlen(string);
    //get str length of string
    stringLengthStr = getLengthOfStringAsString(string);
    //write length to socket
    n = write(sockfd, stringLengthStr, 3);
    if (n < 0) {
        printf("error writing to socket\n");
        return n;
    }
    //write string to socket
    n = write(sockfd, string , stringLengthInt );
    if (n < 0 ){
        printf("error writing to socket\n");
        return n;
    }
    return n;
}



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

int createSocket( char * hostname, char * message, int portno){

    int newsockfd, sockfd;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;


    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));


    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
        sizeof(serv_addr)) < 0) 
        error("ERROR on binding"); 
    printf("%s  %s %i \n", hostname, message, portno);
    listen(sockfd,5);
   
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) 
        error("ERROR on accept");
    printf("Client connected on port number %i\n", portno);

    return newsockfd;
}

int getDataPort(int sockfd){
    int dataSockFd, n;
    char * portNoStr = malloc(10);
    

    bzero(portNoStr, 10);

    n = read(sockfd, portNoStr, 10);
    if (n < 0) 
         error("ERROR reading from socket");

    dataSockFd = atoi(portNoStr);
    printf("the data port number string is %s, int is %i\n ", portNoStr, dataSockFd);

    return dataSockFd;
}

int main(int argc, char *argv[])
{
    char * fileStr = "\nDirectory Listing From Server:\n"; //holds string with all files
    int stringLengthInt = 0; //to store length of string as int
    char * stringLengthStr; //to store length of string as char *
    int sockfd, commandSockFd, dataSockFd, dataSockFdportno;
    //socklen_t clilen;
    char buffer[256];
    //struct sockaddr_in serv_addr, cli_addr;
    int n, i;
    char * stringToSend; //string to sent to client
    char * fileName; //filename from client
    int option = 0; //if -l or -g
    int fp;
    char * message;
    int portno;
    
    

    /*print hostname hostname*/
    char hostname[128];
    gethostname(hostname, sizeof hostname);
    

    //check port number provided
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    portno = atoi(argv[1]);
    commandSockFd = createSocket(hostname, " open on command port ", portno);
    

    n = write(commandSockFd,"Server Connected\n",17);
    if (n < 0) error("ERROR writing to socket");
   
    //get data port
    dataSockFdportno = getDataPort(commandSockFd);
    
    //create data connection
    dataSockFd = createSocket(hostname, " open on data port ", dataSockFdportno);
    //get option from client (-l or -g)
    option = readOptionFromClient(commandSockFd);


    //if listing files
    if(option == 1){
        //Sends file names
        fileStr = createFileNameString(fileStr);
        writeToSocket(dataSockFd, fileStr);
        printf("Directory listing sent to client\n");
    }

    //send file
    else if( option == 2){
        //read in file name
        printf("read file name\n"); 
        fileName = readFileName(commandSockFd, fileName);
        //verify file name, 0 if unable to open file, file descriptor if openable
        printf("verify name\n");
        fp = verifyFileName(commandSockFd, fileName);
        //send file
        printf("send file in main\n");
        sendFile(fileName, commandSockFd, fp);
    }
   // }

    return 0; 
}