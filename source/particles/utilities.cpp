#include "..\definitions\includes.h"

// ************  Utility Functions - can be made accessible in Lua, unless stated otherwise ************

int Round(float x)
{
	return (x > 0 ? int(x + 0.5f) : int(x - 0.5f));
}


phd_vector Round(const Vector3f& v)
{
	return phd_vector(Round(v.x), Round(v.y), Round(v.z));
}


int Clamp(int x, int min, int max)
{
	return (x < min ? min : (x > max ? max : x));
}


float Clamp(float x, float min, float max)
{
	return (x < min ? min : (x > max ? max : x));
}


float Lerp(float a, float b, float t)
{
	t = Clamp(t, 0.0f, 1.0f);

	return a + t * (b - a);
}


float InverseLerp(float val1, float val2, float x)
{
	float t = (x - val1) / (val2 - val1);

	t = Clamp(t, 0.0f, 1.0f);

	return t;
}


short GetOrientDiff(short sourceOrient, short targetOrient)
{
	int diff = targetOrient - sourceOrient;
	
	return short(diff); // returns (+) if to the right of source orient, (-) if to the left
}


float ShortToRad(short rotation)
{
	return float(M_PI) * rotation / 32768;
}


short RadToShort(float angle)
{
	return Round(32768 * angle / M_PI);
}


float SimpleDist(const Vector3f &v1, const Vector3f &v2)
{
	Vector3f d = (v1 - v2);
	d.x = abs(d.x);
	d.y = abs(d.y);
	d.z = abs(d.z);

	float maxDist = d.x;
	if (d.y > maxDist)
		maxDist = d.y;
	if (d.z > maxDist)
		maxDist = d.z;

	return maxDist;
}


int CheckDistFast(const Vector3f& v1, const Vector3f& v2, float dist)
{
	auto vdif = v1 - v2;

	float vdifsqr = (vdif.x*vdif.x + vdif.y*vdif.y + vdif.z*vdif.z);
	float distsqr = dist * dist;

	if (vdifsqr < distsqr)
		return -1;

	if (vdifsqr > distsqr)
		return 1;

	return 0;
}


float RealDist(const Vector3f& v1, const Vector3f& v2)
{
	return (v1 - v2).magnitude();
}


long TestCollisionSpheres(const Vector3f& posTest, Tr4ItemInfo* item, float radius)
{
	long flags = 0;
	int num = 0;

	if (item)
	{
		MeshSphere SphereList[34];
		num = GetSpheres(item, SphereList, 1);

		for (int i = 0; i < num; ++i)
		{
			auto sph = &SphereList[i];
			int r = sph->r;
			if (r > 0)
			{
				Vector3f vSph(sph->x, sph->y, sph->z);

				if (CheckDistFast(posTest, vSph, radius + sph->r) < 0)
					flags |= (1 << i);
			}
		}
	}

	return flags;
}


int FindNearestTarget(const Vector3f& posTest, float radius, short* const slotList)
{
	int itemIndex = -1;
	int nearest = 0x7FFFFFFF;

	for (int i = 0; i < level_items; ++i)
	{
		auto item = &items[i];

		bool slotCheck = false;
		const short* slotIter = slotList;

		if (*slotIter == -1)
		{
			if (objects[item->object_number].intelligent &&
				item->object_number != SLOT_GUIDE &&
				item->object_number != SLOT_VON_CROY) {
				slotCheck = true;
			}
		}
		else
		{
			while (*slotIter >= 0)
			{
				if (*(slotIter++) == item->object_number)
				{
					slotCheck = true;
					break;
				}
			}
		}


		if (slotCheck && item->active)
		{
			if (objects[item->object_number].intelligent && item->hit_points <= 0)
				continue;

			Vector3f target(item->pos.xPos, item->pos.yPos, item->pos.zPos);

			int dist = Round(SimpleDist(posTest, target));
			if (dist < radius && dist < nearest)
			{
				nearest = dist;
				radius = dist;
				itemIndex = i;
			}
		}
	}

	return itemIndex;
}


