// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenu.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "UObject/ConstructorHelpers.h"

#include "ServerRow.h"

 UMainMenu::UMainMenu(const FObjectInitializer& ObjectInitializer)
{
	 ConstructorHelpers::FClassFinder<UUserWidget> ServerRowBPClass(TEXT("/Game/MenuSystem/WBP_ServerRow"));
	 if (!ensure(ServerRowBPClass.Class != nullptr)) return;

	 ServerRowClass = ServerRowBPClass.Class;
}


 bool UMainMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;

	HostButton->OnClicked.AddDynamic(this, &UMainMenu::OpenHostMenu);

	if (!ensure(CancelHostMenuButton != nullptr)) return false;
	CancelHostMenuButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

	if (!ensure(ConfirmHostMenuButton != nullptr)) return false;
	ConfirmHostMenuButton->OnClicked.AddDynamic(this, &UMainMenu::HostServer);

	if (!ensure(JoinButton != nullptr)) return false;
	JoinButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);

	if (!ensure(CancelJoinMenuButton != nullptr)) return false;
	CancelJoinMenuButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

	if (!ensure(ConfirmJoinMenuButton != nullptr)) return false;
	ConfirmJoinMenuButton->OnClicked.AddDynamic(this, &UMainMenu::JoinServer);

	if (!ensure(QuitGameButton != nullptr)) return false;
	QuitGameButton->OnClicked.AddDynamic(this, &UMainMenu::QuitPressed);

	return true;
}

 void UMainMenu::OpenHostMenu()
 {
	 MenuSwitcher->SetActiveWidget(HostMenu);
 }

void UMainMenu::HostServer()
{
	if (MainMenuInterface != nullptr)
	{
		FString ServerName = ServerHostName->Text.ToString();
		MainMenuInterface->Host(ServerName);
	}
}

 void UMainMenu::SetServerList(TArray<FServerData> ServerNames)
 {
	 UWorld* World = this->GetWorld();
	 if (!ensure(World != nullptr)) return;

	 ServerList->ClearChildren();

	 uint32 i = 0;

	 for (const FServerData& ServerData : ServerNames)
	 {
		 UServerRow* Row = CreateWidget<UServerRow>(World, ServerRowClass);
		 if (!ensure(Row != nullptr)) return;

		 Row->ServerName->SetText(FText::FromString(ServerData.Name));
		 Row->HostUser->SetText(FText::FromString(ServerData.HostUsername));
		 FString FractionText = FString::Printf(TEXT("%d/%d"), ServerData.CurrentPlayers, ServerData.MaxPlayers);
		 Row->ConnectionFraction->SetText(FText::FromString(FractionText));
		 Row->Setup(this, i);
		 ++i;

		 ServerList->AddChild(Row);
	 }

 }

 void UMainMenu::SelectIndex(uint32 Index)
 {
	 SelectedIndex = Index;
	 UpdateChildren();
 }

 void UMainMenu::UpdateChildren()
 {
	 for (int32 i = 0; i < ServerList->GetChildrenCount(); ++i)
	 {
		 auto Row = Cast<UServerRow>(ServerList->GetChildAt(i));
		 if (Row != nullptr)
		 {
			 Row->Selected = (SelectedIndex.IsSet() && SelectedIndex.GetValue() == i);
		 }
	 }
 }

void UMainMenu::JoinServer()
{
	if (SelectedIndex.IsSet() && MainMenuInterface != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected index %d."), SelectedIndex.GetValue());
		MainMenuInterface->Join(SelectedIndex.GetValue());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected index not set."));
	}
}

void UMainMenu::OpenJoinMenu()
{
	if (!ensure(MenuSwitcher != nullptr)) return;
	if (!ensure(JoinMenu != nullptr)) return;
	MenuSwitcher->SetActiveWidget(JoinMenu);
	if (MainMenuInterface != nullptr)
	{
		MainMenuInterface->RefreshServerList();
	}
}

void UMainMenu::OpenMainMenu()
{
	if (!ensure(MenuSwitcher != nullptr)) return;
	if (!ensure(JoinMenu != nullptr)) return;
	MenuSwitcher->SetActiveWidget(MainMenu);
}

void UMainMenu::QuitPressed()
{
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ConsoleCommand("quit");
}
