#include "EventBus.hpp"
#include<QDebug>
namespace basic_comm_eventbus {

void EventBus::AddHandler(std::type_index type_id, void * const handler){
    std::unique_lock<std::mutex> locker(_mutex);
    Registrations* holders = handlers[type_id];
    //创建列表
    if (holders == nullptr) {
        holders = new Registrations();
        handlers[type_id] = holders;
    }

    EventHolder* holder = new EventHolder(handler);
    holders->push_back(holder);
}

void EventBus::RemoveHandler(std::type_index type_id, void * const handler){
    std::unique_lock<std::mutex> locker(_mutex);
    Registrations* holders = handlers[type_id];
    if (holders == nullptr) {
        return;
    }
    for(EventHolder* item: *holders){
        if(item->getHandler() == handler){
            holders->remove(item);
            break;
        }
    }
}

void EventBus::SendEvent(std::type_index type_id, Event& e){
    //最好在子线程处理
    std::unique_lock<std::mutex> locker(_mutex);
    Registrations* holders = handlers[type_id];
    if (holders == nullptr) {
        return;
    }

    for (auto & holder : *holders) {
        EventHandler<Event>* handler = static_cast<EventHandler<Event>*>(holder->getHandler());
        if(handler){
            handler->dispatch(e);
        }
    }
}

}
