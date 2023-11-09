#pragma once

#include <cstdint>
#include "Matrix.h"

#include <iostream>

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Scene;

	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer() = default;

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Render(Scene* pScene) const;
		void RenderPixel(Scene* pScene, uint32_t pixelIndex, float fov, float aspectRatio, const Matrix cameraToWorld, const Vector3 cameraOrigin) const;


		bool SaveBufferToImage() const;

		void CycleLightingMode() {
			switch (m_CurrentLightingMode)
			{
			case LightingMode::ObservedArea:
				m_CurrentLightingMode = LightingMode::Radiance;
				std::cout << " \nLIGHTING MODE: " << "RADIANCE" << std::endl;
				break;
			case LightingMode::Radiance:
				m_CurrentLightingMode = LightingMode::BRDF;
				std::cout << " \nLIGHTING MODE: " << "BRDF" << std::endl;
				break;
			case LightingMode::BRDF:
				m_CurrentLightingMode = LightingMode::Combined;
				std::cout << " \nLIGHTING MODE: " << "COMBINED" << std::endl;
				break;
			case LightingMode::Combined:
				m_CurrentLightingMode = LightingMode::ObservedArea;
				std::cout << " \nLIGHTING MODE: " << "OBSERVED AREA" << std::endl;
				break;
			}
		};
		void ToggleShadows() { m_ShadowEnabled = !m_ShadowEnabled; }

	private:
		SDL_Window* m_pWindow{};

		SDL_Surface* m_pBuffer{};
		uint32_t* m_pBufferPixels{};

		int m_Width{};
		int m_Height{};

		enum class LightingMode
		{
			ObservedArea,
			Radiance,
			BRDF,
			Combined
		};

		LightingMode m_CurrentLightingMode{ LightingMode::Combined };
		bool m_ShadowEnabled{true};

	};
}
