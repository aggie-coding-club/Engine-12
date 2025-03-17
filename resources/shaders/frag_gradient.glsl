#version 450 core

const float PI = 3.1415;

layout(location = 0) out vec4 fragColor;


uniform vec2 _ScreenParams;
uniform vec3 _WorldSpaceCamPos;
uniform vec3 viewParams;
uniform mat4x4 camLocalToWorldMatrix;
uniform int useSky;
uniform float sunFocus;
uniform float sunIntensity;
uniform vec3 sunColor;

struct Ray
{
	vec3 origin;
	vec3 dir;
	vec3 invDir;
};

struct HitInfo
{
	bool didHit;
	float dst;
	vec3 hitPoint;
	vec3 normal;
};

// Hit Info Struct
struct TriangleHitInfo
{
	bool didHit;                        // Did the ray hit?
	float dst;                          // distance the ray had to travel
	vec3 hitPoint;                    // The point that the ray hit the object
	vec3 normal;                      // The normal of the object at that point
	vec2 uv;                     // Location of hit on UV
	int triIndex;
};

struct Triangle
{
	vec3 posA, posB, posC;
	vec3 normalA, normalB, normalC;
	vec2 uvA, uvB, uvC;
};

struct Material
{
	int textureIndex;
	int normalIndex;
	float emissionStrength;
	float specularProbability;
	float smoothness;
	float refractIndx;
	float transparency;
	vec4 color;
	vec4 emissionColor;
	vec4 specularColor;
};

struct Model
{
	int nodeOffset;
	int triOffset;
	mat4x4 worldToLocalMatrix;
	mat4x4 localToWorldMatrix;
	Material material;
};

struct BVHNode
{
	vec3 boundsMin;
	vec3 boundsMax;
	int startIndex;
	int triangleCount;
};

struct ModelHitInfo
{
	bool didHit;
	vec3 normal;
	vec3 hitPoint;
	vec2 uv;
	float dst;
	Material material;
};

struct Light
{
	vec4 emissionColor;
	float emissionStrength;
	vec3 modelPosition;
	int modelIndex;
};

layout(std430, binding = 0) buffer ModelBuffer {
	Model models[];
};

layout(std430, binding = 1) buffer TriangleBuffer {
	Triangle triangles[];
};

layout(std430, binding = 4) buffer TriangleIndexesBuffer {
	int triIndex[];
};

layout(std430, binding = 2) buffer NodeBuffer {
	BVHNode nodes[];
};

layout(std430, binding = 3) buffer LightBuffer {
	Light lights[];
};

uniform int triangleCount;
uniform int modelCount;
uniform int lightCount;

float lerp(float start, float end, float time)
{
	return start + (end - start) * time;
}

vec3 lerp(vec3 start, vec3 end, float time)
{
	return vec3(lerp(start.x, end.x, time), lerp(start.y, end.y, time), lerp(start.z, end.z, time));
}

float RandomValue(inout uint state)
{
	state = state * uint(747796405) + uint(2891336453);
	uint result = ((state >> ((state >> 28) + 4)) ^ state) * 277803737;
	result = (result >> 22) ^ result;
	return result / 4294967295.0;
}

vec2 RandomPointInCircle(inout uint rngState)
{
	float angle = RandomValue(rngState) * 2 * PI;
	vec2 pointOnCircle = vec2(cos(angle), sin(angle));
	return pointOnCircle * sqrt(RandomValue(rngState));
}

float RandomValueNormalDist(inout uint state)
{
	float t = 2 * 3.14159263 * RandomValue(state);
	float r = sqrt(-2 * log(RandomValue(state)));
	return r * cos(t);
}

vec3 RandomDirection(inout uint state)
{
	float x = RandomValueNormalDist(state);
	float y = RandomValueNormalDist(state);
	float z = RandomValueNormalDist(state);
	return normalize(vec3(x,y,z));
}

