// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

UCLASS()
class THIRDPERSONSHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Called for forwards and a backwards movement */
	void MoveFoward(float Value);

	void MoveRight(float Value);

	/*Called via input to turn at a given rate
	 * @param Rate This is a normalized rate, i.e 1.0 means 100% of desired turn rate.
	 */
	void TurnAtRate(float Rate);

	/*Called via input to look up/down at a given rate
	* @param Rate This is a normalized rate, i.e 1.0 means 100% of desired turn rate.
	*/
	void LookUpAtRate(float Rate);

	//called when the fire button is pressed
	void FireWeapon();
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = Camera,meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	//Camera thata follows the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = Camera,meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/**Base turn rate, in deg/sec. Other scaling may affect turn rate*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = Camera,meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	/**Base look up/down rate, in deg/sec. Other scaling may affect turn rate*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = Camera,meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;

	//Randomized gunshot sound	cue
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound;

	//Flash Spwaned at barrel socket
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;

	//Montage for firing the weapon
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HipFireMontage;

	//Particles spawned upon bullet impact
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;
	
public:
	/**Returns CameraBoom subobject */
	FORCEINLINE USpringArmComponent* GetCameraBoom() const {return CameraBoom;};

	/**Returns FollowCamera object*/
	FORCEINLINE UCameraComponent* GetFollowCamera() const {return FollowCamera;};
	
};
