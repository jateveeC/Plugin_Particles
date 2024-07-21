#include "..\definitions\includes.h"
#include <random>

#define RGBA(r, g, b, a) (b | (g << 8) | (r << 16) | (a << 24))

#define SIN_PI6 0.5f
#define COS_PI6 0.8660254f


// ************  namespace Particles  ****************
namespace Particles
{
	// Particle Group array
	ParticleGroup partGroups[MAX_PARTGROUPS];
	int nextPartGroup;

	// Sprite Particle array
	SpriteParticle spriteParts[MAX_SPRITEPARTS];
	int nextSprite;
	
	// Mesh Particle array
	MeshParticle meshParts[MAX_MESHPARTS];
	int nextMesh;

	WindStreak windStreaks[MAX_WINDSTREAKS];
	int nextStreak;

	// in-game tick counter
	ulong gameTick;

	std::mt19937 mtRand;

	static void SetXY4Float(D3DTLVERTEX* v, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float z, short* clip)
	{
		float zv;
		short clip_distance;

		clip_distance = 0;
		zv = f_mpersp / z;
		v->sx = x1;
		v->sy = y1;
		v->sz = z;
		v->rhw = f_moneopersp * zv;

		if (phd_winxmin > v->sx)
			clip_distance = 1;
		else if (phd_winxmax < v->sx)
			clip_distance = 2;

		if (phd_winymin > v->sy)
			clip_distance += 4;
		else if (phd_winymax < v->sy)
			clip_distance += 8;

		clip[0] = clip_distance;

		v++;
		clip_distance = 0;
		v->sx = x2;
		v->sy = y2;
		v->sz = z;
		v->rhw = f_moneopersp * zv;

		if (phd_winxmin > v->sx)
			clip_distance = 1;
		else if (phd_winxmax < v->sx)
			clip_distance = 2;

		if (phd_winymin > v->sy)
			clip_distance += 4;
		else if (phd_winymax < v->sy)
			clip_distance += 8;

		clip[1] = clip_distance;

		v++;
		clip_distance = 0;
		v->sx = x3;
		v->sy = y3;
		v->sz = z;
		v->rhw = f_moneopersp * zv;

		if (phd_winxmin > v->sx)
			clip_distance = 1;
		else if (phd_winxmax < v->sx)
			clip_distance = 2;

		if (phd_winymin > v->sy)
			clip_distance += 4;
		else if (phd_winymax < v->sy)
			clip_distance += 8;

		clip[2] = clip_distance;

		v++;
		clip_distance = 0;
		v->sx = x4;
		v->sy = y4;
		v->sz = z;
		v->rhw = f_moneopersp * zv;

		if (phd_winxmin > v->sx)
			clip_distance = 1;
		else if (phd_winxmax < v->sx)
			clip_distance = 2;

		if (phd_winymin > v->sy)
			clip_distance += 4;
		else if (phd_winymax < v->sy)
			clip_distance += 8;

		clip[3] = clip_distance;
	}


	static void SetXYZ4Float(D3DTLVERTEX* v, float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float x4, float y4, float z4, short* clip)
	{
		float zv;
		short clip_distance;

		clip_distance = 0;
		zv = f_mpersp / z1;
		v->sx = x1;
		v->sy = y1;
		v->sz = z1;
		v->rhw = f_moneopersp * zv;

		if (phd_winxmin > v->sx)
			clip_distance = 1;
		else if (phd_winxmax < v->sx)
			clip_distance = 2;

		if (phd_winymin > v->sy)
			clip_distance += 4;
		else if (phd_winymax < v->sy)
			clip_distance += 8;

		clip[0] = clip_distance;

		v++;
		clip_distance = 0;
		zv = f_mpersp / z2;
		v->sx = x2;
		v->sy = y2;
		v->sz = z2;
		v->rhw = f_moneopersp * zv;

		if (phd_winxmin > v->sx)
			clip_distance = 1;
		else if (phd_winxmax < v->sx)
			clip_distance = 2;

		if (phd_winymin > v->sy)
			clip_distance += 4;
		else if (phd_winymax < v->sy)
			clip_distance += 8;

		clip[1] = clip_distance;

		v++;
		clip_distance = 0;
		zv = f_mpersp / z3;
		v->sx = x3;
		v->sy = y3;
		v->sz = z3;
		v->rhw = f_moneopersp * zv;

		if (phd_winxmin > v->sx)
			clip_distance = 1;
		else if (phd_winxmax < v->sx)
			clip_distance = 2;

		if (phd_winymin > v->sy)
			clip_distance += 4;
		else if (phd_winymax < v->sy)
			clip_distance += 8;

		clip[2] = clip_distance;

		v++;
		clip_distance = 0;
		zv = f_mpersp / z4;
		v->sx = x4;
		v->sy = y4;
		v->sz = z4;
		v->rhw = f_moneopersp * zv;

		if (phd_winxmin > v->sx)
			clip_distance = 1;
		else if (phd_winxmax < v->sx)
			clip_distance = 2;

		if (phd_winymin > v->sy)
			clip_distance += 4;
		else if (phd_winymax < v->sy)
			clip_distance += 8;

		clip[3] = clip_distance;
	}


	static int ClipLineFloat(float& x1, float& y1, float z1, float& x2, float& y2, float z2, float xMin, float yMin, float w, float h)
	{
		float clip;

		if (z1 < 20 || z2 < 20)
			return 0;

		if (x1 < xMin && x2 < xMin || y1 < yMin && y2 < yMin)
			return 0;

		if (x1 > w && x2 > w || y1 > h && y2 > h)
			return 0;

		if (x1 > w)
		{
			clip = (w - x2) / (x1 - x2);
			x1 = w;
			y1 = ((y1 - y2) * clip + y2);
		}

		if (x2 > w)
		{
			clip = (w - x1) / (x2 - x1);
			x2 = w;
			y2 = ((y2 - y1) * clip + y1);
		}

		if (x1 < xMin)
		{
			clip = (xMin - x1) / (x2 - x1);
			x1 = xMin;
			y1 = ((y2 - y1) * clip + y1);
		}

		if (x2 < xMin)
		{
			clip = (xMin - x2) / (x1 - x2);
			x2 = xMin;
			y2 = ((y1 - y2) * clip + y2);
		}

		if (y1 > h)
		{
			clip = (h - y2) / (y1 - y2);
			y1 = h;
			x1 = ((x1 - x2) * clip + x2);
		}

		if (y2 > h)
		{
			clip = (h - y1) / (y2 - y1);
			y2 = h;
			x2 = ((x2 - x1) * clip + x1);
		}

		if (y1 < yMin)
		{
			clip = (yMin - y1) / (y2 - y1);
			y1 = yMin;
			x1 = ((x2 - x1) * clip + x1);
		}

		if (y2 < yMin)
		{
			clip = (yMin - y2) / (y1 - y2);
			y2 = yMin;
			x2 = ((x1 - x2) * clip + x2);
		}

		return 1;
	}


	ColorRGB ColorRGB::Lerp(const Particles::ColorRGB& C1, const Particles::ColorRGB& C2, float t)
	{
		return ColorRGB(
			Round(C1.R + (C2.R - C1.R) * t),
			Round(C1.G + (C2.G - C1.G) * t),
			Round(C1.B + (C2.B - C1.B) * t)
		);
	}

