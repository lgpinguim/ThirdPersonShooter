// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UThirdPersonShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>( TryGetPawnOwner());
	}
	if (ShooterCharacter)
	{
		//Get the lateral speed from velocity
		FVector Velocity {ShooterCharacter->GetVelocity()};
		Velocity.Z=0;
		Speed = Velocity.Size();

		//Is the character in the air?
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

		//Is the character accelerating
		if (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0)
		{
			bIsAccelerating = true;
		}
		else
		{
			bIsAccelerating = false;
		}
		
	}
}

void UThirdPersonShooterAnimInstance::NativeInitializeAnimation()
{
	ShooterCharacter = Cast<AShooterCharacter>( TryGetPawnOwner());
}
