#include "../definitions/includes.h"

PerlinNoise perlin_eff;

#define RGBA(r, g, b, a) (b | (g << 8) | (r << 16) | (a << 24))


void ClearEffect(int group, short emitter_number)
{
	for (int i = 0; i < MAX_SPRITEPARTS; ++i)
	{
		auto part = &Particles::spriteParts[i];
		if (part->lifeCounter > 0 && part->groupIndex == group && part->emitterIndex == emitter_number)
		{
			part->lifeCounter = 0;
		}
	}
}



void GlitterInit(int group, int count)
{
	for (int i = 0; i < count; ++i)
	{
		//sprk->emitterIndex = (lara_item - items);
		// initialise particle fields
		int x = (Particles::RandomInt() % 12288) - 6144;
		int y = (Particles::RandomInt() % 12288) - 6144;
		int z = (Particles::RandomInt() % 12288) - 6144;
		x += camera->pos.x;
		y += camera->pos.y;
		z += camera->pos.z;

		short room = camera->pos.room;

		if (!TestForWall(x, y, z, &room))
		{
			if (rooms[room].flags & 32)
			{
				auto sprk = Particles::GetFreeSpritePart();
				sprk->groupIndex = group;
				sprk->pos.x = x;
				sprk->pos.y = y;
				sprk->pos.z = z;
				sprk->rot = Particles::RandomInt() * 2;
				sprk->rotVel = 1536;
				sprk->sizeStart = sprk->sizeEnd = 30 + (Particles::RandomInt() & 15);

				sprk->vel.y = -(64 + (Particles::RandomInt() & 31)) * 0.125f;

				sprk->lifeSpan = sprk->lifeCounter = 15 + (Particles::RandomInt() & 3);

				sprk->spriteIndex = 33 + (Particles::RandomInt() % 3);

				sprk->colStart.R = sprk->colEnd.R = 95 + (Particles::RandomInt() & 63);
				sprk->colStart.G = sprk->colEnd.G = 170 + (Particles::RandomInt() & 63);
				sprk->colStart.B = sprk->colEnd.B = 240 + (Particles::RandomInt() & 15);

				sprk->fadeIn = 7;
				sprk->fadeOut = 7;
			}
		}
	}
}


void GlitterUpdate(Particles::SpriteParticle* spart)
{
	if (spart)
	{
		float t = abs((-spart->Parameter() * 2) + 1);
		spart->sizeCust = Round(Lerp(spart->sizeStart + 20, spart->sizeStart, t));
	}
}



void FireflyInit(int group, short emitter_number)
{
	auto item = &items[emitter_number];

	short troom = item->room_number;
	int xrand = (Particles::RandomInt() & 2047) - 1024;
	int yrand = (Particles::RandomInt() & 511) - 256;
	int zrand = (Particles::RandomInt() & 2047) - 1024;

	if (!TestForWall(item->pos.xPos + xrand, item->pos.yPos + yrand, item->pos.zPos + zrand, &troom))
	{
		auto fly = Particles::GetFreeSpritePart();
		// initialise particle fields
		fly->pos.x = float(xrand + item->pos.xPos);
		fly->pos.y = float(yrand + item->pos.yPos);
		fly->pos.z = float(zrand + item->pos.zPos);
		fly->emitterIndex = emitter_number;
		fly->roomIndex = item->room_number;

		short angle = Particles::RandomInt() * 2;
		float speed = 20.0f;
		fly->vel.x = speed * sinf(ShortToRad(angle));
		fly->vel.z = speed * cosf(ShortToRad(angle));
		fly->vel.y = ((Particles::RandomInt() & 63) - 32) * 0.25f;

		fly->lifeSpan = fly->lifeCounter = 320 + (Particles::RandomInt() & 63);

		fly->spriteIndex = 11;
		fly->sizeStart = fly->sizeEnd = 64 + (Particles::RandomInt() & 15);

		fly->colStart.R = fly->colEnd.R = 14;
		fly->colStart.G = fly->colEnd.G = 20;
		fly->colStart.B = fly->colEnd.B = 10;

		fly->fadeIn = 15;
		fly->fadeOut = 15;
		fly->groupIndex = group;
	}
}


