#pragma once

#include "Globals.h"
#include <functional>


typedef void	(SubscriptionFunction(void* eventData));	 //arguements are (void*	eventData) //functions that should be called if the calling condition is met
typedef bool	(CallingConditionFunction(double eventID));  //arguements are (double	eventID)   //functions that describe the calling condition relative to an event id upcast or deltaTime

class Subscriber {
public:
	Subscriber(void* owner);
	Subscriber(void* owner, double watchedEvent);
	Subscriber(Subscriber &other);

	bool DefaultCondition(double value);
	void DefaultAction(void* data);


	std::function<SubscriptionFunction> method;
	std::function<CallingConditionFunction> callingCondition;

	void* owner;
	double eventID;
};