#!/usr/bin/python           

# Sarah Cawley 3/12/2017, Intro to computer networks
# Program 2 "Design and implement a simple file transfer system"
# python client architecture borrowed from https://www.tutorialspoint.com/python/python_networking.htm


import socket               # Import socket module
import sys					# Import system to read from command line
import os.path				# checks files in the path

s = socket.socket()         # Create a socket object

#function to read from socket
# returns the string read from the server
def readFromServer(s):
	more = 0 # there is no more, 1 more to print
	#get length of file
	fileLength = s.recv(3)
	fileLength = int(fileLength)

	#see if there is more coming, max buffer size is 512
	if fileLength > 511:
		more = 1
	# exit if there is nothing left to read
	if fileLength < 1:
		return ("Nothing to print", more)

	#read from server
	stringFromServer = s.recv(fileLength)

	return (stringFromServer, more)

def sendOptionToServer(option):
	s.send(option)
	return

def writeToFile(fileName, s):
	f = open(path,"a+")

	while True:
		fileContents, more = readFromServer(s)
		#see if there is anything to write
		if fileContents == "Nothing to print":
			return

		else:
			f.write(fileContents)
			print "writing to file more is "
			print more
			if more == 0:
				f.close 
				return

# Checks if a file with a name already exists
# if so informs user and changes new file to 'copy-fileName'
# returns the path to the new file
def isFileOnClient(fileName):
	# check if file exists in local dir
	path = "./" + fileName
	#check if file already exists on client
	if os.path.isfile(path):
		print path + "is already in the directory. Naming file copy-" + fileName
		path = "./copy-" + fileName
		# check if ./copy-fileName already exists if so remove it
		if os.path.isfile(path):
			os.remove(path)
		return path
	else:
		return path



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
	files = readFromServer(s)
	print files

# if is getting files
elif (sys.argv[3] == '-g'):
	# get file name from user
	sendOptionToServer('-g')
	fileName = raw_input("Please enter the name of the file you wish to download ")
	s.send(fileName)
	# get verification of valid name
	valid = s.recv(1)
	valid = int(valid)

	if valid == 1:
		path = isFileOnClient(fileName)
		writeToFile(fileName, s)
		
	else:
		print fileName + " is not a valid file name. Please try again"

# wrong option
else:
	print argv[3] + " is not a valid option"




s.close                     # Close the socket when done