void FireflyUpdate(Particles::SpriteParticle* spart)
{
	if (spart)
	{
		auto v1 = spart->BoidDistanceRule(384.0f, 0.005f);
		auto v2 = spart->BoidGatherRule(1024.0f, 0.0025f);
		auto v3 = spart->BoidVelocityRule(768.0f, 0.0015f);
		auto v4 = spart->AvoidRoomGeometry(512, 512, 4.0f);
		auto v5 = spart->CheckDistanceEmitter(2560, 2);

		spart->vel += (v1 + v2 + v3 + v4 + v5);
		spart->LimitSpeed(32.0f);

		int life = spart->lifeSpan - spart->lifeCounter;
		life *= 4;
		life %= spart->lifeSpan;
		life -= spart->lifeSpan / 2;

		int cR, cG, cB;

		if (life <= 0)
		{
			int r = Particles::RandomInt() % 12;
			cR = spart->colStart.R - r + (Particles::RandomInt() & 3);
			cG = spart->colStart.G - r;
			cB = spart->colStart.B - r;

			spart->colEnd.R = Clamp(cR, 0, 255);
			spart->colEnd.G = Clamp(cG, 0, 255);
			spart->colEnd.B = Clamp(cB, 0, 255);
		}
		else
		{
			float f = life * float(M_PI) * 2 / float(spart->lifeSpan);
			float s = sin(f);

			cR = spart->colEnd.R + Round(spart->colEnd.R * s * 9);
			cG = spart->colEnd.G + Round(spart->colEnd.G * s * 9);
			cB = spart->colEnd.B;
		}
		
		spart->colCust.R = Clamp(cR, 0, 255);
		spart->colCust.G = Clamp(cG, 0, 255);
		spart->colCust.B = Clamp(cB, 0, 255);
	}
	
}



void PixieInit(int group, short emitter_number)
{
	auto item = &items[emitter_number];
	auto pix = Particles::GetFreeSpritePart();

	// initialise particle fields
	pix->emitterIndex = emitter_number;
	pix->roomIndex = item->room_number;
	pix->pos.x = float(item->pos.xPos + (Particles::RandomInt() & 1023) - 512);
	pix->pos.y = float(item->pos.yPos);
	pix->pos.z = float(item->pos.zPos + (Particles::RandomInt() & 1023) - 512);

	pix->vel.x = pix->vel.z = 0;
	pix->vel.y = (16 + (Particles::RandomInt() & 7)) / 8.0f;

	pix->lifeSpan = pix->lifeCounter = 320 + (Particles::RandomInt() & 63);

	pix->spriteIndex = 11;
	pix->sizeStart = pix->sizeEnd = 64 + (Particles::RandomInt() & 31);

	pix->colStart.R = pix->colStart.B = 128;
	pix->colStart.G = 48;

	pix->fadeIn = pix->fadeOut = 30;

	pix->groupIndex = group;
}


void PixieUpdate(Particles::SpriteParticle* spart)
{
	if (spart)
	{
		// track head joint
		auto target = GetJointPos(lara_item, 14, 0, 0, 0);

		if (CheckDistFast(spart->pos, target, 3072) < 0)
		{
			// randomize position
			target.x += ((Particles::RandomInt() & 15) - 8) * 96;
			target.y += ((Particles::RandomInt() & 15) - 8) * 96;
			target.z += ((Particles::RandomInt() & 15) - 8) * 96;

			target.x += sin(ShortToRad(lara_item->pos.yRot)) * lara_item->speed * 8;
			target.z += cos(ShortToRad(lara_item->pos.yRot)) * lara_item->speed * 8;

			// move towards target
			auto dif = (target - spart->pos).normalized();
			spart->vel += dif * 10.0f;
			spart->vel *= 0.9f;

			spart->LimitSpeed(70.0f);

			// test collision with Lara's mesh spheres
			if ((Particles::RandomInt() & 1) && TestCollisionSpheres(spart->pos, lara_item, 16))
			{
				lara_item->hit_points -= 1;
			}

			// flash color
			float cR = (perlin_eff.Noise2D(1.0f, 91.0f, spart->lifeCounter * 0.75f) + 0.75f) * 0.67f;

			spart->colCust.G = Clamp(Round((0.5f - cR) * 160) + 96, 0, 255);
			spart->colCust.R = spart->colCust.B = Clamp(Round(cR * 160) + 128, 0, 255);
		}
		else
		{
			float c = sin(spart->Parameter() * 3.1416f * 20) * 0.25f + 0.75f;

			spart->colCust.R = spart->colCust.B = Clamp(Round(c * 128) + 96, 0, 255);
			spart->colCust.G = Clamp(Round(c * 48) + 48, 0, 255);

			spart->vel.x = perlin_eff.Noise2D(256, spart->pos.y, spart->pos.z) * 5;
			spart->vel.z = perlin_eff.Noise2D(256, spart->pos.x, spart->pos.y) * 5;
		}
	}
}