Vector3f RotatePoint3D(const Vector3f& point, short xrot, short yrot, short zrot)
{
	Vector3f res;

	phd_PushUnitMatrix();
	phd_RotYXZ(yrot, xrot, zrot);

	res.x = (phd_mxptr[M00] * point.x + phd_mxptr[M01] * point.y + phd_mxptr[M02] * point.z) * (1.0f / 16384);
	res.y = (phd_mxptr[M10] * point.x + phd_mxptr[M11] * point.y + phd_mxptr[M12] * point.z) * (1.0f / 16384);
	res.z = (phd_mxptr[M20] * point.x + phd_mxptr[M21] * point.y + phd_mxptr[M22] * point.z) * (1.0f / 16384);

	phd_PopMatrix();

	return res;
}


Vector3f GetJointPos(Tr4ItemInfo* item, int joint, int xOff, int yOff, int zOff)
{
	static const uchar lara_joint_map[] = { 0, 1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 12, 13, 14, 8 };
	
	phd_vector v(xOff, yOff, zOff);

	if (!item->object_number)
		GetLaraJointPos(&v, lara_joint_map[joint]);
	else
		GetJointAbsPosition((StrItemTr4*)item, (StrMovePosition*)&v, joint);

	return Vector3f(v.x, v.y, v.z);
}


Vector3f SphericalToCartesian(float r, float phi, float theta)
{
	float x = r * sin(phi) * cos(theta);
	float z = r * sin(phi) * sin(theta);
	float y = r * cos(phi);
	return Vector3f(x, y, z);
}


// should not be available in Lua
int TestForWall(int x, int y, int z, short* room)
{
	void* floor = GetFloor(x, y, z, room);

	int fh = GetHeight(floor, x, y, z);
	int ch = GetCeiling(floor, x, y, z);

	if (fh != (-0x7F00))
	{
		if ((fh-y) > 0 && (ch-y) < 0)
			return 0; // No wall

		return 2; // Block (floor or ceiling)
	}

	return 1; // Wall
}


// should not be available in Lua
Vector3f GetSlopeNormal(Tr4FloorInfo* floor, int x, int y, int z)
{
	if (!floor)
		return Vector3f(0, -1, 0);

	WORD tilts = GetTiltType(floor, x, y, z);
	signed char tilt_x = tilts & 0xFF;
	signed char tilt_z = tilts >> 8;

	return Vector3f(float(-tilt_x), -4.0f, float(-tilt_z)).normalized();
}


Vector3f Spline(const Vector3f &v0, const Vector3f &v1, const Vector3f &v2, const Vector3f &v3, float t)
{
	auto a = v1*2;
	auto b = (-v0 + v2) * t;
	auto c = (v0*2 - v1*5 + v2*4 - v3) * (t*t);
	auto d = (-v0 + v1*3 - v2*3 + v3) * (t*t*t);

	return (a + b + c + d) * 0.5f;
}


Vector3f SplineStart(const Vector3f &v0, const Vector3f &v1, const Vector3f &v2, float t)
{
	auto a = v0*2;
	auto b = (-v0*3 + v1*4 - v2) * t;
	auto c = (v0 - v1*2 + v2) * (t*t);

	return (a + b + c) * 0.5f;
}


Vector3f SplineEnd(const Vector3f &v0, const Vector3f &v1, const Vector3f &v2, float t)
{
	auto a = v1*2;
	auto b = (-v0 + v2) * t;
	auto c = (v0 - v1*2 + v2) * (t*t);

	return (a + b + c) * 0.5f;
}


Vector3f SplineDerivative(const Vector3f &v0, const Vector3f &v1, const Vector3f &v2, const Vector3f &v3, float t)
{
	auto b = (-v0 + v2);
	auto c = (v0*2 - v1*5 + v2*4 - v3) * 2 * t;
	auto d = (-v0 + v1*3 - v2*3 + v3) * 3 * (t*t);

	return (b + c + d) * 0.5f;
}


Vector3f SplineStartDerivative(const Vector3f &v0, const Vector3f &v1, const Vector3f &v2, float t)
{
	auto b = (-v0*3 + v1*4 - v2);
	auto c = (v0 - v1*2 + v2) * 2 * t;

	return (b + c) * 0.5f;
}


Vector3f SplineEndDerivative(const Vector3f &v0, const Vector3f &v1, const Vector3f &v2, float t)
{
	auto b = (-v0 + v2);
	auto c = (v0 - v1*2 + v2) * 2 * t;

	return (b + c) * 0.5f;
}
