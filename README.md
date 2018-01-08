# Server-Client Sorter
Multithreaded C Server and C Client program to sort a list of records of movies from imdb alphabetically by the data in a given column

## Overview
* Designed a search request/response protocol for client and server
* Implemented a multi-threaded server to respond to client requests using the protocol
* Implemented a client program that will send requests to the server using the protocol

## Sending to Server
* Data to sort
* Column to sort the data on 
* Request for the fully sorted list

## Methodology
* Read in a set of parameters via the command line
* Records will be stored in CSV files in the provided directory
* Directories may have multiple levels and the program will find all CSV files

## Usage
### Client
* `-c` to indiate sorting by a column. The files will be sorted by the column name that immediately follows '-c'. (Required)
* `-h` to indicate the hostname of the server (Required)
* `-p` to indicate the port number the server is listening on (Required)
* `-d` to indicate the directory the program should search for .csv files (Optional)
* `-o` to indicate the output where the sorted CSV should be written to (Optional)

### Server
* `-p` to indicate the port number the server is listening on.  This should match the `-p` in the client (Required)

## Operation
### Server
* The server will open a port and wait for connection requests from multiple clients
* On connection, it will spawn a service thread to handle that connection and go back to waiting for requests
* Each service thread will read in a client request, if it is a sort request, it will perform the sort and store the results at the server 
* If it is a dump request, it will merge the current collection of sorted results into one sorted list and send the result back to the client.
