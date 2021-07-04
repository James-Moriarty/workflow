/*
  Copyright (c) 2019 Sogou, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  Author: Xie Han (xiehan@sogou-inc.com)
*/

#include "SubTask.h"

void SubTask::subtask_done()
{
	SubTask *cur = this;
	ParallelTask *parent;
	SubTask **entry;

	while (1)
	{
		parent = cur->parent;
		entry = cur->entry;
		cur = cur->done();
		if (cur)
		{
			cur->parent = parent;
			cur->entry = entry;
			if (parent)
				*entry = cur;

			cur->dispatch();
		}
		else if (parent)
		{
			//* 如果该任务为并行任务的子任务，执行完后将并行任务的剩余任务数减1
			//* 如果减1后剩余任务数为0，那么就执行并行任务的回调函数
			if (__sync_sub_and_fetch(&parent->nleft, 1) == 0)
			{
				cur = parent;
				continue;
			}
		}

		break;
	}
}

void ParallelTask::dispatch()
{
	SubTask **end = this->subtasks + this->subtasks_nr;
	SubTask **p = this->subtasks;

	this->nleft = this->subtasks_nr;
	if (this->nleft != 0)
	{
		do
		{
			(*p)->parent = this;
			(*p)->entry = p;
			(*p)->dispatch();
		} while (++p != end);
	}
	else
		this->subtask_done();
}

