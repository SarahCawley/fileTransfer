#!/usr/bin/python           # This is client.py file

# python client architecture borrowed from https://www.tutorialspoint.com/python/python_networking.htm


import socket               # Import socket module
import sys					# Import system to read from command line

s = socket.socket()         # Create a socket object

#function to read from socket
def readFromServer(s):
	fileLength = s.recv(10)
	print "the file length is: " + fileLength
	fileLength = int(fileLength)
	stringFromServer = s.recv(fileLength)
	print stringFromServer
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
	readFromServer(s)
if (sys.argv[3] == '-g'):
	print "here is where getting the file will go"


# if is getting files


s.close                     # Close the socket when done