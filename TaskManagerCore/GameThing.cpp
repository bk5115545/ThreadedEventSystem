#include "GameThing.h"
#include "Dispatcher.h"


GameThing::GameThing(int i) {
	this->number = i;

	this->localSubscriber = new Subscriber(this);
	localSubscriber->callingCondition = std::bind(&GameThing::testcondition, this, std::placeholders::_1);
	localSubscriber->method = std::bind(&GameThing::testaction, this, std::placeholders::_1);

	int randNum = abs(rand())%1000;
	std::cout << "Random Number for object " << i << " is " << randNum << "." << std::endl;
	random = randNum;

	if(randNum < 400) 
		Dispatcher::AddEventSubscriber(localSubscriber);
	
	
	this->updateSubscriber = new Subscriber(this);
	updateSubscriber->method = std::bind(&GameThing::Update, this, std::placeholders::_1);
	Dispatcher::AddSpecificEventSubscriber(updateSubscriber, EVENT_UPDATE);

	this->renderSubscriber = new Subscriber(this);
	renderSubscriber->method = std::bind(&GameThing::Render, this, std::placeholders::_1);
	Dispatcher::AddSpecificEventSubscriber(renderSubscriber, EVENT_RENDER);
}


GameThing::~GameThing() {
	Dispatcher::RemoveSpecificEventSubscriber(updateSubscriber, EVENT_UPDATE);
	Dispatcher::RemoveSpecificEventSubscriber(renderSubscriber, EVENT_RENDER);
	Dispatcher::RemoveEventSubscriber(localSubscriber);

	delete localSubscriber;
	delete updateSubscriber;
	delete renderSubscriber;
}

void GameThing::Update(void* time) {
	std::cout << "Updating..." << std::endl;
}

void GameThing::Render(void* time) {
	std::cout << "Rendering..." << std::endl;
}


void GameThing::testaction(void* data) {
	int randNum = abs(rand())%1000;
	std::cout << "Random Number for object " << number << " is " << randNum << "." << std::endl;
	random = randNum;
}

bool GameThing::testcondition(double eventID) {
	if((int)eventID == 8)
		return true;
	return false;
}