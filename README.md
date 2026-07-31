# N Graduation Project

Unreal Engine 5.4 기반의 3D 어드벤처 RPG 졸업 프로젝트입니다. 플레이어가 몬스터를 처치하며 마나를 모으고, 해금한 몬스터 프리셋으로 변신해 다른 스킬과 전투 방식을 사용하는 구조를 중심으로 제작했습니다.

프로젝트는 C++로 핵심 시스템을 구현하고, Unreal Blueprint와 에셋을 결합해 전투, 보스 패턴, 레벨 진행, UI, 저장/불러오기 흐름을 구성했습니다.

## 프로젝트 개요

| 항목 | 내용 |
| --- | --- |
| 장르 | 3D 액션 RPG / 어드벤처 |
| 엔진 | Unreal Engine 5.4 |
| 개발 언어 | C++ / Blueprint |
| 주요 모듈 | Enhanced Input, UMG, AI Module, NavigationSystem, GameplayTasks, Niagara |
| 플러그인 | PCG, PCGGeometryScriptInterop, ModelingToolsEditorMode |
| 저장소 구조 | Unreal 프로젝트 폴더 `N_Graduation_project/` 중심 구성 |

## 핵심 플레이

- 3인칭 캐릭터 조작, 점프, 대시, 마우스 방향 기반 전투
- 일반 스킬과 특수 스킬을 분리한 액션 입력 구조
- 마나를 사용해 몬스터 프리셋으로 변신하는 캐릭터 성장/전환 시스템
- 근접 공격, 원거리 투사체, 차지, 방어, 상태 이상 등 다양한 스킬 타입
- 보스전 전용 패턴과 이펙트 기반 전투 연출
- 방 단위 진행, 레벨 스트리밍, 저장/불러오기 기반의 진행 관리

## 주요 구현 내용

### 데이터 테이블 기반 전투 데이터

엔티티와 스킬 데이터를 Unreal DataTable로 관리하도록 구성했습니다.

- `FABEntityData`: 몬스터/플레이어 프리셋의 HP, 이동 속도, 일반 스킬, 특수 스킬, 변신 마나 비용, 공격 타입 관리
- `FSkillData`: 스킬 ID, 타입, 사거리, 지속 시간, 쿨타임, 판정 크기, 투사체 속도 관리
- `FSkillEffectData`: 스킬별 데미지, 버프, 상태 이상 등 효과 데이터 관리
- `UABGameSingleton`: 런타임에서 DataTable을 로드하고 `EntityGroupID`, `SkillNameID` 기준으로 조회하는 싱글톤 데이터 허브

이 구조 덕분에 스킬과 몬스터 밸런스 조정이 코드 수정 없이 에디터 데이터 수정 중심으로 이루어질 수 있습니다.

### 플레이어 전투 및 변신 시스템

`AN_Graduation_projectCharacter`, `UPlayerSkillComponent`, `UMyPlayerStatComponent`를 중심으로 플레이어 전투를 구성했습니다.

- Enhanced Input 기반 이동, 점프, 대시, 일반 공격, 특수 공격, 파이 메뉴 입력 처리
- 스킬별 박스/스피어 판정 생성 및 중복 피격 방지
- 투사체 발사, 차지 이동, 넉백, 방어, 쿨타임 타이머 처리
- HP와 마나 상태 관리, 마나 재생, 변신 시 스탯과 프리셋 갱신
- 피격 머티리얼, 무적 처리, 화상/기절 등 상태 효과 연동

### 몬스터 AI와 스킬 실행

몬스터는 `AEntityPreset`, `AEntitySpawner`, `UEntitySkillComponent`와 Behavior Tree Task 계열 클래스로 구성했습니다.

- 스포너가 `EntityGroupID`를 기준으로 데이터 테이블에서 몬스터 데이터를 읽고 프리셋 생성
- 몬스터별 일반/특수 스킬 ID를 데이터로 지정
- HitBox, Projectile, Buff, Charge, FireBall, FreezeBreath, EarthBreaker, SplinterArrow, ShieldGuard 등 스킬 타입별 실행 함수 분리
- `BTTask_Cast...`, `BTService_UpdatePlayerLocation`, `BTD_SkillCondition` 등을 통한 AI 전투 흐름 구성