void EmberInit(int group, short emitter_number)
{
	ushort chance = (ushort)Particles::RandomInt();
	if (chance < 16384)
	{
		auto item = &items[emitter_number];
		auto sprk = Particles::GetFreeSpritePart();

		sprk->pos.x = float(item->pos.xPos + (Particles::RandomInt() & 255) - 128);
		sprk->pos.y = float(item->pos.yPos + (Particles::RandomInt() & 127) - 64);
		sprk->pos.z = float(item->pos.zPos + (Particles::RandomInt() & 255) - 128);

		sprk->rot = Particles::RandomInt() * 2;
		sprk->sizeStart = sprk->sizeEnd = 56 + (Particles::RandomInt() & 15);
		sprk->sizeEnd = sprk->sizeStart >> 2;
		sprk->vel.x = (2 * sprk->sizeStart / 3) * sin(ShortToRad(sprk->rot));
		sprk->vel.z = (2 * sprk->sizeStart / 3) * cos(ShortToRad(sprk->rot));
		sprk->vel.y = -float(64 + (Particles::RandomInt() & 31));

		//sprk->accel.y = 1.0f;

		sprk->lifeSpan = sprk->lifeCounter = 40 + (Particles::RandomInt() & 15);

		sprk->colStart.R = 255;
		sprk->colStart.G = 160;
		sprk->colStart.B = 0;

		sprk->colEnd.R = 200;
		sprk->colEnd.G = 0;
		sprk->colEnd.B = 0;

		sprk->fadeIn = 10;
		sprk->fadeOut = 20;

		sprk->spriteIndex = 11;
		sprk->groupIndex = group;

		if (chance >= 6144)
			sprk->groupIndex++;
	}
}


void EmberUpdate(Particles::SpriteParticle* spart)
{
	if (spart)
	{
		spart->rot += (768 + (spart->sizeCust << 4));
		spart->vel.x = (2 * spart->sizeCust / 3) * sin(ShortToRad(spart->rot));
		spart->vel.z = (2 * spart->sizeCust / 3) * cos(ShortToRad(spart->rot));
		spart->vel.y *= 0.95f;
	}
}



void LavaInit(int group, const Vector3f& v, bool bomb)
{
	if (bomb || !(Particles::RandomInt() & 7))
	{
		auto sprk = Particles::GetFreeSpritePart();

		sprk->pos.x = v.x + (Particles::RandomInt() & 127) - 64;
		sprk->pos.z = v.z + (Particles::RandomInt() & 127) - 64;
		sprk->pos.y = v.y;

		sprk->lifeSpan = sprk->lifeCounter = 120 + (Particles::RandomInt() & 31);

		sprk->spriteIndex = 11;
		sprk->sizeStart = 80 + (Particles::RandomInt() & 31);
		sprk->sizeEnd = sprk->sizeEnd >> 2;
		sprk->vel.x = ((Particles::RandomInt() & 127) - 64) * 0.25f;
		sprk->vel.z = ((Particles::RandomInt() & 127) - 64) * 0.25f;
		sprk->vel.y = (Particles::RandomInt() & 31) - 100;
		sprk->accel.y = 1.5f;
		sprk->fadeIn = 30;
		sprk->fadeOut = 30;

		if (bomb)
		{
			sprk->vel.y = (Particles::RandomInt() & 31) - 70;
			sprk->accel.y = 2.0f;
			sprk->vel.x *= 1.5f;
			sprk->vel.z *= 1.5f;
			sprk->lifeSpan = sprk->lifeCounter /= 2;
			sprk->fadeIn = 10;
		}

		sprk->colStart.R = sprk->colEnd.R = 255;
		sprk->colStart.B = sprk->colEnd.B = 0;
		sprk->colStart.G = 96 + (Particles::RandomInt() & 63);
		sprk->colEnd.G = 0 + (Particles::RandomInt() & 15);

		sprk->groupIndex = group;
	}
}


