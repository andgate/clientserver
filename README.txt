-----------------------------------------------------------------
-- Building
To buil

-----------------------------------------------------------------
-- Running a server

Usage: server [rows=10] [columns=10] [port=8080]

Rows and columns specify the size of the ticket table.
The rows and columns must be given together.
To set the port, the rows and columns must be given first.

-----------------------------------------------------------------
-- Client configuration

Usage: client [mode=manual]

The default mode is manual.
The client may also be set to automatic, in which case the client
will reserve tickets until there are no more tickets to reserve,
in which case it will exit.

Client's must be run in a directory containing a config.ini file.
The config file must contain the following fields:

IP=<ip address>
Port=<port number>
Timeout=<timeout>

Spaces are allowed and order does not matter. 
Each variable must be specified on a seperate line.
If two variables are set twice, then the second variable will be used.
Timeout specifies the number of connection attempts the client will make.