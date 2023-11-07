#pragma once
#include <cassert>
#include <fstream>
#include "Math.h"
#include "DataTypes.h"
#include <iostream>

namespace dae
{
	namespace GeometryUtils
	{
#pragma region Sphere HitTest
		//SPHERE HIT-TESTS
		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W1

			//float a = ray.direction.x * ray.direction.x + ray.direction.y * ray.direction.y + ray.direction.z * ray.direction.z;
			float a = Vector3::Dot(ray.direction, ray.direction) /** hitRecord.t * hitRecord.t*/;
			float b = 2.f * Vector3::Dot(ray.direction, (ray.origin - sphere.origin)) /** hitRecord.t*/;
			float c = Vector3::Dot((ray.origin - sphere.origin), (ray.origin - sphere.origin)) - (sphere.radius * sphere.radius);

			// quadratic formula discriminant
			// b^2 - 4ac
			float discriminant = b * b - 4 * a * c;

			//float sq = sqrt(max_align_t(discriminant));
			if (discriminant > 0.f)
			{
				float sqrtDiscriminiant = sqrt(discriminant);
				float t1 = (-b - sqrtDiscriminiant) / (a * 2);

				float t2 = (-b + sqrtDiscriminiant) / (a * 2);

				if (t1 >= ray.min && t1 < ray.max && t1 < hitRecord.t)
				{
					//Vector3 p = ray.origin + ((-b - sqrt(discriminant) / 2 * a) * ray.direction);
					hitRecord.t = t1;
					hitRecord.origin = ray.origin + ray.direction * t1;

					Vector3 normal = hitRecord.origin - sphere.origin;
					normal.Normalize();
					hitRecord.normal = normal;

					hitRecord.didHit = true;
					hitRecord.materialIndex = sphere.materialIndex;

					return true;
				}
				if (t2 >= ray.min && t2 < ray.max && t2 < hitRecord.t)
				{
					hitRecord.t = t2;

					hitRecord.origin = ray.origin + ray.direction * t2;

					Vector3 normal = hitRecord.origin - sphere.origin;
					normal.Normalize();
					hitRecord.normal = normal; 

					hitRecord.didHit = true;
					hitRecord.materialIndex = sphere.materialIndex;
					return true;
				}
				return true;
			}

			return false;
			
		}

		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Sphere(sphere, ray, temp, true);
		}
#pragma endregion
#pragma region Plane HitTest
		//PLANE HIT-TESTS
		inline bool HitTest_Plane(const Plane& plane, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W1

			//t
			//float a = (plane.origin - ray.origin) * plane.normal;
			Vector3 origin = (plane.origin - ray.origin);
			float a = Vector3::Dot(origin, plane.normal);
			float b = Vector3::Dot(ray.direction, plane.normal);

			//float t = ray.origin + (a / b) * ray.direction;
			float t = (a / b);

			if (t >= ray.min && t < ray.max && t < hitRecord.t)
			{
				hitRecord.t = t;
				
				hitRecord.origin = ray.origin + ray.direction * t;
				hitRecord.normal = plane.normal;

				hitRecord.didHit = true;
				hitRecord.materialIndex = plane.materialIndex;

				//return true;
			}

			
			return false;
		}

		inline bool HitTest_Plane(const Plane& plane, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Plane(plane, ray, temp, true);
		}
#pragma endregion
#pragma region Triangle HitTest
		//TRIANGLE HIT-TESTS
		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			
			Vector3 edge1 = triangle.v1 - triangle.v0;
			Vector3 edge2 = triangle.v2 - triangle.v0;

			Vector3 normal = Vector3::Cross(edge1, edge2);
			normal.Normalize();
			float det = Vector3::Dot(normal, ray.direction);

			float t = Vector3::Dot(triangle.v0 - ray.origin, normal) / det;

			if (triangle.cullMode == TriangleCullMode::FrontFaceCulling && Vector3::Dot(normal, ray.direction) < 0) {
				return false;
			}

			if (triangle.cullMode == TriangleCullMode::BackFaceCulling && Vector3::Dot(normal, ray.direction) > 0) {
				return false;
			}

			if (t >= ray.min && t < ray.max && t < hitRecord.t)
			{
				Vector3 intersectionPoint = ray.origin + ray.direction * t;

				Vector3 edge1ToIntersection = intersectionPoint - triangle.v0;
				float dot11 = Vector3::Dot(edge1, edge1);
				float dot12 = Vector3::Dot(edge1, edge2);
				float dot22 = Vector3::Dot(edge2, edge2);
				float dot1ToIntersection = Vector3::Dot(edge1, edge1ToIntersection);
				float dot2ToIntersection = Vector3::Dot(edge2, edge1ToIntersection);

				float denominator = dot11 * dot22 - dot12 * dot12;
				float u = (dot22 * dot1ToIntersection - dot12 * dot2ToIntersection) / denominator;
				float v = (dot11 * dot2ToIntersection - dot12 * dot1ToIntersection) / denominator;

				if (u >= 0 && v >= 0 && u + v <= 1) {
					
					hitRecord.t = t;
					hitRecord.origin = intersectionPoint;
					hitRecord.normal = triangle.normal;
					hitRecord.didHit = true;
					hitRecord.materialIndex = triangle.materialIndex;
					return true;
				}
			}

			return false;

		}
		
		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Triangle(triangle, ray, temp, true);
		}
