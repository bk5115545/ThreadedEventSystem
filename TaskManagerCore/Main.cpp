#include "Globals.h"

#include "Dispatcher.h"
#include "GameThing.h"

#include <Windows.h> // for Sleep
#pragma comment(lib, "Winmm.lib")


double deltaTime = 5.0;

int main(char* argc, int argsv) {
	Dispatcher::Initialize();

	srand(timeGetTime());


	//Add some game objects to a list
	std::list<GameThing*> gameStuff = std::list<GameThing*>();
	for(int i=0; i<10; i++) {
		gameStuff.push_back(new GameThing(i));
	}

	
	//add some early events for thread order variability
	Dispatcher::DispatchEvent(EVENT_UPDATE, 0);
	Dispatcher::DispatchEvent(EVENT_UPDATE, 0);


	//until we break with Crtl+C
	while(true) {
		std::cout << "Before events dispatched." << std::endl;
		Dispatcher::DispatchEvent(EVENT_UPDATE, &deltaTime);
		//Dispatcher::DispatchEvent(EVENT_RENDER, 0);
		std::cout << "After events dispatched." << std::endl;

		Sleep(1); // Give the other thread some time to execute
	}


	Sleep(750); //wait for events to be processed to shutdown the dispatcher

	//stop processing and terminate underlying thread handle
	Dispatcher::Stop();
	Dispatcher::Terminate();

	for(auto it : gameStuff) {
		delete it; //these game objects should remove their subscribers from the dispatcher
	}

	//delete Dispatcher::GetInstance(); //not necessary

	system("pause");


	return 0;
}