### 보스전 패턴 시스템

`ABossPatternManager`를 통해 보스전에서 여러 패턴을 관리합니다.

- Thunder: 지정 횟수와 딜레이를 가진 낙뢰 패턴
- Laser: 보스 소켓 기준 레이저 액터 스폰
- Spin: 소켓 배열을 활용한 회전 투사체 패턴
- Heal: 크리스탈 소환 기반 회복 패턴
- Meteor: 일정 간격으로 다수의 운석을 생성하는 광역 패턴

패턴별 타이머와 소켓 기반 스폰을 사용해 보스 액터와 전투 연출을 분리했습니다.

### 레벨 진행과 저장 시스템

`UMyGameInstance`, `UMySaveGame`, `UAutoLevelStreamingComponent`를 통해 진행 상태를 관리합니다.

- 현재 방, 다음 방, 저장 위치, 플레이어 HP, 카메라 각도 저장
- 해금된 몬스터 프리셋 상태 저장
- 보스 시작/클리어 상태 및 패턴 플래그 관리
- 현재 로드된 스트리밍 레벨 이름 조회 기능 제공

## 기술 스택

- Unreal Engine 5.4
- C++
- Blueprint
- Enhanced Input
- UMG
- AI Module / Behavior Tree
- NavigationSystem
- GameplayTasks
- Niagara
- PCG

## 프로젝트 구조

```text
N_Graduation_project/
├─ Config/                         # 엔진, 입력, 게임 설정
├─ Content/
│  ├─ DataTable/                   # EntityDataTable, SkillDataTable, SkillEffectDataTable
│  ├─ BP/                          # Blueprint 기반 게임 오브젝트
│  ├─ Characters/                  # 캐릭터 관련 에셋
│  ├─ Entity/                      # 몬스터/엔티티 관련 에셋
│  ├─ ThirdPerson/                 # 맵과 기본 3인칭 템플릿 기반 리소스
│  ├─ UI/                          # UI 에셋
│  └─ VFX/, Sounds/, Movies/       # 이펙트, 사운드, 영상 리소스
└─ Source/N_Graduation_project/
   ├─ Public/                      # 게임 시스템 헤더
   └─ Private/                     # 게임 시스템 구현
```

## 대표 클래스

| 클래스 | 역할 |
| --- | --- |
| `AN_Graduation_projectCharacter` | 플레이어 캐릭터 입력, 전투, 변신, 피격 처리 |
| `UPlayerSkillComponent` | 플레이어 스킬 실행, 판정, 쿨타임, 투사체, 방어 처리 |
| `UMyPlayerStatComponent` | HP, 마나, 변신 상태, 상태 이상 관리 |
| `UABGameSingleton` | 엔티티/스킬/스킬 효과 DataTable 로드 및 조회 |
| `AEntitySpawner` | 데이터 기반 몬스터 프리셋 스폰 |
| `UEntitySkillComponent` | 몬스터 스킬 실행 및 쿨타임 관리 |
| `ABossPatternManager` | 보스 패턴 실행 및 패턴별 액터 스폰 |
| `UMyGameInstance` | 게임 진행 상태, 저장/불러오기, 보스 상태 관리 |
| `UMySaveGame` | 저장 데이터 구조 |
| `UAutoLevelStreamingComponent` | 스트리밍 레벨 상태 조회 |

## 실행 방법

1. Unreal Engine 5.4를 설치합니다.
2. 저장소를 클론합니다.
3. `N_Graduation_project/N_Graduation_project.uproject`를 Unreal Editor로 엽니다.
4. 필요한 경우 Visual Studio에서 C++ 프로젝트 파일을 생성하고 빌드합니다.
5. Unreal Editor에서 플레이 모드로 실행합니다.