void LavaSmoke(Particles::MeshParticle* mesh, int group, bool trail)
{
	auto sprk = Particles::GetFreeSpritePart();

	sprk->pos.x = mesh->pos.x + (Particles::RandomInt() & 127) - 64;
	sprk->pos.y = mesh->pos.y + (Particles::RandomInt() & 127) - 64;
	sprk->pos.z = mesh->pos.z + (Particles::RandomInt() & 127) - 64;

	sprk->rot = Particles::RandomInt() * 2;
	sprk->rotVel = 384 + (Particles::RandomInt() & 255);
	if (Particles::RandomInt() & 1)
		sprk->rotVel = -sprk->rotVel;

	sprk->sizeStart = 160 + (Particles::RandomInt() & 63);
	sprk->sizeEnd = sprk->sizeStart * 4;

	sprk->spriteIndex = 0;

	if (trail)
	{
		sprk->vel.x = mesh->vel.x * 0.5f;
		sprk->vel.y = mesh->vel.y * 0.5f;
		sprk->vel.z = mesh->vel.z * 0.5f;
		sprk->lifeSpan = sprk->lifeCounter = 15 + (Particles::RandomInt() & 3);
		sprk->fadeOut = 15;
	}
	else
	{
		sprk->vel.x = ((Particles::RandomInt() & 127) - 64) * 0.125f;
		sprk->vel.z = ((Particles::RandomInt() & 127) - 64) * 0.125f;
		sprk->vel.y = (Particles::RandomInt() & 15) - 16;
		sprk->lifeSpan = sprk->lifeCounter = 70 + (Particles::RandomInt() & 31);
		sprk->fadeIn = 15;
		sprk->fadeOut = 30;
		//sprk->sizeEnd = sprk->sizeStart * 4;
	}

	sprk->colStart.R = sprk->colStart.G = sprk->colStart.B = 128 + (Particles::RandomInt() & 31);
	sprk->colEnd.R = sprk->colEnd.G = sprk->colEnd.B = sprk->colStart.R / 3;

	sprk->groupIndex = group;
}


void LavaSpark(Particles::MeshParticle* mesh, int group)
{
	auto sprk = Particles::GetFreeSpritePart();

	sprk->pos.x = mesh->pos.x + (Particles::RandomInt() & 127) - 64;
	sprk->pos.y = mesh->pos.y + (Particles::RandomInt() & 127) - 64;
	sprk->pos.z = mesh->pos.z + (Particles::RandomInt() & 127) - 64;

	sprk->rot = Particles::RandomInt() * 2;
	sprk->rotVel = 384 + (Particles::RandomInt() & 255);
	if (Particles::RandomInt() & 1)
		sprk->rotVel = -sprk->rotVel;

	sprk->sizeStart = 40 + (Particles::RandomInt() & 15);
	sprk->sizeEnd = sprk->sizeStart << 1;

	sprk->spriteIndex = 0;

	sprk->vel.x = mesh->vel.x * 0.5f;
	sprk->vel.y = mesh->vel.y * 0.5f;
	sprk->vel.z = mesh->vel.z * 0.5f;
	sprk->lifeSpan = sprk->lifeCounter = 15 + (Particles::RandomInt() & 3);
	sprk->fadeOut = 15;

	sprk->colStart.R = sprk->colEnd.R = 255;
	sprk->colStart.G = 96 + (Particles::RandomInt() & 63);
	sprk->colEnd.G = 0 + (Particles::RandomInt() & 15);
	sprk->colStart.B = sprk->colEnd.B = 0;

	sprk->groupIndex = group;
}


