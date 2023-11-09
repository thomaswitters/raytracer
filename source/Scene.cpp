#include "Scene.h"
#include "Utils.h"
#include "Material.h"

namespace dae {

#pragma region Base Scene
	//Initialize Scene with Default Solid Color Material (RED)
	Scene::Scene():
		m_Materials({ new Material_SolidColor({1,0,0})})
	{
		m_SphereGeometries.reserve(32);
		m_PlaneGeometries.reserve(32);
		m_TriangleMeshGeometries.reserve(32);
		m_Lights.reserve(32);
	}

	Scene::~Scene()
	{
		for(auto& pMaterial : m_Materials)
		{
			delete pMaterial;
			pMaterial = nullptr;
		}

		m_Materials.clear();
	}

	void dae::Scene::GetClosestHit(const Ray& ray, HitRecord& closestHit) const
	{
		
		for (const Sphere& sphere : m_SphereGeometries)
		{
			GeometryUtils::HitTest_Sphere(sphere, ray, closestHit);
		}

		for (const Plane& plane : m_PlaneGeometries)
		{
			GeometryUtils::HitTest_Plane(plane, ray, closestHit);
		}
		for (const TriangleMesh& triagleMesh : m_TriangleMeshGeometries)
		{
			GeometryUtils::HitTest_TriangleMesh(triagleMesh, ray, closestHit);
		}
		for (const Triangle& triagle : m_Triangles)
		{
			GeometryUtils::HitTest_Triangle(triagle, ray, closestHit);
		}

	}

	bool Scene::DoesHit(const Ray& ray) const
	{
		//todo W3
		for (const Sphere& sphere : m_SphereGeometries)
		{
			if (GeometryUtils::HitTest_Sphere(sphere, ray)) return true;
		}			
		for (const Plane& plane : m_PlaneGeometries)
		{
			if (GeometryUtils::HitTest_Plane(plane, ray)) return true;
		}
		for (const Triangle& triagle : m_Triangles)
		{
			if (GeometryUtils::HitTest_Triangle(triagle, ray)) return true;
		}
		for (const TriangleMesh& triagleMesh : m_TriangleMeshGeometries)
		{
			if(GeometryUtils::HitTest_TriangleMesh(triagleMesh, ray)) return true;
		}
		
		return false;
	}

#pragma region Scene Helpers
	Sphere* Scene::AddSphere(const Vector3& origin, float radius, unsigned char materialIndex)
	{
		Sphere s;
		s.origin = origin;
		s.radius = radius;
		s.materialIndex = materialIndex;

		m_SphereGeometries.emplace_back(s);
		return &m_SphereGeometries.back();
	}

	Plane* Scene::AddPlane(const Vector3& origin, const Vector3& normal, unsigned char materialIndex)
	{
		Plane p;
		p.origin = origin;
		p.normal = normal;
		p.materialIndex = materialIndex;

		m_PlaneGeometries.emplace_back(p);
		return &m_PlaneGeometries.back();
	}

	TriangleMesh* Scene::AddTriangleMesh(TriangleCullMode cullMode, unsigned char materialIndex)
	{
		TriangleMesh m{};
		m.cullMode = cullMode;
		m.materialIndex = materialIndex;

		m_TriangleMeshGeometries.emplace_back(m);
		return &m_TriangleMeshGeometries.back();
	}

	Light* Scene::AddPointLight(const Vector3& origin, float intensity, const ColorRGB& color)
	{
		Light l;
		l.origin = origin;
		l.intensity = intensity;
		l.color = color;
		l.type = LightType::Point;

		m_Lights.emplace_back(l);
		return &m_Lights.back();
	}

	Light* Scene::AddDirectionalLight(const Vector3& direction, float intensity, const ColorRGB& color)
	{
		Light l;
		l.direction = direction;
		l.intensity = intensity;
		l.color = color;
		l.type = LightType::Directional;

		m_Lights.emplace_back(l);
		return &m_Lights.back();
	}