	ColorRGB ColorRGB::FromHSL(float hue, float sat, float light)
	{
		hue = fmod(hue, 360.0f);
		if (hue < 0)
			hue += 360.0f;

		sat = Clamp(sat, 0.0f, 1.0f);
		light = Clamp(light, 0.0f, 1.0f);

		int hextant = int(hue / 60.0f);
		float fhue = (hue - hextant * 60.0f) / 60.0f;

		float chroma = (1 - abs(2 * light - 1)) * sat;
		float xval = chroma * (1 - abs(fmod(hextant + fhue, 2.0f) - 1));
		float m = light - chroma / 2;
		float r = 0;
		float g = 0;
		float b = 0;

		switch (hextant)
		{
		case 0:
			r = chroma;
			g = xval;
			break;

		case 1:
			r = xval;
			g = chroma;
			break;

		case 2:
			g = chroma;
			b = xval;
			break;

		case 3:
			g = xval;
			b = chroma;
			break;

		case 4:
			r = xval;
			b = chroma;
			break;

		case 5:
			r = chroma;
			b = xval;
		}

		return ColorRGB(Round((r + m) * 255),
			Round((g + m) * 255),
			Round((b + m) * 255));
	}


	MeshParticle* GetFreeMeshPart()
	{
		int i, free;
		MeshParticle* part;

		for (part = &meshParts[nextMesh], free = nextMesh, i = 0; i < MAX_MESHPARTS; ++i)
		{
			if (!part->lifeCounter)
			{
				nextMesh = (free + 1) % MAX_MESHPARTS;

				ClearMemory(part, sizeof(MeshParticle));
				part->emitterIndex = -1;
				part->emitterNode = -1;
				part->scaleX = part->scaleY = part->scaleZ = 16384;
				return part;
			}

			if (free == MAX_MESHPARTS - 1)
			{
				part = &meshParts[0];
				free = 0;
			}
			else
			{
				part++;
				free++;
			}
		}

		int eldest = 0x7FFFFFFF;
		free = 0;
		part = &meshParts[0];

		for (i = 0; i < MAX_MESHPARTS; ++i, ++part)
		{
			if (part->lifeCounter < eldest)
			{
				free = i;
				eldest = part->lifeCounter;
			}
		}

		nextMesh = (free + 1) % MAX_MESHPARTS;

		part = &meshParts[free];
		ClearMemory(part, sizeof(MeshParticle));
		part->emitterIndex = -1;
		part->emitterNode = -1;
		part->scaleX = part->scaleY = part->scaleZ = 16384;
		return part;
	}


	ParticleGroup* GetFreeParticleGroup()
	{
		ParticleGroup* group;

		if (nextPartGroup < MAX_PARTGROUPS)
		{
			group = &partGroups[nextPartGroup];

			group->groupIndex = nextPartGroup;
			group->blendingMode = 2;
			group->spriteSlot = SLOT_DEFAULT_SPRITES;
			group->UpdateS = nullptr;
			group->UpdateM = nullptr;
			nextPartGroup++;

			return group;
		}

		return nullptr;
	}

	WindStreak* GetFreeWindStreak()
	{
		int i, free;
		WindStreak* streak;

		for (streak = &windStreaks[nextStreak], free = nextStreak, i = 0; i < MAX_WINDSTREAKS; ++i)
		{
			if (!streak->lifeCounter)
			{
				nextStreak = (free + 1) % MAX_WINDSTREAKS;

				ClearMemory(streak, sizeof(WindStreak));
				return streak;
			}

			if (free == MAX_WINDSTREAKS - 1)
			{
				streak = &windStreaks[0];
				free = 0;
			}
			else
			{
				streak++;
				free++;
			}
		}

		int eldest = 0x7FFFFFFF;
		free = 0;
		streak = &windStreaks[0];

		for (i = 0; i < MAX_WINDSTREAKS; ++i, ++streak)
		{
			if (streak->lifeCounter < eldest)
			{
				free = i;
				eldest = streak->lifeCounter;
			}
		}

		nextStreak = (free + 1) % MAX_WINDSTREAKS;

		streak = &windStreaks[free];
		ClearMemory(streak, sizeof(WindStreak));
		return streak;
	}


	void ClearParticleSystem()
	{
		ClearMemory(partGroups, sizeof(ParticleGroup) * MAX_PARTGROUPS);
		ClearMemory(spriteParts, sizeof(SpriteParticle) * MAX_SPRITEPARTS);
		ClearMemory(meshParts, sizeof(MeshParticle) * MAX_MESHPARTS);
		ClearMemory(windStreaks, sizeof(WindStreak) * MAX_WINDSTREAKS);

		nextSprite = 0;
		nextMesh = 0;
		nextStreak = 0;

		gameTick = 0;
	}


	void ClearParts()
	{
		ClearMemory(spriteParts, sizeof(SpriteParticle) * MAX_SPRITEPARTS);
		ClearMemory(meshParts, sizeof(MeshParticle) * MAX_MESHPARTS);
		ClearMemory(windStreaks, sizeof(WindStreak) * MAX_WINDSTREAKS);

		nextSprite = 0;
		nextMesh = 0;
		nextStreak = 0;

		for (int i = 0; i < nextPartGroup; ++i)
		{
			partGroups[i].partCount = 0;
		}
	}


	void ClearPartGroup(int partGroup)
	{
		for (int i = 0; i < MAX_SPRITEPARTS; ++i)
		{
			if (spriteParts[i].groupIndex == partGroup)
				spriteParts[i].lifeCounter = 0;
		}

		for (int i = 0; i < MAX_MESHPARTS; ++i)
		{
			if (meshParts[i].groupIndex == partGroup)
				meshParts[i].lifeCounter = 0;
		}

		partGroups[partGroup].partCount = 0;
	}


	void InitParts()
	{
		/*
		for (int i = 0; i < nextPartGroup; i++)
		{
			if (partGroups[i].Init)
				partGroups[i].Init(partGroups[i].groupIndex);
		}
		*/
	}


	void UpdateSprites()
	{
		SpriteParticle* part = &spriteParts[0];

		for (int i = 0; i < MAX_SPRITEPARTS; ++i, ++part)
		{
			if (part->lifeCounter <= 0)
				continue;

			const auto& pgroup = partGroups[part->groupIndex];

			if (part->emitterIndex >= 0 || part->emitterNode >= 0)
			{
				int cutoff = -1;
				// particle attachment cutoff 

				if (pgroup.attach.cutoff > 0)
				{
					cutoff = pgroup.attach.cutoff;
					if (pgroup.attach.random > 1)
						cutoff += (GetRandomDraw() % pgroup.attach.random);
				}

				if ((part->lifeSpan - part->lifeCounter) > cutoff)
					part->Deattach();
			}

			int fadetime = part->lifeSpan;
			int lifefactor = (part->lifeSpan - part->lifeCounter);

			if (part->colorFadeTime)
			{
				if (part->colorFadeTime < 0 && part->lifeSpan >(-part->colorFadeTime))
				{
					fadetime = -part->colorFadeTime;
					lifefactor = -(part->lifeCounter + part->colorFadeTime);
				}
				else if (part->lifeSpan > part->colorFadeTime)
					fadetime = part->colorFadeTime;
			}

			float t = Clamp((lifefactor / float(fadetime)), 0.0f, 1.0f);
			part->colCust = ColorRGB::Lerp(part->colStart, part->colEnd, t);

			part->sizeCust = Round(Lerp(float(part->sizeStart), float(part->sizeEnd), part->Parameter()));

			if (partGroups[part->groupIndex].UpdateS)
			{
				partGroups[part->groupIndex].UpdateS(part);
			}

			part->vel += part->accel;
			part->pos += part->vel;
			part->rot += part->rotVel;

			if (--part->lifeCounter <= 0)
			{
				pgroup.partCount--;
			}
		}
	}


