# 《第一次接触 GAS：60 分钟上手教程》外链依赖内容内联研究

> 研究日期：2026-09-09  
> 目标文档：`docs/your-first-60-minutes-with-gas-zh.md`  
> 范围：找出读者必须跳到外部链接才能理解或完成的内容，并依据 Epic 官方教程、官方文档和官方 API，整理成可以直接补进正文的中文内容。本文是研究稿，不修改正式教程。

## 结论先行

当前教程的主流程完整，但有 10 组知识被一句“详见 / 不展开 / 更多信息”压缩了。其中最明显的断点是冷却与消耗，此外还有 ASC 的放置和初始化时机、属性初始化和动态 AttributeSet、属性变化监听和重钳制、SetByCaller、Execution 与伤害类型、能力输入任务、反应式能力、Gameplay Cue 的打包与触发、联网移动预测。

建议正式改稿时遵循两条原则：

1. 正文给出“能独立完成练习”的最小闭环：概念、编辑器配置、调用时机、失败分支和联网注意事项都写在原位置。
2. Epic 链接保留为段末“官方来源”，不再让链接承担缺失正文的职责。

另外发现两处应在补内容时一并修正：

- 正文 4.3 的链接锚点错指向 `when is ASC ready`，实际应指向 `how do ability costs and cooldowns work`。
- 复制属性的 `OnRep_Health` 不能只是自行广播委托；应调用 `GAMEPLAYATTRIBUTE_REPNOTIFY`。官方推荐的 `DOREPLIFETIME_CONDITION_NOTIFY(..., REPNOTIFY_Always)` + `GAMEPLAYATTRIBUTE_REPNOTIFY` 才能正确更新 GAS 的聚合器、预测值和属性变化委托。

---

## 1. ASC 放在哪里，以及何时初始化 Owner / Avatar

### 建议替换位置

- 0.3 “创建 ASC 子类并挂到角色上”中“详见 Best Practices”之后。
- 0.5 “什么时候该重新调用？”整段。

### 可直接补入正文

ASC 可以放在任意需要属性、GameplayTag、GameplayEffect 或 GameplayAbility 的 Actor 上，但一个 Actor 不应有多个 ASC。对玩家而言，放置位置首先由“状态是否跨重生保留”决定：

| ASC 所在 Actor | 适合场景 | 生命周期 / 联网含义 |
|---|---|---|
| Character / Pawn | 死亡重生后技能、Buff、冷却和属性都重置 | 最简单；Pawn 销毁时 ASC 一起销毁 |
| PlayerState | 换 Pawn、死亡重生后仍保留技能、Buff、冷却和属性 | 多人游戏最常用；PlayerState 会复制给各客户端 |
| PlayerController | 单机中需要跨 Pawn 保留状态 | 多人游戏不适合作为 ASC Owner，因为别人的 PlayerController 不存在于本客户端 |

AI 如果也需要与玩家采用相同的 PlayerState + ASC 架构，可以让 AIController 的 `bWantsPlayerState` 为 true。AIController 只存在于服务器，不适合作为需要复制的 ASC Owner。

`OwnerActor` 是长期代表该实体的 Actor，`AvatarActor` 是当前在世界中承担位置、动画和移动的 Actor。它们可以相同，也可以是 `PlayerState` 与当前 `Character`。Owner 必须能沿所有权关系解析到玩家的 Pawn、PlayerState 或 PlayerController；GAS 会据此缓存 PlayerController，而本地预测能力依赖这个缓存。Avatar 可以暂时为空，但依赖 Pawn、位置或动画的能力此时不能运行。

`InitAbilityActorInfo` 不是“一生只能调一次”的初始化函数。服务器和客户端必须分别调用；只要 Owner 或 Avatar 在本机出现、发生变化，或者先前缺失的 PlayerController 终于可用，就应再次调用：

```cpp
ASC->InitAbilityActorInfo(OwnerActor, AvatarActor);
```

如果 Owner 和 Avatar 没变，只是需要让 ASC 重新解析 PlayerController，可调用：

```cpp
ASC->RefreshAbilityActorInfo();
```

典型调用点如下：

- ASC 在 Character 上：服务器可在 `PossessedBy`，客户端可在 `OnRep_Controller` 或能确认 Controller 已复制完成的位置调用；`BeginPlay` 可以做首次尝试，但不能假定多人客户端此时一定已有 PlayerController。
- ASC 在 PlayerState 上：服务器在 Pawn 的 `PossessedBy` 中以 `PlayerState` 为 Owner、Pawn 为 Avatar 初始化；客户端在 `OnRep_PlayerState` 中做同样的事。PlayerController 自己的 `OnRep_PlayerState` 也是可靠的重试点，因为此时本地 PC 已经存在。
- 换 Pawn / 重生：保留 PlayerState 上的 ASC，只把 Avatar 更新为新 Pawn；持续效果和冷却因此可以保留。

```cpp
// ASC 位于 PlayerState 的常见形态（伪代码，服务器与客户端各自调用）
void AMyCharacter::InitializeAbilityActorInfo()
{
    AMyPlayerState* PS = GetPlayerState<AMyPlayerState>();
    if (!PS) return;

    UMyASC* ASC = PS->GetMyASC();
    ASC->InitAbilityActorInfo(PS, this);
}
```

只有 `InitAbilityActorInfo` 完成，且玩家的 PlayerController 成功缓存后，Local Predicted 能力才算具备激活条件。对于“获得后自动运行”的被动能力，应在能力授予时尝试一次，并在 Owner、Avatar、PlayerController 后续复制完成时再次尝试，而不是只在 BeginPlay 赌一次时序。

### 官方来源

