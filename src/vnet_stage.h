#ifndef vnet_stage_h_
#define vnet_stage_h_

#include "vnet_message.h"
#include "vnet_support.h"

namespace vnet {

// A stage is a pluggable network piece (filter, transport).
// In the sender side, the message travels following the Stage send method.
// In the receiving side, the message travels following the Stage received method.

// In the sender side, however, the message might be addressed to a group of nodes.
// It's after the "transport" stage that messages have a single recipient.
// (The original recipients are however given so a node can know that it's not the only recipient).

    class Stage {
    public:
        Stage () : upstream_ (NULL), downstream_ (NULL) { };
        
        virtual void send     (const Message &msg, const Envelope &meta) = 0;
        virtual void received (const Message &msg, const Envelope &meta, const NodeId &receiver) = 0;        
        
        //  These setters are thread-safe
        void set_upstream   (Stage &upstream)   { upstream_  .set (&upstream);   };
        void set_downstream (Stage &downstream) { downstream_.set (&downstream); };
        
        virtual ~Stage() {};

    protected:
        //  These accessors are thread-safe
        Stage *upstream()   const { return upstream_  .get (); };
        Stage *downstream() const { return downstream_.get (); };
    
    private:  
        atomic<Stage *> upstream_  ;
        atomic<Stage *> downstream_;
    };

    
    // A filter is intended to receive a message and selectively drop it if necessary
    class Filter : public Stage {
    public:
        virtual void send     (const Message &msg, const Envelope &meta);
        virtual void received (const Message &msg, const Envelope &meta, const NodeId &receiver);
    
    protected:
        virtual bool accept_send    (const Message &msg, const Envelope &meta) const = 0;
        // Override this with the actual test against the message when sending
        
        virtual bool accept_receive (const Message &msg, const Envelope &meta, const NodeId &receiver) const = 0;
        // Override this with the actual test against the message when receiving
    };

    // A transport is a special stage in the sense that it is at the end of a stage chain. 
    // It is intended to actually pass along over a message to a (possibly) remote endpoint.
    class Transport : public Stage {
    };

}

#endif