#include "Weapon/LMABaseWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeapon, All, All);

ALMABaseWeapon::ALMABaseWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponComponent = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	SetRootComponent(WeaponComponent);

}

void ALMABaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	CurrentAmmoWeapon = AmmoWeapon;
}

void ALMABaseWeapon::Fire()
{
	Shoot();
	WeaponComponent->GetOwner()->GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ALMABaseWeapon::Shoot, FireFrequency, true);
}

void ALMABaseWeapon::Shoot()
{
	if (CanFire)
	{
		const FTransform SocketTransform = WeaponComponent->GetSocketTransform("Muzzle");
		const FVector TraceStart = SocketTransform.GetLocation();
		const FVector ShootDirection = SocketTransform.GetRotation().GetForwardVector();
		const FVector TraceEnd = TraceStart + ShootDirection * TraceDistance;
		//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Black, false, 1.0f, 0, 2.0f);
		FHitResult HitResult;
		FVector TracerEnd = TraceEnd;
		GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);
		if (HitResult.bBlockingHit)
		{
			MakeDamage(HitResult);
			DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 5.0f, 24, FColor::Red, false, 1.0f);
			TracerEnd = HitResult.ImpactPoint;
		}
		SpawnTrace(TraceStart, TracerEnd);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShootWave, TraceStart);
		DecrementBullets();
	}
}

void ALMABaseWeapon::StopFire() {
	WeaponComponent->GetOwner()->GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void ALMABaseWeapon::ChangeClip()
{
	CurrentAmmoWeapon.Bullets = AmmoWeapon.Bullets;
}

bool ALMABaseWeapon::IsClipFull() const {
	return CurrentAmmoWeapon.Bullets == AmmoWeapon.Bullets;
}

bool ALMABaseWeapon::IsCurrentClipEmpty() const
{
	return CurrentAmmoWeapon.Bullets == 0;
}

void ALMABaseWeapon::DecrementBullets()
{
	CurrentAmmoWeapon.Bullets--;
	UE_LOG(LogWeapon, Display, TEXT("Bullets = %s"), *FString::FromInt(CurrentAmmoWeapon.Bullets));
	if (IsCurrentClipEmpty())
	{
		OnClipEmpty.Broadcast();
	}
}

void ALMABaseWeapon::SpawnTrace(const FVector& TraceStart, const FVector& TraceEnd)
{
	const auto TraceFX = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceEffect, TraceStart);
	if (TraceFX)
	{
		TraceFX->SetNiagaraVariableVec3(TraceName, TraceEnd);
	}
}

void ALMABaseWeapon::MakeDamage(const FHitResult& HitResult)
{
	const auto Zombie = HitResult.GetActor();
	if (!Zombie)
		return;
	const auto Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Pawn)
		return;
	const auto Controller = Pawn->GetController<APlayerController>();
	if (!Controller)
		return;
	Zombie->TakeDamage(Damage, FDamageEvent(), Controller, this);
}
