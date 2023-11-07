#pragma once
#include <cassert>

#include "Math.h"
#include "vector"

namespace dae
{
#pragma region GEOMETRY
	struct Sphere
	{
		Vector3 origin{};
		float radius{};

		unsigned char materialIndex{ 0 };
	};

	struct Plane
	{
		Vector3 origin{};
		Vector3 normal{};

		unsigned char materialIndex{ 0 };
	};

	enum class TriangleCullMode
	{
		FrontFaceCulling,
		BackFaceCulling,
		NoCulling
	};

	struct Triangle
	{
		Triangle() = default;
		Triangle(const Vector3& _v0, const Vector3& _v1, const Vector3& _v2, const Vector3& _normal):
			v0{_v0}, v1{_v1}, v2{_v2}, normal{_normal.Normalized()}{}

		Triangle(const Vector3& _v0, const Vector3& _v1, const Vector3& _v2) :
			v0{ _v0 }, v1{ _v1 }, v2{ _v2 }
		{
			const Vector3 edgeV0V1 = v1 - v0;
			const Vector3 edgeV0V2 = v2 - v0;
			normal = Vector3::Cross(edgeV0V1, edgeV0V2).Normalized();
		}

		Vector3 v0{};
		Vector3 v1{};
		Vector3 v2{};

		Vector3 normal{};

		TriangleCullMode cullMode{};
		unsigned char materialIndex{};
	};

	struct TriangleMesh
	{
		TriangleMesh() = default;
		TriangleMesh(const std::vector<Vector3>& _positions, const std::vector<int>& _indices, TriangleCullMode _cullMode):
		positions(_positions), indices(_indices), cullMode(_cullMode)
		{
			//Calculate Normals
			CalculateNormals();

			//Update Transforms
			UpdateTransforms();
		}

		TriangleMesh(const std::vector<Vector3>& _positions, const std::vector<int>& _indices, const std::vector<Vector3>& _normals, TriangleCullMode _cullMode) :
			positions(_positions), indices(_indices), normals(_normals), cullMode(_cullMode)
		{
			UpdateTransforms();
		}

		std::vector<Vector3> positions{};
		std::vector<Vector3> normals{};
		std::vector<int> indices{};
		unsigned char materialIndex{};

		TriangleCullMode cullMode{TriangleCullMode::BackFaceCulling};

		Matrix rotationTransform{};
		Matrix translationTransform{};
		Matrix scaleTransform{};

		std::vector<Vector3> transformedPositions{};
		std::vector<Vector3> transformedNormals{};

		void Translate(const Vector3& translation)
		{
			translationTransform = Matrix::CreateTranslation(translation);
		}

		void RotateY(float yaw)
		{
			rotationTransform = Matrix::CreateRotationY(yaw);
		}

		void Scale(const Vector3& scale)
		{
			scaleTransform = Matrix::CreateScale(scale);
		}

		void AppendTriangle(const Triangle& triangle, bool ignoreTransformUpdate = false)
		{
			int startIndex = static_cast<int>(positions.size());

			positions.push_back(triangle.v0);
			positions.push_back(triangle.v1);
			positions.push_back(triangle.v2);

			indices.push_back(startIndex);
			indices.push_back(++startIndex);
			indices.push_back(++startIndex);

			normals.push_back(triangle.normal);

			//Not ideal, but making sure all vertices are updated
			if(!ignoreTransformUpdate)
				UpdateTransforms();
		}

		void CalculateNormals()
		{
			
			normals.clear();
			normals.reserve(positions.size());

			// Calculate normals for each triangle in the mesh.
			for (size_t i = 0; i < indices.size(); i += 3)
			{
				// Get the indices of the vertices for the current triangle.
				int index0 = indices[i];
				int index1 = indices[i + 1];
				int index2 = indices[i + 2];

				// Calculate the normal for the current triangle using the cross product of its edges.
				Vector3 edge1 = positions[index1] - positions[index0];
				Vector3 edge2 = positions[index2] - positions[index0];
				Vector3 normal = Vector3::Cross(edge1, edge2);

				// Add the calculated normal to the normals array for each vertex of the triangle.
				normals.push_back(normal);
				normals.push_back(normal);
				normals.push_back(normal);
			}

			// Normalize the normals in a separate loop.
			for (size_t i = 0; i < normals.size(); ++i)
			{
				normals[i].Normalize();
			}
		}

		void UpdateTransforms()
		{
			 // Calculate the final transformation matrix.
			Matrix finalTransform = scaleTransform * rotationTransform * translationTransform;

			// Clear the transformed positions and normals and preallocate memory for them.
			transformedPositions.clear();
			transformedPositions.reserve(positions.size());
			transformedNormals.clear();
			transformedNormals.reserve(normals.size());

			// Transform each vertex and normal using the final transformation matrix.
			for (size_t i = 0; i < positions.size(); ++i)
			{
				const Vector3& position = positions[i];

				// Transform the position using the final transformation matrix.
				Vector3 transformedPosition = finalTransform.TransformPoint(position);
				transformedPositions.push_back(transformedPosition);

			}
			for (size_t i = 0; i < normals.size(); ++i)
			{
				const Vector3& normal = normals[i];

				// To transform a normal vector, apply only the rotation part of the matrix.
				Vector3 transformedNormal = rotationTransform.TransformVector(normal).Normalized();
				transformedNormals.push_back(transformedNormal);
			}
			
		}
	};
	//// Clear any existing normals
	//normals.clear();
	//normals.resize(positions.size(), Vector3(0.0f, 0.0f, 0.0f));

	//for (size_t i = 0; i < indices.size(); i += 3) {
	//	int idx0 = indices[i];
	//	int idx1 = indices[i + 1];
	//	int idx2 = indices[i + 2];

	//	// Get vertex positions
	//	Vector3 A = positions[idx0];
	//	Vector3 B = positions[idx1];
	//	Vector3 C = positions[idx2];

	//	// Calculate the triangle normal
	//	Vector3 AB = B - A;
	//	Vector3 AC = C - A;
	//	Vector3 crossABC = Vector3::Cross(AB, AC);
	//	crossABC.Normalize();
	//	Vector3 N = crossABC;

	//	// Accumulate the normals for each vertex of the triangle
	//	normals[idx0] += N;
	//	normals[idx1] += N;
	//	normals[idx2] += N;
	//}

	//// Normalize all normals
	//for (size_t i = 0; i < normals.size(); ++i) {
	//	normals[i] = normals[i].Normalized();
	//}
#pragma endregion
#pragma region LIGHT
	enum class LightType
	{
		Point,
		Directional
	};

	struct Light
	{
		Vector3 origin{};
		Vector3 direction{};
		ColorRGB color{};
		float intensity{};

		LightType type{};
	};
#pragma endregion
#pragma region MISC
	struct Ray
	{
		Vector3 origin{};
		Vector3 direction{};

		float min{ 0.0001f };
		float max{ FLT_MAX };
	};

	struct HitRecord
	{
		Vector3 origin{};
		Vector3 normal{};
		float t = FLT_MAX;

		bool didHit{ false };
		unsigned char materialIndex{ 1 };
	};
#pragma endregion
}