#include <iostream>
#include <string>
#include <zmq.hpp>

int main( void )
{
    std::string submessage  = "Sebbe>subbed";
    try
    {
        zmq::context_t context(1);

        //Incoming messages come in here
        zmq::socket_t subscriber( context, ZMQ_SUB );
        zmq::socket_t pusher( context, ZMQ_PUSH );
//		subscriber.connect( "tcp://192.168.1.8:24042" );
//		subscriber.connect( "tcp://localhost:24042" );
        pusher.connect("tcp://benternet.pxl-ea-ict.be:24041");
        subscriber.connect( "tcp://benternet.pxl-ea-ict.be:24042" );
        subscriber.setsockopt( ZMQ_SUBSCRIBE, submessage.c_str(), submessage.length() );

        zmq::message_t * msg = new zmq::message_t();


        while( subscriber.connected() )
        {
            char buffer [50];
            int n;
            subscriber.recv( msg );

            std::string receivedmsg = (char*) msg->data();
            std::cout << "Subscribed : [" << std::string( (char*) msg->data(), msg->size() ) << "]" << std::endl;

            for(int i=0; i<3; i++)
            {

                n=sprintf (buffer, "Sebbe>pushed>%s", receivedmsg.substr(receivedmsg.length()-5,2).c_str());
                std::cout << "Responded : " << buffer << std::endl;
                pusher.send( buffer, n);
                std::cout << "Responded : " << buffer << std::endl;
            }
        }
    }
    catch( zmq::error_t & ex )
    {
        std::cerr << "Caught an exception : " << ex.what();
    }

    return 0;
}
