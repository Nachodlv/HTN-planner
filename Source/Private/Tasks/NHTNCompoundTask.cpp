#include "Tasks/NHTNCompoundTask.h"


void UNHTNCompoundTask::InitializeTask(UNHTNComponent& HTNComp)
{
	for (FNHTNMethod& Method : Methods)
	{
		for (UNHTNBaseTask* Task : Method.Tasks)
		{
			Task->InitializeTask(HTNComp);
		}
	}
}

