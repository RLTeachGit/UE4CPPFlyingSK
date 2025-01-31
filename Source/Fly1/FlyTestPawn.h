// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "FlyTestPawn.generated.h"

UCLASS()
class FLY1_API AFlyTestPawn : public APawn
{
    GENERATED_BODY()

    /** StaticMesh component that will be the visuals for our flying crow */
    UPROPERTY(Category = Mesh, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    class USkeletalMeshComponent* FlyingPawnComp;

    UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    class USphereComponent* FakeRoot;

    /** Spring arm that will offset the camera */
    UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* SpringArm;

    /** Camera component that will be our viewpoint */
    UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* Camera;
public:
    AFlyTestPawn();

    // Begin AActor overrides
    virtual void Tick(float DeltaSeconds) override;
    virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
    // End AActor overrides

protected:

    // Begin APawn overrides
    virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override; // Allows binding actions/axes to functions
    // End APawn overrides

    /** Bound to the thrust axis */
    void ThrustInput(float Val);
    
    /** Bound to the vertical axis */
    void MoveUpInput(float Val);

    /** Bound to the horizontal axis */
    void MoveRightInput(float Val);

private:

    /** How quickly forward speed changes */
    UPROPERTY(Category=Plane, EditAnywhere)
    float Acceleration;

    /** How quickly pawn can steer */
    UPROPERTY(Category=Plane, EditAnywhere)
    float TurnSpeed;

    /** Max forward speed */
    UPROPERTY(Category = Pitch, EditAnywhere)
    float MaxSpeed;

    /** Min forward speed */
    UPROPERTY(Category=Yaw, EditAnywhere)
    float MinSpeed;

    /** Current forward speed */
    float CurrentForwardSpeed;

    /** Current yaw speed */
    float CurrentYawSpeed;

    /** Current pitch speed */
    float CurrentPitchSpeed;

    /** Current roll speed */
    float CurrentRollSpeed;

public:
    /** Returns PlaneMesh subobject **/
    FORCEINLINE class USkeletalMeshComponent* GetPlaneMesh() const { return FlyingPawnComp; }
    /** Returns SpringArm subobject **/
    FORCEINLINE class USpringArmComponent* GetSpringArm() const { return SpringArm; }
    /** Returns Camera subobject **/
    FORCEINLINE class UCameraComponent* GetCamera() const { return Camera; }
};
