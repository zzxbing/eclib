﻿/*!
\file c11_event.h
\author kipway@outlook.com
\update 2018.1.3

eclib class cEvent  with c++11. Adapt for c_event.h 

eclib Copyright (c) 2017-2018, kipway
source repository : https://github.com/kipway/eclib

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#pragma once
#include <mutex>
#include <condition_variable>
namespace ec {
	class cEvent
	{
	public:
		cEvent(bool bInitiallyOwn = false, bool bManualReset = false) :_nready(bInitiallyOwn), _bManualReset(bManualReset)
		{
		}
		bool SetEvent()
		{
			std::unique_lock<std::mutex> lck(_mtx);
			_nready = true;
			_cv.notify_one();
			return true;
		};
		bool ResetEvent()
		{
			std::unique_lock<std::mutex> lck(_mtx);
			_nready = false;
			return true;
		}
		bool Wait(int milliseconds)
		{
			std::unique_lock<std::mutex> lck(_mtx);
			if (_cv.wait_for(lck, std::chrono::milliseconds(milliseconds)) != std::cv_status::timeout)
			{
				if (_nready)
				{
					if (!_bManualReset)
						_nready = false;
					return true;
				}
			}
			return false;
		}
	protected:
		bool _nready;
		bool _bManualReset;
		std::mutex _mtx;
		std::condition_variable _cv;
	};
}