	unsigned char Scene::AddMaterial(Material* pMaterial)
	{
		m_Materials.push_back(pMaterial);
		return static_cast<unsigned char>(m_Materials.size() - 1);
	}
#pragma endregion
#pragma endregion

#pragma region SCENE W1
	void Scene_W1::Initialize()
	{
		//default: Material id0 >> SolidColor Material (RED)
		constexpr unsigned char matId_Solid_Red = 0;
		const unsigned char matId_Solid_Blue = AddMaterial(new Material_SolidColor{ colors::Blue });

		const unsigned char matId_Solid_Yellow = AddMaterial(new Material_SolidColor{ colors::Yellow });
		const unsigned char matId_Solid_Green = AddMaterial(new Material_SolidColor{ colors::Green });
		const unsigned char matId_Solid_Magenta = AddMaterial(new Material_SolidColor{ colors::Magenta });

		//Spheres
		AddSphere({ -25.f, 0.f, 100.f }, 50.f, matId_Solid_Red);
		AddSphere({ 25.f, 0.f, 100.f }, 50.f, matId_Solid_Blue);

		//Plane
		AddPlane({ -75.f, 0.f, 0.f }, { 1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 75.f, 0.f, 0.f }, { -1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 0.f, -75.f, 0.f }, { 0.f, 1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 75.f, 0.f }, { 0.f, -1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 0.f, 125.f }, { 0.f, 0.f,-1.f }, matId_Solid_Magenta);
	}

	void Scene_W2::Initialize()
	{
		m_Camera.origin = { 0.f, 3.f, -9.f };
		m_Camera.fovAngle = 45.f;

		constexpr unsigned char matId_Solid_Red = 0;

		const unsigned char matId_Solid_Blue = AddMaterial(new Material_SolidColor{ colors::Blue });
		const unsigned char matId_Solid_Yellow = AddMaterial(new Material_SolidColor{ colors::Yellow });
		const unsigned char matId_Solid_Green = AddMaterial(new Material_SolidColor{ colors::Green });
		const unsigned char matId_Solid_Magenta = AddMaterial(new Material_SolidColor{ colors::Magenta });

		// planes
		AddPlane(Vector3{ 0.f, 0.f, 10.f }, Vector3{ 0.f, 0.f, -1.f }, matId_Solid_Magenta); //BACK
		AddPlane(Vector3{ 0.f, 0.f, 0.f }, Vector3{ 0.f, 1.f, 0.f }, matId_Solid_Yellow); //BOTTOM
		AddPlane(Vector3{ 0.f, 10.f, 0.f }, Vector3{ 0.f, -1.f, 0.f }, matId_Solid_Yellow); //TOP
		AddPlane(Vector3{ 5.f, 0.f, 0.f }, Vector3{ -1.f, 0.f, 0.f }, matId_Solid_Green); //RIGHT
		AddPlane(Vector3{ -5.f, 0.f, 0.f }, Vector3{ 1.f, 0.f, 0.f }, matId_Solid_Green); //LEFT

		AddSphere(Vector3{ -1.75f, 1.f, 0.f }, .75f, matId_Solid_Red);
		AddSphere(Vector3{ 0.f, 1.f, 0.f }, .75f, matId_Solid_Blue);
		AddSphere(Vector3{ 1.75f, 1.f, 0.f }, .75f, matId_Solid_Red);
		AddSphere(Vector3{ -1.75f, 3.f, 0.f }, .75f, matId_Solid_Blue);
		AddSphere(Vector3{ 0.f, 3.f, 0.f }, .75f, matId_Solid_Red);
		AddSphere(Vector3{ 1.75f, 3.f, 0.f }, .75f, matId_Solid_Blue);

		AddPointLight(Vector3{ 0.f, 5.f, -5.f }, 75.f, colors::White); //Backlight
		//AddPointLight(Vector3{ 0.f, 5.f, 5.f }, 75.f, colors::White); //Backlight
	}

	//void Scene_W3::Initialize()
	//{
	//	m_Camera.origin = { 0.f, 1.f, -5.f };
	//	m_Camera.fovAngle = 45.f;

	//	//default: Material id0 >> SolidColor Material (RED)
	//	constexpr unsigned char matId_Solid_Red = 0;
	//	const unsigned char matId_Solid_Blue = AddMaterial(new Material_SolidColor{ colors::Blue });
	//	const unsigned char matId_Solid_Yellow = AddMaterial(new Material_SolidColor{ colors::Yellow });

