#pragma once
#include "Math.h"
#include "DataTypes.h"
#include "BRDFs.h"

namespace dae
{
#pragma region Material BASE
	class Material
	{
	public:
		Material() = default;
		virtual ~Material() = default;

		Material(const Material&) = delete;
		Material(Material&&) noexcept = delete;
		Material& operator=(const Material&) = delete;
		Material& operator=(Material&&) noexcept = delete;

		/**
		 * \brief Function used to calculate the correct color for the specific material and its parameters
		 * \param hitRecord current hitrecord
		 * \param l light direction
		 * \param v view direction
		 * \return color
		 */
		virtual ColorRGB Shade(const HitRecord& hitRecord = {}, const Vector3& l = {}, const Vector3& v = {}) = 0;
	};
#pragma endregion

#pragma region Material SOLID COLOR
	//SOLID COLOR
	//===========
	class Material_SolidColor final : public Material
	{
	public:
		Material_SolidColor(const ColorRGB& color): m_Color(color)
		{

		}

		ColorRGB Shade(const HitRecord& hitRecord, const Vector3& l, const Vector3& v) override
		{
			return m_Color;
		}

	private:
		ColorRGB m_Color{colors::White};
	};


	
#pragma endregion

#pragma region Material LAMBERT
	//LAMBERT
	//=======
	class Material_Lambert final : public Material
	{
	public:
		Material_Lambert(const ColorRGB& diffuseColor, float diffuseReflectance) :
			m_DiffuseColor(diffuseColor), m_DiffuseReflectance(diffuseReflectance){}

		ColorRGB Shade(const HitRecord& hitRecord = {}, const Vector3& l = {}, const Vector3& v = {}) override
		{
			return BRDF::Lambert(m_DiffuseReflectance, m_DiffuseColor);
		}

	private:
		ColorRGB m_DiffuseColor{colors::White};
		float m_DiffuseReflectance{1.f}; //kd
	};
#pragma endregion

#pragma region Material LAMBERT PHONG
	//LAMBERT-PHONG
	//=============
	class Material_LambertPhong final : public Material
	{
	public:
		Material_LambertPhong(const ColorRGB& diffuseColor, float kd, float ks, float phongExponent):
			m_DiffuseColor(diffuseColor), m_DiffuseReflectance(kd), m_SpecularReflectance(ks),
			m_PhongExponent(phongExponent)
		{
		}

		ColorRGB Shade(const HitRecord& hitRecord = {}, const Vector3& l = {}, const Vector3& v = {}) override
		{
			//todo: W3
			

			return BRDF::Lambert(m_DiffuseReflectance, m_DiffuseColor) 
				+ BRDF::Phong(m_SpecularReflectance, m_PhongExponent, l, -v, hitRecord.normal);
			
		}
		
	private:
		ColorRGB m_DiffuseColor{colors::White};
		float m_DiffuseReflectance{0.5f}; //kd
		float m_SpecularReflectance{0.5f}; //ks
		float m_PhongExponent{1.f}; //Phong Exponent
	};
#pragma endregion

#pragma region Material COOK TORRENCE
	//COOK TORRENCE
	class Material_CookTorrence final : public Material
	{
	public:
		Material_CookTorrence(const ColorRGB& albedo, float metalness, float roughness):
			m_Albedo(albedo), m_Metalness(metalness), m_Roughness(roughness)
		{
		}

		ColorRGB Shade(const HitRecord& hitRecord = {}, const Vector3& l = {}, const Vector3& v = {}) override
		{
			ColorRGB specularComponent(0.0f, 0.0f, 0.0f);
			ColorRGB diffuseComponent(0.0f, 0.0f, 0.0f);

			Vector3 h = Vector3(v.Normalized() + l.Normalized()).Normalized();

			if (AreEqual(m_Metalness, 0)) {
				ColorRGB f0Dielectric(0.04f, 0.04f, 0.04f);

				ColorRGB fresnel = BRDF::FresnelFunction_Schlick(hitRecord.normal, v, f0Dielectric);
				float normalDistribution = BRDF::NormalDistribution_GGX(hitRecord.normal, h, m_Roughness);
				//float geometry = BRDF::GeometryFunction_SchlickGGX(hitRecord.normal, v, m_Roughness);
				float geometry = BRDF::GeometryFunction_Smith(hitRecord.normal, v, l, m_Roughness);

				specularComponent += (fresnel * geometry * normalDistribution) / (4 * (Vector3::Dot(v, hitRecord.normal) * (Vector3::Dot(l, hitRecord.normal))));
			}
			else {
				ColorRGB fresnel = BRDF::FresnelFunction_Schlick(hitRecord.normal, v, m_Albedo);
				float normalDistribution = BRDF::NormalDistribution_GGX(hitRecord.normal, h, m_Roughness);
				//float geometry = BRDF::GeometryFunction_SchlickGGX(hitRecord.normal, v, m_Roughness);
				float geometry = BRDF::GeometryFunction_Smith(hitRecord.normal, v, l, m_Roughness);

				specularComponent += (fresnel * geometry * normalDistribution) / (4 * (Vector3::Dot(v, hitRecord.normal) * (Vector3::Dot(l, hitRecord.normal))));
			}

			if (m_Metalness < 0.5f) {
				float LambertianFactor = 1.f / (PI);
				diffuseComponent.r = m_Albedo.r * LambertianFactor;
				diffuseComponent.g = m_Albedo.g * LambertianFactor;
				diffuseComponent.b = m_Albedo.b * LambertianFactor;
				//diffuseComponent = m_Albedo * LambertianFactor;
			}


			return specularComponent + diffuseComponent;
		} 
	
	private:
		ColorRGB m_Albedo{0.955f, 0.637f, 0.538f}; //Copper
		float m_Metalness{1.0f};
		float m_Roughness{0.1f}; // [1.0 > 0.0] >> [ROUGH > SMOOTH]
	};
#pragma endregion
}