	void UpdateMeshes()
	{
		auto part = &meshParts[0];

		for (int i = 0; i < MAX_MESHPARTS; ++i, ++part)
		{
			if (part->lifeCounter <= 0)
				continue;

			const auto& pgroup = partGroups[part->groupIndex];

			if (part->emitterIndex >= 0 || part->emitterNode >= 0)
			{
				int cutoff = -1;
				// particle attachment cutoff 

				if (pgroup.attach.cutoff > 0)
				{
					cutoff = pgroup.attach.cutoff;
					if (pgroup.attach.random > 1)
						cutoff += (GetRandomDraw() % pgroup.attach.random);
				}

				if ((part->lifeSpan - part->lifeCounter) > cutoff)
					part->Deattach();
			}

			if (partGroups[part->groupIndex].UpdateM)
			{
				partGroups[part->groupIndex].UpdateM(part);
			}

			part->vel += part->accel;
			part->pos += part->vel;
			part->rotX += part->rotVelX;
			part->rotY += part->rotVelY;
			part->rotZ += part->rotVelZ;

			--part->lifeCounter;
		}
	}


	void UpdateStreaks()
	{
		auto streak = &windStreaks[0];

		for (int i = 0; i < MAX_WINDSTREAKS; ++i, ++streak)
		{
			if (streak->lifeCounter > 0)
			{
				auto& points = streak->points;

				long gt = gameTick * 2;

				Vector3f vel = perlin_eff.CurlNoise3D(2048, points[0].x - gt, points[0].y + gt, points[0].z - gt) * 4;

				float angle = ShortToRad(-items[streak->emitterIndex].pos.yRot) + M_PI_2;

				vel.x -= cos(angle) * 50.0f;
				vel.z -= sin(angle) * 50.0f;

				points[0] += vel;

				for (int i = 1; i < STREAK_POINTS; ++i)
				{
					auto difVec = points[i - 1] - points[i];
					float length = difVec.magnitude();
					float correction = 1 - (streak->length / length);
					points[i] += difVec * correction;
				}

				streak->lifeCounter--;
			}
		}
	}


	void UpdateParts()
	{
		InitParts();
		UpdateSprites();
		UpdateMeshes();
		UpdateStreaks();
		gameTick++;
	}


	void DrawSprites()
	{
		phd_PushMatrix();
		phd_TranslateAbs(lara_item->pos.xPos, lara_item->pos.yPos, lara_item->pos.zPos);

		SpriteParticle* part = &spriteParts[0];

		for (int i = 0; i < MAX_SPRITEPARTS; ++i, ++part)
		{
			if (part->lifeCounter <= 0)
				continue;

			const auto& pgroup = partGroups[part->groupIndex];
			float viewCoords[6] = { 0, 0, 0, 0, 0, 0 };

			if (pgroup.drawMode == DrawMode::DRAW_NONE)
				continue;

			if (pgroup.ScreenSpace)
			{
				if (part->pos.x < -1.0f || part->pos.x > 2.0f || part->pos.y < -1.0f || part->pos.y > 2.0f)
				{
					part->lifeCounter = 0;
					continue;
				}

				viewCoords[0] = part->pos.x * phd_winxmax;
				viewCoords[1] = part->pos.y * phd_winxmax;
				viewCoords[2] = part->pos.z + f_mznear;

				if (pgroup.drawMode >= DrawMode::DRAW_LINE)
				{
					float size = part->sizeCust;
					auto vel = part->vel;

					size *= (1.0f / 32.0f);

					if (pgroup.LineIgnoreVel)
						vel = vel.normalized() * (1.0f / 32.0f);

					vel *= size;

					float xf = (part->pos.x - vel.x) * phd_winxmax;
					float yf = (part->pos.y - vel.y) * phd_winxmax;

					viewCoords[3] = xf;
					viewCoords[4] = yf;
					viewCoords[5] = viewCoords[2];
				}
			}
			else
			{
				float x1 = 0, y1 = 0, z1 = 0;
				auto partPos = part->AbsPos();

				x1 = partPos.x;
				y1 = partPos.y;
				z1 = partPos.z;

				x1 -= lara_item->pos.xPos;
				y1 -= lara_item->pos.yPos;
				z1 -= lara_item->pos.zPos;

				if (x1 < -MAX_DRAWDIST || x1 > MAX_DRAWDIST ||
					y1 < -MAX_DRAWDIST || y1 > MAX_DRAWDIST ||
					z1 < -MAX_DRAWDIST || z1 > MAX_DRAWDIST)
				{
					part->lifeCounter = 0;
					continue;
				}

				// convert from world coordinates to screen coordinates
				float result[3] = { 0, 0, 0 };
				float zv = 0;

				result[0] = (phd_mxptr[M00] * x1 + phd_mxptr[M01] * y1 + phd_mxptr[M02] * z1 + phd_mxptr[M03]);
				result[1] = (phd_mxptr[M10] * x1 + phd_mxptr[M11] * y1 + phd_mxptr[M12] * z1 + phd_mxptr[M13]);
				result[2] = (phd_mxptr[M20] * x1 + phd_mxptr[M21] * y1 + phd_mxptr[M22] * z1 + phd_mxptr[M23]);

				zv = f_persp / float(result[2]);
				viewCoords[0] = result[0] * zv + f_centerx;
				viewCoords[1] = result[1] * zv + f_centery;
				viewCoords[2] = result[2] * (1.0f / 16384.0f);
				
				// if particle is a line do world to screen transform for second vertex
				if (pgroup.drawMode >= DrawMode::DRAW_LINE)
				{
					float size = part->sizeCust;
					auto vel = part->vel;

					if (pgroup.LineIgnoreVel)
						vel = vel.normalized(); // ignore speed contribution to particle's size
					else
						size *= (1.0f / 32.0f); // else scale down size
						
					vel *= size;

					if (pgroup.attach.tether == TetherType::TETHER_ROTATING && (part->emitterIndex >= 0 || part->emitterNode >= 0))
					{
						auto item = lara_item;
						if (part->emitterIndex >= 0 && part->emitterIndex < level_items)
							item = &items[part->emitterIndex];

						if (part->emitterNode >= 0) // if attached to specific mesh node of item
						{
							int node = Clamp(part->emitterNode, 0, objects[item->object_number].nmeshes);

							vel = GetJointPos(item, node, Round(vel.x), Round(vel.y), Round(vel.z)) - GetJointPos(item, node, 0, 0, 0);
						}
						else if (part->emitterIndex >= 0) // no mesh node, use item's pos
						{
							vel = RotatePoint3D(vel, item->pos.xRot, item->pos.yRot, item->pos.zRot);
						}
					}

					float x2 = (x1 - vel.x);
					float y2 = (y1 - vel.y);
					float z2 = (z1 - vel.z);

					result[0] = (phd_mxptr[M00] * x2 + phd_mxptr[M01] * y2 + phd_mxptr[M02] * z2 + phd_mxptr[M03]);
					result[1] = (phd_mxptr[M10] * x2 + phd_mxptr[M11] * y2 + phd_mxptr[M12] * z2 + phd_mxptr[M13]);
					result[2] = (phd_mxptr[M20] * x2 + phd_mxptr[M21] * y2 + phd_mxptr[M22] * z2 + phd_mxptr[M23]);

					zv = f_persp / float(result[2]);
					viewCoords[3] = (result[0] * zv + f_centerx);
					viewCoords[4] = (result[1] * zv + f_centery);
					viewCoords[5] = result[2] * (1.0f / 16384.0f);
				}
			}

			long minSize = (pgroup.drawMode) ? 1 : 4;

			// draw the particle to the given screen coordinates
			part->DrawSpritePart(pgroup, viewCoords, minSize);
		}

		phd_PopMatrix();
	}


	void DrawMeshes()
	{
		auto part = &meshParts[0];

		for (int i = 0; i < MAX_MESHPARTS; ++i, ++part)
		{
			if (part->lifeCounter > 0)
			{
				if (part->object > 0 && objects[part->object].loaded)
				{
					part->DrawMeshPart();
				}
			}
		}
	}
	

	Vector3f FindBisector(const Vector3f& v1, const Vector3f& v2)
	{
		Vector3f nv1 = v1 * v2.magnitude();
		Vector3f nv2 = v2 * v1.magnitude();

		return Vector3f(-nv1.y - nv2.y, nv1.x + nv2.x, 0).normalized();
	}


