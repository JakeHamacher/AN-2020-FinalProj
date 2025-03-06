# UDP File Transfer

This project implements a UDP-based file transfer system with a client-server architecture. The client sends a file to the server using UDP, and the server receives and saves the file while ensuring reliable data transfer.

## Features

- UDP-based file transfer
- Supports retransmission for reliability
- File integrity verification using sequence numbers
- Dynamic server address and port configuration

## Files

- `udpclient.cpp` - The client-side application that sends a file to the server.
- `udpserver.cpp` - The server-side application that receives and stores the file.

## How It Works

1. **Client Execution**
   - The client prompts the user to enter the server's IP address and port number.
   - It then asks for the filename to be sent.
   - The file is read in chunks of 1000 bytes and sent to the server.
   - The client waits for acknowledgment after sending each chunk to ensure correct sequencing.
   - Once the file transfer is complete, the client sends a "Quit!" message.

2. **Server Execution**
   - The server prompts the user to enter the listening port.
   - It waits for incoming file transfer requests.
   - Upon receiving the filename, it acknowledges receipt and starts writing the incoming data to a file.
   - The server ensures data integrity by checking the sequence number of each received packet.
   - Once the file transfer is complete, it closes the connection.

## Compilation and Execution

### Compile the programs
Use `g++` to compile both client and server:

```sh
g++ -o udpclient udpclient.cpp
g++ -o udpserver udpserver.cpp
