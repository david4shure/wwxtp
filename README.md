wwxtp-server
============

Weather Server written in C

Here are the steps for using the server side socket API in C

1. socket() "Socket() creates a socket that returns an integer which is the file descriptor of the socket"

2. bind() "Bind() binds the socket to a specific port and returns an integer describing the error state of the command"

3. listen() "listen() makes the server listen at a specified port, until a client sends a request to connect"

4. accept() "accept() is after the client has connected to the server, and that signifies the two computers are ready communicate"

5. recv() "recv() is fairly self-explainatory, and receives data from the client"

6. send() "send() is also self-explainatory and sends data to a client, and they both return a -1 if an error arises"
...

7. close() "close() closes the connection, usually listen is called after close()"

We are calling each socket API function with the preprocessor macro check() which takes in a function and parameters and applies the function to the supplied parameters. It automatically checks for errors and if errors arise, it prints to stderr.