vec3 RandomHemiDir(vec3 normal, inout uint rngState)
{
	vec3 dir = RandomDirection(rngState);
	return dir * sign(dot(normal, dir));
}

// Get Light From Enviornment
vec3 GetEnvironmentLight(vec3 dir)
{
	if (useSky == 0) return vec3(0);
	const vec3 GroundColor = vec3(0.35, 0.3, 0.35);
	const vec3 SkyColorHorizon = vec3(1, 1, 1);
	const vec3 SkyColorZenith = vec3(0.08, 0.37, 0.73);

	float skyGradientT = pow(smoothstep(0.f, 0.4f, dir.y), 0.35);
	float groundToSkyT = smoothstep(-0.01f, 0.f, dir.y);
	vec3 skyGradient = lerp(SkyColorHorizon, SkyColorZenith, skyGradientT);
	float sun = pow(max(0, dot(dir, vec3(1,1,1))), sunFocus) * sunIntensity;
	// Combine ground, sky, and sun
	vec3 composite = lerp(GroundColor, skyGradient, groundToSkyT) + sunColor * sun * (groundToSkyT >= 1 ? 1 : 0);
	return composite;
}

TriangleHitInfo RayTriangle(Ray ray, Triangle tri) {
	vec3 edgeAB = tri.posB - tri.posA;
	vec3 edgeAC = tri.posC - tri.posA;
	vec3 normalVector = cross(edgeAB, edgeAC);
	vec3 ao = ray.origin - tri.posA;
	vec3 dao = cross(ao, ray.dir);

	float determinant = -dot(ray.dir, normalVector);
	float invDet = 1 / determinant;

	// Calculate dst to Triangle and barycentric coordinates of intersection point
	float dst = dot(ao, normalVector) * invDet;
	float u = dot(edgeAC, dao) * invDet;
	float v = -dot(edgeAB, dao) * invDet;
	float w = 1 - u - v;

	// Initialize hit info
	TriangleHitInfo hitInfo;
	hitInfo.didHit = determinant >= 1E-6 && dst >= -1E-6 && u >= -1E-6 && v >= -1E-6 && w >= -1E-6;
	if (!hitInfo.didHit) return hitInfo;

	hitInfo.hitPoint = ray.origin + ray.dir * dst;
	hitInfo.dst = dst;

	hitInfo.normal = normalize(tri.normalA * w + tri.normalB * u + tri.normalC * v);

	return hitInfo;
}

float RayBoundingBoxDst(Ray ray, vec3 boxMin, vec3 boxMax) {
	vec3 tMin = (boxMin - ray.origin) * ray.invDir;
	vec3 tMax = (boxMax - ray.origin) * ray.invDir;

	vec3 t1 = min(tMin, tMax);
	vec3 t2 = max(tMin, tMax);

	float tNear = max(max(t1.x, t1.y), t1.z);
	float tFar = min(min(t2.x, t2.y), t2.z);

	if((tFar >= tNear) && (tFar > 0))
	{
		return uintBitsToFloat(0x7F800000);
	}

	return max(0.0, tNear);
}

