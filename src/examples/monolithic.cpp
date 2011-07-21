#include <boost/random.hpp>
#include <boost/thread.hpp>
#include <exception>
#include <iostream>

#include "../vnet.h"
#include "../transports/vnet_localtransport.h"

using namespace vnet;

//  This basic example creates the vnet and the clients in a single process.
//  Two example clients are created as new threads.
//  Local delivery and client connections are shown.

LocalTransport local_transport;
Network        net (local_transport);

boost::array<NodeId, 2> sender_ids   = {{"Ari", "Ben"}};
const NodeId receiver_id = "Zak";

const Channel channel = "monotest";

void sender (int index) {
  
  try {
      
    const NodeId sender_id = sender_ids.at (index);
  
    std::cout << "Sender " << sender_id << " starting..." << std::endl;
    
    LocalClientConnectionRef conn = net.open (sender_id, channel);
    
    boost::mt19937 gen (index);
    boost::uniform_int<> dist (350, 750);
    boost::variate_generator<boost::mt19937 &, boost::uniform_int<> > rnd (gen, dist);
    
    while (true) {
        const int pause = rnd();
        
        std::cout << "I (" << sender_id << ") shall sleep for " << pause << " ms." << std::endl;
        
        boost::this_thread::sleep
          (boost::posix_time::milliseconds
            (pause));
        
        conn->send (receiver_id, channel, Message::build<int>(pause));
    }
    
  } 
  catch (std::exception &e) {
    std::cout << "Exception [sender]: " << e.what () << std::endl;
  }
  catch (...) {
    std::cout << "Unknown exception." << std::endl;
  }
}

void receiver () {
    try {
        
        std::cout << "Receiver starting..." << std::endl;
        
        LocalClientConnectionRef conn = net.open (receiver_id, channel);
        
        while (true) {            
            const ParcelRef parcel = conn->receive ();
            std::cout << "Sender "  
                << parcel->envelope ().sender ()
                << " slept " 
                << parcel->message().get<int>()
                << " milliseconds" << std::endl;
        }
        
    } 
    catch (std::exception &e) {
        std::cout << "Exception [receiver]: " << e.what () << std::endl;
    }
    catch (...) {
        std::cout << "Unknown exception." << std::endl;
    }
}

int main(int argc, char **argv) {
  
    std::cout << "vNet started, accepting connections..." << std::endl;
    
    boost::thread sender1   (sender, 0);
    boost::thread sender2   (sender, 1);
    boost::thread receiver1 (receiver);
    
    sender1.join();
    sender2.join();
    receiver1.join();
    
    return 0;
}