	//	const unsigned char matId_Lambert_Yellow = AddMaterial(new Material_Lambert{ colors::Yellow , 1.f});
	//	const unsigned char matId_Lambert_Blue = AddMaterial(new Material_Lambert{ colors::Blue , 1.f});
	//	const unsigned char matId_Lambert_Red = AddMaterial(new Material_Lambert{ colors::Red , 1.f});
	//	//const unsigned char matId_Solid_Red = AddMaterial(new Material_SolidColor{ colors::Red });
	//	const auto matLambertPhong3 = AddMaterial(new Material_LambertPhong(colors::Blue, 4.0f, 2.f, 10.f));

	//	const auto matCT_GraySmoothPlastic = AddMaterial(new Material_CookTorrence({ 0.f, 0.f, 0.f }, 1.f, 0.5f));


	//	//Spheres
	//	AddSphere({ -.75f, 1.f, .0f }, 1.f, matId_Solid_Red);
	//	AddSphere({ .75f, 1.f, .0f }, 1.f, matCT_GraySmoothPlastic);

	//	//Plane
	//	AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, matId_Solid_Yellow);

	//	//Light
	//	AddPointLight({ 0.f, 5.f, 5.f }, 25.f, colors::White);
	//	AddPointLight(Vector3{ 0.f, 2.5f, -5.f }, 25.f, colors::White);

	//}
	void Scene_W3::Initialize()
	{
		sceneName = "Week 3";
		m_Camera.origin = { 0,3,-9 };
		m_Camera.fovAngle = 45.f;

		const auto matCT_GrayRoughMetal = AddMaterial(new Material_CookTorrence({ .972f, .960f, .915f }, 1.f, 1.f));
		const auto matCT_GrayMediumMetal = AddMaterial(new Material_CookTorrence({ .972f, .960f, .915f }, 1.f, .5f));
		const auto matCT_GraySmoothMetal = AddMaterial(new Material_CookTorrence({ .972f, .960f, .915f }, 1.f, .01f));

		const auto matCT_GrayRoughPlastic = AddMaterial(new Material_CookTorrence({ 1.f, 1.f, 1.f }, 0.0f, 1.0f));
		const auto matCT_GrayMediumPlastic = AddMaterial(new Material_CookTorrence({ 1.f, 1.f, 1.f }, 0.0f, 0.2f));
		const auto matCT_GraySmoothPlastic = AddMaterial(new Material_CookTorrence({ 1.f, 1.f, 1.f }, 0.0f, 0.01f));
		
		const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({ .49f, 0.57f, 0.57f }, 1.f));
		//const auto matLambertPhong3 = AddMaterial(new Material_CookTorrence(colors::Blue, 0.5f, 0.5f, 50.f));

		AddPlane(Vector3{ 0.f, 0.f, 10.f }, Vector3{ 0.f, 0.f, -1.f }, matLambert_GrayBlue); //BACK
		AddPlane(Vector3{ 0.f, 0.f, 0.f }, Vector3{ 0.f, 1.f, 0.f }, matLambert_GrayBlue); //BOTTOM
		AddPlane(Vector3{ 0.f, 10.f, 0.f }, Vector3{ 0.f, -1.f, 0.f }, matLambert_GrayBlue); //TOP
		AddPlane(Vector3{ 5.f, 0.f, 0.f }, Vector3{ -1.f, 0.f, 0.f }, matLambert_GrayBlue); //RIGHT
		AddPlane(Vector3{ -5.f, 0.f, 0.f }, Vector3{ 1.f, 0.f, 0.f }, matLambert_GrayBlue); //LEFT

		AddSphere(Vector3{ -1.75f, 1.f, 0.f }, .75f, matCT_GrayRoughMetal);
		AddSphere(Vector3{ 0.f, 1.f, 0.f }, .75f, matCT_GrayMediumMetal);
		AddSphere(Vector3{ 1.75f, 1.f, 0.f }, .75f, matCT_GraySmoothMetal);
		AddSphere(Vector3{ -1.75f, 3.f, 0.f }, .75f, matCT_GrayRoughPlastic);
		AddSphere(Vector3{ 0.f, 3.f, 0.f }, .75f, matCT_GrayMediumPlastic);
		AddSphere(Vector3{ 1.75f, 3.f, 0.f }, .75f, matCT_GraySmoothPlastic);



