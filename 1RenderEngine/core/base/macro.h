#pragma once

/**
* ��־����߳���غ�
* 
*/

#include "..\log\log.h"
#include "public_singleton.h"

#include <thread>
#include <chrono>

#define OESleep(_ms) std::this_thread::sleep_for(std::chrono::milliseconds(_ms));

#define OENameOf(name) #name