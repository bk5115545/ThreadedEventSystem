#pragma once
#include "Globals.h"

class GameThing {

public:
	GameThing(int);
	~GameThing();

	void testaction(void* data);
	bool testcondition(double eventID);

	void Update(void* time);
	void Render(void* time);

	Subscriber* localSubscriber;
	Subscriber* updateSubscriber;
	Subscriber* renderSubscriber;
	int number;
	int random;

};