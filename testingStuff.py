#!/usr/bin/python           # This is client.py file

# python client architecture borrowed from https://www.tutorialspoint.com/python/python_networking.htm


import socket               # Import socket module
import sys					# Import system to read from command line

s = socket.socket()         # Create a socket object

#function to read from socket
# if option is 1 print to screen
# if option is 2 save to file
def readFromServer(s, option):
	fileLength = s.recv(10)
	#print "the file length is: " + fileLength
	fileLength = int(fileLength)
	stringFromServer = s.recv(fileLength)
	print stringFromServer
	return

def sendOptionToServer(option):
	s.send(option)
	return


# check hostname and portnumber from command line
if( len(sys.argv) < 4):
	print "usage: python ftclient.py [server name] [port number] [-l (list files) -g (get files)\n"
	quit()

# set server and port
server = sys.argv[1]
port = int(sys.argv[2])

# connect to server
s.connect((server, port))
print s.recv(17)

#if is listing files
if (sys.argv[3] == '-l'):
	sendOptionToServer('-l')
	readFromServer(s, 1)

# if is getting files
elif (sys.argv[3] == '-g'):
	sendOptionToServer('-g', 2)


# wrong option
else:
	print argv[3] + " is not a valid option"




s.close                     # Close the socket when done