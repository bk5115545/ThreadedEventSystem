#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <list>

#include "Subscriber.h"

//Some Event ID's
const enum Events {
	EVENT_UPDATE,
	EVENT_RENDER,
	EVENT_MOUSEUP,
	EVENT_MOUSEDOWN,
	EVENT_MOUSEMOVE,
	EVENT_MOUSEDRAGED,
	EVENT_KEYDOWN,
	EVENT_KEYUP
};