- [GAS Best Practices：ASC、Owner/Avatar 与初始化时机](https://dev.epicgames.com/community/learning/tutorials/DPpd/unreal-engine-gameplay-ability-system-best-practices-for-setup#1abilitysystemcomponent)
- [Epic 文档：Ability System Component And Attributes](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-ability-system-component-and-gameplay-attributes-in-unreal-engine)

---

## 2. 属性初始值、AttributeSet 的添加方式与动态添加风险

### 建议替换位置

- 1.2 “想要设计师可配置的初始值，见 Best Practices”。
- 1.3 “把 Attribute Set 挂到角色”后扩充。

### 可直接补入正文

AttributeSet 有四种常见添加方式：

1. **构造函数中的默认子对象（优先）**：已知某类 Actor 一定拥有该属性集时，用 `CreateDefaultSubobject`。客户端无需等待服务器动态 UObject 复制，就能立即拿到同一个子对象并绑定委托。必须保存为 `UPROPERTY`，否则某些 PIE / GC 时机会把它回收。
2. **`PostInitializeComponents` / `BeginPlay` 中 `AddSet<T>`**：此时蓝图默认值和关卡实例值已经加载，可按配置决定要不要添加某属性集。
3. **运行时 `AddSet<T>`**：装备某件物品后才增加一组属性等场景可用。
4. **ASC 的 `DefaultStartingData`**：设计师在 ASC 细节面板里选择 AttributeSet 类和 DataTable。DataTable 必须使用 `AttributeMetaData` 行结构，行名为 `AttributeSetClass.AttributeName`，否则属性集不会被创建或数值不会匹配。

```cpp
// 已知所有该角色都需要生命属性：首选默认子对象
HealthSet = CreateDefaultSubobject<ULabHealthAttributeSet>(TEXT("HealthSet"));

// 需要读取蓝图配置后再决定：在生命周期稍后添加
void AMyCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    if (bUsesHealth)
    {
        AbilitySystemComp->AddSet<ULabHealthAttributeSet>();
    }
}
```

多人游戏中，运行时创建的 AttributeSet 最终以**服务器创建并复制来的对象**为准。客户端可以先临时创建一个，让此期间到达的 GE 有属性可写，但服务器对象复制到达后，ASC 的 `SpawnedAttributes` 会换成服务器对象。如果你直接绑定了 AttributeSet 自己的委托，应覆写 `OnRep_SpawnedAttributes`：从被替换 / 移除的集合解绑，再向新集合绑定。

活动 GE 与 AttributeSet 的复制到达顺序并无保证。因此需要某属性集的 GE 不应与 `AddSet` 在服务器同一帧完成；应提前添加。移除时反过来，先提前移除依赖它的 GE，再移除 AttributeSet。大部分项目没有必要在运行时移除属性集。

属性初值可按需要的灵活度选择：

| 方法 | 适用情况 |
|---|---|
| AttributeSet 构造函数中 `InitHealth(100)` | 固定编译期常量；只用于任何 GE 生效之前 |
| `PostInitializeComponents` / `BeginPlay` 调 `Init*` | 要读取蓝图默认值或 SpawnActorDeferred 设置的值 |
| BeginPlay 施加初始化 GE | 设计师可编辑；可查曲线、引用其他属性或做动态计算 |
| `AttributeSetInitter` + CurveTable | 大型项目、多角色组、等级曲线；Fortnite 采用此类方案 |
| `DefaultStartingData` | 完全由 ASC 蓝图配置 AttributeSet 和 DataTable 初值 |

初始值若只是“从现在起 Health 就是 100”，通常用 Instant GE 修改 Base Value；若必须保留每个来源，之后可单独移除或重算，例如装备带来的 `MaxHealth +20%`，应使用 Duration / Infinite GE 保存 modifier，而不是把百分比一次性烙进 Base Value。

一个易于设计师维护的初始化方案是：

- Infinite GE：给 `MaxHealth` 一个可移除 / 可叠加的基础 modifier；
- Instant GE：用 Override 把 `Health` 初始化为当前 `MaxHealth`。

### 官方来源

- [GAS Best Practices：如何把 AttributeSet 放到 Actor 上](https://dev.epicgames.com/community/learning/tutorials/DPpd/unreal-engine-gameplay-ability-system-best-practices-for-setup#howshouldiputattributesetsonanactor?)
- [GAS Best Practices：如何初始化属性](https://dev.epicgames.com/community/learning/tutorials/DPpd/unreal-engine/gameplay-ability-system-best-practices-for-setup#howshouldiinitializeattributevalues?)
- [GAS Best Practices：Instant 与 Infinite 初始化选择](https://dev.epicgames.com/community/learning/tutorials/DPpd/unreal-engine/gameplay-ability-system-best-practices-for-setup#iwanttoinitializeattributevaluesshouldiuseaninstantorinfinitegameplayeffect?)

---

## 3. 属性复制、变化监听与 Max 变化后的重钳制

### 建议替换位置

- 1.1 的属性复制示例。
- 3.1 “MaxHealth 变化，重钳制方法见 Best Practices”。
- 3.2 属性监听内容。

### 可直接补入正文

复制属性时建议使用 `ReplicatedUsing`、`DOREPLIFETIME_CONDITION_NOTIFY` 和 `GAMEPLAYATTRIBUTE_REPNOTIFY` 的完整组合。`REPNOTIFY_Always` 很重要：即使服务器复制来的值与客户端预测值表面相同，GAS 也要收到通知才能正确协调预测、聚合器与属性变化委托。

```cpp
UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Health)
FGameplayAttributeData Health;

UFUNCTION()
void OnRep_Health(const FGameplayAttributeData& OldHealth);

void ULabHealthAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(ULabHealthAttributeSet, Health, OldHealth);
}

void ULabHealthAttributeSet::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME_CONDITION_NOTIFY(
        ULabHealthAttributeSet, Health, COND_None, REPNOTIFY_Always);
}
```

跨单机、服务器、本地预测和复制校正都更稳妥的监听方式，是订阅 ASC 的逐属性委托：

```cpp
HealthChangedHandle = AbilitySystemComp
    ->GetGameplayAttributeValueChangeDelegate(
        ULabHealthAttributeSet::GetHealthAttribute())
    .AddUObject(this, &ThisClass::HandleHealthChanged);

void AMyCharacter::HandleHealthChanged(const FOnAttributeChangeData& Data)
{
    UE_LOG(LogTemp, Log, TEXT("Health %.1f -> %.1f"),
        Data.OldValue, Data.NewValue);
}
```

其它可选方式：

- GameplayAbility 蓝图内使用 `WaitForAttributeChange` 能力任务；
- 在 AttributeSet 的 `PostAttributeChange` / `PostAttributeBaseChange` 广播自定义 `BlueprintAssignable` 委托；
- 像 Lyra 的 `HealthComponent` 一样，用可复用 ActorComponent 封装 ASC / AttributeSet 查找和委托转发，让 Widget 不直接理解 GAS。

`PreAttributeChange` 与 `PreAttributeBaseChange` 用来在外部系统观察前阻止非法值。为了覆盖 Current Value 与 Base Value 两种变化路径，通用钳制函数可被两者共同调用：

```cpp
float ULabHealthAttributeSet::ClampAttribute(
    const FGameplayAttribute& Attribute, float NewValue) const
{
    if (Attribute == GetHealthAttribute())
    {
        return FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
    }
    return NewValue;
}
```

但钳制 Health 本身还不够：如果 `MaxHealth` 从 100 降到 60，而 Health 当时是 90，Health 没有发生自己的修改，因而不会自动重新经过 `PreAttributeChange`。需要在 `PostAttributeChange` 监视边界属性并主动把依赖值压回新范围：

```cpp
void ULabHealthAttributeSet::PostAttributeChange(
    const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
    Super::PostAttributeChange(Attribute, OldValue, NewValue);

    if (Attribute == GetMaxHealthAttribute() && GetHealth() > NewValue)
    {
        if (UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent())
        {
            ASC->ApplyModToAttribute(
                GetHealthAttribute(), EGameplayModOp::Override, NewValue);
        }
    }
}
```

这个规则同样适用于 `Mana / MaxMana`、`Ammo / MagazineSize`。不要期待仅仅在 Max 的 `PreAttributeChange` 中改 Health；预回调只应修改当前传入的 `NewValue`，重新约束关联属性应放在 Max 已改变后的回调里。

注意：属性复制可能把服务器同一帧中的多次修改合并成客户端看到的一次最终变化。UI 刷新适合用属性委托；“每一击造成多少伤害”“暴击一次”等离散事件应由 Gameplay Cue 或 Gameplay Event 传递。

### 官方来源

- [GAS Best Practices：监听属性变化](https://dev.epicgames.com/community/learning/tutorials/DPpd/unreal-engine/gameplay-ability-system-best-practices-for-setup#howshouldilistentoattributechanges?)
- [GAS Best Practices：属性钳制](https://dev.epicgames.com/community/learning/tutorials/DPpd/unreal-engine/gameplay-ability-system-best-practices-for-setup#howshouldiclampattributevalues?)
- [GAS Best Practices：Min/Max 改变后重新钳制](https://dev.epicgames.com/community/learning/tutorials/DPpd/unreal-engine/gameplay-ability-system-best-practices-for-setup#howshouldire-clampvaluesafterchangingthemin-max?)
- [Epic 文档：Gameplay Attributes and Attribute Sets（含完整复制范式）](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-attributes-and-attribute-sets-for-the-gameplay-ability-system-in-unreal-engine#replication)

---

## 4. SetByCaller：把运行时数值塞进 GameplayEffectSpec

### 建议替换位置

- 2.4 第 3 项，不要再只给一句定义和外链。

### 可直接补入正文

GameplayEffect 蓝图是“效果模板”，真正施加的是 `GameplayEffectSpec`。SetByCaller 是 Spec 上的一张“GameplayTag → float”表，用于把碰撞速度、蓄力时长、武器随机伤害等运行时结果传给 GE。它适合单个数值；Actor 引用、向量或复杂结构应使用项目自定义 `GameplayEffectContext`。

先创建标签，例如 `Abilities.Parameters.Damage`。在 GE 的 Modifier 中把 Magnitude Calculation Type 设为 **Set By Caller**，Data Tag 选择该标签。施加时不能直接用“Apply GE Class”捷径，而要先创建 Spec、写值、再施加 Spec：

```cpp
const FGameplayTag DamageTag = FGameplayTag::RequestGameplayTag(
    TEXT("Abilities.Parameters.Damage"));

FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
Context.AddSourceObject(this);

FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(
    DamageGEClass, AbilityLevel, Context);

if (SpecHandle.IsValid())
{
    SpecHandle.Data->SetSetByCallerMagnitude(DamageTag, RuntimeDamage);
    SourceASC->ApplyGameplayEffectSpecToTarget(
        *SpecHandle.Data.Get(), TargetASC);
}
```

蓝图中是同一流程：`Make Outgoing Gameplay Effect Spec` → `Assign Tag Set By Caller Magnitude` → `Apply Gameplay Effect Spec To Target / Owner`。

Execution Calculation 中读取：

```cpp
const float IncomingDamage = ExecutionParams.GetOwningSpec()
    .GetSetByCallerMagnitude(
        DamageTag,
        /*WarnIfNotFound=*/true,
        /*DefaultIfNotFound=*/0.0f);
```

SetByCaller 还可以直接驱动 GE 的 Modifier，或作为 Duration GE 的运行时时长。标签与 Spec 中的键必须一致；未写入时通常会产生警告并采用指定默认值，所以不要把“缺值”悄悄当成有效伤害。

底层 API 也支持用 `FName` 作为键，但 Epic 在对应蓝图节点文档中明确建议通常使用 GameplayTag 版本。教程应统一采用标签，避免字符串拼写和重命名问题。

SetByCaller 与 Meta Attribute 不是互斥方案：SetByCaller 表示**怎样把运行时值送进 GE**，Meta Attribute 表示**GE 最终把中间结果写到哪个属性**。常见链路是：SetByCaller 原始伤害 → Execution 读取并计算 → 输出到 Damage 元属性 → AttributeSet 扣 Health。

### 官方来源

- [GAS Best Practices：SetByCaller](https://dev.epicgames.com/community/learning/tutorials/DPpd/unreal-engine/gameplay-ability-system-best-practices-for-setup#whataresetbycallermagnitudes?)
- [GAS Best Practices：给 GE 传额外参数](https://dev.epicgames.com/community/learning/tutorials/DPpd/unreal-engine/gameplay-ability-system-best-practices-for-setup#howdoipassinadditionalparameterswhenapplyingagameplayeffect?)
- [Epic Blueprint API：Assign Tag Set by Caller Magnitude](https://dev.epicgames.com/documentation/en-us/unreal-engine/BlueprintAPI/Ability/GameplayEffect/AssignTagSetbyCallerMagnitude)
- [Epic C++ API：FGameplayEffectSpec::SetSetByCallerMagnitude](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Plugins/GameplayAbilities/FGameplayEffectSpec/SetSetByCallerMagnitude)
- [Epic C++ API：FGameplayEffectSpec::GetSetByCallerMagnitude](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Plugins/GameplayAbilities/FGameplayEffectSpec/GetSetByCallerMagnitude)

---

## 5. Meta Attribute、Execution 与可扩展伤害类型

### 建议替换位置

- 2.4 第 4 项。
- 3.4 与 3.5，补成可实现的完整闭环。

### 可直接补入正文

Meta Attribute 是临时保存计算结果的属性。它不是角色长期状态，而更像 GE 与 AttributeSet 之间的数据驱动输入 / 输出变量。以 `Damage` 为例：Execution 和 Modifiers 共同算出 Damage，AttributeSet 在一次回调中把它换算成护盾、生命、受击事件等，然后立即把 Damage 清零，避免污染下一次结算。

一条可扩展的伤害流水线如下：

```text
运行时代码 / Ability
  → 创建 Damage GE Spec，写入 SetByCaller 原始伤害
  → 服务器运行 GameplayEffectExecutionCalculation
  → 捕获攻防属性并按标签筛选伤害类型
  → 输出 +Damage 元属性
  → AttributeSet::PostGameplayEffectExecute
  → 护盾吸收、Health 钳制、Damage 清零、发 Gameplay Cue
```

Execution 类在构造函数中声明要捕获哪些属性，并指定来自 Source 还是 Target、是否 Snapshot；在 `Execute_Implementation` 中读取 Spec、标签、SetByCaller 和捕获值，最后用 `AddOutputModifier` 输出结果：

```cpp
ULabDamageExecution::ULabDamageExecution()
{
    RelevantAttributesToCapture.Add(
        FGameplayEffectAttributeCaptureDefinition(
            ULabCombatSet::GetDamageResistanceAttribute(),
            EGameplayEffectAttributeCaptureSource::Target,
            /*bSnapshot=*/true));
}

void ULabDamageExecution::Execute_Implementation(
    const FGameplayEffectCustomExecutionParameters& Params,
    FGameplayEffectCustomExecutionOutput& Out) const
{
    const FGameplayEffectSpec& Spec = Params.GetOwningSpec();
    const float RawDamage = Spec.GetSetByCallerMagnitude(
        DamageParamTag, true, 0.0f);

    float Resistance = 0.0f;
    FAggregatorEvaluateParameters Eval;
    Eval.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    Eval.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    Params.AttemptCalculateCapturedAttributeMagnitude(
        DamageResistanceCaptureDef, Eval, Resistance);

    const float FinalDamage = FMath::Max(0.0f, RawDamage * (1.0f - Resistance));
    if (FinalDamage > 0.0f)
    {
        Out.AddOutputModifier(FGameplayModifierEvaluatedData(
            ULabHealthAttributeSet::GetDamageAttribute(),
            EGameplayModOp::Additive,
            FinalDamage));
    }
}
```

上段是结构示例，实际捕获定义最好集中在一个静态定义结构中复用。Execution 所在的 GE 应是 Instant，并在其 Executions 数组中选择该 Calculation Class。Execution 只在服务器执行；客户端预测不能安全地回滚任意自定义 Execution 逻辑。

Meta Attribute 也应限定在 Instant GE 的一次性结算链路。Epic 的 PredictionKey 文档明确说明：Meta Attribute 依赖 Instant Effect 后端触发的 AttributeSet Pre/Post Modify 流程；持续 5 秒去修改 `Damage` 这类中转值没有稳定语义。Execution Calculation 和 Meta Attribute 当前都不预测，因此最终伤害、死亡等结果必须由服务器权威确认；客户端可预测 Cue / Montage 等表现，但不能把预测伤害当成最终状态。

AttributeSet 消费元属性时应写回并清零：

```cpp
if (Data.EvaluatedData.Attribute == GetDamageAttribute())
{
    const float Incoming = FMath::Max(GetDamage(), 0.0f);
    SetDamage(0.0f);

    const float Absorbed = FMath::Min(GetShield(), Incoming);
    SetShield(GetShield() - Absorbed);

    const float ToHealth = Incoming - Absorbed;
    const float OldHealth = GetHealth();
    SetHealth(FMath::Clamp(OldHealth - ToHealth, 0.0f, GetMaxHealth()));

    const float ActualHealthDamage = OldHealth - GetHealth();
    // 用 ActualHealthDamage 触发伤害数字 Cue，而不是显示 RawDamage。
}
```

伤害类型不必为火、冰、物理各复制一整套 `FireDamage / FireCritChance / FireResistance` 属性。Epic 在 Fortnite 的做法是：

- 用通用属性表达数值维度：`Damage`、`CritChance`、`DamageResistance`；
- 用 GameplayTag 表达类型：`Damage.Type.Elemental.Fire`、`Damage.Type.Cold`；
- 伤害 GE 和抗性 GE 都带相应 Asset Tag；
- 可复用 Execution 读取自身 GE 的伤害类型，只评估带匹配类型标签的抗性 modifier。

这样新增毒、电等类型只需新增标签和数据资产，不必继续膨胀 AttributeSet。代价是 `DamageResistance` 的总 Current Value 失去独立意义：同时有 30% 火抗和 30% 冰抗时，调试器看到的“60% 总和”不能直接用于某一次伤害；真正有意义的数值只能在 Execution 按类型过滤后得到。

### 官方来源

- [GAS Best Practices：Meta Attributes](https://dev.epicgames.com/community/learning/tutorials/DPpd/unreal-engine/gameplay-ability-system-best-practices-for-setup#whataremetaattributes?)
- [GAS Best Practices：Meta Attribute 与 SetByCaller 的关系](https://dev.epicgames.com/community/learning/tutorials/DPpd/unreal-engine/gameplay-ability-system-best-practices-for-setup#whenshouldiuseametaattributevssetbycaller?)
- [GAS Best Practices：用标签实现伤害类型与抗性](https://dev.epicgames.com/community/learning/tutorials/DPpd/unreal-engine/gameplay-ability-system-best-practices-for-setup#howshouldiimplementdamagetypeslikefiredamage/fireresistance?)
- [Epic 文档：Gameplay Effects](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-effects-for-the-gameplay-ability-system-in-unreal-engine)
- [Epic 官方 Action RPG：Attributes and Effects（Damage 中转范式）](https://dev.epicgames.com/documentation/en-us/unreal-engine/attributes-and-effects-in-arpg?application_version=4.27)
- [Epic API：FPredictionKey（Execution / Meta Attribute 预测边界）](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Plugins/GameplayAbilities/FPredictionKey)

---

## 6. 能力输入、WaitInputPress / Release、Confirm / Cancel

### 建议替换位置

- 5.1 “详细原理见 Best Practices”。
- 5.6 蓄力技能中补充 WaitInputReleased 的前提和第一次抬起语义。

### 可直接补入正文

能力激活有多种入口：立即授予并激活、按 Class、Tag、Handle、InputID，或由 Ability Trigger 响应 GameplayTag / Gameplay Event。所有入口都只是“尝试激活”，最终仍会检查冷却、消耗、标签和自定义条件。联网时客户端使用这些入口即可；GAS 会根据 `Net Execution Policy` 把激活路由到服务器。通过 Handle 激活时，Handle 本身必须正确复制。

InputID 方案的完整链路是：

1. 定义枚举，枚举项对应固定技能动作；
2. 旧输入系统中创建同名 Action Mappings；
3. `SetupPlayerInputComponent` 调 `BindAbilityActivationToInputComponent`，同时提供 Confirm / Cancel 两个动作名；
4. 服务器 `GiveAbility` 时把枚举值转成 InputID 写入 `FGameplayAbilitySpec`；
5. 玩家按动作键后，ASC 根据 InputID 找到并尝试激活能力。

```cpp
FGameplayAbilitySpec Spec(AbilityClass, AbilityLevel,
    static_cast<int32>(EMyAbilitySlotsEnum::PrimaryAbility));
ASC->GiveAbility(Spec); // 只在服务器授予
```

`WaitInputPress` / `WaitInputRelease` 的前提是能力通过 InputID 关联输入，并且 ASC 已绑定输入组件。二者监听的是**激活该能力的那一个输入**：

- `WaitInputPress`：能力已经激活后，等待玩家再次按下该键；
- `WaitInputRelease`：等待玩家第一次松开激活它的按键，适合蓄力；任务返回的按住时间可参与计算；
- `WaitForConfirm` / `WaitForCancel`：监听 `BindAbilityActivationToInputComponent` 时指定的全局 Confirm / Cancel 动作，常用于目标选择。

能力任务是异步对象。技能结束或取消时，仍在运行的 Ability Tasks 会一并结束并清理。正常逻辑完成后必须调用 `EndAbility`；失败、玩家取消或 Commit 失败时可调用 `CancelAbility` / `EndAbility` 并标记取消。结束能力不会自动付消耗或上冷却，那仍然必须显式 `CommitAbility`。

InputID 依赖一个固定枚举，适合教学、小型项目和固定技能槽，但 Epic 的已发布大型项目不用它。大型项目通常像 Lyra 一样由 Enhanced Input + Input Config + GameplayTag 做输入映射，并在项目 ASC 中转发“按下 / 松开”状态，从而保留输入相关 Ability Tasks 的能力而不被单枚举限制。

### 官方来源

- [GAS Best Practices：能力的激活方式](https://dev.epicgames.com/community/learning/tutorials/DPpd/unreal-engine/gameplay-ability-system-best-practices-for-setup#howdoilettheplayeractivateabilities?)
- [GAS Best Practices：Input ID 完整流程](https://dev.epicgames.com/community/learning/tutorials/DPpd/unreal-engine/gameplay-ability-system-best-practices-for-setup#howdoesgivingandtriggeringabilitiesviainputidwork?)
- [GAS Best Practices：WaitInput 与 Confirm/Cancel](https://dev.epicgames.com/community/learning/tutorials/DPpd/unreal-engine/gameplay-ability-system-best-practices-for-setup#howdoiutilizethewaitinputpress/releaseandwaitforconfirm/canceltasksinabilities?)
- [GAS Best Practices：结束与取消能力](https://dev.epicgames.com/community/learning/tutorials/DPpd/unreal-engine/gameplay-ability-system-best-practices-for-setup#whatdoesendingorcancelingabilitiesdo?)

---

## 7. 冷却与消耗：应补成一个能亲手完成的练习

### 建议替换位置

- 用以下内容整体替换 4.3 的“本教程跳过冷却/消耗的实现”。这是当前最大的学习断点。

### 可直接补入正文

能力的 Cost 和 Cooldown 都是 GameplayEffect 类，分别填在 GameplayAbility 的 **Cost Gameplay Effect Class** 与 **Cooldown Gameplay Effect Class**。

先给属性集增加并复制 `Energy` / `MaxEnergy`，初始都设为 100。创建 `GE_Cost_Energy20`：

- Duration Policy：Instant；
- Modifier Attribute：Energy；
- Operation：Add；
- Magnitude：-20。

把它填到能力的 Cost Gameplay Effect Class。尝试激活时，GAS 会预检查每个 Cost modifier 是否会让对应属性变成负数；若 Energy 不足，能力不会激活。能力已经激活后，资源仍可能被别的行为抢先消耗，因此 Commit 时会再检查一次。

然后创建 GameplayTag `Cooldown.DamageSelf`，再创建 `GE_Cooldown_DamageSelf`：

- Duration Policy：Duration；
- Duration Magnitude：例如 3 秒；
- 添加 **Grant Tags to Target Actor** 组件并授予 `Cooldown.DamageSelf`。

把它填到能力的 Cooldown Gameplay Effect Class。冷却 GE 必须是 Duration 或 Infinite，并且必须授予至少一个用于识别冷却的 GameplayTag。只要能力 Owner 身上存在授予同一冷却标签的 GE，该能力就被视为仍在冷却；可调用 `GetCooldownTimeRemaining` 查询剩余时间。

在能力蓝图开头调用 `CommitAbility` 并处理布尔返回值：

```text
Event ActivateAbility
  → CommitAbility
      True  → 真正执行技能 → EndAbility
      False → EndAbility（Was Cancelled = true）
```

`CommitAbility` 成功时会施加消耗和冷却；失败时不要继续生成弹丸、造成伤害或播放不可撤销逻辑。任何配置了 Cost / Cooldown 的能力如果不调用 Commit，就等于绕过这两套规则。`EndAbility` 也不会替你 Commit。

什么时候 Commit 是设计选择：

- 按下即付费：激活后立刻 Commit；
- 先瞄准、确认才付费：WaitTargetData 成功分支中 Commit，Cancel 分支不 Commit；
- 蓄力技能：通常开始蓄力时只做能否激活的初检，松开并确认释放时 Commit；
- 动画有可中断前摇：可以到实际命中 / 发射的承诺点再 Commit，但要处理期间资源变化导致的 Commit 失败。

Local Predicted 能力在有效的 scoped prediction window 中调用 Commit 时，Cost 和 Cooldown GE 可以预测性施加，使本地 UI 立刻扣资源、显示冷却；服务器拒绝后 GAS 会协调预测结果。通常在 `ActivateAbility` 的同步阶段，以及部分 AbilityTask 回调内存在有效预测窗口。任意普通异步回调都不能假定自己仍处于预测窗口；跨网络同步点时应使用 GAS 提供的任务 / 同步机制，而不是自建不受预测管理的本地修改。

更精确地说，初始 PredictionKey 通常只覆盖 `ActivateAbility` 的初始调用栈。Timer 或任意潜伏节点跨帧后，旧 Key 不再自动有效；等待输入之后还要预测新 GE / 标签 / Cue 时，应依靠会建立预测窗口的 AbilityTask 回调，或由 C++ 使用 `FScopedPredictionWindow` 明确建立新的逻辑预测窗口。

最后做两项验证：

1. 连续触发 5 次后 Energy 归零，第 6 次激活失败且不执行技能逻辑；
2. 每次成功触发后 3 秒内再次按键，能力无法激活；等待冷却标签移除后恢复。

### 官方来源

- [GAS Best Practices：能力消耗与冷却](https://dev.epicgames.com/community/learning/tutorials/DPpd/unreal-engine/gameplay-ability-system-best-practices-for-setup#howdoabilitycostsandcooldownswork?)
- [GAS Best Practices：结束能力不会自动 Commit](https://dev.epicgames.com/community/learning/tutorials/DPpd/unreal-engine/gameplay-ability-system-best-practices-for-setup#whatdoesendingorcancelingabilitiesdo?)
- [Epic API：UGameplayAbility::CommitAbility](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Plugins/GameplayAbilities/Abilities/UGameplayAbility/CommitAbility)
- [Epic API：FPredictionKey](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Plugins/GameplayAbilities/FPredictionKey)

---

## 8. 反应式能力：Ability Trigger 与常驻 WaitGameplayEvent

### 建议替换位置

- 5.7 现有教程已有做法，但应补充两种模式的区别、Payload 与联网边界。

### 可直接补入正文

反应式能力有两种主要实现模式：

**模式 A：Ability Trigger 自动激活。** 在 GameplayAbility 的 `Ability Triggers` 中配置 GameplayTag 或 Gameplay Event。能力只需被授予，不必提前处于激活状态；收到匹配事件时，ASC 尝试激活它。如果由 Gameplay Event 激活，蓝图应实现 `ActivateAbilityFromEvent`，从事件 Payload 读取 Instigator、Target、EventMagnitude、OptionalObject、EffectContext 或 TargetData。处理完后正常 EndAbility，下一次事件可以重新激活。

**模式 B：常驻能力 + WaitGameplayEvent。** 能力获得后立刻激活，进行一次初始化，然后持续等待事件。这适合被动天赋、Proc 和“死亡 / 重生状态机”等需要长期保存能力内部状态的逻辑。收到事件后，如果还要继续监听，应确保 `WaitGameplayEvent` 配置为持续触发，或在处理后重新创建等待任务；不要在第一次事件后 EndAbility，除非这个监听本来就是一次性的。

外部触发事件有三种官方入口：

- Ability 蓝图内 `SendGameplayEvent`：发给自身 Owner；
- `SendGameplayEventToActor`：发给任意带 ASC 的 Actor；
- C++ 调目标 ASC 的 `HandleGameplayEvent`。

`FGameplayEventData` 是 Payload。约定双方必须明确每个字段的语义，尤其是 `OptionalObject1/2`。复杂参数可放在项目自定义 GameplayEffectContext 或 TargetData 中。对象指针要考虑生命周期，不要把 Payload 当成永久持有对象的容器。

Local Predicted 能力可在拥有者客户端通过 `SendGameplayEventToActor` 激活，并把 Payload 随激活发往服务器。对“客户端让另一个玩家受影响”的事件仍应由服务器验证目标并权威触发；不能把任意客户端发送的受击 / 死亡事件直接当成事实。

本教程的 `GA_SpookReaction` 若采用常驻模式，其生命周期应明确写成：服务器 `GiveAbilityAndActivateOnce`（或项目自己的 ActivateOnSpawn 机制）→ Activate → `WaitGameplayEvent(Abilities.Reactions.Spooked)` → Launch / 表现 → 再次等待。若改用 Ability Trigger，则只 `GiveAbility`，事件到来时进入 `ActivateAbilityFromEvent`，跳起后 EndAbility。

### 官方来源

- [GAS Best Practices：实现响应外部事件的能力](https://dev.epicgames.com/community/learning/tutorials/DPpd/unreal-engine/gameplay-ability-system-best-practices-for-setup#howdoiimplementabilitiesthatreacttoexternalevents?)
- [GAS Best Practices：反应能力的 Payload](https://dev.epicgames.com/community/learning/tutorials/DPpd/unreal-engine/gameplay-ability-system-best-practices-for-setup#howdoipassinadditionalparameterstoanabilitythatreactstoexternalevents?)
- [Epic 文档：Gameplay Abilities](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-abilities-in-unreal-engine)

---

## 9. Gameplay Cue：发现 / 打包、事件语义与三种触发方式

### 建议替换位置

- 第 6 步开头的三条“使用注意点”。
- 6.2 与 6.3 扩充成完整触发模型。

### 可直接补入正文

Gameplay Cue 只负责客户端视听反馈，不能承担伤害、移动判定或状态修改。GameplayCueNotify 不会在 Dedicated Server 执行；服务器负责产生 / 复制 Cue，客户端负责响应。

Cue 标签必须位于 `GameplayCue` 根标签下，例如 `GameplayCue.DamageNumber`、`GameplayCue.ElectricalSparks`。Cue 的网络传输以低成本表现为目标，并不保证像可靠 RPC 那样每条必达；这进一步说明它不能承载必须可靠执行的玩法逻辑。

常用 Notify 类型：

- `GameplayCueNotify_Static`：无实例状态、使用类默认对象处理，适合一次性 `OnExecute`，例如命中特效、单次声音、伤害数字；
- `GameplayCueNotify_Actor`：为 Cue 生成 Actor，能保存状态和处理 `OnActive / WhileActive / OnRemove`，适合持续光环、循环声音和需要成对创建 / 销毁的表现。

Instant GE 通常触发 **Executed**；Duration / Infinite GE 随激活和移除可触发 **OnActive / WhileActive / Removed**。具体蓝图事件由 Notify 基类暴露。Cue 参数可携带 RawMagnitude、NormalizedMagnitude、Instigator、EffectCauser、SourceObject、Location 等，但 Notify 应把这些只用于表现。

GCN 不是被普通硬引用找到，而是由 GameplayCueManager 扫描发现。`UGameplayCueManager::AddGameplayCueNotifyPath` 可把 `/Game/...` 或插件内容目录加入 `GameplayCueNotifyPaths` 并按需重扫运行时对象库。正式项目要做三件事：

1. 把 GCN 放在项目配置允许扫描的 Gameplay Cue 路径中；新建 / 删除 GCN 后如果编辑器缓存未更新，重启编辑器；
2. 确认同一个 GameplayCueTag 只有一个实际处理类；若多个类监听同一标签，加载顺序会造成不确定结果；
3. 打包构建中确认这些蓝图被 Cook。若目录没有被其它资产硬引用，加入 **Project Settings → Packaging → Additional Asset Directories to Cook**（或项目等价的 Asset Manager / Cook 配置），并用 Development / Shipping 包实际测试，不要只在 PIE 验证。

触发 Cue 有三类方式：

**A. 由 GameplayEffect 触发（首选）。** 在 GE 的 Gameplay Cues 中配置标签和 Magnitude Attribute。GE 在服务器应用后，相关 Cue 随效果状态复制并在各客户端执行。Local Predicted Ability 预测性施加 GE 时，Cue 也能立即预测播放；若服务器拒绝，预测 GE 与持续 Cue 会被移除。

**B. 直接执行一次性 Cue。** 在拥有 ASC 的目标上调用 `ExecuteGameplayCue` / `ExecuteGameplayCueOnActor`，适合已经在 AttributeSet 或 Execution 中得到“实际伤害值”后发伤害数字。服务器调用即可复制到客户端：

```cpp
FGameplayCueParameters Params;
Params.RawMagnitude = ActualDamage;
Params.Instigator = Data.EffectSpec.GetEffectContext().GetInstigator();
TargetASC->ExecuteGameplayCue(DamageNumberCueTag, Params);
```

**C. 手动维护持续 Cue。** `AddGameplayCue` 开始，`RemoveGameplayCue` 结束。必须保证成对调用；适合不方便用持续 GE 表达、但确实需要持续表现的情况。若状态本来就由 GE 表达，优先让 GE 管 Cue 生命周期，减少漏移除。

伤害数字应传“最终实际伤害”，而非 GE 原始 modifier：目标剩 5 血、收到 15 伤害时显示 5；有护盾时可分别发 `GameplayCue.Damage.Absorbed` 和 `GameplayCue.Damage.Health`。因此最合适的触发点通常是 AttributeSet 消费 Damage 元属性之后。

GameplayAbility 实例通常只存在于服务器和拥有者客户端，不能依赖 GA 自己的 NetMulticast 去给所有观察者播表现。Epic 推荐由能力施加 GE 触发 Cue，或由服务器直接对目标执行 Cue。

### 官方来源

- [Epic 官方 60 分钟 GAS 教程：Gameplay Cue 章节](https://dev.epicgames.com/community/learning/tutorials/8Xn9/unreal-engine-epic-for-indies-your-first-60-minutes-with-gameplay-ability-system)
- [GAS Best Practices：能力的跨客户端视听反馈](https://dev.epicgames.com/community/learning/tutorials/DPpd/unreal-engine/gameplay-ability-system-best-practices-for-setup#canitreatgameplayabilitiesasanyreplicatedobject?)
- [Epic 文档：Gameplay Effects（Cue 事件与标签规则）](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-effects-for-the-gameplay-ability-system-in-unreal-engine)
- [Epic 文档：GAS Overview / Handling Cosmetic Effects](https://dev.epicgames.com/documentation/en-us/unreal-engine/understanding-the-unreal-engine-gameplay-ability-system#handlingcosmeticeffects)
- [Epic API：ExecuteGameplayCue](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Plugins/GameplayAbilities/UAbilitySystemComponent/ExecuteGameplayCue)
- [Epic API：AddGameplayCue](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Plugins/GameplayAbilities/UAbilitySystemComponent/AddGameplayCue)
- [Epic API：AddGameplayCueNotifyPath](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Plugins/GameplayAbilities/UGameplayCueManager/AddGameplayCueNotifyPath)
- [Epic 项目设置：Packaging / Additional Asset Directories to Cook](https://dev.epicgames.com/documentation/en-us/unreal-engine/project-section-of-the-unreal-engine-project-settings#packaging)

---

## 10. 联网移动预测：为什么 Launch / Dash 会拉扯，以及 GAS 中应怎么做

### 建议替换位置

- 5.6 的“联网移动警告”不能只说超出范围，至少补上下列最小模型与选型。

### 可直接补入正文

`CharacterMovementComponent` 的联网移动不是简单复制 Transform，而是“客户端先走、服务器复演、出错再纠正”：

1. 拥有者客户端（Autonomous Proxy）立即运行移动，记录成 `FSavedMove_Character`；
2. 客户端把压缩后的输入 / 移动数据通过 ServerMove RPC 发给服务器；
3. 服务器从自己的权威位置复演同一移动；
4. 结果接近则 ACK，偏差过大则发 `ClientAdjustPosition`；
5. 客户端收到纠正后回到服务器位置，并重放尚未确认的 SavedMoves；
6. 其他客户端（Simulated Proxy）接收服务器的 ReplicatedMovement，并用 Network Smoothing 避免每次更新都瞬移。

因此能力里直接 `SetActorLocation`、每帧改 Transform，或只在客户端施加一股服务器不知道的速度，会被服务器判为位置错误并拉回。只在服务器移动则权威正确，但拥有者要等往返延迟才看到动作，手感迟钝。

这里还要分清两套预测：GAS 的 `FPredictionKey` 管理 Ability 激活以及与它关联的 GE、Tag、Cue、Montage 等副作用；角色常规位移的输入记录、服务器复演和纠正由 `UCharacterMovementComponent` 管理。能力设为 Local Predicted 并不意味着任意 `SetActorLocation` 会自动进入 CharacterMovement 的 SavedMove。

选型建议：

- 普通走、跳、重力和可由 CharacterMovement 表达的力：尽量走 CharacterMovement 的已有接口，并保证服务器和拥有者客户端以相容方式执行；
- 瞬移：只在服务器调用 SetLocation / Teleport；使用 SetLocation 时把 `bTeleport` 设为 true，让复制系统把它识别为瞬移而非需平滑的普通移动；
- Dash、击退、拉拽、跳向动态目标：优先使用 Root Motion Source。它能由代码提供实时参数，同时写入 SavedMove 和移动预测流程；
- GAS 中可直接采用 `AbilityTask_ApplyRootMotionMoveToForce` 等 Root Motion Ability Tasks，而不是自己每帧 SetActorLocation；
- 动画驱动冲刺可用 Root Motion AnimMontage；GAS 会同步触发 Montage 的能力，CharacterMovement 会复制 / 校正 Root Motion 状态。

Root Motion Source 应在拥有者客户端的 Autonomous Proxy 上应用，服务器也会处理权威移动；完成后用返回 Handle 移除。GAS 的 Root Motion AbilityTask 会管理大部分生命周期，更适合蓝图技能。

本教程的蓄力 `LaunchCharacter` 示例可以保留为入门演示，但应明确：客户端和服务器都执行同一 Launch 只能降低可感知延迟，不等于任意移动已自动可预测。高速度、碰撞时序差异、不同帧触发或其它力叠加仍会造成服务器纠正。要做产品级蓄力跳 / 冲刺，应在有延迟和丢包模拟的多人 PIE 中观察 Network Correction，并改用可记录的 CharacterMovement / Root Motion Source 方案。

GAS 的回滚范围也有边界：大多数与 PredictionKey 关联的非 Instant GE、标签、Cue 等可以协调或移除，但 Instant Attribute 修改、Meta Attribute 和 Execution 结果不能当作可安全回滚的客户端预测。因此“预测播放起跳 / 特效”和“服务器最终确认伤害 / 位移结果”应分层设计。

### 官方来源

- [Epic 文档：Character Movement Component 的联网移动](https://dev.epicgames.com/documentation/en-us/unreal-engine/understanding-networked-movement-in-the-character-movement-component-for-unreal-engine)
- [同页：Root Motion Sources 与特殊能力移动](https://dev.epicgames.com/documentation/en-us/unreal-engine/understanding-networked-movement-in-the-character-movement-component-for-unreal-engine#replicatingspecialcasemovementwithrootmotion)
- [Epic 文档：Ability Tasks](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-ability-tasks-in-unreal-engine)
- [Epic 文档：GAS Replication and Prediction](https://dev.epicgames.com/documentation/en-us/unreal-engine/understanding-the-unreal-engine-gameplay-ability-system#replicatingabilitiesandusingprediction)
- [Epic API：FPredictionKey](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Plugins/GameplayAbilities/FPredictionKey)

---

## 11. 其余外链 / “不展开”措辞的处理建议

这些地方不一定需要长篇新增，但不应让链接代替必要步骤：

| 位置 | 当前问题 | 建议处理 |
|---|---|---|
| 开头“配套进阶阅读” | 属于可选延伸，不阻断正文 | 可保留 |
| 1.4 调试工具外链 | 当前已告诉读者如何打开 Gameplay Debugger | 保留为可选来源；另把 4.4 用到的控制台命令与预期结果保留在正文 |
| 2.3 “本地预测窗口，本教程不展开” | 读者不知道它与 GA 的关系 | 在 4.2 / 4.3 补一句：Prediction Key 标识一组可回滚预测操作；ActivateAbility 同步阶段和部分 AbilityTask 回调会打开 scoped prediction window，普通异步代码不能假定仍可预测 |
| 3.3 Widget 设计细节“不展开” | 截图和随后步骤已能完成，但 Percent 计算未明说 | 加 `Percent = MaxHealth > 0 ? Health / MaxHealth : 0`，并明确解绑旧委托、防止重复绑定 |
| 4.4 调试工具链接 | 正文已有完整命令 | 改成段末官方来源即可 |
| 结尾“下一步阅读” | 是可选进阶，不是正文缺失 | 可保留 |
| 附 A 社区论坛 | 社区来源不应用于关键技术结论 | 常见坑可保留并标“版本 / 社区反馈”；正文关键做法以 Epic 官方教程和 API 为准 |

---

## 推荐正式改稿顺序

1. 先修 4.3 冷却 / 消耗和错误锚点，这是最直接的用户痛点。
2. 修属性复制 `OnRep` 示例，再补 MaxHealth 重钳制；否则读者照抄后会遇到真实联网问题。
3. 补 ASC 初始化时机和 AttributeSet 动态复制时序。
4. 把 SetByCaller → Execution → Meta Attribute → Gameplay Cue 串成一条伤害数据流，避免现在四处只见名词不见闭环。
5. 补 Input Task、反应式能力两种模式与联网事件边界。
6. 最后补 Gameplay Cue Cook 检查和联网移动预测最小模型。

完成后，正文中的外链应只承担“证据 / 延伸阅读”作用；读者不打开链接，也能完成本教程涉及的每一个练习并理解其联网边界。
