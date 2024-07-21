#pragma once

#define MAX_SPRITEPARTS 2048
#define MAX_MESHPARTS 1024
#define MAX_PARTGROUPS 256
#define MAX_WINDSTREAKS 512

#define STREAK_SEGMENTS 4
#define STREAK_POINTS STREAK_SEGMENTS + 1

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;


// ************  namespace Particles - declarations ****************

namespace Particles
{
	// ************  Particle-related classes  ****************

	// ColorRGB struct
	struct ColorRGB
	{
		uchar R, G, B;

		ColorRGB(): R(0), G(0), B(0) {}

		ColorRGB(uchar red, uchar green, uchar blue): R(red), G(green), B(blue) {}

		static ColorRGB Lerp(const Particles::ColorRGB& C1, const Particles::ColorRGB& C2, float t);

		static ColorRGB FromHSL(float hue, float sat, float light);
	};


	enum TetherType
	{
		TETHER_NONE,
		TETHER_ROTATING,
		TETHER_STATIC
	};


	enum DrawMode
	{
		DRAW_SPRITE,
		DRAW_SQUARE,
		DRAW_LINE,
		DRAW_ARROW,
		DRAW_NONE
	};


	struct NodeAttachment
	{
		TetherType tether;
		short cutoff, random;
	};


	// Base Particle class
	struct BaseParticle
	{
		// fields

		Vector3f	pos;
		Vector3f	vel;
		Vector3f	accel;

		short		roomIndex;
		short		lifeSpan;
		short		lifeCounter;

		short		emitterIndex;
		char		emitterNode;
		uchar		groupIndex;



		// functions

		float		Parameter();
		Vector3f	AbsPos();
		void		LimitSpeed(float maxSpeed);
		void		Attach(int itemIndex, int node);
		void		Deattach();
		bool		CollideWalls(float rebound);
		bool		CollideFloors(float rebound, float minBounce, int collMargin, bool accurate);
		bool		CollidedWithItem(Tr4ItemInfo* item, int radius);
		bool		HomingTarget(Tr4ItemInfo* item, int targetNode, float homingFactor, float homingAccel, bool predict);
		Vector3f	FollowTarget(const Vector3f& v, float factor, float maxSpeed);
		Vector3f	SplinePos(const Vector3f v[], int arrSize, float t);
		Vector3f	SplineVel(const Vector3f v[], int arrSize, float t);
		Vector3f	AvoidRoomGeometry(int wallMargin, int floorMargin, float factor);
		Vector3f	AvoidItem(Tr4ItemInfo* item, float radius, float factor);
		Vector3f	CheckDistanceEmitter(int dist, int val);

		//virtual void Animate(int start, int end, int framerate) = 0;

		//virtual Vector3f BoidDistanceRule(float radius, float factor) = 0;
		//virtual Vector3f BoidGatherRule(float radius, float factor) = 0;
		//virtual Vector3f BoidVelocityRule(float radius, float factor) = 0;
	};


	// Particle Group class
	struct ParticleGroup;


	// Sprite Particle class
	struct SpriteParticle : public BaseParticle
	{
		// fields

		ushort		sizeStart;
		ushort		sizeEnd;
		ushort		sizeCust;

		short		skew;

		short		rot;
		short		rotVel;

		uchar		spriteIndex;
		uchar		fadeIn;
		uchar		fadeOut;

		short		colorFadeTime;
		ColorRGB	colStart;
		ColorRGB	colEnd;
		ColorRGB	colCust;

		// functions

		void		Animate(int startSprite, int endSprite, int frameRate);

		void		DrawSpritePart(const ParticleGroup& pgroup, float* const view, long smallest_size);

		// boid-specific

		Vector3f BoidDistanceRule(float radius, float factor);
		Vector3f BoidGatherRule(float radius, float factor);
		Vector3f BoidVelocityRule(float radius, float factor);
	};


	// Mesh Particle class
	struct MeshParticle : public BaseParticle
	{
		// fields

		short		rotX;
		short		rotY;
		short		rotZ;

		short		rotVelX;
		short		rotVelY;
		short		rotVelZ;

		ushort		scaleX;
		ushort		scaleY;
		ushort		scaleZ;

		short		object;
		uchar		mesh;
		uchar		transparency;
		ColorRGB	tint;	

		// functions

		void		AlignToVel(float factor, bool invert);
		void		AlignToTarget(const Vector3f &v, float factor, bool invert);
		void		Shatter();

		void		Animate(int startMesh, int endMesh, int framerate);

		void		DrawMeshPart();

		// boid-specific

		Vector3f BoidDistanceRule(float radius, float factor);
		Vector3f BoidGatherRule(float radius, float factor);
		Vector3f BoidVelocityRule(float radius, float factor);
	};

	struct ParticleGroup
	{
		//void (*Init)(int group);
		void (*UpdateS)(SpriteParticle*);
		void (*UpdateM)(MeshParticle*);

		NodeAttachment attach;
		short spriteSlot;

		int partLimit;
		mutable int partCount;

		uchar groupIndex;
		uchar drawMode;
		uchar blendingMode;

		bool Saved;
		bool LineIgnoreVel;
		bool IsBoid;
		bool ScreenSpace;
	};


	struct WindStreak
	{
		Vector3f points[STREAK_POINTS];
		short emitterIndex;
		short lifeSpan, lifeCounter;
		ushort size;
		ushort length;
		uchar spriteIndex;
		uchar fadeIn, fadeOut;
		ColorRGB col;
	};

	// other members

	extern ulong gameTick;
	extern int nextPartGroup;

	extern SpriteParticle spriteParts[];
	extern MeshParticle meshParts[];
	extern ParticleGroup partGroups[];
	extern WindStreak windStreaks[];

	int RandomInt();

	void ClearParticleSystem();
	void ClearParts();
	void ClearPartGroup(int partGroup);

	void UpdateParts();

	void DrawParts();

	SpriteParticle* GetFreeSpritePart();
	MeshParticle* GetFreeMeshPart();
	ParticleGroup* GetFreeParticleGroup();
	WindStreak* GetFreeWindStreak();
};
