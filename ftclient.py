#!/usr/bin/python           

# Sarah Cawley 3/12/2017, Intro to computer networks
# Program 2 "Design and implement a simple file transfer system"
# python client architecture borrowed from https://www.tutorialspoint.com/python/python_networking.htm


import socket               # Import socket module
import sys					# Import system to read from command line
import os.path				# checks files in the path
import time

#to handle error messages
def alert(msg):
    print >>sys.stderr, msg
    sys.exit(1)

#function to read from socket
# returns the string read from the server
def readFromServer(sockfd):
	more = 0 # there is no more, 1 more to print
	#get length of file
	fileLength = sockfd.recv(3)
	#print "file length " + fileLength
	fileLength = int(fileLength)


	#see if there is more coming, max buffer size is 512
	if fileLength > 511:
		more = 1
	# exit if there is nothing left to read
	if fileLength < 1:
		return ("Nothing to print", more)

	#read from server
	stringFromServer = sockfd.recv(fileLength)

	return (stringFromServer, more)

def sendOptionToServer(option, sockfd):
	sockfd.send(option)
	return

def writeToFile(fileName, sockfd):
	f = open(path,"a+")
	i = 1

	while True:
		fileContents, more = readFromServer(sockfd)
		#see if there is anything to write
		if fileContents == "Nothing to print":
			return

		else:
			f.write(fileContents)

			i += 1
			if more == 0:
				print "File transfer complete"
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
		print path + " is already in the directory. Naming file copy-" + fileName
		path = "./copy-" + fileName
		# check if ./copy-fileName already exists if so remove it
		if os.path.isfile(path):
			os.remove(path)
		return path
	else:
		return path

def createDataSocket(dataPort):
	#create data socket
	data  = socket.socket()
	dataPort = int(dataPort)
	data.connect((server, dataPort))



# check hostname and portnumber from command line
if( len(sys.argv) < 5):
	print "usage: python ftclient.py [server name] [command port number] [-l (list files)] [data transfer port]  OR"
	print "usage: python ftclient.py [server name] [command port number] [-g (transfer files)] [file name] [data transfer port]" 
	quit()

# set server and port
server = sys.argv[1]
port = int(sys.argv[2])

print "connect to command server"
command = socket.socket() 
command.connect((server, port))
print command.recv(26)



#if is listing files
if (sys.argv[3] == '-l'):
	if( len(sys.argv) < 5):
		print "usage: python ftclient.py [server name] [command port number] [-l (list files)] [data transfer port]"
		quit()
	sendOptionToServer('-l', command)
	
	dataPort = sys.argv[4]
	#send data port number
	command.send(dataPort)
	# create data connection
	data  = socket.socket()
	dataPort = int(dataPort)
	# added a delay because sometimes it would try to connect before the server was ready
	time.sleep(.25) 
	data.connect((server, dataPort))

	
	print "connected to datport reading from server"
	files, n = readFromServer(data)
	print files
	data.close

# if is getting files
elif (sys.argv[3] == '-g'):
	if( len(sys.argv) < 6):
		print "usage: python ftclient.py [server name] [command port number] [-g (transfer files)] [data transfer port] [file name] "
		quit()	
	 
	sendOptionToServer('-g', command)
	# get file name
	fileName = sys.argv[5] 
	command.send(fileName)
	# get verification of valid name
	valid = command.recv(1)
	valid = int(valid)

	if valid == 1:

		dataPort = sys.argv[4]
		#send data port number
		command.send(dataPort)
		# create data connection
		data  = socket.socket()
		dataPort = int(dataPort)
		time.sleep(.25)
		data.connect((server, dataPort))


		path = isFileOnClient(fileName)
		writeToFile(fileName, data)
		data.close
		
	else:
		print fileName + " is not a valid file name. Please try again"

# wrong option
else:
	print argv[3] + " is not a valid option"

#close command socket
command.close                     