TriangleHitInfo RayTriangleBVH(inout Ray ray, float rayLength, int nodeOffset, int triOffset)
{
	TriangleHitInfo result;
	result.dst = rayLength;
	result.triIndex = -1;

	int stack[64];
	int stackIndex = 0;
	stack[stackIndex] = nodeOffset;
	++stackIndex;

	while (stackIndex > 0)
	{
		--stackIndex;
		int nodeIndex = stack[stackIndex];

		BVHNode node = nodes[nodeIndex];
		bool isLeaf = bool(node.triangleCount > 0);

		if(isLeaf)
		{
			for(int i = 0; i < node.triangleCount; i++)
			{
				Triangle tri = triangles[triIndex[triOffset + node.startIndex + i]];
				TriangleHitInfo triHitInfo = RayTriangle(ray, tri);

				if (triHitInfo.didHit && triHitInfo.dst < result.dst)
				{
					result = triHitInfo;
					result.triIndex = triOffset + node.startIndex + i;
				}
			}
		}
		else
		{
			int childIndexA = nodeOffset + node.startIndex + 0;
			int childIndexB = nodeOffset + node.startIndex + 1;
			BVHNode childA = nodes[childIndexA];
			BVHNode childB = nodes[childIndexB];

			float dstA = RayBoundingBoxDst(ray, childA.boundsMin, childA.boundsMax);
			float dstB = RayBoundingBoxDst(ray, childB.boundsMin, childB.boundsMax);

			if (dstA > result.dst && dstB > result.dst)  // Skip both children
			{
				continue;
			}

			// Look at closeset child node first
			bool isNearestA = bool(dstA <= dstB);
			float dstNear = isNearestA ? dstA : dstB;
			float dstFar = isNearestA ? dstB : dstA;
			int childIndexNear = isNearestA ? childIndexA : childIndexB;
			int childIndexFar = isNearestA ? childIndexB : childIndexA;

			if(dstFar < result.dst)
			{
				stack[stackIndex] = childIndexFar;
				stackIndex++;
			}
			if(dstNear < result.dst)
			{
				stack[stackIndex] = childIndexNear;
				stackIndex++;
			}
		}
	}

	return result;
}

ModelHitInfo CalculateRayCollision(Ray worldRay)
{
	ModelHitInfo result;
	result.dst = uintBitsToFloat(0x7F800000);
	Ray localRay;

	for(int i = 0; i < modelCount; i++)
	{
		Model model = models[i];
		localRay.origin = (model.worldToLocalMatrix * vec4(worldRay.origin, 1)).xyz;
		localRay.dir = (model.worldToLocalMatrix * vec4(worldRay.dir, 0)).xyz;
		localRay.invDir = 1 / localRay.dir;

		TriangleHitInfo hit = RayTriangleBVH(localRay, result.dst, model.nodeOffset, model.triOffset);

		if(hit.dst < result.dst)
		{
			result.didHit = hit.didHit;
			result.dst = hit.dst;
			result.normal = normalize(model.localToWorldMatrix * vec4(worldRay.dir, 0)).xyz;
			result.uv = hit.uv;
			result.hitPoint = worldRay.origin + worldRay.dir * hit.dst;
			result.material = model.material;
		}
	}

	return result;
}

vec3 Trace(vec3 rayOrigin, vec3 rayDir)
{
	Ray ray;
	ray.origin = rayOrigin;
	ray.dir = rayDir;

	uint rngState = 0;
	ModelHitInfo hitInfo = CalculateRayCollision(ray);
	if (!hitInfo.didHit)
	{
		return GetEnvironmentLight(rayDir);
	}

	return hitInfo.material.color.rgb;
}

void main()
{
	vec2 ndc = vec2(
	(gl_FragCoord.x / _ScreenParams.x) * 2.0 - 1.0,
	(gl_FragCoord.y / _ScreenParams.y) * 2.0 - 1.0
	);

	vec3 viewPointLocal = vec3(ndc, 1.0) * viewParams;
	vec3 focusPoint = (camLocalToWorldMatrix * vec4(viewPointLocal, 1.0)).xyz;

	vec3 rayOrigin = _WorldSpaceCamPos;
	vec3 rayDir = normalize(focusPoint - rayOrigin);

	Ray ray;
	ray.dir = rayDir;
	ray.origin = rayOrigin;
	ray.invDir = 1.f / rayDir;

	fragColor = vec4(Trace(rayOrigin, rayDir), 1);

//	fragColor = vec4(RayTriangle(ray, triangles[0]).didHit);

//	fragColor = vec4(RayBoundingBoxDst(ray, nodes[0].boundsMin, nodes[0].boundsMax));

//	fragColor = vec4(RayTriangleBVH(ray, uintBitsToFloat(0x7F800000), 0, 0).didHit);

//	fragColor = vec4(CalculateRayCollision(ray).didHit);
}