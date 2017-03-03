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

/*zero pad the length of the file buffer to be the correct size
* takes: fileLength, how many int characters in string
* zeroPadded, char * to hold zero padded length
* fileLength, how many int characters in string
* zeroPaddedLength, zero pad length
*/
// char * zeroPadLengthString(char * filelengthStr, char * zeroPaddedFileLengthStr, int fileLength ,  int zeroPaddedLength ){
    
//     char * zeroInt = "0";
//     int i;

//     if(fileLength < zeroPaddedLength){
//         zeroPaddedFileLengthStr = concat( zeroInt, filelengthStr);
//         for( i=0; i < zeroPaddedLength-1; i++){
//             zeroPaddedFileLengthStr = concat( zeroInt, zeroPaddedFileLengthStr);
//         }
//     }
//     return zeroPaddedFileLengthStr;
// }

/* concatinate strings together 
 * takes two string pointers, concats sencond to the end of the first
 * returns the pointer to the new string
*/
char* concat(char *s1,  char *s2){
    char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

/* write to socket 
* takes socket and string
*/
void writeToSocket(int newsockfd, char * stringToSend){
    //how many int characters in string
    int fileLength = strlen(stringToSend);
    int n;
    
    // how many chars to zero pad to
//    int zeroPaddedLength = 10;
    
    // string to convert int to char in to
    char filelengthStr[fileLength];

    // char * to hold zero padded length
 //   char* zeroPaddedFileLengthStr;
    //convert length into char *
    sprintf(filelengthStr, "%d", fileLength);
    // zero pad length
  //  zeroPaddedFileLengthStr = zeroPadLengthString(filelengthStr, zeroPaddedFileLengthStr, fileLength , zeroPaddedLength );

    //send message length
    n = write(newsockfd, filelengthStr , fileLength);
    if (n < 0) 
         error("ERROR writing to socket");

    //send message
    n = write(newsockfd, stringToSend ,fileLength);
    if (n < 0) 
         error("ERROR writing to socket");

}


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
    
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
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


    //listen for the new connection
    listen(sockfd,5);
   
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    
    if (newsockfd < 0) 
        error("ERROR on accept");
    printf("Client connected\n");


    n = write(newsockfd,"Server Connected",16);
    if (n < 0) error("ERROR writing to socket");

    //Send file names
    stringToSend = createFileNameString(stringToSend);
    writeToSocket(newsockfd, stringToSend);




    bzero(buffer,256);
    
    n = read(newsockfd,buffer,255);

    if (n < 0) error("ERROR reading from socket");
  



    printf("Here is the message: %s\n",buffer);
    
    close(newsockfd);
    close(sockfd);
    return 0; 
}