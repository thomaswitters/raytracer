#pragma once
#include <cassert>
#include "Math.h"


namespace dae
{
	namespace BRDF
	{
		/**
		 * \param kd Diffuse Reflection Coefficient
		 * \param cd Diffuse Color
		 * \return Lambert Diffuse Color
		 */
		static ColorRGB Lambert(float kd, const ColorRGB& cd)
		{
			//todo: W3
			//kd = std::max(0.0f, std::min(1.0f, kd));

			// Calculate the Lambertian diffuse reflection color
			ColorRGB result;
			result.r = kd * (cd.r / PI);
			result.g = kd * (cd.r / PI);
			result.b = kd * (cd.b / PI);

			//return kd * (cd / PI);

			return result;

			//assert(false && "Not Implemented Yet");
			//return {};
		}

		static ColorRGB Lambert(const ColorRGB& kd, const ColorRGB& cd)
		{
			//todo: W3
			/*ColorRGB clamped_kd(std::max(0.0f, std::min(1.0f, kd.r)),
				std::max(0.0f, std::min(1.0f, kd.g)),
				std::max(0.0f, std::min(1.0f, kd.b)));*/

			// Calculate the Lambertian diffuse reflection color
			ColorRGB result;
			result.r = /*clamped_*/kd.r * cd.r;
			result.g = /*clamped_*/kd.g * cd.g;
			result.b = /*clamped_*/kd.b * cd.b;

			return result;
			//return {};
		}

		/**
		 * \brief todo
		 * \param ks Specular Reflection Coefficient
		 * \param exp Phong Exponent
		 * \param l Incoming (incident) Light Direction
		 * \param v View Direction
		 * \param n Normal of the Surface
		 * \return Phong Specular Color
		 */
		static ColorRGB Phong(float ks, float exp, const Vector3& l, const Vector3& v, const Vector3& n)
		{
			//exp = std::max(0.0f, exp);

			//Vector3 reflection = Vector3::Reflect(l, n);
			//float dotRV = Vector3::Dot(reflection, v);


			Vector3 reflection = l - (2 * Vector3::Dot(n, l)) * n;
			float dotRV = Vector3::Dot(reflection, v);

			
			

			
			
			
			if(dotRV > 0)
			{
				float phongSpecularReflection = ks * (std::pow(dotRV, exp));
				return ColorRGB(phongSpecularReflection, phongSpecularReflection, phongSpecularReflection);
			}
			else
			{
				return ColorRGB(0.0f, 0.0f, 0.0f);
			}
		}

		/**
		 * \brief BRDF Fresnel Function >> Schlick
		 * \param h Normalized Halfvector between View and Light directions
		 * \param v Normalized View direction
		 * \param f0 Base reflectivity of a surface based on IOR (Indices Of Refrection), this is different for Dielectrics (Non-Metal) and Conductors (Metal)
		 * \return
		 */
		static ColorRGB FresnelFunction_Schlick(const Vector3& h, const Vector3& v, const ColorRGB& f0)
		{
			//1. Calculate the Base Reflecticity (F0)
			//2. Implement BRDF::FresnelFunction_Schlick(Screenshot reflects the output of this function)

			ColorRGB one(1.f, 1.f, 1.f);
			ColorRGB juist{};
			juist.r = one.r - f0.r;
			juist.g = one.g - f0.g;
			juist.b = one.b - f0.b;

			// Calculate the Schlick approximation for the Fresnel reflectivity
			ColorRGB fresnel = f0 + (juist * std::pow(1.0f - Vector3::Dot(h, v), 5.0f));

			return fresnel;
		}

		/**
		 * \brief BRDF NormalDistribution >> Trowbridge-Reitz GGX (UE4 implemetation - squared(roughness))
		 * \param n Surface normal
		 * \param h Normalized half vector
		 * \param roughness Roughness of the material
		 * \return BRDF Normal Distribution Term using Trowbridge-Reitz GGX
		 */
		static float NormalDistribution_GGX(const Vector3& n, const Vector3& h, float roughness)
		{
			float alpha2 = roughness * roughness;
			float NdotH2 = float(std::pow(Vector3::Dot(n, h), 2));
			return float(alpha2 /  (PI * (std::pow(NdotH2 * (alpha2 - 1) + 1, 2))));

		}


		/**
		 * \brief BRDF Geometry Function >> Schlick GGX (Direct Lighting + UE4 implementation - squared(roughness))
		 * \param n Normal of the surface
		 * \param v Normalized view direction
		 * \param roughness Roughness of the material
		 * \return BRDF Geometry Term using SchlickGGX
		 */
		static float GeometryFunction_SchlickGGX(const Vector3& n, const Vector3& v, float roughness)
		{
			float k = (roughness + 1.0f) * (roughness + 1.0f) / 8.0f;
			float NdotV = Vector3::Dot(n, v);
			return NdotV / (NdotV * (1.0f - k) + k);

		}

		/**
		 * \brief BRDF Geometry Function >> Smith (Direct Lighting)
		 * \param n Normal of the surface
		 * \param v Normalized view direction
		 * \param l Normalized light direction
		 * \param roughness Roughness of the material
		 * \return BRDF Geometry Term using Smith (> SchlickGGX(n,v,roughness) * SchlickGGX(n,l,roughness))
		 */
		static float GeometryFunction_Smith(const Vector3& n, const Vector3& v, const Vector3& l, float roughness)
		{
			float G1 = GeometryFunction_SchlickGGX(n, v, roughness);
			float G2 = GeometryFunction_SchlickGGX(n, l, roughness);
			return G1 + G2;
		}

	}
}