	Vector3f FindOrthogonal(const Vector3f& v)
	{
		Vector3f nv = v.normalized();
		return Vector3f(-nv.y, nv.x, 0);
	}


	void StreakDraw(WindStreak* streak, Vector3f* points)
	{
		int cliplines = 0;

		for (int i = 0; i < STREAK_SEGMENTS; ++i)
		{
			float x1 = points[i].x;
			float y1 = points[i].y;
			float z1 = points[i].z;

			float x2 = points[i + 1].x;
			float y2 = points[i + 1].y;
			float z2 = points[i + 1].z;

			cliplines |= ClipLineFloat(x1, y1, z1, x2, y2, z2, phd_winxmin, phd_winymin, phd_winxmax, phd_winymax) << 1;
		}
			
		if (!cliplines)
			return;

		float segLength = 1.0f / STREAK_SEGMENTS;

		int cR = streak->col.R;
		int cG = streak->col.G;
		int cB = streak->col.B;

		SpriteStruct* sprite = (spriteinfo + objects[SLOT_DEFAULT_SPRITES].mesh_index + streak->spriteIndex);

		if (streak->fadeIn)
		{
			int lifeDif = streak->lifeSpan - streak->lifeCounter;
			if (lifeDif < streak->fadeIn)
			{
				float s = lifeDif / float(streak->fadeIn);
				cR = Round(Lerp(0, cR, s));
				cG = Round(Lerp(0, cG, s));
				cB = Round(Lerp(0, cB, s));
			}
		}

		if (streak->fadeOut)
		{
			if (streak->lifeCounter < streak->fadeOut)
			{
				float s = streak->lifeCounter / float(streak->fadeOut);
				cR = Round(Lerp(0, cR, s));
				cG = Round(Lerp(0, cG, s));
				cB = Round(Lerp(0, cB, s));
			}
		}

		float xprev1 = 0, xprev2 = 0, yprev1 = 0, yprev2 = 0;

		for (int i = 0; i < STREAK_SEGMENTS; ++i)
		{
			if (points[i].z < 0 || points[i].z > 65536)
				continue;

			float lx1 = points[i].x;
			float ly1 = points[i].y;
			float lz1 = points[i].z;
			float lx2 = points[i + 1].x;
			float ly2 = points[i + 1].y;
			float lz2 = points[i + 1].z;
#if 0
			D3DTLVERTEX vl[2];

			long c1 = RGBA(cR, cG, cB, 0xFF);

			vl[0].sx = lx1;
			vl[0].sy = ly1;
			vl[0].rhw = f_mpersp / lz1 * f_moneopersp;
			vl[0].sz = f_a - vl[0].rhw * f_boo;
			vl[0].color = c1;
			vl[0].specular = 0xFF000000;

			vl[1].sx = lx2;
			vl[1].sy = ly2;
			vl[1].rhw = f_mpersp / lz2 * f_moneopersp;
			vl[1].sz = f_a - vl[1].rhw * f_boo;
			vl[1].color = c1;
			vl[1].specular = 0xFF000000;

			(*AddLineSorted)(&vl[0], &vl[1], 6);
#else
			float size1 = phd_persp * streak->size * 0.5f / lz1;
			float size2 = phd_persp * streak->size * 0.5f / lz2;

			float xnext1 = 0, ynext1 = 0, xnext2 = 0, ynext2 = 0;

			if (!i)
			{
				auto bisect = FindOrthogonal(Vector3f(lx1 - lx2, ly1 - ly2, 0));
				float xsize = bisect.x * size1;
				float ysize = bisect.y * size1;

				xprev1 = lx1 + xsize;
				xprev2 = lx1 - xsize;
				yprev1 = ly1 + ysize;
				yprev2 = ly1 - ysize;
			}

			if (i == (STREAK_SEGMENTS - 1))
			{
				auto bisect = FindOrthogonal(Vector3f(lx1 - lx2, ly1 - ly2, 0));
				float xsize = bisect.x * size2;
				float ysize = bisect.y * size2;

				xnext1 = lx2 + xsize;
				xnext2 = lx2 - xsize;
				ynext1 = ly2 + ysize;
				ynext2 = ly2 - ysize;
			}
			else
			{
				auto bisect = FindBisector(Vector3f(lx2 - points[i + 2].x, ly2 - points[i + 2].y, 0), Vector3f(lx1 - lx2, ly1 - ly2, 0));
				float xsize = bisect.x * size2;
				float ysize = bisect.y * size2;

				xnext1 = lx2 + xsize;
				xnext2 = lx2 - xsize;
				ynext1 = ly2 + ysize;
				ynext2 = ly2 - ysize;
			}

			D3DTLVERTEX v[4];
			
			/*float xmax = max(max(xprev1, xprev2), max(xnext1, xnext2));
			float ymax = max(max(yprev1, yprev2), max(ynext1, ynext2));
			float xmin = min(min(xprev1, xprev2), min(xnext1, xnext2));
			float ymin = min(min(yprev1, yprev2), min(ynext1, ynext2));

			

			if (xmin >= phd_winxmin && xmax < phd_winxmax && ymin >= phd_winymin && ymax < phd_winymax)*/
			{
				float z = (lz1 + lz2) * 0.5f;
				//SetXYZ4Float(v, xprev1, yprev1, lz1, xnext1, ynext1, lz2, xnext2, ynext2, lz2, xprev2, yprev2, lz1, clipflags);

				SetXY4Float(v, xprev1, yprev1, xnext1, ynext1, xnext2, ynext2, xprev2, yprev2, z, clipflags);
				//SetXY4Float(v, x0, y0, x1, y1, x2, y2, x3, y3, clipflags);

				xprev1 = xnext1;
				xprev2 = xnext2;
				yprev1 = ynext1;
				yprev2 = ynext2;

				float mz1 = (lz1 - f_mznear - 1024.0f) / 512.0f;
				float mz2 = (lz2 - f_mznear - 1024.0f) / 512.0f;
				if (lz1 > 20480)
					mz1 = (32768 - lz1) / 12288.0f;
				if (lz2 > 20480)
					mz2 = (32768 - lz2) / 12288.0f;

				int col1R = Round(Lerp(0, cR, mz1));
				int col1G = Round(Lerp(0, cG, mz1));
				int col1B = Round(Lerp(0, cB, mz1));
				int col2R = Round(Lerp(0, cR, mz2));
				int col2G = Round(Lerp(0, cG, mz2));
				int col2B = Round(Lerp(0, cB, mz2));

				long c1 = RGBA(col1R, col1G, col1B, 0xFF);
				long c2 = RGBA(col2R, col2G, col2B, 0xFF);

				v[0].color = c1;
				v[1].color = c2;
				v[2].color = c2;
				v[3].color = c1;
				v[0].specular = 0xFF000000;
				v[1].specular = 0xFF000000;
				v[2].specular = 0xFF000000;
				v[3].specular = 0xFF000000;

				TextureStruct tex;

				tex.drawtype = 2;

				tex.tpage = sprite->tpage;

				tex.u1 = Lerp(sprite->x1, sprite->x2, i * segLength);
				tex.v1 = sprite->y1;
				tex.u2 = Lerp(sprite->x1, sprite->x2, (i + 1) * segLength);
				tex.v2 = sprite->y1;
				tex.u3 = tex.u2;
				tex.v3 = sprite->y2;
				tex.u4 = tex.u1;
				tex.v4 = sprite->y2;

				(*AddQuadSorted)(v, 0, 1, 2, 3, &tex, 0);
			}
#endif
		}
	}


