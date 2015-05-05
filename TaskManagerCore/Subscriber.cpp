#include "Subscriber.h"

Subscriber::Subscriber(void* owner) {
	this->owner = owner;

	callingCondition = std::bind(&Subscriber::DefaultCondition, this, 0.0);
	method = std::bind(&Subscriber::DefaultAction, this, nullptr);
}

Subscriber::Subscriber(void* owner, double watchedEvent) {
	this->owner = owner;
	this->eventID = watchedEvent;

	//callingCondition = std::bind(&Subscriber::DefaultCondition, this, 0.0);
	method = std::bind(&Subscriber::DefaultAction, this, nullptr);
}

Subscriber::Subscriber(Subscriber &other) {
	this->callingCondition = other.callingCondition;
	this->method = other.method;
	this->owner = other.owner;
	this->eventID = other.eventID;
}

bool Subscriber::DefaultCondition(double value) {
	return true;
}

void Subscriber::DefaultAction(void* data) {
	std::cout << "DefaultAction not overriden in subscriber object." << std::endl;
}