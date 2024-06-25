#ifndef EVENT_EVENT_HPP_
#define EVENT_EVENT_HPP_

#include <typeindex>
#include <typeinfo>
#include <vector>
#include <stdexcept>

namespace basic_comm_eventbus {
class Event
{
public:
    Event() { }

	virtual ~Event() { }

    Event (const Event& other) { }
};
}
#endif /* EVENT_EVENT_HPP_ */
