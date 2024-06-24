#ifndef EVENT_BUS_HPP_
#define EVENT_BUS_HPP_
#include "EventHandler.hpp"
#include "Event.hpp"
#include <mutex>
#include <list>
#include <typeinfo>
#include <unordered_map>
namespace basic_comm_eventbus {

class EventBus{
public:
	/**
     * 注册handler
	 * @param handler The event handler class
	 */
	template <class T>
    static void AddHandler(EventHandler<T>* handler);

	template <class T>
    static void const RemoveHandler(EventHandler<T>* handler);

	static void SendEvent(Event & e);

public:
    virtual ~EventBus() { }

private:
    EventBus() {}

	static EventBus* const GetInstance() {
        static EventBus instance;
        return &instance;
	}

    void AddHandler(std::type_index, void * const handler);
    void RemoveHandler(std::type_index, void * const handler);
    void SendEvent(std::type_index, Event & e);

private:
    class EventHolder
    {
    public:
        EventHolder(void * const handler) :
            handler(handler)
        { }

        virtual ~EventHolder() { }

        void * const getHandler() {
            return handler;
        }

    private:
        void * const handler;
    };

    typedef std::list<EventHolder*> Registrations;
    typedef std::unordered_map<std::type_index, std::list<EventHolder*>*> TypeMap;

    TypeMap handlers;

    std::mutex _mutex;
};

}
#endif /* _SRC_EVENT_EVENT_BUS_HPP_ */
