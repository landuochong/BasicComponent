#include "EventBus.hpp"
namespace basic_comm_eventbus {

template <class T>
void EventBus::AddHandler(EventHandler<T>* handler) {
     GetInstance()->AddHandler(typeid(T), static_cast<void*>(&handler));
}

template <class T>
void const EventBus::RemoveHandler(EventHandler<T>* handler) {
    GetInstance()->RemoveHandler(typeid(T), static_cast<void*>(&handler));
}

void EventBus::SendEvent(Event & e) {
    GetInstance()->SendEvent(typeid(e), e);
}


void EventBus::AddHandler(std::type_index type_id, void * const handler){
    std::unique_lock<std::mutex> locker(_mutex);
    Registrations* registrations = handlers[type_id];
    //创建列表
    if (registrations == nullptr) {
        registrations = new Registrations();
        handlers[type_id] = registrations;
    }

    EventHolder* registration = new EventHolder(handler);
    registrations->push_back(registration);
}

void EventBus::RemoveHandler(std::type_index type_id, void * const handler){
    std::unique_lock<std::mutex> locker(_mutex);
    Registrations* registrations = handlers[type_id];
    if (registrations == nullptr) {
        return;
    }
    for(EventHolder* item: *registrations){
        if(item->getHandler() == handler){
            registrations->remove(item);
            break;
        }
    }
}

void EventBus::SendEvent(std::type_index type_id, Event& e){
    std::unique_lock<std::mutex> locker(_mutex);
    Registrations* registrations = handlers[type_id];
    if (registrations == nullptr) {
        return;
    }

    for (auto & reg : *registrations) {
        static_cast<EventHandler<Event>*>(reg->getHandler())->dispatch(e);
    }
}

}
