// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

#include "Camera/CameraComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"

// Sets default values
AShooterCharacter::AShooterCharacter() :
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create a Camera boom (pulls in towards the character if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.f; // the camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // rotate the arm based on the controller

	//Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // attach camera to end of boom
	FollowCamera->bUsePawnControlRotation=false; // Camera does not rotate relative to arm

	/*Don't rotate when the controller rotates, let the controller only affect the camera*/
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character moviment
	GetCharacterMovement()->bOrientRotationToMovement = true; // character moves in the direction of input
	GetCharacterMovement()->RotationRate = FRotator(0.f,540.f,0.f); // at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Log Examples
	// UE_LOG(LogTemp,Warning,TEXT("BeginPlay() Called"));
	// int myInt{42};
	// UE_LOG(LogTemp,Warning,TEXT("int myInt: %d"),myInt);
	//
	// //Format Specifiers
	// float myFloat{ 3.14159f};
	// UE_LOG(LogTemp,Warning,TEXT("float myFloat: %f"),myFloat);
	//
	// double myDouble{0.000756};
	// UE_LOG(LogTemp,Warning,TEXT("double myDouble: %lf"),myDouble);
	//
	// char myChar{'L'};
	// UE_LOG(LogTemp,Warning,TEXT("char myChar: %c"),myChar);
	//
	// wchar_t wideChar{L'J'};
	// UE_LOG(LogTemp,Warning,TEXT("wchar_t wideChar: %lc"),wideChar);
	//
	// bool myBool{true};
	// UE_LOG(LogTemp,Warning,TEXT("bool myBool: %d"),myBool);
	//
	// UE_LOG(LogTemp,Warning,TEXT("int: %d, float: %f, bool: %d"),myInt,myFloat,false);
	//
	// FString myString{TEXT("My String")};
	// UE_LOG(LogTemp,Warning,TEXT("FString myString: %s"),*myString);
	//
	// UE_LOG(LogTemp,Warning,TEXT("Name of Instance: %s"),*GetName());
	
}

void AShooterCharacter::MoveFoward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		///find out which way is foward
		const FRotator Rotation{Controller->GetControlRotation()};
		const FRotator YawRotation{0,Rotation.Yaw,0};

		const FVector Direction{FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X)};
		AddMovementInput(Direction,Value);
	}
}

void AShooterCharacter::MoveRight(float Value)
{
	///find out which way is right
	const FRotator Rotation{Controller->GetControlRotation()};
	const FRotator YawRotation{0,Rotation.Yaw,0};

	const FVector Direction{FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y)};
	AddMovementInput(Direction,Value);
	
}

void AShooterCharacter::TurnAtRate(float Rate)
{
	//Calculate Delta for this frame from the rate information
	AddControllerYawInput(Rate* BaseTurnRate * GetWorld()->GetDeltaSeconds()); // deg/sec *sec/frame
}

void AShooterCharacter::LookUpAtRate(float Rate)
{
	//Calculate Delta for this frame from the rate information
	AddControllerPitchInput(Rate* BaseLookUpRate * GetWorld()->GetDeltaSeconds()); // deg/sec *sec/frame
}

void AShooterCharacter::FireWeapon()
{
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this,FireSound);
	}
	const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket");
	if (BarrelSocket)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());

		if(MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),MuzzleFlash,SocketTransform);
		}

		FHitResult FireHit;
		const FVector Start{SocketTransform.GetLocation()};
		const FQuat Rotation{SocketTransform.GetRotation()};
		const FVector RotationAxis{Rotation.GetAxisX()};
		const FVector End{Start + RotationAxis *50'000.f};
		
		GetWorld()->LineTraceSingleByChannel(FireHit,Start,End,ECollisionChannel::ECC_Visibility);
		if (FireHit.bBlockingHit)
		{
			DrawDebugLine(GetWorld(),Start,End,FColor::Red,false,2.f);
			DrawDebugPoint(GetWorld(),FireHit.Location,5.f,FColor::Red,false,2.f);

			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),ImpactParticles,FireHit.Location);
			}
		}
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward",this,&AShooterCharacter::MoveFoward);
	PlayerInputComponent->BindAxis("MoveRight",this,&AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnRate",this,&AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate",this,&AShooterCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("Turn",this,&APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp",this,&APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump",IE_Pressed,this,&ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump",IE_Released,this,&ACharacter::StopJumping);

	PlayerInputComponent->BindAction("FireButton",IE_Pressed,this,&AShooterCharacter::FireWeapon);

}