void PyroclastInit(int group, int x, int y, int z, short room)
{
	if (!(objects[SLOT_UW_PROPULSOR_LARA].loaded))
		return;

	auto fx = Particles::GetFreeMeshPart();

	fx->lifeCounter = fx->lifeSpan = 240;

	fx->object = SLOT_UW_PROPULSOR_LARA;
	fx->mesh = 0;

	fx->pos.x = x;
	fx->pos.y = y;
	fx->pos.z = z;

	fx->roomIndex = room;

	fx->rotX = Particles::RandomInt() * 2;
	fx->rotY = Particles::RandomInt() * 2;
	fx->rotZ = Particles::RandomInt() * 2;
	fx->rotVelZ = 1024 + (Particles::RandomInt() & 1023);
	if (Particles::RandomInt() & 1)
	{
		fx->rotVelZ = -fx->rotVelZ;
	}

	fx->vel.y = (Particles::RandomInt() & 15) - 160;
	float speed = (Particles::RandomInt() & 15) + 40;
	fx->vel.x = sin(ShortToRad(fx->rotY)) * speed;
	fx->vel.z = cos(ShortToRad(fx->rotY)) * speed;
	fx->accel.y = 6;

	fx->groupIndex = group;
}


void PyroclastUpdate(Particles::MeshParticle* mpart)
{
	if (mpart)
	{
		int tx = Round(mpart->pos.x + mpart->vel.x);
		int ty = Round(mpart->pos.y + mpart->vel.y);
		int tz = Round(mpart->pos.z + mpart->vel.z);

		short room_num = mpart->roomIndex;
		auto floor = GetFloor(tx, ty, tz, &room_num);
		int c = GetCeiling(floor, tx, ty, tz);
		int h = GetHeight(floor, tx, ty, tz);

		if (room_num != mpart->roomIndex)
			mpart->roomIndex = room_num;

		if (mpart->pos.y < c)
		{
			mpart->pos.y = c;
			mpart->vel.y = -mpart->vel.y;
		}

		auto spos = Round(mpart->pos);

		if (mpart->pos.y >= h || TestForWall(spos.x, spos.y, spos.z, &room_num))
		{
			if (h == -32512)
				h = mpart->pos.y + 64;

			mpart->pos.y = h - 64;
			int count = 15 + (Particles::RandomInt() % 6);
			for (int i = 0; i < count; ++i)
			{
				LavaInit(4, mpart->pos, true);
				LavaSmoke(mpart, 5, false);
			}

			int outer_inner = (0xF0 << 16) + (0x02);
			int speed = 60 + (Particles::RandomInt() & 15);
			long col = RGBA(255, 100, 0, 20);
			int flags = 0x0;

			spos.y = h - 64;

			TriggerShockwave(&spos, outer_inner, speed, col, flags);


			float dist = RealDist(mpart->pos, Vector3f(lara_item->pos.xPos, lara_item->pos.yPos, lara_item->pos.zPos));
			if (dist < 1280)
			{
				float t = (1280 - dist) * (1.0f / 1536.0f) + 0.1f;
				lara_item->hit_points -= int(250 * t);

				if (lara_item->hit_points < 250)
					LaraBurn();
			}

			//StopEffect(21);

			mpart->Shatter();

			return;
		}

		SoundEffect(21, &spos, 0);

		if (Particles::RandomInt() & 1)
			LavaSmoke(mpart, 5, true);
		else
			LavaSpark(mpart, 6);

		if (ItemNearLara(&spos, 200))
			lara_item->hit_points -= 10;
	}
}


void InitStreakSegments(Particles::WindStreak* streak, const Vector3f& pos, const Vector3f& vel)
{
	auto lengthVect = vel.normalized() * (streak->length);
	for (int i = 0; i < STREAK_POINTS; ++i)
		streak->points[i] = pos - (lengthVect * i);
}