	void DrawStreaks()
	{
		phd_PushMatrix();
		phd_TranslateAbs(lara_item->pos.xPos, lara_item->pos.yPos, lara_item->pos.zPos);

		WindStreak* streak = &windStreaks[0];

		for (int i = 0; i < MAX_WINDSTREAKS; ++i, ++streak)
		{
			if (streak->lifeCounter <= 0)
				continue;

			Vector3f pointCoords[STREAK_POINTS];
			
			float x1 = 0, y1 = 0, z1 = 0;

			x1 = streak->points[0].x;
			y1 = streak->points[0].y;
			z1 = streak->points[0].z;

			x1 -= lara_item->pos.xPos;
			y1 -= lara_item->pos.yPos;
			z1 -= lara_item->pos.zPos;

			if (x1 < -MAX_DRAWDIST || x1 > MAX_DRAWDIST ||
				y1 < -MAX_DRAWDIST || y1 > MAX_DRAWDIST ||
				z1 < -MAX_DRAWDIST || z1 > MAX_DRAWDIST)
			{
				streak->lifeCounter = 0;
				continue;
			}

			// convert from world coordinates to screen coordinates
			float result[3] = { 0, 0, 0 };
			float zv = 0;

			for (int n = 0; n < STREAK_POINTS; ++n)
			{
				x1 = streak->points[n].x - lara_item->pos.xPos;
				y1 = streak->points[n].y - lara_item->pos.yPos;
				z1 = streak->points[n].z - lara_item->pos.zPos;

				result[0] = (phd_mxptr[M00] * x1 + phd_mxptr[M01] * y1 + phd_mxptr[M02] * z1 + phd_mxptr[M03]);
				result[1] = (phd_mxptr[M10] * x1 + phd_mxptr[M11] * y1 + phd_mxptr[M12] * z1 + phd_mxptr[M13]);
				result[2] = (phd_mxptr[M20] * x1 + phd_mxptr[M21] * y1 + phd_mxptr[M22] * z1 + phd_mxptr[M23]);
				zv = f_persp / float(result[2]);

				pointCoords[n].x = result[0] * zv + f_centerx;
				pointCoords[n].y = result[1] * zv + f_centery;
				pointCoords[n].z = result[2] * (1.0f / 16384.0f);
			}

			StreakDraw(streak, pointCoords);
		}

		phd_PopMatrix();
	}


	void DrawParts()
	{
		DrawSprites();
		DrawMeshes();
		DrawStreaks();
	}


	// ************ Base Particle public functions  ****************


	float BaseParticle::Parameter()
	{
		return (lifeSpan - lifeCounter) / float(lifeSpan);
	}


	void BaseParticle::LimitSpeed(float speedMax)
	{
		float speed = vel.magnitude();
		if (speed > speedMax)
			vel *= (speedMax / speed);
	}


	Vector3f BaseParticle::CheckDistanceEmitter(int dist, int val)
	{
		Vector3f c;

		if (emitterIndex >= 0 && emitterIndex < level_items)
		{
			auto item = &items[emitterIndex];

			Vector3f posItem(item->pos.xPos, item->pos.yPos, item->pos.zPos);

			auto p = Round(AbsPos() - posItem);

			if (p.x < -dist)
				c.x += val;
			else if (p.x > dist)
				c.x -= val;

			if (p.y < -dist)
				c.y += val;
			else if (p.y > dist)
				c.y -= val;

			if (p.z < -dist)
				c.z += val;
			else if (p.z > dist)
				c.z -= val;
		}

		return c;
	}


	Vector3f BaseParticle::AbsPos()
	{
		auto absPos = pos;
		const auto& pgroup = partGroups[groupIndex];

		if (pgroup.attach.tether && (emitterIndex >= 0 || emitterNode >= 0)) // particle attachment
		{
			auto item = lara_item;
			if (emitterIndex >= 0 && emitterIndex < level_items)
				item = &items[emitterIndex];

			auto relPos = pos;

			if (emitterNode >= 0) // if attached to specific mesh node of item
			{
				int node = Clamp(emitterNode, 0, objects[item->object_number].nmeshes);

				if (pgroup.attach.tether == TetherType::TETHER_ROTATING)
					relPos = GetJointPos(item, node, Round(relPos.x), Round(relPos.y), Round(relPos.z));
				else
					relPos += GetJointPos(item, node, 0, 0, 0);
			}
			else if (emitterIndex >= 0) // no mesh node, use item's pos
			{
				if (pgroup.attach.tether == TetherType::TETHER_ROTATING)
				{
					relPos = RotatePoint3D(relPos, item->pos.xRot, item->pos.yRot, item->pos.zRot);
				}

				relPos.x += item->pos.xPos;
				relPos.y += item->pos.yPos;
				relPos.z += item->pos.zPos;
			}

			absPos = relPos;
		}

		return absPos;
	}


	void BaseParticle::Attach(int itemIndex, int node)
	{
		auto item = lara_item;
		if (itemIndex >= 0 && itemIndex < level_items)
			item = &items[itemIndex];
		else
			itemIndex = -1;

		const auto& pgroup = partGroups[groupIndex];
		Vector3f relPos;

		if (pgroup.attach.tether)
		{
			if (node >= 0) // if attached to specific mesh node of item
			{
				node = Clamp(node, 0, objects[item->object_number].nmeshes);

				if (pgroup.attach.tether == TetherType::TETHER_ROTATING)
					relPos = GetJointPos(item, node, Round(relPos.x), Round(relPos.y), Round(relPos.z));
				else
					relPos += GetJointPos(item, node, 0, 0, 0);
			}
			else if (itemIndex >= 0) // no mesh node, use item's pos
			{
				if (pgroup.attach.tether == TetherType::TETHER_ROTATING)
				{
					relPos = RotatePoint3D(relPos, item->pos.xRot, item->pos.yRot, item->pos.zRot);
				}

				relPos.x += item->pos.xPos;
				relPos.y += item->pos.yPos;
				relPos.z += item->pos.zPos;

				node = -1;
			}
		}

		pos = (AbsPos() - relPos);
		emitterIndex = itemIndex;
		emitterNode = node;
	}


	void BaseParticle::Deattach()
	{
		pos = AbsPos();

		emitterIndex = -1;
		emitterNode = -1;
	}


	bool BaseParticle::CollideWalls(float rebound)
	{
		if (emitterIndex < 0 && emitterNode < 0)
		{
			rebound = Clamp(rebound, 0.0f, 1.0f);

			auto testp = Round(pos + vel);
			auto p = Round(pos);

			if ((p.x >> 10) != (testp.x >> 10) || (p.z >> 10) != (testp.z >> 10))
			{
				short tRoom = roomIndex;
				int wallCurrentPos = TestForWall(p.x, p.y, p.z, &tRoom);

				if (roomIndex != tRoom)
					roomIndex = tRoom;

				if (!wallCurrentPos)
				{
					bool wallCollision = false;

					if (TestForWall(p.x, p.y, testp.z, &tRoom))
					{
						vel.z = -(vel.z * rebound);
						wallCollision = true;
					}
					if (TestForWall(testp.x, p.y, p.z, &tRoom))
					{
						vel.x = -(vel.x * rebound);
						wallCollision = true;
					}

					return wallCollision;
				}
			}
		}

		return false;
	}


