#pragma once
namespace Particles
{ 
	struct SpriteParticle;
	struct MeshParticle;
}

struct Vector3f;

extern PerlinNoise perlin_eff;

void ClearEffect(int group, short emitter_number);

void GlitterInit(int group, int count);
void GlitterUpdate(Particles::SpriteParticle* part);

void FireflyInit(int group, short emitter_number);
void FireflyUpdate(Particles::SpriteParticle* part);

void PixieInit(int group, short emitter_number);
void PixieUpdate(Particles::SpriteParticle* part);

void EmberInit(int group, short emitter_number);
void EmberUpdate(Particles::SpriteParticle* part);

void LavaInit(int group, const Vector3f& v, bool bomb = false);

void PyroclastInit(int group, int x, int y, int z, short room);
void PyroclastUpdate(Particles::MeshParticle* part);

void PollenInit(int group, short emitter_number);
void PollenInit2(int group, short emitter_number);
void WindstreaksWide(short emitter_number);
void PollenUpdate(Particles::SpriteParticle* part);

void WindStreakInit(short emitter_number, int xrand, int yrand, int zrand);