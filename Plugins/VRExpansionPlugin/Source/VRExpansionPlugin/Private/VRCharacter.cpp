// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "VRExpansionPluginPrivatePCH.h"
#include "Runtime/Engine/Private/EnginePrivate.h"

#include "VRCharacter.h"


AVRCharacter::AVRCharacter(const FObjectInitializer& ObjectInitializer)
	//: Super(ObjectInitializer.SetDefaultSubobjectClass<UVRRootComponent>(ACharacter::CapsuleComponentName).SetDefaultSubobjectClass<UVRCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
	: Super(ObjectInitializer.DoNotCreateDefaultSubobject(ACharacter::MeshComponentName).SetDefaultSubobjectClass<UVRRootComponent>(ACharacter::CapsuleComponentName).SetDefaultSubobjectClass<UVRCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	VRRootReference = NULL;
	if (GetCapsuleComponent())
	{
		VRRootReference = Cast<UVRRootComponent>(GetCapsuleComponent());
		VRRootReference->SetCapsuleSize(20.0f, 96.0f);
	}

	VRReplicatedCamera = CreateDefaultSubobject<UReplicatedVRCameraComponent>(TEXT("VR Replicated Camera"));
	if (VRReplicatedCamera)
	{
		VRReplicatedCamera->SetupAttachment(RootComponent);
		// By default this will tick after the root, root will be one tick behind on position. Doubt it matters much
		//RootComponent->AddTickPrerequisiteComponent(VRReplicatedCamera);
	}

	ParentRelativeAttachment = CreateDefaultSubobject<UParentRelativeAttachmentComponent>(TEXT("Parent Relative Attachment"));
	if (ParentRelativeAttachment && VRReplicatedCamera)
	{
		ParentRelativeAttachment->SetupAttachment(VRReplicatedCamera);

	//	if (GetMesh())
	//	{
		//	GetMesh()->SetupAttachment(ParentRelativeAttachment);
		//}
	}

	LeftMotionController = CreateDefaultSubobject<UGripMotionControllerComponent>(TEXT("Left Grip Motion Controller"));
	if (LeftMotionController)
	{
		LeftMotionController->SetupAttachment(RootComponent);
		LeftMotionController->Hand = EControllerHand::Left;
		
		// Keep the controllers ticking after movement
		if (this->GetCharacterMovement())
		{
			LeftMotionController->AddTickPrerequisiteComponent(this->GetCharacterMovement());
		}
	}

	RightMotionController = CreateDefaultSubobject<UGripMotionControllerComponent>(TEXT("Right Grip Motion Controller"));
	if (RightMotionController)
	{
		RightMotionController->SetupAttachment(RootComponent);
		RightMotionController->Hand = EControllerHand::Right;

		// Keep the controllers ticking after movement
		if (this->GetCharacterMovement())
		{
			RightMotionController->AddTickPrerequisiteComponent(this->GetCharacterMovement());
		}
	}

}


bool AVRCharacter::TeleportTo(const FVector& DestLocation, const FRotator& DestRotation, bool bIsATest, bool bNoCheck)
{
	bool bTeleportSucceeded = Super::TeleportTo(DestLocation, DestRotation, bIsATest, bNoCheck);

	if (bTeleportSucceeded)
	{
		if (LeftMotionController)
			LeftMotionController->PostTeleportMoveGrippedActors();

		if (RightMotionController)
			RightMotionController->PostTeleportMoveGrippedActors();

		// Regenerate the capsule offset location
		if (VRRootReference)
			VRRootReference->GenerateOffsetToWorld();
	}

	return bTeleportSucceeded;
}
