# Unreal Engine Gameplay Ability System（GAS）概念清单

> 一份面向研究的概念总目录：按「基础 → 能力 → 表现 → 网络」组织，每个概念给出定义、职责、依赖与单机/联网归属，并逐条标注出处（官方文档 / 引擎源码类 / GASDocumentation / Lyra）。

## 来源与引用约定

本文件无法直接读取引擎 C++ 源码（当前环境仅有 web 检索），因此「由哪个类定义」的归属以**官方 API 文档中的类名 + GASDocumentation 对源码的引用 + 引擎公开头文件的固定路径**为准，与任务要求一致。主要来源：

| 来源 | 定位 | 引用标识 |
|---|---|---|
| 官方 GAS 总览 | Epic 官方文档 | [GAS 总览](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-ability-system-for-unreal-engine) |
| Gameplay 属性与效果 | 官方文档 | [Attributes & Effects](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-attributes-and-gameplay-effects-for-the-gameplay-ability-system-in-unreal-engine) |
| 使用 Gameplay 技能 | 官方文档 | [Using Gameplay Abilities](https://dev.epicgames.com/documentation/en-us/unreal-engine/using-gameplay-abilities-in-unreal-engine) |
| Gameplay 标签 | 官方文档 | [Gameplay Tags](https://dev.epicgames.com/documentation/en-us/unreal-engine/using-gameplay-tags-in-unreal-engine) |
| Gameplay 技能任务 | 官方文档 | [Ability Tasks](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-ability-tasks-in-unreal-engine) |
| Lyra 示例 | 官方文档 + 仓库 | [Lyra 文档](https://dev.epicgames.com/documentation/en-us/unreal-engine/lyra-sample-game-in-unreal-engine) · [Lyra 仓库](https://github.com/EpicGames/LyraSampleGame) |
| GASDocumentation | tranek 社区权威参考（逐条引用官方文档与源码） | [GASDocumentation](https://github.com/tranek/GASDocumentation)（主文档 `README.md`） |
| 引擎源码头文件 | `GameplayAbilities` 插件 / `GameplayTags` 模块 | 见各条 `源码` 字段 |

引擎公开头文件根路径（引用简写）：

- `GameplayAbilities` 插件：`Engine/Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Public/…`
- `GameplayTags` 模块：`Engine/Source/Runtime/GameplayTags/Classes/…`

---

## 1. 核心概念清单

每个概念按四个维度记录：**(a) 定义**、**(b) 职责**、**(c) 依赖（需先理解）**、**(d) 单机 vs 联网**。

### 1.1 Gameplay Tag（Gameplay 标签）

- **(a) 定义**：一个分层的、由句点连接的名称（如 `Status.Stunned`、`Ability.Skill.Fireball`），内部以 `FName` 存储，本质上是一枚「名字标签」而非枚举。可单枚使用（`FGameplayTag`），也可放进集合（`FGameplayTagContainer`）。
- **(b) 职责**：GAS 的通用「分类/状态标记语言」——标记技能、效果、状态、输入、冷却等，驱动 GAS 的过滤与匹配逻辑（能否施放、能否命中、被谁阻挡等）。
- **(c) 依赖**：无（最底层概念，只需理解「层次化命名」）。
- **(d) 单机/联网**：单机即可完整使用；联网时标签容器会随 ASC 复制（见 1.24 复制）。
- **来源**：[Gameplay Tags](https://dev.epicgames.com/documentation/en-us/unreal-engine/using-gameplay-tags-in-unreal-engine) · 源码 `GameplayTags/Classes/GameplayTagContainer.h`（`FGameplayTag`、`FGameplayTagContainer`）· [GASDocumentation](https://github.com/tranek/GASDocumentation)「Gameplay Tags」

### 1.2 Gameplay Attribute（Gameplay 属性）

- **(a) 定义**：一个浮点数值（如生命值、法力值、攻击力），由 `FGameplayAttributeData` 承载（含 `CurrentValue` 与 `BaseValue`），用 `FGameplayAttribute` 作为「指向某个属性数据的指针/元数据」。
- **(b) 职责**：角色状态的数值载体；是 GAS 中「被修改的最小单位」。
- **(c) 依赖**：Gameplay Tag（用于来源/用途标签）；须先理解「属性只存值、不存逻辑」。
- **(d) 单机/联网**：单机完整可用；联网时属性值通过复制同步（见 1.24 复制）。
- **来源**：[Attributes & Effects](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-attributes-and-gameplay-effects-for-the-gameplay-ability-system-in-unreal-engine) · 源码 `AttributeSet.h`（`FGameplayAttribute`、`FGameplayAttributeData`）· [GASDocumentation](https://github.com/tranek/GASDocumentation)「Attributes」

### 1.3 Attribute Set（属性集）

- **(a) 定义**：`UAttributeSet` 是一个把若干 `FGameplayAttributeData` 成员聚合成一组的数据类，用宏 `ATTRIBUTE_ACCESSORS(...)` 自动生成 `FGameplayAttribute` 访问器。
- **(b) 职责**：组织与管理属性；提供三个关键回调钩子——`PreAttributeChange`（修改前钳制，如把生命值夹到 [0, Max]）、`PostGameplayEffectExecute`（效果结算后响应）、`Pre/PostAttributeBaseChange`。
- **(c) 依赖**：Gameplay Attribute（属性是 AttributeSet 的成员）。
- **(d) 单机/联网**：单机可用；联网时 AttributeSet 通常放在被复制的 Actor 上并随 ASC 一起复制。
- **来源**：[Attributes & Effects](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-attributes-and-gameplay-effects-for-the-gameplay-ability-system-in-unreal-engine) · 源码 `AttributeSet.h`（`UAttributeSet`）· [GASDocumentation](https://github.com/tranek/GASDocumentation)「Attribute Set」

### 1.4 Ability System Component（技能系统组件，ASC）

- **(a) 定义**：`UAbilitySystemComponent` 是 GAS 的中枢组件（通常挂在玩家控制器或角色上，通过 `UAbilitySystemInterface` 暴露）。
- **(b) 职责**：持有并管理**授予的技能**、**激活中的效果**、**标签**、**属性集**；是技能施放、效果施加、Gameplay Event 发送、Gameplay Cue 触发、预测与复制的中转站。
- **(c) 依赖**：Gameplay Tag、Gameplay Attribute、Attribute Set（ASC 把它们串起来）。
- **(d) 单机/联网**：单机可用；联网时 ASC 负责复制自己的属性、激活效果、授予的技能与预测键（见 1.24 复制、1.25 预测）。
- **来源**：[GAS 总览](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-ability-system-for-unreal-engine) · 源码 `AbilitySystemComponent.h`（`UAbilitySystemComponent`）· [GASDocumentation](https://github.com/tranek/GASDocumentation)「Ability System Component」

### 1.5 Gameplay Effect（Gameplay 效果，GE）

- **(a) 定义**：`UGameplayEffect` 是一个**纯数据定义**（只在 CDO 上配置、不改写实例），通过 Modifier（1.6）或 Execution Calculation（1.8）修改属性，并通过标签（`GameplayEffectTags`：授予/持续/移除其他效果的标签、`GrantedAbilities`）施加状态与额外能力。
- **(b) 职责**：GAS 中**唯一**合法地修改属性的手段；同时是施加状态标签、授予技能、触发 Gameplay Cue、启动冷却/消耗（见 1.15）的载体。
- **(c) 依赖**：Gameplay Attribute、Attribute Set、Gameplay Tag、Modifier/Execution（GE 靠它们改属性）。
- **(d) 单机/联网**：单机完整可用；联网时效果会复制到客户端（复制模式 Minimal/Mixed/Full），执行计算只在权威端跑（见 1.24 复制）。
- **来源**：[Attributes & Effects](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-attributes-and-gameplay-effects-for-the-gameplay-ability-system-in-unreal-engine) · 源码 `GameplayEffect.h`（`UGameplayEffect`）· [GASDocumentation](https://github.com/tranek/GASDocumentation)「Gameplay Effects」

### 1.6 Gameplay Effect Modifier（效果修改器）

- **(a) 定义**：GE 内的一条 `FGameplayModifierInfo`，指定「改哪个属性 + 改多少 + 怎么改」：运算 `ModOp`（`EGameplayModOp`：Add / Multiply / Divide / Override），幅值 `FGameplayEffectModifierMagnitude`（`ScalableFloat` 曲线、`AttributeBased` 基于属性捕获、`CustomCalculationClass` 即 MMC、`SetByCaller` 运行时传入）。
- **(b) 职责**：对单个属性做简单的声明式修改（无需写 C++），是绝大多数数值效果的实现方式。
- **(c) 依赖**：Gameplay Attribute、Attribute Set；属性捕获（`FGameplayEffectAttributeCaptureDefinition`）用于 `AttributeBased` 幅值。
- **(d) 单机/联网**：单机可用；`SetByCaller` 的值随 Effect Spec 复制到客户端。
- **来源**：[Attributes & Effects](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-attributes-and-gameplay-effects-for-the-gameplay-ability-system-in-unreal-engine) · 源码 `GameplayEffect.h`（`FGameplayModifierInfo`、`FGameplayEffectModifierMagnitude`、`EGameplayModOp`）· [GASDocumentation](https://github.com/tranek/GASDocumentation)「Gameplay Effects > Modifiers」

### 1.7 Mod Magnitude Calculation（幅值计算，MMC）

- **(a) 定义**：`UGameplayModMagnitudeCalculation`，一个蓝图友好的类，为**单个** Modifier 提供自定义幅值（捕获若干属性、返回一个浮点）。
- **(b) 职责**：在纯声明式 `ScalableFloat` 不够用、但又不必上 Execution Calculation 时，写轻量公式。
- **(c) 依赖**：Gameplay Attribute、Modifier、属性捕获。
- **(d) 单机/联网**：单机可用；联网时在服务器权威计算，结果随效果复制。
- **来源**：[Attributes & Effects](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-attributes-and-gameplay-effects-for-the-gameplay-ability-system-in-unreal-engine) · 源码 `GameplayModMagnitudeCalculation.h`（`UGameplayModMagnitudeCalculation`）· [GASDocumentation](https://github.com/tranek/GASDocumentation)「Gameplay Effects > Modifiers」

### 1.8 Gameplay Effect Execution Calculation（效果执行计算）

- **(a) 定义**：`UGameplayEffectExecutionCalculation`，一个纯 C++ 类，通过 `FGameplayEffectCustomExecutionParameters` 读取任意数量的捕获属性，用 `FGameplayEffectCustomExecutionOutput` 一次写入多个属性（可完全自定义公式，如护甲减伤 + 随机暴击的伤害）。
- **(b) 职责**：处理复杂、多属性、多来源参与的计算（典型：伤害）；只在权威端（服务器）执行。
- **(c) 依赖**：Gameplay Attribute、Attribute Set、属性捕获、Effect Spec/Context（见 1.9/1.10）。
- **(d) 单机/联网**：单机可用；联网时**仅在服务器执行**，客户端只收到结果。
- **来源**：[Attributes & Effects](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-attributes-and-gameplay-effects-for-the-gameplay-ability-system-in-unreal-engine) · 源码 `GameplayEffectExecutionCalculation.h`（`UGameplayEffectExecutionCalculation`、`FGameplayEffectCustomExecutionOutput`）· [GASDocumentation](https://github.com/tranek/GASDocumentation)「Gameplay Effects > Executions」

### 1.9 Gameplay Effect Spec（效果实例，Spec）

- **(a) 定义**：`FGameplayEffectSpec` 是某个 GE 定义在**运行时的一次实例化**，携带已捕获的来源/目标数据（`SetByCaller` 值、捕获属性快照、等级、时长、叠加数等）；`FGameplayEffectSpecHandle` 是对它的轻量句柄。
- **(b) 职责**：把「静态 GE 定义」变成「可施加、可复制、可预测的一次具体效果」；由 `UAbilitySystemComponent::MakeOutgoingSpec` / `MakeEffectContext` 创建。
- **(c) 依赖**：Gameplay Effect、Effect Context、SetByCaller、属性捕获。
- **(d) 单机/联网**：单机可用；联网时 Spec 会被复制到客户端，是预测的载体之一。
- **来源**：源码 `GameplayEffectTypes.h`（`FGameplayEffectSpec`、`FGameplayEffectSpecHandle`）· [GASDocumentation](https://github.com/tranek/GASDocumentation)「Gameplay Effects > Gameplay Effect Spec」

### 1.10 Gameplay Effect Context（效果上下文）

- **(a) 定义**：`FGameplayEffectContext` 记录一次效果的来源信息（`Instigator` 发起者、`EffectCauser` 因果对象、`Origin`、`HitResult` 等），可被继承扩展（如 Lyra 的 `FLyraGameplayEffectContext` 加入暴击/击退标记）；`FGameplayEffectContextHandle` 是其句柄。
- **(b) 职责**：为 Execution Calculation 和 Gameplay Cue 提供「谁、在哪儿、怎么打中」的上下文；是项目扩展 GAS 数据的主要接入点之一。
- **(c) 依赖**：Gameplay Effect Spec。
- **(d) 单机/联网**：单机可用；联网时 Context 随 Spec 复制（自定义字段需实现 `NetSerialize`）。
- **来源**：源码 `GameplayEffectTypes.h`（`FGameplayEffectContext`、`FGameplayEffectContextHandle`）· [GASDocumentation](https://github.com/tranek/GASDocumentation)「Gameplay Effects > Gameplay Effect Context」· [Lyra 仓库](https://github.com/EpicGames/LyraSampleGame)（`FLyraGameplayEffectContext`）

### 1.11 Active Gameplay Effect / Handle（激活中的效果与句柄）

- **(a) 定义**：`FActiveGameplayEffect` 是已被 ASC 接受、正在（或已经）作用的运行时效果；由 ASC 内的 `FActiveGameplayEffectsContainer` 统一跟踪；`FActiveGameplayEffectHandle` 是定位单个激活效果的句柄。
- **(b) 职责**：查询/移除某个具体效果（如「移除这个流血 DEBUFF」）、读取其剩余时长与叠加数。
- **(c) 依赖**：Gameplay Effect、Effect Spec、ASC。
- **(d) 单机/联网**：单机可用；联网时激活效果是复制单元（见 1.24 复制）。
- **来源**：源码 `AbilitySystemComponent.h`（`FActiveGameplayEffect`、`FActiveGameplayEffectsContainer`）、`GameplayEffectTypes.h`（`FActiveGameplayEffectHandle`）· [GASDocumentation](https://github.com/tranek/GASDocumentation)「Gameplay Effects」

### 1.12 Gameplay Ability（Gameplay 技能，GA）

- **(a) 定义**：`UGameplayAbility` 定义一次「动作/技能」，核心回调 `CanActivateAbility`（检查标签、冷却、消耗）与 `ActivateAbility`（真正执行）；还配有 `CommitAbility`、`Check/ApplyCooldown`、`Check/ApplyCost`、`EndAbility` 及一组**能力标签**（`AbilityTags`、`ActivationOwnedTags`、`CancelAbilitiesWithTag`、`BlockAbilitiesWithTag`、`ActivationBlockedTags` 等）。
- **(b) 职责**：把玩家的操作映射为可复用、可联网、可预测的动作逻辑；是 GAS 面向玩法的最上层抽象。
- **(c) 依赖**：ASC（施放主体）、Gameplay Tag（资格/阻挡）、Gameplay Effect（冷却/消耗/效果）、Ability Task（异步逻辑）、Ability Spec/Handle（实例化）。
- **(d) 单机/联网**：单机可用；联网时受 `NetExecutionPolicy` 控制（`LocalOnly` / `LocalPredicted` / `ServerOnly` / `ServerInitiated`，见 1.25 预测）。
- **来源**：[Using Gameplay Abilities](https://dev.epicgames.com/documentation/en-us/unreal-engine/using-gameplay-abilities-in-unreal-engine) · 源码 `GameplayAbility.h`（`UGameplayAbility`）· [GASDocumentation](https://github.com/tranek/GASDocumentation)「Gameplay Abilities」

### 1.13 Gameplay Ability Spec / Handle（技能实例与句柄）

- **(a) 定义**：`FGameplayAbilitySpec` 是某个技能**授予到某个 ASC 后**的运行时实例（保存技能 CDO、等级、输入 ID、来源对象、激活信息）；`FGameplayAbilitySpecHandle` 是定位它的轻量句柄。
- **(b) 职责**：把「技能定义」和「某个角色拥有该技能」解耦；支持同一技能以不同等级/来源授予多次。
- **(c) 依赖**：Gameplay Ability、ASC。
- **(d) 单机/联网**：单机可用；联网时授予的技能会被复制给客户端（见 1.24 复制）。
- **来源**：源码 `GameplayAbilitySpec.h`（`FGameplayAbilitySpec`、`FGameplayAbilitySpecHandle`）· [GASDocumentation](https://github.com/tranek/GASDocumentation)「Gameplay Abilities」

### 1.14 Gameplay Ability Graph（技能蓝图图）

- **(a) 定义**：一个 `GameplayAbility` 的 Blueprint 事件图——以 `ActivateAbility` / `EndAbility` 事件为入口，串接 Ability Task 节点（1.16）、Gameplay Event（1.17）与普通蓝图逻辑。
- **(b) 职责**：技能逻辑的「可视化编排层」；绝大多数技能在蓝图里用节点搭出来，只有复杂计算下沉到 Execution Calculation / MMC。
- **(c) 依赖**：Gameplay Ability、Ability Task、Gameplay Event、Gameplay Effect（调用施加）。
- **(d) 单机/联网**：单机可用；联网语义由技能本身的 `NetExecutionPolicy` 决定，图只是逻辑表达。
- **来源**：[Using Gameplay Abilities](https://dev.epicgames.com/documentation/en-us/unreal-engine/using-gameplay-abilities-in-unreal-engine) · [GASDocumentation](https://github.com/tranek/GASDocumentation)「Gameplay Abilities」

### 1.15 Cooldown / Cost（冷却与消耗）

- **(a) 定义**：GAS **没有**专门的「冷却/消耗」字段，二者都实现为**Gameplay Effect**：冷却 = 一个带冷却标签（如 `Cooldown.<Ability>`）的限时（`HasDuration`）GE（`CooldownGameplayEffectClass`），消耗 = 一个 Instant GE（`CostGameplayEffectClass`）用 Modifier 扣减资源属性（法力/耐力）。
- **(b) 职责**：技能通过 `CheckCooldown`/`CheckCost` 判断可否施放，通过 `CommitAbility` → `ApplyCooldown`/`ApplyCost` 生效；冷却标签同时能阻止技能被再次施放。
- **(c) 依赖**：Gameplay Effect、Gameplay Tag、Gameplay Attribute、Gameplay Ability。
- **(d) 单机/联网**：单机可用；联网时冷却/消耗 GE 正常复制，客户端据此禁用技能。
- **来源**：源码 `GameplayAbility.h`（`CooldownGameplayEffectClass`、`CostGameplayEffectClass`、`CheckCooldown`/`ApplyCooldown`/`CheckCost`/`ApplyCost`）· [GASDocumentation](https://github.com/tranek/GASDocumentation)「Gameplay Abilities > Cooldowns」「Cost」

### 1.16 Ability Task（技能任务）

- **(a) 定义**：`UAbilityTask` 是只能从 `GameplayAbility` 里启动的**异步/跨帧节点**（潜伏节点），如 `WaitTargetData`、`PlayMontageAndWait`、`WaitGameplayEvent`、`WaitDelay`；结束时回调（`OnFinished`）或销毁技能（`EndTask` → `OnDestroy`）。
- **(b) 职责**：把「等待玩家选目标、等待动画/事件、分帧推进」这类异步流程装进技能蓝图，避免在 `ActivateAbility` 里硬编码时序。
- **(c) 依赖**：Gameplay Ability（宿主）、Gameplay Ability Graph（挂载点）。
- **(d) 单机/联网**：单机可用；联网时多用于等待目标数据、同步动画等（配合 1.25 预测）。
- **来源**：[Ability Tasks](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-ability-tasks-in-unreal-engine) · 源码 `Abilities/Tasks/AbilityTask.h`（`UAbilityTask`）· [GASDocumentation](https://github.com/tranek/GASDocumentation)「Ability Tasks」

### 1.17 Gameplay Event（Gameplay 事件）

- **(a) 定义**：`FGameplayEventData`（含 `EventTag`、`Instigator`、`Target`、`Magnitude`、`ContextHandle`）配合 `UAbilitySystemComponent::SendGameplayEvent` 发送；技能可用 `AbilityTriggers`（`FAbilityTriggerData`）声明「收到某标签事件即触发」。
- **(b) 职责**：GAS 内部松耦合的「消息传递」——把「某事发生了」（受击、命中、进入范围）广播给监听方，而不直接引用对方。
- **(c) 依赖**：Gameplay Tag（事件用标签命名）、Effect Context、ASC。
- **(d) 单机/联网**：单机可用；联网时事件由服务器权威转发（客户端通常只做本地/预测触发）。
- **来源**：源码 `GameplayAbilityTypes.h`（`FGameplayEventData`）、`AbilitySystemComponent.h`（`SendGameplayEvent`）· [GASDocumentation](https://github.com/tranek/GASDocumentation)「Gameplay Abilities > Ability Triggers」

### 1.18 Gameplay Cue（Gameplay 提示）

- **(a) 定义**：以 `GameplayCue.<CueName>` 标签触发的**纯表现**（粒子/声音/震屏/贴花），由 `IGameplayCueInterface` 与 `GameplayCueNotify` 类处理——`AGameplayCueNotify_Actor`（`OnActive`/`OnExecute`，带实例）与 `UGameplayCueNotify_Static`（无实例的静态执行）；`FGameplayCueParameters` 携带参数。
- **(b) 职责**：把「数值/逻辑」与「视听表现」解耦；效果与技能只发标签，具体表现由 Cue 实现，可被覆盖、被多端一致地触发。
- **(c) 依赖**：Gameplay Tag（Cue 标签）、Effect Spec/Context（参数来源）。
- **(d) 单机/联网**：单机可用；联网时 Cue 通过 RPC 在各客户端触发（含预测 Cue 的拒绝/追平，见 1.25 预测）。
- **来源**：[GAS 总览](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-ability-system-for-unreal-engine)（Gameplay Cues 部分）· 源码 `GameplayCueTypes.h`（`FGameplayCueParameters`）、`GameplayCueNotify_Actor.h`、`GameplayCueNotify_Static.h`、`GameplayCueInterface.h` · [GASDocumentation](https://github.com/tranek/GASDocumentation)「Gameplay Cues」· [Lyra 仓库](https://github.com/EpicGames/LyraSampleGame)（统一 Cue 路由）

### 1.19 Gameplay Cue Manager（Gameplay 提示管理器）

- **(a) 定义**：`UGameplayCueManager`，GAS 的单例（经 `UAbilitySystemGlobals::GetGameplayCueManager()` 获取），负责把 Gameplay Cue 事件路由到正确的处理器、加载 Cue 集合（`GameplayCueSet`）、批量复制 Cue 参数。
- **(b) 职责**：集中管理「标签 → 表现实现」的映射与联网分发，避免逐个硬编码。
- **(c) 依赖**：Gameplay Cue、Gameplay Tag、Ability System Globals。
- **(d) 单机/联网**：单机可用；联网时负责 Cue 的 RPC 批量转发（一个 GE 上所有 Cue 合并为一次 RPC）。
- **来源**：源码 `GameplayCueManager.h`（`UGameplayCueManager`）、`GameplayCueSet.h` · [GASDocumentation](https://github.com/tranek/GASDocumentation)「Gameplay Cues > Gameplay Cue Manager」

### 1.20 Ability System Globals（技能系统全局配置）

- **(a) 定义**：`UAbilitySystemGlobals`，GAS 的全局单例，持有跨项目配置：Gameplay Cue Manager、全局属性集默认值表、预测键初始化、目标数据(TargetData)重建、各类查找表。
- **(b) 职责**：GAS 的「启动与全局查找」入口；许多子系统（Cue、预测、属性默认值）通过它取到。
- **(c) 依赖**：Gameplay Cue Manager、Prediction Key、Attribute Set。
- **(d) 单机/联网**：单机可用；联网时参与预测键初始化与 TargetData 重建。
- **来源**：源码 `AbilitySystemGlobals.h`（`UAbilitySystemGlobals`）· [GASDocumentation](https://github.com/tranek/GASDocumentation)「Ability System Globals」

### 1.21 Prediction Key（预测键）

- **(a) 定义**：`FPredictionKey`，为一次**被预测的动作**分配的唯一标识，用于把「客户端本地预测」与「服务器权威结果」对应起来；含有效/基础/依赖键，并通过 `FPredictionKeyDelegates` 派发 `NewRejectedCue` / `NewCaughtUpCue` 等通知；`FScopedPredictionWindow` 为一整段预测逻辑开新键。
- **(b) 职责**：预测/回滚的「对账凭证」——客户端据此决定「我的预测被服务器认可（caught up）还是作废（rejected）」。
- **(c) 依赖**：Prediction（1.25）概念本身。
- **(d) 单机/联网**：**联网专用**；单机游戏无需预测键。
- **来源**：[FPredictionKey API](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Plugins/GameplayAbilities/FPredictionKey) · 源码 `GameplayPrediction.h`（`FPredictionKey`、`FScopedPredictionWindow`、`FPredictionKeyDelegates`）· [GASDocumentation](https://github.com/tranek/GASDocumentation)「Gameplay Prediction」

### 1.22 Prediction（客户端预测）

- **(a) 定义**：GAS 对 `LocalPredicted` 技能/效果的客户端预测：客户端**先本地执行**并带上 Prediction Key，服务器随后**权威重放**并复制结果；客户端把「本地预测结果」与「服务器结果」对齐——一致则「追平」(caught up)，不一致则「拒绝」(rejected) 并回滚。瞬时伤害等副作用通常**不预测**（预测错误的伤害会带来糟糕体验）。
- **(b) 职责**：消除本地操作的网络延迟，让施放手感即时，同时保证最终状态以服务器为准。
- **(c) 依赖**：Prediction Key、Replication、Gameplay Effect Spec、Gameplay Cue。
- **(d) 单机/联网**：**联网专用**；单机游戏无延迟、无预测需求。
- **来源**：[GAS 总览](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-ability-system-for-unreal-engine)（Gameplay Prediction 部分）· 源码 `GameplayPrediction.h` · [GASDocumentation](https://github.com/tranek/GASDocumentation)「Gameplay Prediction」

### 1.23 Replication（复制）

- **(a) 定义**：GAS 的联网同步机制，由 ASC 统一承担，复制四类对象：**属性**（属性值复制）、**激活中的 Gameplay Effect**（`Minimal` / `Mixed` / `Full` 三种模式，`Minimal` 只复制效果 ID 与预测键）、**授予的技能 Spec**、**标签与预测键**。
- **(b) 职责**：让每个客户端拥有足以本地表现、但以服务器为权威的 GAS 状态；是预测与 Cue 分发的传输底座。
- **(c) 依赖**：ASC、Gameplay Effect Spec、Active Gameplay Effect、Gameplay Tag、Prediction Key。
- **(d) 单机/联网**：**联网专用**；单机游戏不需要任何复制。
- **来源**：[GAS 总览](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-ability-system-for-unreal-engine) · 源码 `AbilitySystemComponent.h`（复制相关成员）· [GASDocumentation](https://github.com/tranek/GASDocumentation)「Gameplay Effects > Replication」· [Lyra 文档](https://dev.epicgames.com/documentation/en-us/unreal-engine/lyra-sample-game-in-unreal-engine)

### 1.24 Gameplay Tag Query（标签查询）

- **(a) 定义**：`FGameplayTagQuery`，对某个标签容器做结构化布尔查询（`Any`/`All`/`None` 组合），用于 GE 的目标/应用资格（`TargetTagRequirements`、`ApplicationTagRequirements`）与技能的源/目标必需标签（`SourceRequiredTags`、`TargetRequiredTags`）。
- **(b) 职责**：把「匹配规则」从代码里抽成数据，供 GE/技能声明式过滤。
- **(c) 依赖**：Gameplay Tag。
- **(d) 单机/联网**：单机可用（纯本地判断，无联网差异）。
- **来源**：源码 `GameplayTags/Classes/GameplayTagQuery.h`（`FGameplayTagQuery`）、`GameplayEffect.h` · [GASDocumentation](https://github.com/tranek/GASDocumentation)「Gameplay Effects」

---

## 2. 依赖摘要（基础 → 高级）

按「必须先懂谁」分层。**单机基础层**与**联网层**明确分开：前 4 层在单机游戏里就能完整运行，第 5 层只有联网（复制/预测）才需要。

### 第 1 层 —— 命名与数值（最基础）

| 概念 | 一句话 | 依赖 |
|---|---|---|
| 1.1 Gameplay Tag | 分层名字标签 | 无 |
| 1.2 Gameplay Attribute | 浮点数值（存值不存逻辑） | Tag |
| 1.3 Attribute Set | 属性的集合 + 钳制/结算钩子 | Attribute |

### 第 2 层 —— 中枢与修改（把数值串起来）

| 概念 | 一句话 | 依赖 |
|---|---|---|
| 1.4 Ability System Component | GAS 中枢组件 | Tag / Attribute / AttributeSet |
| 1.6 Modifier | 声明式改单个属性 | Attribute / AttributeSet |
| 1.7 MMC | 轻量自定义幅值 | Attribute / Modifier |
| 1.8 Execution Calculation | 重量级多属性公式（服务器执行） | Attribute / Spec / Context |
| 1.5 Gameplay Effect | 唯一合法的改属性手段 + 状态/能力载体 | Modifier / Execution / Tag |

### 第 3 层 —— 效果实例化（运行期对象）

| 概念 | 一句话 | 依赖 |
|---|---|---|
| 1.10 Effect Context | 效果来源信息（可扩展） | Spec |
| 1.9 Effect Spec | 一次具体效果的运行时实例 | Effect / Context |
| 1.11 Active Effect / Handle | 已激活效果的跟踪与句柄 | Effect / Spec / ASC |
| 1.24 Tag Query | 声明式匹配规则 | Tag |

### 第 4 层 —— 能力与表现（面向玩法）

| 概念 | 一句话 | 依赖 |
|---|---|---|
| 1.12 Gameplay Ability | 动作定义（资格/阻挡/执行） | ASC / Tag / Effect / Spec |
| 1.13 Ability Spec / Handle | 技能授予后的实例 | Ability / ASC |
| 1.14 Ability Graph | 技能蓝图编排 | Ability / Task / Event |
| 1.15 Cooldown / Cost | 用 GE 实现的冷却与消耗 | Effect / Tag / Attribute |
| 1.16 Ability Task | 技能内异步跨帧节点 | Ability / Graph |
| 1.17 Gameplay Event | 松耦合消息（标签命名） | Tag / Context / ASC |
| 1.18 Gameplay Cue | 纯表现（标签触发） | Tag / Spec / Context |
| 1.19 Cue Manager | Cue 路由与批量分发 | Cue / Tag / Globals |
| 1.20 Ability System Globals | 全局配置与查找入口 | CueManager / PredictionKey |

### 第 5 层 —— 联网层（复制与预测）

| 概念 | 一句话 | 依赖 |
|---|---|---|
| 1.21 Prediction Key | 预测动作的对账凭证 | Prediction 概念 |
| 1.23 Replication | ASC 复制属性/效果/技能/标签 | ASC / Spec / ActiveEffect / Tag |
| 1.22 Prediction | 客户端本地先算、服务器权威追平/拒绝 | PredictionKey / Replication / Spec / Cue |

> 阅读顺序建议：**Tag → Attribute → AttributeSet → ASC → Modifier → Execution → Effect → Spec/Context → Ability → Spec/Handle → Cooldown/Cost → Task → Event → Cue →（联网）Replication → PredictionKey → Prediction**。

---

## 3. 来源索引

### 官方文档

- [Gameplay Ability System 总览](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-ability-system-for-unreal-engine)
- [Gameplay Attributes and Gameplay Effects](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-attributes-and-gameplay-effects-for-the-gameplay-ability-system-in-unreal-engine)
- [Using Gameplay Abilities](https://dev.epicgames.com/documentation/en-us/unreal-engine/using-gameplay-abilities-in-unreal-engine)
- [Using Gameplay Tags](https://dev.epicgames.com/documentation/en-us/unreal-engine/using-gameplay-tags-in-unreal-engine)
- [Gameplay Ability Tasks](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-ability-tasks-in-unreal-engine)
- [Lyra Sample Game](https://dev.epicgames.com/documentation/en-us/unreal-engine/lyra-sample-game-in-unreal-engine)
- [FPredictionKey API](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Plugins/GameplayAbilities/FPredictionKey)

### 社区权威参考

- [tranek/GASDocumentation](https://github.com/tranek/GASDocumentation)（主文档 `README.md`，逐条引用官方文档与引擎源码）

### 引擎源码（概念归属类）

- `GameplayAbilities/Public/AbilitySystemComponent.h` — `UAbilitySystemComponent`、`FActiveGameplayEffect`
- `GameplayAbilities/Public/GameplayAbility.h` — `UGameplayAbility`、冷却/消耗
- `GameplayAbilities/Public/GameplayAbilitySpec.h` — `FGameplayAbilitySpec`、`FGameplayAbilitySpecHandle`
- `GameplayAbilities/Public/GameplayEffect.h` — `UGameplayEffect`、`FGameplayModifierInfo`、`EGameplayModOp`
- `GameplayAbilities/Public/GameplayEffectTypes.h` — `FGameplayEffectSpec`、`FGameplayEffectContext`、句柄
- `GameplayAbilities/Public/GameplayEffectExecutionCalculation.h` — `UGameplayEffectExecutionCalculation`
- `GameplayAbilities/Public/GameplayModMagnitudeCalculation.h` — `UGameplayModMagnitudeCalculation`
- `GameplayAbilities/Public/AttributeSet.h` — `UAttributeSet`、`FGameplayAttribute`、`FGameplayAttributeData`
- `GameplayAbilities/Public/GameplayCueTypes.h` / `GameplayCueNotify_Actor.h` / `GameplayCueNotify_Static.h` / `GameplayCueInterface.h` — Gameplay Cue
- `GameplayAbilities/Public/GameplayCueManager.h` — `UGameplayCueManager`
- `GameplayAbilities/Public/AbilitySystemGlobals.h` — `UAbilitySystemGlobals`
- `GameplayAbilities/Public/GameplayPrediction.h` — `FPredictionKey`、`FScopedPredictionWindow`
- `GameplayAbilities/Public/Abilities/Tasks/AbilityTask.h` — `UAbilityTask`
- `GameplayAbilities/Public/GameplayAbilityTypes.h` — `FGameplayEventData`
- `GameplayTags/Classes/GameplayTagContainer.h` — `FGameplayTag`、`FGameplayTagContainer`
- `GameplayTags/Classes/GameplayTagQuery.h` — `FGameplayTagQuery`

### 第一方示例

- [EpicGames/LyraSampleGame](https://github.com/EpicGames/LyraSampleGame)（`FLyraGameplayEffectContext` 扩展、统一 Gameplay Cue 路由、GAS 输入绑定等实践）
