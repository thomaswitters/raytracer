//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
#include "Math.h"
#include "Matrix.h"
#include "Material.h"
#include "Scene.h"
#include "Utils.h"

#include <execution>

using namespace dae;

#define PARRALEL_EXECUTION

Renderer::Renderer(SDL_Window * pWindow) :
	m_pWindow(pWindow),
	m_pBuffer(SDL_GetWindowSurface(pWindow))
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_pBufferPixels = static_cast<uint32_t*>(m_pBuffer->pixels);

	
}

void Renderer::Render(Scene* pScene) const
{
	Camera& camera = pScene->GetCamera();
	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();
	float offset = 0.5f;
	float aspectRatio = static_cast<float>(m_Width) / static_cast<float>(m_Height);

	float fov = tanf(camera.fovAngle * TO_RADIANS / 2);
	const Matrix cameraToWorld = camera.CalculateCameraToWorld();




#if defined(PARRALEL_EXECUTION)
	uint32_t amountOfPixels{ uint32_t(m_Width * m_Height) };
	std::vector<uint32_t> pixelIndices{};

	pixelIndices.reserve(amountOfPixels);

	for (uint32_t index{}; index < amountOfPixels; index++)
	{
		pixelIndices.emplace_back(index);
	}

	std::for_each(std::execution::par, pixelIndices.begin(), pixelIndices.end(), [&](int i) {
		RenderPixel(pScene, i, fov, aspectRatio, cameraToWorld, camera.origin);
		});

#else
	uint32_t amountOfPixels{ uint32_t(m_Width * m_Height) };
	for (uint32_t index{}; index < amountOfPixels; index++)
	{
		RenderPixel(pScene, index, fov, aspectRatio, cameraToWorld, camera.origin);
	}
#endif
	
	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

void Renderer::RenderPixel(Scene* pScene, uint32_t pixelIndex, float fov, float aspectRatio, const Matrix cameraToWorld, const Vector3 cameraOrigin) const
{
	auto materials{ pScene->GetMaterials() };

	const uint32_t px{ pixelIndex % m_Width }, py{ pixelIndex / m_Width};
	float rx{ px + 0.5f }, ry{ py + 0.5f };
	float cx{ (2 * (rx / float(m_Width)) - 1) * aspectRatio * fov };
	float cy{ (1 - (2 * (ry / float(m_Height)))) * fov };


	auto& lights = pScene->GetLights();

	Vector3 rayDirection = { cx, cy, 1 };
	rayDirection.Normalize();

	rayDirection = cameraToWorld.TransformVector(rayDirection);

	Ray viewRay{ cameraOrigin, rayDirection };

	HitRecord closestHit{};
	pScene->GetClosestHit(viewRay, closestHit);


	ColorRGB finalColor{ 0,0,0 };
	if (closestHit.didHit)
	{
		for (const auto& Light : lights)
		{
			Ray lightRay{};
			Vector3 lightRayDirection = LightUtils::GetDirectionToLight(Light, closestHit.origin);

			lightRayDirection.Normalize();
			lightRay.origin = closestHit.origin;
			lightRay.direction = lightRayDirection;

			float lambert = Vector3::Dot(closestHit.normal, lightRayDirection);

			

			if (lambert > 0.0f)
			{
				ColorRGB lightShading = materials[closestHit.materialIndex]->Shade(closestHit, lightRayDirection, -rayDirection);

				if (m_CurrentLightingMode == LightingMode::ObservedArea)
				{
					finalColor.r += lambert;
					finalColor.g += lambert;
					finalColor.b += lambert;
				}
				else if (m_CurrentLightingMode == LightingMode::BRDF)
				{
					finalColor += lightShading; 
				}
				else if (m_CurrentLightingMode == LightingMode::Combined)
				{
					finalColor += LightUtils::GetRadiance(Light, closestHit.origin) * lightShading * lambert;
				}
				
			}
			if (m_CurrentLightingMode == LightingMode::Radiance)
			{
				finalColor += LightUtils::GetRadiance(Light, closestHit.origin);
			}
			
			


			if (m_ShadowEnabled)
			{
				
				if (pScene->DoesHit(lightRay))
				{
					finalColor *= 0.95f;
				}

			}
			

		}
	}

	finalColor.MaxToOne();

	m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
		static_cast<uint8_t>(finalColor.r * 255),
		static_cast<uint8_t>(finalColor.g * 255),
		static_cast<uint8_t>(finalColor.b * 255));
}


bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}
