#include "util.h"

void getAxisRotationQuaternion(Eigen::Quaterniond& q, int axis, double angle)
{
	// extract the rest of turns
	double a = angle;
	if(angle >= 0)
		a = angle - (floor(angle/360.0)*360);
	else
		a = angle - (ceil(angle/360.0)*360);

	if(a == 0)
	{
		q = Eigen::Quaterniond().Identity();
		return;
	}

	// This switch changes the case axis
	Eigen::Vector3d v, Axis2, Axis3;
	switch(axis)
	{
	case 2: // over X
		Axis2 << 0,1,0;
		Axis3 << 0,0,1;
	break;
	case 1: // over Y
		Axis2 << 0,0,1;
		Axis3 << 1,0,0;
	break;
	case 0: // over Z
		Axis2 << 1,0,0;
		Axis3 << 0,1,0;
	break;
	}

	v = Axis2*cos(Deg2Rad(a))+Axis3*sin(Deg2Rad(a));

	if(a >= 0)
	{
		if(a == 180)
		{
			q = Eigen::Quaterniond().setFromTwoVectors(Axis2, Axis3) * 
				Eigen::Quaterniond().setFromTwoVectors(Axis3, -Axis2);
		}
		else if(a < 180)
		{
			q = Eigen::Quaterniond().setFromTwoVectors(Axis2, v);
		}
		else if(a > 180)
		{
			q = Eigen::Quaterniond().setFromTwoVectors(Axis2, Axis3) * 
				Eigen::Quaterniond().setFromTwoVectors(Axis3, -Axis2) *
				Eigen::Quaterniond().setFromTwoVectors(-Axis2, v);
		}
	}
	else
	{
		if(a == -180)
		{
			q = Eigen::Quaterniond().setFromTwoVectors(Axis2, -Axis3) * 
				Eigen::Quaterniond().setFromTwoVectors(-Axis3, -Axis2);
		}
		else if(a > -180)
		{
			q = Eigen::Quaterniond().setFromTwoVectors(Axis2, v);
		}
		else if(a > -360)
		{
			q = Eigen::Quaterniond().setFromTwoVectors(Axis2, -Axis3) * 
				Eigen::Quaterniond().setFromTwoVectors(-Axis3, -Axis2) *
				Eigen::Quaterniond().setFromTwoVectors(-Axis2, v);
		}
	}
	
}

Eigen::Quaternion<double> fromEulerAngles(double alpha, double beta, double gamma) {
	Eigen::Quaterniond qX;
	Eigen::Quaterniond qY;
	Eigen::Quaterniond qZ;

	getAxisRotationQuaternion(qX, 0, alpha);
	getAxisRotationQuaternion(qY, 1, beta);
	getAxisRotationQuaternion(qZ, 2, gamma);

	Eigen::Quaterniond q =  qZ * qY * qX;
	return q;
}

void toEulerAngles(const Eigen::Quaterniond& q, double& alpha, double& beta, double& gamma) {
	alpha = atan2( 2*(q.w()*q.x() + q.y()*q.z()), 1 - 2*(q.x()*q.x() + q.y()*q.y()) );
	beta = asin(2*(q.w()*q.y() - q.z()*q.x()));
	gamma = atan2( 2*(q.w()*q.z() + q.x()*q.y()), 1 - 2*(q.y()*q.y() + q.z()*q.z()) );

	alpha *= 180 / M_PI;
	beta *= 180 / M_PI;
	gamma *= 180 / M_PI;
}

// devuelve el signo del valor de entrada.
double sign(double v)
{
    if(v >= 0)
        return 1;
    else
        return -1;
}

double det(Eigen::Vector3d u1, Eigen::Vector3d u2, Eigen::Vector3d u3)
{
    return u1[0]*u2[1]*u3[2] + u2[0]*u1[2]*u3[1] + u3[0]*u1[1]*u2[2]
            - u1[2]*u2[1]*u3[0] - u2[0]*u1[1]*u3[2] - u3[1]*u1[0]*u2[2] ;
}

double round(double x)
{
	return floor(x + 0.5);
}

double timelapse(clock_t clock1,clock_t clock2)
{
	double diffticks=clock2-clock1;
    double diffms=diffticks/CLOCKS_PER_SEC;
    return diffms;
}