#pragma endregion
#pragma region TriangeMesh HitTest
		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//Implement the HitTest_TriangleMesh function(+make sure to alter the Scene::GetClosestHit & DoesHit
			//functions)
			//• Each set of 3 indices represents a Triangle – use HitTest_Triangle to find the triangle of the TriangleMesh with the(!) closest hit
			//• Use the ‘transformedPositions’ & ‘transformedNormals’ to define each individual triangle!
			//bool didHit = false;

			float closestT = FLT_MAX;
			bool didHit = false;

			for (size_t i = 0; i < mesh.indices.size(); i += 3)
			{
				int index0 = mesh.indices[i];
				int index1 = mesh.indices[i + 1];
				int index2 = mesh.indices[i + 2];

				const Vector3& v0 = mesh.transformedPositions[index0];
				const Vector3& v1 = mesh.transformedPositions[index1];
				const Vector3& v2 = mesh.transformedPositions[index2];

				Triangle triangle(v0, v1, v2, mesh.transformedNormals[index0]);
				// it doesnt use this mesh.cullMode;
				triangle.cullMode = mesh.cullMode;

				if (HitTest_Triangle(triangle, ray, hitRecord))
				{
					if (hitRecord.t < closestT)
					{
						closestT = hitRecord.t;
						if (!ignoreHitRecord || !hitRecord.didHit)
						{
							hitRecord.didHit = true;
							hitRecord.materialIndex = mesh.materialIndex;
						}
						didHit = true;
					}
				}
			}

			return didHit;
			
		}

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_TriangleMesh(mesh, ray, temp, true);
		}
#pragma endregion
	}

	namespace LightUtils
	{
		//Direction from target to light
		inline Vector3 GetDirectionToLight(const Light& light, const Vector3 origin)
		{
			//todo W3
			Vector3 direction;

			if (light.type == LightType::Point) {
				direction.x = light.origin.x - origin.x;
				direction.y = light.origin.y - origin.y;
				direction.z = light.origin.z - origin.z;
			}
			else if (light.type == LightType::Directional) {
				direction.x = FLT_MAX;
				direction.y = FLT_MAX;
				direction.z = FLT_MAX;
			}

			return direction;
		}

		inline ColorRGB GetRadiance(const Light& light, const Vector3& target)
		{
			
			if (light.type == LightType::Point)
			{
				Vector3 lightDirection = light.origin - target;
				float lightDistance = lightDirection.Magnitude();
				lightDirection.Normalize();
				
				float cosTheta = Vector3::Dot(lightDirection, lightDirection);
				//cosTheta = std::max(0.0f, cosTheta);
				
				if (cosTheta > 0)
				{
					float intensity = (2 * PI * light.intensity) /  (PI * lightDistance * lightDistance);
					ColorRGB radiance = light.color * cosTheta * intensity;
					return radiance;
					
				}
			}
			else if (light.type == LightType::Directional)
			{
				
				Vector3 lightDirection = -light.direction;

				float cosTheta = Vector3::Dot(lightDirection, lightDirection);
				if (cosTheta > 0) 
				{
					float intensity = 2 * light.intensity / (PI);
					ColorRGB radiance = light.color * cosTheta * intensity;

					return radiance;
				}
			}
			
			//float lightDirection = (light.origin - target).Magnitude();

			//assert(true && "No Implemented Yet!");
			//return { radiance };
		}
	}

	namespace Utils
	{
		//Just parses vertices and indices
#pragma warning(push)
#pragma warning(disable : 4505) //Warning unreferenced local function
		static bool ParseOBJ(const std::string& filename, std::vector<Vector3>& positions, std::vector<Vector3>& normals, std::vector<int>& indices)
		{
			std::ifstream file(filename);
			if (!file)
				return false;

			std::string sCommand;
			// start a while iteration ending when the end of file is reached (ios::eof)
			while (!file.eof())
			{
				//read the first word of the string, use the >> operator (istream::operator>>) 
				file >> sCommand;
				//use conditional statements to process the different commands	
				if (sCommand == "#")
				{
					// Ignore Comment
				}
				else if (sCommand == "v")
				{
					//Vertex
					float x, y, z;
					file >> x >> y >> z;
					positions.push_back({ x, y, z });
				}
				else if (sCommand == "f")
				{
					float i0, i1, i2;
					file >> i0 >> i1 >> i2;

					indices.push_back((int)i0 - 1);
					indices.push_back((int)i1 - 1);
					indices.push_back((int)i2 - 1);
				}
				//read till end of line and ignore all remaining chars
				file.ignore(1000, '\n');

				if (file.eof()) 
					break;
			}

			//Precompute normals
			for (uint64_t index = 0; index < indices.size(); index += 3)
			{
				uint32_t i0 = indices[index];
				uint32_t i1 = indices[index + 1];
				uint32_t i2 = indices[index + 2];

				Vector3 edgeV0V1 = positions[i1] - positions[i0];
				Vector3 edgeV0V2 = positions[i2] - positions[i0];
				Vector3 normal = Vector3::Cross(edgeV0V1, edgeV0V2);

				if(isnan(normal.x))
				{
					int k = 0;
				}

				normal.Normalize();
				if (isnan(normal.x))
				{
					int k = 0;
				}

				normals.push_back(normal);
			}

			return true;
		}
#pragma warning(pop)
	}
}