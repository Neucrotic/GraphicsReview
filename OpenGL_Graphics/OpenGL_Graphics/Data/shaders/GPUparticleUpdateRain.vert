#version 410

layout(location=0) in vec3 Position;
layout(location=1) in vec3 Velocity;
layout(location=2) in float Lifetime;
layout(location=3) in float Lifespan;

out vec3 position;
out vec3 velocity;
out float lifetime;
out float lifespan;

uniform float time;
uniform float deltaTime;
uniform float lifeMin;
uniform float lifeMax;
uniform vec3 emitterPosition;

const float INVERSE_MAX_UINT = 1.0f / 4294967295.0f;

float praiseRNGesus(uint seed, float range)
{
	uint i = (seed ^12345391u) * 2654435769u;
	i ^= (i << 6u) ^(i >> 26u);
	i *= 2654435769u;
	i += (i << 5u) ^(i >> 12u);
	return float(range * i) * INVERSE_MAX_UINT;
}

void main()
{
	position = Position + Velocity * deltaTime;
	velocity = Velocity;
	lifetime = Lifetime + deltaTime;
	lifespan = Lifespan;
	
	//if the particle is dead emit a new one
	if (lifetime > lifespan)
	{
		vec3 randPos;
		uint posSeed = uint(time * 1000.0) + uint(gl_VertexID);
		randPos.x = praiseRNGesus(posSeed++, 20);
		randPos.y = 0;
		randPos.z = praiseRNGesus(posSeed++, 20);
		
		uint seed = uint(time * 1000.0) + uint(gl_VertexID);
		float speed = 10;
		velocity.x = 2.0f;
		velocity.y = praiseRNGesus(seed++, -1000);
		velocity.z = 0.5f;
		velocity = normalize(velocity) * speed;
		position = emitterPosition + randPos;
		lifetime = 0;
		lifespan = praiseRNGesus(seed++, lifeMax - lifeMin) + lifeMin;
	}
}