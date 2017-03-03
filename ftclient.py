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
# numFiles = s.recv(10)
# print "there are " + numFiles + " files\n"
# numFiles = int(numFiles)
# print  "The files are:\n"
# x = 0
# while x < numFiles:
# 	print x
# 	print s.recv(1024)
# 	x += 1

# display list of files
while True:
	#get file name length
	fileLength = s.recv(2)
	#rint "file name is " + fileLength + " long\n"
	fileLength = int(fileLength)

	#check if filename is less than 10 char long
	if fileLength > 90:
		fileLength -= 90

	fileLength += 2
	fileName = s.recv(fileLength)
	fileName = fileName.rstrip()
	print fileName
	# while fileLength > 0:
	# 	print fileLength 
	# 	print fileName
	# 	fileLength -= 1
	#fileName = s.recv(fileLength)
	# if fileName == "no":
	# 	quit()
	#print s.recv(fileLength)
	#print s.recv(fileLength)


# if is getting files


s.close                     # Close the socket when done