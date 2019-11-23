#include "server.h"

using namespace std;

server::server() //TODO - change comm port and endpoint from default
{
    
}

server::~server()
{
    this->kill();
}
void server::start_server()
{
    server_comm = new Communicator();
    
    bool server_send = false;
    
    while(!server_send)
    {
        if(server_comm->message_available())
        {
            cout << "Message available" << std::endl << "Receiving...." << std::endl
                << std::endl;
            recv_buff.push(server_comm->read_message());
        }
        
        //Send logic
        //sends if there is stuff pending in the send buffer
        
        else if(!send_buff.empty())
        {
            cout << "Messages pending to send !!" << std::endl << "Sending..."
                << endl << endl;
            while(!send_buff.empty())
            {
                server_comm->send_message(send_buff.front());
                send_buff.pop();
            }
                
            cout << "Send queue empty" << std::endl << std::endl;
       }     
    }
    
}

void server::kill()
{
    server_comm->kill();
}


