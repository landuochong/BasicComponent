#ifndef EVENT_HANDLER_HPP_
#define EVENT_HANDLER_HPP_
#include <typeinfo>
#include <type_traits>
#include <typeindex>

namespace basic_comm_eventbus {
class Event;

template <class T, typename std::enable_if<std::is_base_of<Event, T>::value, T>::type* = nullptr>
class EventHandler {
public:
	EventHandler() {
		static_assert(std::is_base_of<Event, T>::value, "EventHandler<T>: T must be a class derived from Event");
	}

	virtual ~EventHandler() { }

	virtual void onEvent(T &) = 0;

    //事件分发
	void dispatch(Event & e) {
        onEvent(dynamic_cast<T &>(e));
	}
};
}
#endif /* EVENT_HANDLER_HPP_ */
