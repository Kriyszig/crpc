# cRPC

Is this an RPC program? Probably  

Similar to AWS Lambda, this server-client couple allows you to remotely execute and intercept the output of a C++ program over a local network.

### How to run the project?

```bash
git clone https://github.com/Kriyszig/crpc.git
cd crpc

# For running the server
cd server
g++ -o server server.cc 
#Don't use a.out as it's the same name for the executable generated during RPC
./server

# For running the client
cd client
g++ -o client client.cc
./client
```

Note:  
* Remember to store the programs to transfer in `/client/programs/` else the transfer won't work.