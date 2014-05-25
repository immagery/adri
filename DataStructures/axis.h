#ifndef AXIS_H
#define AXIS_H

#include <DataStructures/Object.h>

class axis 
{
public:

    axis()
    {
        pos = Vector3d(0,0,0);
        rot = Quaterniond::Identity();
    }

	Vector3d pos;
	Quaterniond rot;

	// Get the diference from one situation to another
	void getReorentation(const axis& ref);

};

#endif // AXIS_H