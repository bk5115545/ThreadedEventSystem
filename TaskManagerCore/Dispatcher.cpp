#include "Dispatcher.h"

#include <thread>
#include <Windows.h>

bool inited;

std::vector<std::pair<double,void*>>* dispatchEvents;
std::list<Subscriber*>*	subscriberQueue;
std::map<int,std::list<Subscriber*>*>* mappedEvents;


std::thread processingThread;

double* localDeltaTime;

bool running;
bool processing;
bool dispatchLock;
bool subscriberLock;
bool mappedLock;

//Begin Class Methods

Dispatcher::Dispatcher() { }

Dispatcher* Dispatcher::GetInstance() {
	Dispatcher theInstance;
	return &theInstance;
}

//Does not invalidate subscriber objects
Dispatcher::~Dispatcher() {
	if(subscriberQueue) delete subscriberQueue;

	if(dispatchEvents)	delete dispatchEvents;

	if(mappedEvents)	{
		std::list<int> lists;
		for(auto pair : *mappedEvents) {
			lists.push_back(pair.first);
		}

		for(auto num : lists) delete mappedEvents->at(num);
		delete mappedEvents;
	}

	running = false;
	mappedLock = false;
	dispatchLock = false;
	subscriberLock = false;

	//if(processingThread.joinable()) processingThread.join();
}

void Dispatcher::Initialize() {
	inited = true;
	subscriberQueue = new std::list<Subscriber*>();
	dispatchEvents	= new std::vector<std::pair<double,void*>>();
	mappedEvents	= new std::map<int,std::list<Subscriber*>*>();

	running = true;
	processing = true;
	dispatchLock = false;
	mappedLock = false;
	subscriberLock = false;


	localDeltaTime = (double*)0;
	processingThread = std::thread(Process, localDeltaTime); //starts the processing thread
	processingThread.detach(); //it probably won't terminate before the end of this program so we want to ignor errors
}

void Dispatcher::Process(double* deltaTime) {
	while(running) {
		std::this_thread::sleep_for(std::chrono::milliseconds(5));

		while(processing && running) {
			if(!dispatchLock) {
				if(dispatchEvents->size()==0) { // if no events have been dispatched, wait a little longer
					std::this_thread::sleep_for(std::chrono::milliseconds(2));
					continue;	
				}
				//events have been dispatched so force other threads to block when dispatching new events
				dispatchLock = true;

				//get the first event pair
				std::pair<double, void*> eventPair = std::pair<double, void*>(dispatchEvents->at(dispatchEvents->size()-1).first, dispatchEvents->at(dispatchEvents->size()-1).second);  
				dispatchEvents->pop_back();
											//copy consturctor for thread safety
				dispatchLock = false;		//and so we can unlock the list so more can be dispatched

				//wait to acquire the map lock
				while(mappedLock && processing) std::this_thread::sleep_for(std::chrono::milliseconds(1));
				if(!processing) break;
				mappedLock = true;

				//got the lock but only lock the subscribers if we need to
				if(mappedEvents->count(eventPair.first) > 0) { 
					// if subscribers are listening for this event specifically
					//we need to acquire the subscriber lock too
					while(subscriberLock && processing) std::this_thread::sleep_for(std::chrono::milliseconds(1));
					if(!processing) { mappedLock = false; break; }
					subscriberLock = true;
					//got it

					for(auto subscriber : *mappedEvents->at(eventPair.first)) {
						if(!subscriber) { // warn of null subscribers
							std::cout << "Dispatcher --->  Null subscriber in SpecificEventSubscribers." << std::endl;
							continue;
						}
						//if the subscriber has a method to be called then call it
						if(subscriber && subscriber->method)
							subscriber->method(eventPair.second);
					}
				}
				//we're done with the map so unlock it
				mappedLock = false;

				for(auto subscriber : (*subscriberQueue)) {
					if(!subscriber) {
						std::cout << "Dispatcher --->  Null subscriber in EventSubscriberQueue." << std::endl;
						continue;
					}
					//if the calling condition returns true, call the method
					if(subscriber->callingCondition != nullptr && subscriber->callingCondition(eventPair.first)) {
						subscriber->method(eventPair.second);
					} else if(subscriber->callingCondition == nullptr) std::cout << "Dispatcher --->  Null calling condition in general subscriber object for event_id " << eventPair.first << "." << std::endl;
				}//else warn about a null calling condition

				//done with subscribers now
				subscriberLock = false;
			}
		}
	}
	//Something forced us out of our loop
	std::cout << "Processing thread terminated?" << std::endl;
}