	bool BaseParticle::CollideFloors(float rebound, float minBounce, int collMargin, bool accurate)
	{
		if (emitterIndex < 0 && emitterNode < 0)
		{
			rebound = Clamp(rebound, 0.0f, 1.0f);

			auto testp = Round(pos + vel);
			short tRoom = roomIndex;

			Tr4FloorInfo* floor = (Tr4FloorInfo*)GetFloor(testp.x, testp.y, testp.z, &tRoom);

			if (roomIndex != tRoom)
				roomIndex = tRoom;

			int height = GetHeight(floor, testp.x, testp.y, testp.z);
			int fh = height - collMargin;
			int ch = GetCeiling(floor, testp.x, testp.y, testp.z);

			if (height != (-0x7F00) && (testp.y >= fh || testp.y <= ch))
			{
				if (abs(vel.y) > minBounce)
				{
					if (testp.y > fh && accurate)
					{
						auto n = GetSlopeNormal(floor, testp.x, testp.y, testp.z);

						// get reflection vector off slope surface
						auto reflected = vel - (n * vel.dot(n) * 2);
						vel = reflected * rebound;
					}
					else
						vel.y = -(vel.y * rebound);

				}
				else if (testp.y >= fh)
				{
					pos.y = fh;
					vel.y = 0;
				}
					

				if (pos.y > fh)
					pos.y = fh;
				else if (pos.y < ch)
					pos.y = ch;

				return true;
			}
		}

		return false;
	}


	bool BaseParticle::CollidedWithItem(Tr4ItemInfo* item, int radius)
	{
		if (item && emitterIndex < 0 && emitterNode < 0)
		{
			StrBoxCollisione* bounds = GetBoundsAccurate((StrItemTr4*)item);
			auto p = Round(pos);

			int y = p.y - item->pos.yPos;
			if (y >= bounds->MinY - radius && y <= bounds->MaxY + radius)
			{
				int dx = p.x - item->pos.xPos;
				int dz = p.z - item->pos.zPos;
				float angle = ShortToRad(item->pos.yRot);
				float s = sin(angle);
				float c = cos(angle);
				int x = int(dx * c - dz * s);
				int z = int(dx * s + dz * c);

				if (x >= bounds->MinX - radius &&
					x <= radius + bounds->MaxX &&
					z >= bounds->MinZ - radius &&
					z <= radius + bounds->MaxZ) {
					return true;
				}
			}
		}

		return false;
	}


	Vector3f BaseParticle::FollowTarget(const Vector3f& v, float factor, float maxSpeed)
	{
		auto dirVect = (v - pos);

		if (dirVect.magnitude() > maxSpeed)
			dirVect *= maxSpeed / dirVect.magnitude();

		if (factor < 0)
		{
			dirVect = -dirVect;
			factor = -factor;
		}

		return dirVect * factor;
	}


	bool BaseParticle::HomingTarget(Tr4ItemInfo* item, int targetNode, float homingFactor, float homingAccel, bool predict)
	{
		if (!item)
			return false;

		targetNode = Clamp(targetNode, 0, objects[item->object_number].nmeshes);

		auto targetPos = GetJointPos(item, targetNode, 0, 0, 0);

		if (predict)
		{
			float angle = ShortToRad(item->pos.yRot);
			float s = sin(angle);
			float c = cos(angle);

			Vector3f heading(item->speed * s, item->fallspeed, item->speed * c);

			float time = RealDist(pos, targetPos) / vel.magnitude();

			targetPos += (heading * time);
		}

		auto dir = (targetPos - pos).normalized();

		if (homingFactor < 0)
		{
			dir = -dir;
			homingFactor = -homingFactor;
		}

		auto slerpVel = vel.normalized().slerp(dir, homingFactor);
		vel = slerpVel * (vel.magnitude() + homingAccel);

		return true;
	}


	Vector3f BaseParticle::SplinePos(const Vector3f v[], int arrSize, float t)
	{
		if (arrSize < 2)
			return pos;

		if (arrSize == 2)
			return v[0].lerp(v[1], t);

		int div = arrSize - 1;
		int index = int(t * div);
		float tp = (t * div) - index;

		Vector3f splinePos;

		if (!index)
			splinePos = SplineStart(v[index], v[index + 1], v[index + 2], tp);
		else if (index + 1 == div)
			splinePos = SplineEnd(v[index - 1], v[index], v[index + 1], tp);
		else
			splinePos = Spline(v[index - 1], v[index], v[index + 1], v[index + 2], tp);

		return splinePos;
	}


	Vector3f BaseParticle::SplineVel(const Vector3f v[], int arrSize, float t)
	{
		Vector3f splineVel;
		int div = arrSize - 1;

		if (div < 1)
			return splineVel;

		if (div == 1)
			return (v[1] - v[0]) * (1.0f / lifeSpan);

		int index = int(t * div);
		float tp = (t * div) - index;

		if (!index)
			splineVel = SplineStartDerivative(v[index], v[index + 1], v[index + 2], tp);
		else if (index + 1 == div)
			splineVel = SplineEndDerivative(v[index - 1], v[index], v[index + 1], tp);
		else
			splineVel = SplineDerivative(v[index - 1], v[index], v[index + 1], v[index + 2], tp);

		return splineVel * (float(div) / lifeSpan);
	}


	Vector3f BaseParticle::AvoidRoomGeometry(int wallMargin, int floorMargin, float factor)
	{
		Vector3f v;
		auto p = Round(pos);

		short room = roomIndex;

		if (TestForWall(p.x + wallMargin, p.y, p.z, &room))
			v.x -= factor;

		if (TestForWall(p.x - wallMargin, p.y, p.z, &(room = roomIndex)))
			v.x += factor;

		if (TestForWall(p.x, p.y, p.z + wallMargin, &(room = roomIndex)))
			v.z -= factor;

		if (TestForWall(p.x, p.y, p.z - wallMargin, &(room = roomIndex)))
			v.z += factor;

		Tr4FloorInfo* floor = (Tr4FloorInfo*)GetFloor(p.x, p.y, p.z, &roomIndex);

		int h = GetHeight(floor, p.x, p.y, p.z) - p.y;
		if (h < floorMargin)
			v.y -= factor;

		int c = p.y - GetCeiling(floor, p.x, p.y, p.z);
		if (c < floorMargin)
			v.y += factor;

		return v;
	}


	Vector3f BaseParticle::AvoidItem(Tr4ItemInfo* item, float radius, float factor)
	{
		Vector3f v;

		Vector3f ipos(item->pos.xPos, item->pos.yPos, item->pos.zPos);

		if (SimpleDist(ipos, pos) < radius)
		{
			float dist = RealDist(ipos, pos);

			if (dist < radius)
				v = (ipos - pos) * (1 / dist) * factor;
		}

		return v;
	}

	// ************ Sprite Particle functions  ****************

	SpriteParticle* GetFreeSpritePart()
	{
		int i, free;
		SpriteParticle* part;

		for (part = &spriteParts[nextSprite], free = nextSprite, i = 0; i < MAX_SPRITEPARTS; ++i)
		{
			if (!part->lifeCounter)
			{
				nextSprite = (free + 1) % MAX_SPRITEPARTS;

				ClearMemory(part, sizeof(SpriteParticle));
				part->emitterIndex = -1;
				part->emitterNode = -1;
				return part;
			}

			if (free == MAX_SPRITEPARTS - 1)
			{
				part = &spriteParts[0];
				free = 0;
			}
			else
			{
				part++;
				free++;
			}
		}

		int eldest = 0x7FFFFFFF;
		free = 0;
		part = &spriteParts[0];

		for (i = 0; i < MAX_SPRITEPARTS; ++i, ++part)
		{
			if (part->lifeCounter < eldest)
			{
				free = i;
				eldest = part->lifeCounter;
			}
		}

		nextSprite = (free + 1) % MAX_SPRITEPARTS;

		part = &spriteParts[free];
		ClearMemory(part, sizeof(SpriteParticle));
		part->emitterIndex = -1;
		part->emitterNode = -1;
		return part;
	}

	void SpriteParticle::Animate(int startSprite, int endSprite, int frameRate)
	{
		int length = endSprite - startSprite;
		if (!frameRate || length <= 0)
			return;

		int usedSprite = startSprite;

		if (frameRate > 0)
			usedSprite += ((lifeSpan - lifeCounter) / frameRate) % (length + 1);
		else
		{
			// repeat animation cycle (-framerate) times through particle lifespan
			int numCycles = -frameRate;
			usedSprite = int(Lerp(float(usedSprite), float(usedSprite + length), fmod(numCycles * Parameter(), 1.0f)));
		}

		spriteIndex = (uchar)(usedSprite);
	}


