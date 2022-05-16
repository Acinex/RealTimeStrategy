#include "RTSCameraBoundsVolume.h"

#include "Components/BrushComponent.h"


ARTSCameraBoundsVolume::ARTSCameraBoundsVolume()
{
	GetBrushComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
}