void Dispatcher::DispatchEvent(double eventID, void* eventData) {
	//std::cout << "Dispatcher --->  Received event " << eventID << "." << std::endl;
	dispatchEvents->push_back(std::pair<double,void*>(eventID, eventData));
}

void Dispatcher::AddEventSubscriber(Subscriber* requestor) {
	while(subscriberLock && processing) std::this_thread::sleep_for(std::chrono::milliseconds(1));
	subscriberLock = true;
	subscriberQueue->push_back(requestor); //allow duplicates
	subscriberLock = false;
}

void Dispatcher::AddSpecificEventSubscriber(Subscriber *requestor, double event_id) {
	if(mappedEvents->count(event_id) > 0) {
		mappedEvents->at(event_id)->push_back(requestor);
	} else {
		std::cout << "Dispatcher --->  Dynamically allocating list for Specific EventID " << event_id << "." << std::endl << "Dispatcher --->  This should be avoided for performance reasons." << std::endl;
		mappedEvents->emplace(event_id, new std::list<Subscriber*>());
		mappedEvents->at(event_id)->push_back(requestor);
	}
}

Subscriber* Dispatcher::RemoveEventSubscriber(Subscriber* requestor) {
	//i love the new c++11 syntax :: I just hope that it's not slow
	while(subscriberLock && processing) std::this_thread::sleep_for(std::chrono::milliseconds(1));
	subscriberLock = true;


	auto list = std::list<Subscriber*>();

	for(auto sub : (*subscriberQueue)) {
		list.push_back(sub);
	}

	for(auto sub : list) {
		subscriberQueue->remove(sub);
	}

	subscriberLock = false;

	return requestor;
}

Subscriber* Dispatcher::RemoveSpecificEventSubscriber(Subscriber *requestor, double event_id) {
	while(mappedLock && processing) std::this_thread::sleep_for(std::chrono::milliseconds(1));
	mappedLock = true;

	if(mappedEvents->find(event_id) != mappedEvents->end()) {

		while(subscriberLock && processing) std::this_thread::sleep_for(std::chrono::milliseconds(1));
		subscriberLock = true;

		auto list = std::list<Subscriber*>();
		Subscriber* s = nullptr;

		for(auto sub : *mappedEvents->at(event_id)) {
			if(sub == requestor) list.push_back(sub);
		}

		for(auto sub : list) {
			s = sub;
			mappedEvents->at(event_id)->remove(sub);
		}
		mappedLock = false;
		subscriberLock = false;
		return s;
	}
	return nullptr;
}

//Does not include the SpecificEventSubscriber
std::list<Subscriber*> Dispatcher::GetAllSubscribers(void* owner) {
	std::list<Subscriber*> tmp;

	while(subscriberLock && processing) std::this_thread::sleep_for(std::chrono::milliseconds(1));
	subscriberLock = true;

	for(Subscriber* sub : (*subscriberQueue)) {
		if(sub->owner == owner) tmp.push_back(sub);
	}
	subscriberLock = false;
	return tmp;
}

void Dispatcher::Stop() {
	processing = false;
	Sleep(100);
}

void Dispatcher::Start() {
	processing = true;
}

void Dispatcher::Terminate() {
	running = false;
	processing = false;
	Sleep(100);
}