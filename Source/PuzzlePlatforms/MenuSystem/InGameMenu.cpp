
#include "InGameMenu.h"
#include "Components/Button.h"

bool UInGameMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;

	if (!ensure(BackButton != nullptr)) return false;
	BackButton->OnClicked.AddDynamic(this, &UInGameMenu::BackPressed);

	if (!ensure(QuitButton != nullptr)) return false;
	QuitButton->OnClicked.AddDynamic(this, &UInGameMenu::QuitPressed);

	return true;
}

void UInGameMenu::BackPressed()
{
	Teardown();
}

void UInGameMenu::QuitPressed()
{
	if (MainMenuInterface != nullptr) {
		Teardown();
		MainMenuInterface->LoadMainMenu();
	}
}
