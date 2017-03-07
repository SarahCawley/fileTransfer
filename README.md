# fileTransfer
File transfer system, transfers .txt files. (file transfer server in C  and a file transfer client in Python)

Compile the server:
	1) run `make`

Start the server:
	1) run `./ftserver [command port number]`
	2) The terminal will display the server host name and port number

Start the client:
	1) To list files from server directory
		run `python ftclient.py [command port number] -l [data transfer port number]`
	2) To transfer a file
		run `python ftclient.py [command port number] -g [data transfer port number] [file name]`

To exit:
	1) The server will stay running and listening on the original port unless force closed by an admin (`CRTL + C`)
	2) The client will either list or get files each run through, then will close the data and command connections
	3) Make sure to use a different data port number for each transaction as a busy port will boot you out of the program