	Vector3f SpriteParticle::BoidDistanceRule(float radius, float factor)
	{
		Vector3f v;

		for (int i = 0; i < MAX_SPRITEPARTS; ++i)
		{
			auto part = &spriteParts[i];
			if (part->lifeCounter <= 0 || part == this)
				continue;

			if (part->groupIndex != groupIndex)
				continue;

			if (SimpleDist(pos, part->pos) < radius)
			{
				if (CheckDistFast(pos, part->pos, radius) < 0)
					v -= (part->pos - pos);
			}
		}

		return v * factor;
	}


	Vector3f SpriteParticle::BoidGatherRule(float radius, float factor)
	{
		Vector3f v;

		int neighbors = 0;
		for (int i = 0; i < MAX_SPRITEPARTS; ++i)
		{
			auto part = &spriteParts[i];
			if (part->lifeCounter <= 0 || part == this)
				continue;

			if (part->groupIndex != groupIndex)
				continue;

			if (SimpleDist(pos, part->pos) < radius)
			{
				if (CheckDistFast(pos, part->pos, radius) < 0)
				{
					v += part->pos;
					++neighbors;
				}
			}
		}

		if (neighbors)
		{
			v *= (1.0f / neighbors);
			v = (v - pos) * factor;
		}

		return v;
	}


	Vector3f SpriteParticle::BoidVelocityRule(float radius, float factor)
	{
		Vector3f v;

		int neighbors = 0;
		for (int i = 0; i < MAX_SPRITEPARTS; ++i)
		{
			auto part = &spriteParts[i];
			if (part->lifeCounter <= 0 || part == this)
				continue;

			if (part->groupIndex != groupIndex)
				continue;

			if (SimpleDist(pos, part->pos) < radius)
			{
				if (CheckDistFast(pos, part->pos, radius) < 0)
				{
					v += part->vel;
					++neighbors;
				}
			}
		}

		if (neighbors)
		{
			v *= (1.0f / neighbors);
			v = (v - vel) * factor;
		}

		return v;
	}


	float FindMin(float f1, float f2, float f3, float f4)
	{
		float f = 1000000000.0f;

		if (f1 < f)
			f = f1;
		if (f2 < f)
			f = f2;
		if (f3 < f)
			f = f3;
		if (f4 < f)
			f = f4;

		return f;
	}


	float FindMax(float f1, float f2, float f3, float f4)
	{
		float f = -1000000000.0f;

		if (f1 > f)
			f = f1;
		if (f2 > f)
			f = f2;
		if (f3 > f)
			f = f3;
		if (f4 > f)
			f = f4;

		return f;
	}


	void SpriteParticle::DrawSpritePart(const ParticleGroup& pgroup, float* const view, long smallest_size)
	{
		float z1 = view[2];

		if (z1 <= 0)
			return;
		if (z1 >= 20480)
			return;

		int cR = colCust.R;
		int cG = colCust.G;
		int cB = colCust.B;

		if (fadeIn)
		{
			int lifeDif = lifeSpan - lifeCounter;
			if (lifeDif < fadeIn)
			{
				float s = lifeDif / float(fadeIn);
				cR = Round(Lerp(0, cR, s));
				cG = Round(Lerp(0, cG, s));
				cB = Round(Lerp(0, cB, s));
			}
		}

		if (fadeOut)
		{
			if (lifeCounter < fadeOut)
			{
				float s = lifeCounter / float(fadeOut);
				cR = Round(Lerp(0, cR, s));
				cG = Round(Lerp(0, cG, s));
				cB = Round(Lerp(0, cB, s));
			}
		}

		if (pgroup.drawMode >= DrawMode::DRAW_LINE) // line or arrow
		{
			float x1 = view[0];
			float y1 = view[1];
			float z1 = view[2];
			float x2 = view[3];
			float y2 = view[4];
			float z2 = view[5];

			if (ClipLineFloat(x1, y1, z1, x2, y2, z2, phd_winxmin, phd_winymin, phd_winxmax, phd_winymax))
			{
				D3DTLVERTEX v[2];

				long c1 = RGBA(cR, cG, cB, 0xFF);
				long c2 = RGBA(cR >> 2, cG >> 2, cB >> 2, 0xFF);

				v[0].sx = x1;
				v[0].sy = y1;
				v[0].rhw = f_mpersp / z1 * f_moneopersp;
				v[0].sz = f_a - v[0].rhw * f_boo;
				v[0].color = c1;
				v[0].specular = 0xFF000000;

				v[1].sx = x2;
				v[1].sy = y2;
				v[1].rhw = f_mpersp / z2 * f_moneopersp;
				v[1].sz = f_a - v[1].rhw * f_boo;
				v[1].color = c2;
				v[1].specular = 0xFF000000;

				(*AddLineSorted)(&v[0], &v[1], 6);

				if (pgroup.drawMode == DrawMode::DRAW_ARROW)
				{
					float dx = (x2 - x1) * 0.5f;
					float dy = (y2 - y1) * 0.5f;

					float sx = dx * SIN_PI6;
					float cx = dx * COS_PI6;
					float sy = dy * SIN_PI6;
					float cy = dy * COS_PI6;

					v[1].rhw = (v[0].rhw + v[1].rhw) * 0.5f;
					v[1].sz = (v[0].sz + v[1].sz) * 0.5f;

					v[1].sx = x1 + cx + sy;
					v[1].sy = y1 + cy - sx;

					(*AddLineSorted)(&v[0], &v[1], 6);

					v[1].sx = x1 + cx - sy;
					v[1].sy = y1 + cy + sx;

					(*AddLineSorted)(&v[0], &v[1], 6);
				}
			}
		}
		else
		{
			float size;

			if (pgroup.ScreenSpace)
				size = sizeCust;
			else
			{
				size = phd_persp * sizeCust / float(z1);

				if (size < smallest_size)
					size = smallest_size;
			}

			float xsize = 0.5f, ysize = 0.5f;

			if (skew)
			{
				xsize = (skew + 32768.0f) / 65536.0f;
				ysize = 1.0f - xsize;
			}

			float s1h = size * xsize;
			float s2h = size * ysize;

			float xmin = view[0] + s1h;
			float xmax = view[0] - s1h;
			float ymin = view[1] + s2h;
			float ymax = view[1] - s2h;

			if (xmin >= phd_winxmin && xmax < phd_winxmax && ymin >= phd_winymin && ymax < phd_winymax)
			{
				D3DTLVERTEX v[4];
				float x1, y1, x2, y2, x3, y3, x4, y4;

				if (rot)
				{
					float angle = ShortToRad(rot);
					float s = sinf(angle);
					float c = cosf(angle);

					float sx1 = (-s1h * s);
					float sx2 = (s1h * s);

					float sy1 = (-s2h * s);
					float sy2 = (s2h * s);

					float cx1 = (-s1h * c);
					float cx2 = (s1h * c);

					float cy1 = (-s2h * c);
					float cy2 = (s2h * c);

					x1 = (sx1 - cy1 + view[0]);
					x2 = (sx2 - cy1 + view[0]);
					x3 = (sx2 - cy2 + view[0]);
					x4 = (sx1 - cy2 + view[0]);

					y1 = (cx1 + sy1 + view[1]);
					y2 = (cx2 + sy1 + view[1]);
					y3 = (cx2 + sy2 + view[1]);
					y4 = (cx1 + sy2 + view[1]);
				}
				else
				{
					x1 = x4 = view[0] - s1h;
					x2 = x3 = view[0] + s1h;

					y1 = y2 = view[1] - s2h;
					y3 = y4 = view[1] + s2h;
				}

				SetXY4Float(v, x1, y1, x2, y2, x3, y3, x4, y4, z1, clipflags);

				long c1 = RGBA(cR, cG, cB, 0xFF);

				v[0].color = c1;
				v[1].color = c1;
				v[2].color = c1;
				v[3].color = c1;
				v[0].specular = 0xFF000000;
				v[1].specular = 0xFF000000;
				v[2].specular = 0xFF000000;
				v[3].specular = 0xFF000000;

				TextureStruct tex;

				tex.drawtype = pgroup.blendingMode;

				if (!pgroup.drawMode)
				{
					int spriteNum = Clamp(spriteIndex, 0, (-objects[pgroup.spriteSlot].nmeshes) - 1);
					SpriteStruct* sprite = (spriteinfo + objects[pgroup.spriteSlot].mesh_index + spriteNum);

					tex.tpage = sprite->tpage;
					tex.u1 = sprite->x1;
					tex.v1 = sprite->y1;
					tex.u2 = sprite->x2;
					tex.v2 = sprite->y1;
					tex.u3 = sprite->x2;
					tex.v3 = sprite->y2;
					tex.u4 = sprite->x1;
					tex.v4 = sprite->y2;
				}
				else
				{
					tex.flag = 0;
					tex.tpage = 0;
				}

				(*AddQuadSorted)(v, 0, 1, 2, 3, &tex, 0);
			}
		}
	}

