// Server side C/C++ program to demonstrate Socket programming
#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define PORT 8080

namespace cRPC
{
    class Server
    {
    public:
        void saveFile(std::string name, std::string prog)
        {
            std::ofstream fs("./programs/" + name);

            if(!fs)
            {
                std::cout<<"Cannot open the output file."<<std::endl;
                return;
            }

            fs<<prog;
            fs.close();
        }

        int run()
        {
            int server_fd, new_socket, valread;
            struct sockaddr_in address;
            int opt = 1;
            int addrlen = sizeof(address);
            const char *hello = "Hello from server";

            if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
            {
                std::cout<<"Failed to create socket descriptor\n";
                return -1;
            }

            if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                        &opt, sizeof(opt)))
            {
                std::cout<<"Failed to set socket descriptor\n";
                return -1;
            }

            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = htons( PORT );

            // Forcefully attaching socket to the port 8080
            if (bind(server_fd, (struct sockaddr *)&address,
                                        sizeof(address))<0)
            {
                std::cout<<"Failed to bind socket descriptor\n";
                return -1;
            }

            if (listen(server_fd, 3) < 0)
            {
                std::cout<<"Failed to listen on port "<<PORT<<"\n";
                return -1;
            }

            if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                            (socklen_t*)&addrlen))<0)
            {
                std::cout<<"Failed to accept connection\n";
                return -1;
            }

            while(true)
            {
                char buffer[1024] = {0};
                valread = read( new_socket , buffer, 1024);
                std::string buf(buffer);

                if(buf[0] == '/' && buf[1] == '/' && buf[2] == ' ')
                {
                    std::string fname = "";
                    int cnt = 3;

                    while(buf[cnt] != '\n')
                    {
                        char chstream = buf[cnt];
                        fname += chstream;
                        ++cnt;
                    }

                    saveFile(fname, buf);
                    const char* successmsg = "File Recieved";
                    send(new_socket , successmsg , strlen(successmsg) , 0 );
                }
                else if(buf[0] == '$')
                {
                    std::string fname = buf.substr(1);
                    // Handle STOP directive for the server
                    if(fname == "STOP")
                    {
                        const char* sendmsg = "Server shutdown";
                        send(new_socket , sendmsg, strlen(sendmsg) , 0 );
                        break;
                    }
                    
                    fname = fname.substr(0, fname.find('.')) + ".cc";
                    std::string compile = "g++ ./programs/" + fname + ">send.txt 2>send.txt";
                    std::system("if [ ! -f ./send.txt ]; then\n	touch ./send.txt\nelse\n    rm ./send.txt\n	touch ./send.txt\nfi");
                    int exit_code = std::system(compile.c_str());

                    if(exit_code != 0)
                    {
                        std::ifstream ifs("send.txt");
                        std::string errormsg;
                        getline(ifs, errormsg, (char)ifs.eof());

                        const char* sendmsg = errormsg.c_str();
                        send(new_socket , sendmsg, strlen(sendmsg) , 0 );
                    }
                    else
                    {
                        std::system("./a.out>send.txt 2>send.txt");
                        std::ifstream ifs("send.txt");
                        std::string outmsg;
                        getline(ifs, outmsg, (char)ifs.eof());

                        const char* sendmsg = outmsg.c_str();
                        send(new_socket , sendmsg, strlen(sendmsg) , 0 );
                    }
                }
            }

            close(new_socket);
            close(server_fd);
            return 0;
        }
    };
}

int main(int argc, char const *argv[])
{
	cRPC::Server sr;
    sr.run();
    return 0;
}
