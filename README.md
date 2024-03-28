# file-download-server-c
A server program capable of serving to multiple clients tried to process and download files. The code is purely written in C using system calls.




1. serverw process which serves file processing and download request.
	-- Continuously listen_sckt to client request.
	-- Must be able to cater multiple clients.
	-- uses fork() and method CREQUEST() function to process the request.
	-- use only SOCKETS for the communication channel.	

	Actions/Features
	
	-- searches for the files starting from home directory '~'
	-- returns the file or files to the client as per the request



2. Multiple instances of the same server process to be run on different machines. The server names are: server, mirror1, mirror2


3. Client process continuously listen_sckt to user input,

	Action on commands
	-- verify the command syntax
	-- print message in case of command mismatch or any error


	and only the following commands are acceptable


>>dirlist -a 
the serverw24 must return the list of subdirectories/folders(only) under its
home directory in the alphabetical order and the client must print the same
	
		>>ex: clientw24$ dirlist -a

>>dirlist -t 
the serverw24 must return the list of subdirectories/folders(only) under its
home directories in the order in which they were created (with the oldest created directory listed first) and the client must print the same

		>>ex: clientw24$ dirlist -t


>>w24fn filename
If the file filename is found in its file directory tree rooted at ~, the serverw24 must return the filename, size(in bytes), date created and file permissions to the client and the client prints the received information on its terminal.


>>Note: if the file with the same name exists in multiple folders in the
directory tree rooted at ~, the serverw24 sends information pertaining
to the first successful search/match of filename

>>Else the client prints “File not found”
		>>Ex: client24$ w24fs sample.txt


>>w24fz size1 size2
The serverw24 must return to the client temp.tar.gz that contains all the files in the directory tree rooted at its ~ whose file-size in bytes is >=size1 and <=size2
	
		>>size1 < = size2 (size1>= 0 and size2>=0)
-- If none of the files of the specified size are present, the serverw24 sends “No file found” to the client (which is then printed on the client terminal by the client)

		>>client24$ w24fz 1240 12450


>> w24ft <extension list> //up to 3 different file types
the serverw24 must return temp.tar.gz that contains all the files in its directory tree rooted at ~ belonging to the file type/s listed in the extension list, else the serverw24 sends the message “No file found” to the client (which is printed on the client terminal by the client)

-- The extension list must have at least one file type and can have up to 3
different file types
			>> client24$ w24ft c txt
 			>> client24$ w24ft jpg bmp pdf



>>w24fdb date
The serverw24 must return to the client temp.tar.gz that contains all the files in the directory tree rooted at ~ whose date of creation is <=date

 			>> client24$ w24fdb 2023-01-01



>>w24fda date
The serverw24 must return to the client temp.tar.gz that contains all the files in the directory tree rooted at ~ whose date of creation is >=date
		
			>> client24$ w24fda 2023-03-31


>>quitc 
The command is transferred to the serverw24 and the client process is
terminated
