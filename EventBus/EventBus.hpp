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
    template <class T, typename std::enable_if<std::is_base_of<Event, T>::value, T>::type* = nullptr>
    static void AddHandler(EventHandler<T>* handler) {
         GetInstance()->AddHandler(typeid(T), static_cast<void*>(handler));
    }

    template <class T, typename std::enable_if<std::is_base_of<Event, T>::value, T>::type* = nullptr>
    static void const RemoveHandler(EventHandler<T>* handler) {
        GetInstance()->RemoveHandler(typeid(T), reinterpret_cast<void*>(handler));
    }

    static void SendEvent(Event & e){
        GetInstance()->SendEvent(typeid(e), e);
    }

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
#endif /* EVENT_BUS_HPP_ */
