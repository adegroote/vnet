#include "vnet_core.h"

vnet::Network::Network (Transport &downstream) : Stage (), centralized_ (true) 
{ 
  set_downstream (downstream); 
  downstream.set_upstream (*this);
};

vnet::LocalClientConnectionRef vnet::Network::open(const NodeId &id, const Channel &channel)
{
  boost::upgrade_lock<boost::shared_mutex> ro_lock (clients_mutex_);
  
  const IdConnectionMap::iterator conn = clients_.find (IdChannel (id, channel));
  
  if (conn != clients_.end()) {
    return conn->second;
    //  Here we are returning the same handle even if it's a re-open.
    //  This means that two clients with same id could send via the same channel,
    //  but would steal packets on reception from each other non-deterministically.
    //  TODO: Fail here or remove the unique [id,channel] restriction.
    //  TODO: Implement connection closing (and timeout?).
  }
  else {
    boost::upgrade_to_unique_lock<boost::shared_mutex> rw_lock (ro_lock);
    
    // Add the connection to the list.
    LocalClientConnectionRef new_conn (new LocalClientConnection (id, *this));
    clients_.insert (IdConnectionElem (IdChannel (id, channel), new_conn));
    
    // Add the client id to the channel list.
    const ChannelIdsMap::iterator clients_in_channel = channel_clients_.find (channel);
    if (clients_in_channel != channel_clients_.end ())
        clients_in_channel->second.insert (id);
    else {
        NodeGroup new_group;
        new_group.insert (id);
        channel_clients_.insert (ChannelIdsElem (channel, new_group));
    }
    
    return new_conn;
  }
}

void vnet::Network::send (const Message &msg, const Envelope &meta)
{
    if (centralized_ && meta.receiver ().kind () == Destination::broadcast) {
        NodeGroup new_clients;
        {
            //  We pass along the current clients in the given channel to be able to perform local broadcast at the transport end
            boost::upgrade_lock<boost::shared_mutex> ro_lock (clients_mutex_);
            //  Read lock since a copy of the receivers is going to be made.
            //  The following look-up cannot fail; at least the sender is in the channel:
            new_clients = channel_clients_.find (meta.receiver ().channel ())->second;
        }
        
        //  The actual send to downstream is thread-safe since we have a copy of receivers now.
        downstream ()->send (msg,
                             Envelope (meta.sender (), 
                                       Destination (new_clients, meta.receiver ().channel ())));
    }
    else // Just pass along
        downstream ()->send (msg, meta);
}

void vnet::Network::received(const vnet::Message& msg, const vnet::Envelope& meta, const NodeId &receiver)
{
    LocalClientConnectionRef client;
    
    //  Thread-safely get a reference to the receiving client
    {
        boost::lock_guard<boost::shared_mutex> ro_lock (clients_mutex_);    
        const IdConnectionMap::iterator conn = clients_.find (IdChannel (receiver, meta.receiver ().channel ()));
        if (conn != clients_.end ())
            client = conn->second;
    }
    
    if (client != NULL)
        client->received(msg, meta, receiver);
    //  Otherwise we should log failure I guess.
}
