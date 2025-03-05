#include "ABGameSingleton.h"

DEFINE_LOG_CATEGORY(LogABGameSingleton);

UABGameSingleton::UABGameSingleton()
{
	// 만든 DataTable 주소를 가져온다.
	// DataTable은 맵 형태로 Key,Value 값으로 들어온다.
	static ConstructorHelpers::FObjectFinder<UDataTable> EntityDataTableRef(TEXT("/Game/DataTable/EntityDataTable.EntityDataTable"));
	static ConstructorHelpers::FObjectFinder<UDataTable> SkillDataTableRef(TEXT("/Game/DataTable/SkillData.SkillData"));
	static ConstructorHelpers::FObjectFinder<UDataTable> SkillEffectDataTableRef(TEXT("/Game/DataTable/SkillEffectData.SkillEffectData"));

	//if (nullptr != DataTableRef.Object)
	//{
	//	// 갯수가 맞는지 체크
	//	const UDataTable* DataTable = DataTableRef.Object;
	//	check(DataTable->GetRowMap().Num() > 0);
	//	// Key값은 순차적으로 오기 때문에 Key값은 필요없고, Value값만 Array에 저장
	//	TArray<uint8*> ValueArray;
	//	DataTable->GetRowMap().GenerateValueArray(ValueArray);
	//	Algo::Transform(ValueArray, EntityDataTable,
	//		[](uint8* Value)
	//		{
	//			return *reinterpret_cast<FABEntityData*>(Value);
	//		}
	//	);
	//}

	// DataTable, DataMap에 데이터 저장
	if (EntityDataTableRef.Succeeded())
	{
		UDataTable* DataTable = EntityDataTableRef.Object;
		TArray<FABEntityData*> Rows;
		DataTable->GetAllRows<FABEntityData>(TEXT(""), Rows);

		for (FABEntityData* Row : Rows)
		{
			EntityDataTable.Add(*Row);
			EntityDataMap.Add(Row->EntityGroupID, *Row);
		}
	}

	/*	
	for (const FABEntityData& EntityData : EntityDataTable)
		{
			UE_LOG(LogABGameSingleton, Error, TEXT("Entity Name: %s, HP: %d, Move Speed: %d, Normal Skill: %s, Special Skill: %s"),
				*EntityData.EntityName, EntityData.HP, EntityData.MoveSpeed, *EntityData.NormalSkill, *EntityData.SpecialSkill);
		}
	*/ //잘 출력됨
	// 배열의 갯수가 0보다 큰지 확인
	HowManyEntity = EntityDataTable.Num();
	// EntityDataTable에 저장된 데이터를 콘솔에 출력
	ensure(HowManyEntity > 0);
	//UE_LOG(LogABGameSingleton, Error, TEXT("HowManyEntity: %d"), HowManyEntity);
	//-> LogABGameSingleton: Error: HowManyEntity: 7
	

	// DataTable, DataMap에 데이터 저장
	if (SkillDataTableRef.Succeeded())
	{
		UDataTable* DataTable = SkillDataTableRef.Object;
		TArray<FSkillData*> Rows;
		DataTable->GetAllRows<FSkillData>(TEXT(""), Rows);

		for (FSkillData* Row : Rows)
		{
			SkillDataTable.Add(*Row);
			//FSkillDataMap.Add(Row->EntityGroupID, *Row);
		}
	}

	// 확인 완료 
	//for (const FSkillData& SkillData : SkillDataTable)
	//{
	//	UE_LOG(LogABGameSingleton, Error, TEXT("SkillID: %s, SkillName: %s, SkillType: %s, SkillRange: %d, SkillDuration: %.1f, SkillCoolTime: %.1f, SkillTypeShape: %s, SkillTypeSizeX: %d, SkillTypeSizeY: %d, ProjectileSpeed: %d"),
	//		*SkillData.SkillID, *SkillData.SkillName, *SkillData.SkillType, SkillData.SkillRange, SkillData.SkillDuration, SkillData.SkillCoolTime, *SkillData.SkillTypeShape, SkillData.SkillTypeSizeX, SkillData.SkillTypeSizeY, SkillData.ProjectileSpeed);
	//}
	
	// 배열의 갯수가 0보다 큰지 확인
	HowManySkill = SkillDataTable.Num();
	// SkillDataTable에 저장된 데이터를 콘솔에 출력
	ensure(HowManySkill > 0);
	UE_LOG(LogABGameSingleton, Error, TEXT("HowManySkill: %d"), HowManySkill);


	// DataTable, DataMap에 데이터 저장
	if (SkillEffectDataTableRef.Succeeded())
	{
		UDataTable* DataTable = SkillEffectDataTableRef.Object;
		TArray<FSkillEffectData*> Rows;
		DataTable->GetAllRows<FSkillEffectData>(TEXT(""), Rows);

		for (FSkillEffectData* Row : Rows)
		{
			SkillEffectDataTable.Add(*Row);
			//SkillEffectDataMap.Add(Row->EntityGroupID, *Row);
		}
	}

	// 확인 완료 
	//for (const FSkillEffectData& SkillEffectData : SkillEffectDataTable)
	//{
	//	UE_LOG(LogABGameSingleton, Error, TEXT("SkillNameID: %s, EffectID: %s, EffectType: %s, EffectValue01: %.1f, EffectValue02: %.1f"),
	//		*SkillEffectData.SkillNameID, *SkillEffectData.EffectID, *SkillEffectData.EffectType, SkillEffectData.EffectValue01, SkillEffectData.EffectValue02);
	//}
	// 배열의 갯수가 0보다 큰지 확인
	HowManySkillEffect = SkillEffectDataTable.Num();
	// SkillDataTable에 저장된 데이터를 콘솔에 출력
	ensure(HowManySkillEffect > 0);
	UE_LOG(LogABGameSingleton, Error, TEXT("HowManySkillEffect: %d"), HowManySkillEffect);
}

// Singleton Get() 함수
UABGameSingleton& UABGameSingleton::Get()
{
	// CastChecked로 강력하게 검사, GEngine에 있는 아까 적용한 GameSingleton을 가져오기
	UABGameSingleton* Singleton = CastChecked<UABGameSingleton>(GEngine->GameSingleton);
	if (Singleton)
	{
		return *Singleton;
	}
	// 혹시 몰라서 만약 Singleton이 null 값이라면 에러를 띄우도록 함
	// 코드의 흐름을 위해서 return값의 인스턴스를 생성하고 리턴함
	UE_LOG(LogABGameSingleton, Error, TEXT("Invalide Game Singleton"));
	return *NewObject<UABGameSingleton>();
}

// Entity GroupID를 키 값으로 Data 검색 
bool UABGameSingleton::GetEntityDataByGroupID(const FString& GroupID, FABEntityData& OutEntityData) const
{
	if (const FABEntityData* FoundData = EntityDataMap.Find(GroupID))
	{
		OutEntityData = *FoundData;
		return true;
	}
	return false;
}
