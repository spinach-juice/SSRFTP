#include "server.h"

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
    
    bool server_end = false;
    
    while(!server_send)
    {
        if(server_comm.message_available())
        {
            cout << "Message available" << endl << "Receiving...." << endl
                << endl;
            recv_buff.push(server_comm.read_message());
        }
        
        //Send logic
        //sends if there is stuff pending in the send buffer
        
        else if(!send_buff.empty())
        {
            cout << "Messages pending to send !!" << endl << "Sending..."
                << endl << endl;
            while(!send_buff.empty())
                server_comm.send_message(send_buff.pop())
                
            cout << "Send queue empty" << endl << endl;
       }     
    }
    
}

void server::kill()
{
    server_comm.kill();
}


