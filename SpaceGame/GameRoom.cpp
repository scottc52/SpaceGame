/* GameRoom.cpp */

#include "GameRoom.h"

//=========================
// IO function implementations
//=========================

GameRoom::~GameRoom()
{
	delete this->name;
	int objectSize = this->objects.size();
	for (int i = 0; i < objectSize; i++)
	{
		delete this->objects[i];
	}
}

void GameRoom::AddObject(GameObject *newObject)
{
	GameObject *object = new GameObject(newObject);
	this->objects.push_back(object);
	
	std::sort(this->objects.begin(), this->objects.end(), ObjectComparator);
}

GameObject *GameRoom::GetObject(char *objectName)
{
	int index = binaryObjectSearch(this->objects, objectName);
	if (index != OBJECT_NOT_FOUND)
	{
		return this->objects[index];
	}
	else
	{
		return NULL;
	}
}

void GameRoom::RemoveObject(char *objectName)
{
	int index = binaryObjectSearch(this->objects, objectName);
	if (index != OBJECT_NOT_FOUND)
	{
		delete this->objects[index];
		this->objects.erase(this->objects.begin() + index);
	}
	
	std::sort(this->objects.begin(), this->objects.end(), ObjectComparator);
}

void GameRoom::PrintObjectNames()
{
	int objectSize = this->objects.size();
	for (int i = 0; i < (objectSize - 1); i++)
	{
		cout << this->objects[i]->GetName() << ", ";
	}
	cout << this->objects[objectSize - 1]->GetName() << endl;
}

bool GameRoom::ObjectComparator(GameObject *o1, GameObject *o2)
{
	return (strcmp(o1->GetName(), o2->GetName()) < 0);
}


int GameRoom::binaryObjectSearch(vector<GameObject *> objects, char *objectName)
{
	int imin = 0;
	int imax = objects.size() - 1;
	
	// continue searching while [imin,imax] is not empty
	while (imax >= imin)
	{
		/* calculate the midpoint for roughly equal partition */
		int imid = midpoint(imin, imax);
		// determine which subarray to search
		if (strcmp(objects[imid]->GetName(), objectName) < 0)
		{
			// change min index to search upper subarray
			imin = imid + 1;
		}
		else if (strcmp(objects[imid]->GetName(), objectName) > 0)
		{
			// change max index to search lower subarray
			imax = imid - 1;
		}
		else
		{
			// key found at index imid
			return imid;
		}
	}
	// key not found
	return OBJECT_NOT_FOUND;
}