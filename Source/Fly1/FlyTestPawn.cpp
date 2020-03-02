// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FlyTestPawn.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/InputComponent.h"
#include "Animation/AnimBlueprint.h"
#include "UObject/ConstructorHelpers.h"
#include "Animation/AnimBlueprintGeneratedClass.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/World.h"
#include "Engine/SkeletalMesh.h"

AFlyTestPawn::AFlyTestPawn()
{
    // Structure to hold one-time initialization

    // Create static mesh component
    FakeRoot = CreateDefaultSubobject<USphereComponent>(TEXT("Collider")); //Needs fake root so Crow gets a transform
    RootComponent = FakeRoot;

    FlyingPawnComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FlyingMesh0"));

    FlyingPawnComp->SetWorldRotation(FRotator(0, -90, 0)); //Rotate Crow to face forward
    FlyingPawnComp->SetupAttachment(FakeRoot);
    
    // Create a spring arm component
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
    SpringArm->SetupAttachment(FakeRoot); // Attach SpringArm to RootComponent
    SpringArm->TargetArmLength = 200.0f; // The camera follows at this distance behind the character
    SpringArm->SocketOffset = FVector(0.f,0.f,60.f);
    SpringArm->bEnableCameraLag = false; // Do not allow camera to lag
    SpringArm->CameraLagSpeed = 15.f;

    // Create camera component
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName); // Attach the camera
    Camera->bUsePawnControlRotation = false; // Don't rotate camera with controller

    // Set handling parameters
    Acceleration = 700.f;
    TurnSpeed = 70.f;
    MaxSpeed = 4000.f;
    MinSpeed = 500.f;
    CurrentForwardSpeed = 700.f;
}

void AFlyTestPawn::Tick(float DeltaSeconds)
{
    const FVector LocalMove = FVector(CurrentForwardSpeed * DeltaSeconds, 0.f, 0.f);

    // Move plan forwards (with sweep so we stop when we collide with things)
    AddActorLocalOffset(LocalMove, true);

    // Calculate change in rotation this frame
    FRotator DeltaRotation(0,0,0);
    DeltaRotation.Pitch = CurrentPitchSpeed * DeltaSeconds;
    DeltaRotation.Yaw = CurrentYawSpeed * DeltaSeconds;
    DeltaRotation.Roll = CurrentRollSpeed * DeltaSeconds;

    // Rotate plane
    AddActorLocalRotation(DeltaRotation);

    // Call any parent class Tick implementation
    Super::Tick(DeltaSeconds);
}

void AFlyTestPawn::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
    Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

    // Deflect along the surface when we collide.
    FRotator CurrentRotation = GetActorRotation();
    SetActorRotation(FQuat::Slerp(CurrentRotation.Quaternion(), HitNormal.ToOrientationQuat(), 0.025f));
}


void AFlyTestPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    // Check if PlayerInputComponent is valid (not NULL)
    check(PlayerInputComponent);

    // Bind our control axis' to callback functions
    PlayerInputComponent->BindAxis("Thrust", this, &AFlyTestPawn::ThrustInput);
    PlayerInputComponent->BindAxis("MoveUp", this, &AFlyTestPawn::MoveUpInput);
    PlayerInputComponent->BindAxis("MoveRight", this, &AFlyTestPawn::MoveRightInput);
}

void AFlyTestPawn::ThrustInput(float Val)
{
    // Is there any input?
    bool bHasInput = !FMath::IsNearlyEqual(Val, 0.f);
    // If input is not held down, reduce speed
    float CurrentAcc = bHasInput ? (Val * Acceleration) : (-0.5f * Acceleration);
    // Calculate new speed
    float NewForwardSpeed = CurrentForwardSpeed + (GetWorld()->GetDeltaSeconds() * CurrentAcc);
    // Clamp between MinSpeed and MaxSpeed
    CurrentForwardSpeed = FMath::Clamp(NewForwardSpeed, MinSpeed, MaxSpeed);
}

void AFlyTestPawn::MoveUpInput(float Val)
{
    // Target pitch speed is based in input
    float TargetPitchSpeed = (Val * TurnSpeed * -1.f);

    // When steering, we decrease pitch slightly
    TargetPitchSpeed += (FMath::Abs(CurrentYawSpeed) * -0.2f);

    // Smoothly interpolate to target pitch speed
    CurrentPitchSpeed = FMath::FInterpTo(CurrentPitchSpeed, TargetPitchSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}

void AFlyTestPawn::MoveRightInput(float Val)
{
    // Target yaw speed is based on input
    float TargetYawSpeed = (Val * TurnSpeed);

    // Smoothly interpolate to target yaw speed
    CurrentYawSpeed = FMath::FInterpTo(CurrentYawSpeed, TargetYawSpeed, GetWorld()->GetDeltaSeconds(), 2.f);

    // Is there any left/right input?
    const bool bIsTurning = FMath::Abs(Val) > 0.2f;

    // If turning, yaw value is used to influence roll
    // If not turning, roll to reverse current roll value.
    float TargetRollSpeed = bIsTurning ? (CurrentYawSpeed * 0.5f) : (GetActorRotation().Roll * -2.f);

    // Smoothly interpolate roll speed
    CurrentRollSpeed = FMath::FInterpTo(CurrentRollSpeed, TargetRollSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}
