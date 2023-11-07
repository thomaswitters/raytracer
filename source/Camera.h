#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Math.h"
#include "Timer.h"
#include <iostream>

namespace dae
{
	struct Camera
	{
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle):
			origin{_origin},
			fovAngle{_fovAngle}
		{
		}


		Vector3 origin{};
		float fovAngle{90.f};

		//Vector3 forward{Vector3(0.266f, -0.453f, 0.860f)};
		Vector3 forward{ Vector3::UnitZ };

		

		/*Matrix finalRotation{};

		forward = finalRotation.TransformVector(Vector3::UnitZ)
		forward*/
		Vector3 up{Vector3::UnitY};
		Vector3 right{Vector3::UnitX};

		float totalPitch{0.f};
		float totalYaw{0.f};

		Matrix cameraToWorld{};

		


		Matrix CalculateCameraToWorld()
		{
			//todo: W2
			//Matrix Translation = Matrix::CreateTranslation(origin);
			Matrix rotationY = Matrix::CreateRotationY(totalYaw);
			Matrix rotationX = Matrix::CreateRotationX(totalPitch);

			Matrix finalRotation = rotationX * rotationY;
			forward = finalRotation.TransformVector(Vector3::UnitZ);
			forward.Normalized();

			Vector3 localRight = Vector3::Cross(up, forward);
			localRight.Normalized();

			Vector3 localUp = Vector3::Cross(forward, localRight);
			localUp.Normalized();

			Matrix cameraONB {
				localRight,
				localUp,
				forward,
				origin
			};

			return cameraONB;
		}

		void Update(Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();

			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

			float movementSpeed{6.f};
			//Mouse Input
			
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);
			
			Vector3 localRight = Vector3::Cross(up, forward);
			localRight.Normalized();
			//todo: W2
			if (pKeyboardState[SDL_SCANCODE_W])
			{
				//origin.z += movementSpeed * deltaTime;
				origin += forward * deltaTime * movementSpeed;
			}
			if (pKeyboardState[SDL_SCANCODE_S])
			{
				origin -= forward * deltaTime * movementSpeed;
			}
			if (pKeyboardState[SDL_SCANCODE_A])
			{
				origin -= localRight * deltaTime * movementSpeed;
			}
			if (pKeyboardState[SDL_SCANCODE_D])
			{
				origin += localRight * deltaTime * movementSpeed;
			}


			
			if (SDL_BUTTON_LMASK == mouseState)
			{
				origin -= forward * float(mouseY) * deltaTime * 2.f;
				totalYaw += float(mouseX) * deltaTime/2;
			}
			if (SDL_BUTTON_RMASK == mouseState)
			{
				
				totalPitch += float(mouseY) * deltaTime;
				totalYaw += float(mouseX) * deltaTime;
				
			}
		}
	};
}