float Orientation(float ox, float oy, float oz,
                  float nx, float ny, float nz )
{
  float sigma = std::max(0.0f, - (ox * nx + oy * ny + oz * nz));
                          // / (depth * depth * depth));
  return sigma;
}

void GetColourGlobal(double v,double vmin,double vmax, float& r, float& g, float& b)
{
   r = g = b = 1.0; // white
   double dv;

   if(v>vmax) v = vmax;
   if(v<vmin) v = vmin;

   if(vmin >= 0)
   {
	   dv = vmax - vmin;

	   if (v < (vmin + 0.25 * dv)) {
		  r = 0;
		  g = (4 * (v - vmin) / dv);
	   } else if (v < (vmin + 0.5 * dv)) {
		  r = 0;
		  b = (1 + 4 * (vmin + 0.25 * dv - v) / dv);
	   } else if (v < (vmin + 0.75 * dv)) {
		  r = ( 4 * (v - vmin - 0.5 * dv) / dv );
		  b = (0);
	   } else {
		  g = (1 + 4 * (vmin + 0.75 * dv - v) / dv);
		  b = (0);
	   }
   }
   else
   {
	   if(v>=0)
	   {
		   dv = vmax;
		   vmin = 0;

		   if (v < (vmin + 0.25 * dv)) {
			  r = 0;
			  g = (4 * (v - vmin) / dv);
		   } else if (v < (vmin + 0.5 * dv)) {
			  r = 0;
			  b = (1 + 4 * (vmin + 0.25 * dv - v) / dv);
		   } else if (v < (vmin + 0.75 * dv)) {
			  r = ( 4 * (v - vmin - 0.5 * dv) / dv );
			  b = (0);
		   } else {
			  g = (1 + 4 * (vmin + 0.75 * dv - v) / dv);
			  b = (0);
		   }
	   }
	   else
	   {
		   v = -v;
		   vmax = -vmin;
		   dv = vmax;
		   vmin = 0;

		   b = 1-(v/dv);
		   r = g = 0;
		   /*
		   if (v < (vmin + 0.25 * dv)) {
			  r = 0;
			  g = (4 * (v - vmin) / dv);
		   } else if (v < (vmin + 0.5 * dv)) {
			  r = 0;
			  b = (1 + 4 * (vmin + 0.25 * dv - v) / dv);
		   } else if (v < (vmin + 0.75 * dv)) {
			  r = ( 4 * (v - vmin - 0.5 * dv) / dv );
			  b = (0);
		   } else {
			  g = (1 + 4 * (vmin + 0.75 * dv - v) / dv);
			  b = (0);
		   }
		   */
	   }
   }
}

void GetColour(double v,double vmin,double vmax, float& r, float& g, float& b)
{
   r = g = b = 1.0; // white
   double dv;

   if(vmin < 0 || vmax < 0 || v < 0 || v > vmax || v < vmin)
   {
       //printf("\nValores negativos v:%f, vmin:%f, vmax:%f\n", v, vmin, vmax);
       r = g = b = 0;
   }

   if (v < vmin)
      v = vmin;
   if (v > vmax)
      v = vmax;
   dv = vmax - vmin;

   if (v < (vmin + 0.25 * dv)) {
      r = 0;
      g = (4 * (v - vmin) / dv);
   } else if (v < (vmin + 0.5 * dv)) {
      r = 0;
      b = (1 + 4 * (vmin + 0.25 * dv - v) / dv);
   } else if (v < (vmin + 0.75 * dv)) {
      r = ( 4 * (v - vmin - 0.5 * dv) / dv );
      b = (0);
   } else {
      g = (1 + 4 * (vmin + 0.75 * dv - v) / dv);
      b = (0);
   }
}

int indexOf(vector<int>& labels, int label)
{
    int res = -1;
    for(unsigned int i = 0; i< labels.size(); i++)
    {
        if(labels[i] == label)
            return i;
    }

    return res;
}

double Deg2Rad(double deg)
{
	return (deg/360)*M_PI*2;
}

double Rad2Deg(double rad)
{
	return (rad*360)/(M_PI*2);
}