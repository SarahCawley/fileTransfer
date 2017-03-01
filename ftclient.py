#!/usr/bin/python           # This is client.py file

# python client architecture borrowed from https://www.tutorialspoint.com/python/python_networking.htm


import socket               # Import socket module
import sys					# Import system to read from command line

s = socket.socket()         # Create a socket object

#function to list files


#function to get files



# check hostname and portnumber from command line
if( len(sys.argv) < 4):
	print "usage: python ftclient.py [server name] [port number] [-l (list files) -g (get files)\n"
	quit()

# set server and port
server = sys.argv[1]
port = int(sys.argv[2])

# connect to server
s.connect((server, port))
print s.recv(16)
#if is listing files

# get number of files
numFiles = s.recv(10)
numFiles = int(numFiles)
print  numFiles

# display list of files


# if is getting files


s.close                     # Close the socket when done