	// ************ Mesh Particle public functions  ****************

	void MeshParticle::Animate(int startMesh, int endMesh, int frameRate)
	{
		int length = endMesh - startMesh;
		if (!frameRate || length <= 0)
			return;

		int usedMesh = startMesh;

		if (frameRate > 0)
			usedMesh += ((lifeSpan - lifeCounter) / frameRate) % (length + 1);
		else
		{
			// repeat animation cycle (-framerate) times through particle lifespan
			int numCycles = -frameRate;
			usedMesh = int(Lerp(float(usedMesh), float(endMesh), fmod(numCycles * Parameter(), 1.0f)));
		}

		mesh = (uchar)(usedMesh);
	}


	void MeshParticle::AlignToVel(float factor, bool invert)
	{
		float correction = float(M_PI_2);

		float phi = -(atan2(vel.z, vel.x) - correction);
		float theta = atan2(sqrt(vel.x * vel.x + vel.z * vel.z), vel.y) - correction;

		if (invert)
		{
			theta = -theta;
			phi += float(M_PI);
		}

		int dy = GetOrientDiff(rotY, RadToShort(phi));
		int dx = GetOrientDiff(rotX, RadToShort(theta));

		factor = Clamp(factor, 0.0f, 1.0f);

		rotVelY = Round(dy * factor);
		rotVelX = Round(dx * factor);
	}


	void MeshParticle::AlignToTarget(const Vector3f& v, float factor, bool invert)
	{
		float correction = float(M_PI_2);

		auto dir = v - pos;
		float phi = -(atan2(dir.z, dir.x) - correction);
		float theta = atan2(sqrt(dir.x * dir.x + dir.z * dir.z), dir.y) - correction;

		if (invert)
		{
			theta = -theta;
			phi += float(M_PI);
		}

		int dy = GetOrientDiff(rotY, RadToShort(phi));
		int dx = GetOrientDiff(rotX, RadToShort(theta));

		factor = Clamp(factor, 0.0f, 1.0f);

		rotVelY = Round(dy * factor);
		rotVelX = Round(dx * factor);
	}


	void MeshParticle::Shatter()
	{
		int meshindex = objects[object].mesh_index + (mesh * 2);
		short** meshpp = &meshes[meshindex];
		ShatterItem.meshp = *meshpp;

		auto p = Round(pos);
		ShatterItem.sphere.x = p.x;
		ShatterItem.sphere.y = p.y;
		ShatterItem.sphere.z = p.z;
		ShatterItem.yRot = rotY;

		ShatterItem.bit = 0;
		ShatterItem.flags = 0;
		ShatterObject(&ShatterItem, 0, -32, roomIndex, 0);

		lifeCounter = 1;
	}


	Vector3f MeshParticle::BoidDistanceRule(float radius, float factor)
	{
		Vector3f v;

		for (int i = 0; i < MAX_MESHPARTS; ++i)
		{
			auto part = &meshParts[i];
			if (part->lifeCounter <= 0 || part == this)
				continue;

			if (part->groupIndex != groupIndex)
				continue;

			if (SimpleDist(pos, part->pos) < radius)
			{
				if (CheckDistFast(pos, part->pos, radius) < 0)
					v -= (part->pos - pos);
			}
		}

		return v * factor;
	}


	Vector3f MeshParticle::BoidGatherRule(float radius, float factor)
	{
		Vector3f v;

		int neighbors = 0;
		for (int i = 0; i < MAX_MESHPARTS; ++i)
		{
			auto part = &meshParts[i];
			if (part->lifeCounter <= 0 || part == this)
				continue;

			if (part->groupIndex != groupIndex)
				continue;

			if (SimpleDist(pos, part->pos) < radius)
			{
				if (CheckDistFast(pos, part->pos, radius) < 0)
				{
					v += part->pos;
					++neighbors;
				}
			}
		}

		if (neighbors)
		{
			v *= (1.0f / neighbors);
			v = (v - pos) * factor;
		}

		return v;
	}


	Vector3f MeshParticle::BoidVelocityRule(float radius, float factor)
	{
		Vector3f v;

		int neighbors = 0;
		for (int i = 0; i < MAX_MESHPARTS; ++i)
		{
			auto part = &meshParts[i];
			if (part->lifeCounter <= 0 || part == this)
				continue;

			if (part->groupIndex != groupIndex)
				continue;

			if (SimpleDist(pos, part->pos) < radius)
			{
				if (CheckDistFast(pos, part->pos, radius) < 0)
				{
					v += part->vel;
					++neighbors;
				}
			}
		}

		if (neighbors)
		{
			v *= (1.0f / neighbors);
			v = (v - vel) * factor;
		}

		return v;
	}


	void MeshParticle::DrawMeshPart()
	{
		phd_vector projPos = Round(AbsPos());

		const auto& pgroup = partGroups[groupIndex];

		int testx = projPos.x - lara_item->pos.xPos;
		int testy = projPos.y - lara_item->pos.yPos;
		int testz = projPos.z - lara_item->pos.zPos;

		if (testx < -MAX_DRAWDIST || testx > MAX_DRAWDIST ||
			testy < -MAX_DRAWDIST || testy > MAX_DRAWDIST ||
			testz < -MAX_DRAWDIST || testz > MAX_DRAWDIST)
		{
			lifeCounter = 0;
			return;
		}

		phd_PushMatrix();
		phd_TranslateAbs(projPos.x, projPos.y, projPos.z);

		if (phd_mxptr[M23] > phd_znear && phd_mxptr[M23] < phd_zfar)
		{
			phd_RotYXZ(rotY, rotX, rotZ);

			if (scaleX != 16384 || scaleY != 16384 || scaleZ != 16384)
			{

				StrTripla sVect = { scaleX, scaleY, scaleZ };

				ScaleCurrentMatrix(&sVect);
			}

			int meshindex = objects[object].mesh_index + (mesh * 2);
			auto meshpp = &meshes[meshindex];

			long oldAlpha = GlobalAlpha;

			if (transparency)
				GlobalAlpha = (255 - transparency) << 24;

			auto it_backup = current_item;
			current_item = lara_item;
			/* TODO: allow static as well as dynamic lighting for meshparts */


			phd_PutPolygons(*meshpp, -1);
			GlobalAlpha = oldAlpha;
			current_item = it_backup;
		}

		phd_PopMatrix();
	}


	int RandomInt()
	{
		return mtRand();
	}
};