		//AddPointLight(Vector3{ 0.f, 5.f, 5.f }, 70.f, ColorRGB{ 1.f, .61f, .45f }); //Backlight
		AddPointLight(Vector3{ 0.f, 5.f, 5.f }, 90.f, ColorRGB{ 1.f, 1.f, 1.f }); //Backlight
		//AddPointLight(Vector3{ 0.f, 5.f, -2.5f }, 90.f, ColorRGB{ 1.f, 1.f, 1.f }); //Front Light Left
		AddPointLight(Vector3{ -2.5f, 5.f, -5.f }, 90.f, ColorRGB{ 1.f, .8f, .45f }); //Front Light Left
		//AddPointLight(Vector3{ -2.5f, 5.f, -5.f }, 90.f, ColorRGB{ 1.f, 1.0f, 1.f }); //Front Light Left
		AddPointLight(Vector3{ 2.5f, 2.5f, -5.f }, 50.f, ColorRGB{ 1.f, .8f, .45f });
		//AddPointLight(Vector3{ 2.5f, 2.5f, -5.f }, 70.f, ColorRGB{ 1.0f, 1.f, 1.f });

		//AddPointLight(Vector3{ 0.f, 5.f, 5.f }, 80.f, ColorRGB{ 1.f, .61f, .45f }); //Backlight
		//AddPointLight(Vector3{ -2.5f, 5.f, -5.f }, 90.f, ColorRGB{ 1.f, .8f, .45f }); //Front Light Left
		//AddPointLight(Vector3{ 2.5f, 2.5f, -5.f }, 80.f, ColorRGB{ .34f, .47f, .68f });
	}

	

	void Scene_W4_ReferenceScene::Initialize()
	{
		sceneName = "Reference Scene";
		m_Camera.origin = { 0,3,-9 };
		m_Camera.fovAngle = 45.f;

		const auto matCT_GrayRoughMetal = AddMaterial(new Material_CookTorrence({ .972f, .960f, .915f }, 1.f, 1.f));
		const auto matCT_GrayMediumMetal = AddMaterial(new Material_CookTorrence({ .972f, .960f, .915f }, 1.f, .6f));
		const auto matCT_GraySmoothMetal = AddMaterial(new Material_CookTorrence({ .972f, .960f, .915f }, 1.f, .1f));
		const auto matCT_GrayRoughPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, .0f, 1.f));
		const auto matCT_GrayMediumPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, .0f, .6f));
		const auto matCT_GraySmoothPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, .0f, .1f));

		const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({ .49f, 0.57f, 0.57f }, 1.f));
		const auto matLambert_White = AddMaterial(new Material_Lambert(colors::White, 1.f));

		AddPlane(Vector3{ 0.f, 0.f, 10.f }, Vector3{ 0.f, 0.f, -1.f }, matLambert_GrayBlue); //BACK
		AddPlane(Vector3{ 0.f, 0.f, 0.f }, Vector3{ 0.f, 1.f, 0.f }, matLambert_GrayBlue); //BOTTOM
		AddPlane(Vector3{ 0.f, 10.f, 0.f }, Vector3{ 0.f, -1.f, 0.f }, matLambert_GrayBlue); //TOP
		AddPlane(Vector3{ 5.f, 0.f, 0.f }, Vector3{ -1.f, 0.f, 0.f }, matLambert_GrayBlue); //RIGHT
		AddPlane(Vector3{ -5.f, 0.f, 0.f }, Vector3{ 1.f, 0.f, 0.f }, matLambert_GrayBlue); //LEFT

		AddSphere(Vector3{ -1.75f, 1.f, 0.f }, .75f, matCT_GrayRoughMetal);
		AddSphere(Vector3{ 0.f, 1.f, 0.f }, .75f, matCT_GrayMediumMetal);
		AddSphere(Vector3{ 1.75f, 1.f, 0.f }, .75f, matCT_GraySmoothMetal);
		AddSphere(Vector3{ -1.75f, 3.f, 0.f }, .75f, matCT_GrayRoughPlastic);
		AddSphere(Vector3{ 0.f, 3.f, 0.f }, .75f, matCT_GrayMediumPlastic);
		AddSphere(Vector3{ 1.75f, 3.f, 0.f }, .75f, matCT_GraySmoothPlastic);

		//CW Winding Order!
		const Triangle baseTriangle = { Vector3(-.75f, 1.5f, 0.f), Vector3(.75f, 0.f, 0.f), Vector3(-.75f, 0.f, 0.f) };

		m_Meshes[0] = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White);
		m_Meshes[0]->AppendTriangle(baseTriangle, true);
		m_Meshes[0]->Translate({ -1.75f,4.5f,0.f });
		m_Meshes[0]->UpdateAABB();
		m_Meshes[0]->UpdateTransforms();

		m_Meshes[1] = AddTriangleMesh(TriangleCullMode::FrontFaceCulling, matLambert_White);
		m_Meshes[1]->AppendTriangle(baseTriangle, true);
		m_Meshes[1]->Translate({ 0.f,4.5f,0.f });
		m_Meshes[1]->UpdateAABB();
		m_Meshes[1]->UpdateTransforms();

		m_Meshes[2] = AddTriangleMesh(TriangleCullMode::NoCulling, matLambert_White);
		m_Meshes[2]->AppendTriangle(baseTriangle, true);
		m_Meshes[2]->Translate({ 1.75f,4.5f,0.f });
		m_Meshes[2]->UpdateAABB();
		m_Meshes[2]->UpdateTransforms();

		AddPointLight(Vector3{ 0.f, 5.f, 5.f }, 50.f, ColorRGB{ 1.f, .61f, .45f }); //Backlight
		AddPointLight(Vector3{ -2.5f, 5.f, -5.f }, 70.f, ColorRGB{ 1.f, .8f, .45f }); //Front Light Left
		AddPointLight(Vector3{ 2.5f, 2.5f, -5.f }, 50.f, ColorRGB{ .34f, .47f, .68f });
	}
	void Scene_W4_ReferenceScene::Update(Timer* pTimer)
	{

		Scene::Update(pTimer);

		const auto yawAngle = (cos(pTimer->GetTotal()) + 1) / 2 * PI_2;

		for (const auto mesh : m_Meshes)
		{
			mesh->RotateY(yawAngle);
			mesh->UpdateTransforms();
		}
		
	}


	void Scene_W4_TestScene::Initialize()
	{
		sceneName = "Reference Scene";
		m_Camera.origin = { 0,3,-9 };
		m_Camera.fovAngle = 45.f;

		const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({ .49f, 0.57f, 0.57f }, 1.f));
		const auto matLambert_White = AddMaterial(new Material_Lambert(colors::White, 1.f));

		//Planes
		AddPlane(Vector3{ 0.f, 0.f, 10.f }, Vector3{ 0.f, 0.f, -1.f }, matLambert_GrayBlue); //BACK
		AddPlane(Vector3{ 0.f, 0.f, 0.f }, Vector3{ 0.f, 1.f, 0.f }, matLambert_GrayBlue); //BOTTOM
		AddPlane(Vector3{ 0.f, 10.f, 0.f }, Vector3{ 0.f, -1.f, 0.f }, matLambert_GrayBlue); //TOP
		AddPlane(Vector3{ 5.f, 0.f, 0.f }, Vector3{ -1.f, 0.f, 0.f }, matLambert_GrayBlue); //RIGHT
		AddPlane(Vector3{ -5.f, 0.f, 0.f }, Vector3{ 1.f, 0.f, 0.f }, matLambert_GrayBlue); //LEFT

		//CW Winding Order!
		pMesh = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White);
		Utils::ParseOBJ("Resources/lowpoly_bunny.obj",
			pMesh->positions,
			pMesh->normals,
			pMesh->indices);

		pMesh->Scale({ 2.f, 2.f, 2.f });
		pMesh->UpdateAABB();
		pMesh->UpdateTransforms();


		AddPointLight(Vector3{ 0.f, 5.f, 5.f }, 50.f, ColorRGB{ 1.f, .61f, .45f }); //Backlight
		AddPointLight(Vector3{ -2.5f, 5.f, -5.f }, 70.f, ColorRGB{ 1.f, .8f, .45f }); //Front Light Left
		AddPointLight(Vector3{ 2.5f, 2.5f, -5.f }, 50.f, ColorRGB{ .34f, .47f, .68f });
	}
	void Scene_W4_TestScene::Update(Timer* pTimer)
	{

		Scene::Update(pTimer);

		const auto yawAngle = (cos(pTimer->GetTotal()) + 1) / 2 * PI_2;

		pMesh->RotateY(yawAngle);
		pMesh->UpdateTransforms();
	}
#pragma endregion
}