void WindStreakInit(short emitter_number, int xrand, int yrand, int zrand)
{
	auto streak = Particles::GetFreeWindStreak();

	streak->emitterIndex = emitter_number;
	streak->lifeCounter = streak->lifeSpan = 160 + (Particles::RandomInt() & 63);
	streak->fadeIn = streak->fadeOut = 60;
	streak->length = (320 + (Particles::RandomInt() & 127));
	streak->size = 20 + (Particles::RandomInt() & 7);
	streak->spriteIndex = 14;

	int c = 32 + (Particles::RandomInt() & 31);
	streak->col.G = c;
	streak->col.R = c;
	streak->col.B = c;

	auto item = &items[emitter_number];

	Vector3f pos(item->pos.xPos, item->pos.yPos, item->pos.zPos);

	pos.x += (Particles::RandomInt() % xrand) - (xrand / 2);
	pos.y += (Particles::RandomInt() % yrand) - (yrand / 2);
	pos.z += (Particles::RandomInt() % zrand) - (zrand / 2);

	float angle = ShortToRad(-item->pos.yRot) + M_PI_2;
	Vector3f vel(-cos(angle) * 50.0f, 0, -sin(angle) * 50.0f);

	InitStreakSegments(streak, pos, vel);
}


void PollenInit(int group, short emitter_number)
{
	auto item = &items[emitter_number];

	short troom = item->room_number;
	int xrand = (Particles::RandomInt() & 2047) - 1024;
	int yrand = (Particles::RandomInt() & 2047) - 1024;
	int zrand = (Particles::RandomInt() & 2047) - 1024;

	if (!TestForWall(item->pos.xPos + xrand, item->pos.yPos + yrand, item->pos.zPos + zrand, &troom))
	{
		if (!(Particles::RandomInt() & 3))
		{
			WindStreakInit(emitter_number, 2048, 2048, 2048);
		}
		else
		{
			auto part = Particles::GetFreeSpritePart();
			// initialise particle fields
			part->pos.x = float(xrand);
			part->pos.y = float(yrand);
			part->pos.z = float(zrand);
			part->emitterIndex = emitter_number;
			part->roomIndex = item->room_number;

			part->lifeSpan = part->lifeCounter = 128 + (Particles::RandomInt() & 63);

			part->spriteIndex = 14;
			part->sizeStart = part->sizeEnd = 28 + (Particles::RandomInt() & 7);

			part->colStart.R = part->colEnd.R = 80;
			part->colStart.G = part->colEnd.G = 80;
			part->colStart.B = part->colEnd.B = 80;

			part->rot = Particles::RandomInt() * 2;
			part->rotVel = 384 + (Particles::RandomInt() & 127);

			part->fadeIn = 45;
			part->fadeOut = 45;
			part->groupIndex = group;
		}
	}
}


void PollenInit2(int group, short emitter_number)
{
	auto item = &items[emitter_number];

	short troom = item->room_number;
	int xrand = (Particles::RandomInt() % 5120) - 2560;
	int yrand = (Particles::RandomInt() & 2047) - 1024;
	int zrand = (Particles::RandomInt() % 5120) - 2560;

	if (!TestForWall(item->pos.xPos + xrand, item->pos.yPos + yrand, item->pos.zPos + zrand, &troom))
	{
		if (!(Particles::RandomInt() & 3))
		{
			WindStreakInit(emitter_number, 5120, 2048, 5120);
		}
		else
		{
			auto part = Particles::GetFreeSpritePart();
			// initialise particle fields
			part->pos.x = float(xrand);
			part->pos.y = float(yrand);
			part->pos.z = float(zrand);
			part->emitterIndex = emitter_number;
			part->roomIndex = item->room_number;

			part->lifeSpan = part->lifeCounter = 128 + (Particles::RandomInt() & 63);

			part->spriteIndex = 14;
			part->sizeStart = part->sizeEnd = 28 + (Particles::RandomInt() & 7);

			part->colStart.R = part->colEnd.R = 80;
			part->colStart.G = part->colEnd.G = 80;
			part->colStart.B = part->colEnd.B = 80;

			part->rot = Particles::RandomInt() * 2;
			part->rotVel = 384 + (Particles::RandomInt() & 127);

			part->fadeIn = 45;
			part->fadeOut = 45;
			part->groupIndex = group;
		}
	}
}


void WindstreaksWide(short emitter_number)
{
	WindStreakInit(emitter_number, 12288, 12288, 12288);
}


void PollenUpdate(Particles::SpriteParticle* part)
{
	long gt = Particles::gameTick * 2;
	auto absPos = part->pos;
	part->vel = perlin_eff.CurlNoise3D(2048, absPos.x - gt, absPos.y + gt, absPos.z - gt) * 8;
	part->vel.z -= 50.0f;
}