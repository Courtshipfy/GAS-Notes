# AbilitiesLab —— GAS 学习实验项目（进度：教程第 0 步 ~ 第 4.4 节）

> 对应 [docs/your-first-60-minutes-with-gas-zh.md](../docs/your-first-60-minutes-with-gas-zh.md) 的第 0 步至第 4.4 节，以及仓库根 [LEARNING-ROADMAP.md](../LEARNING-ROADMAP.md) 阶段 A（A1/A2 已覆盖，A3 进行中）。
> 引擎：**UE 5.8.3**（教程原文按 5.5 编写，本节代码在 5.8 同样适用）。
> 最后一次编辑器会话（据 `Saved/Logs/`）：**2026-10-08 15:39**。
> 本页结论来源：C++ 源码逐文件读过；蓝图部分通过资产内部的引用（资产名、类名、函数名、GameplayTag）核对，未逐个在编辑器里打开确认。

## 一、已完成：第 0 步 ~ 第 4.4 节

### C++

| 教程条目 | 落地情况 |
|---|---|
| 0.1 启用 GameplayAbilities 插件 | ✅ 已写在 `AbilitiesLab.uproject` 的 Plugins 里（无需进编辑器勾选） |
| 0.2 Build.cs 引入 GAS 模块 | ✅ `Source/AbilitiesLab/AbilitiesLab.Build.cs` 已加入 `GameplayAbilities / GameplayTags / GameplayTasks` |
| 0.3 创建 ASC 子类并挂到角色 | ✅ `ULabAbilitySystemComponent`（构造函数里 `SetIsReplicatedByDefault(true)`）挂为角色的 `LabAbilitySystemComp` |
| 0.4 实现 IAbilitySystemInterface | ✅ `AAbilitiesLabCharacter : public IAbilitySystemInterface` + `GetAbilitySystemComponent()` |
| 0.5 设置 Owner 与 Avatar | ✅ `BeginPlay` 里 `InitAbilityActorInfo(this, this)`；另在 `PossessedBy / UnPossessed` 补了 `RefreshAbilityActorInfo()`（修复多人 PC 过期崩溃的常见坑） |
| 1.1–1.3 生命属性集 | ✅ `ULabHealthAttributeSet`：`Health / MaxHealth`、`ATTRIBUTE_ACCESSORS_BASIC`（5.6+ 内置宏）、`ReplicatedUsing` + `DOREPLIFETIME_CONDITION_NOTIFY` + `GAMEPLAYATTRIBUTE_REPNOTIFY` 三件套、100/100 初始值，以 DSO 挂到角色 |
| 3.1 属性钳制 | ✅ `PreAttributeChange` 把 `Health` 与 `Energy` 钳制到 `[0, Max]` |
| 3.2 属性变化通知 | ✅ 蓝图可绑定的 `OnHealthChanged` / `OnEnergyChanged`；本地变化（`PostAttributeChange`）与网络复制（`OnRep_*`）都会广播 |
| 3.4 Damage 元属性 | ✅ `Damage` 元属性 + `PostGameplayEffectExecute` 换算扣血并归零；`Health` 已加 `meta = (HideFromModifiers)` |
| 3.5 Execution 概念 | ✅ 代码已具备接收 Modifier 或后续自定义 Execution 输出到 `Damage` 的结算入口（教程本节不要求实现具体 Execution 类） |
| 4.3 Energy 属性 | ✅ `Energy / MaxEnergy` + 访问器 + 复制回调 + 范围钳制 + `OnEnergyChanged` |

### 蓝图 / 资产

| 资产 | 对应节 | 落地情况 |
|---|---|---|
| `GE_Health_Minus15` | 2.2 / 3.4 / 4.4 | ✅ Modifier 已从 `Health` 改为指向 `Damage` 元属性（+15）；已挂 `Damage.Conditions.Immune` 应用阻挡标签 |
| `GE_Cost_Energy20` | 4.3 | ✅ Instant，`Energy -20` |
| `GE_Cooldown_DamageSelf` | 4.3 | ✅ Duration 3s，授予 `Cooldown.DamageSelf` |
| `GE_DamageImmune` | 4.4 | ✅ Infinite，授予 `Damage.Conditions.Immune` |
| `GA_DamageSelf` | 4.1–4.3 | ✅ 图中调用 `ApplyGameplayEffectToOwner` 施加 `GE_Health_Minus15`；已引用 Cost / Cooldown 两个 GE 类；有 `CommitAbility` 引用。**全同步图，没有任何异步节点** |
| `WBP_HealthBarWorld` | 3.3 | ✅ ProgressBar + TextBlock；`SetHealthAttributeSet` 存属性集、`RefreshHealth` 刷新、绑定 `OnHealthChanged` |
| `BP_AbilitiesLabCharacter` | 2.3 / 3.3 / 4.1 | ✅ 授予技能（`GiveAbility`）、调试键激活（`TryActivateAbility`）、血条 Widget Component（引用 `WBP_HealthBarWorld` 并传入属性集） |

### 已创建的 GameplayTag（`Config/DefaultGameplayTags.ini`）

`Cooldown.DamageSelf`、`Damage.Conditions.Immune`

## 二、推迟项 / 已知缺口

| 项 | 状态 | 说明 |
|---|---|---|
| 3.1 后半段：MaxHealth 下降时回收 Health | ⬜ 未做 | 当前 `PostAttributeChange` 只负责广播，没做"上限降低后把 Health 拉回"。目前没有任何资产会改 `MaxHealth`，属休眠缺口；做 buff/debuff 时会暴露 |
| 3.5 具体 Execution 实现（`FireResistance` + `ULabDamageExecution` + `GE_FireDamage`） | ⬜ 未实现 | 教程本节不要求；按路线图属"先用后补"，到 **B4（事件与表现）** 兑现 |
| 4.1 授予技能的权威端判断 | ⚠️ 待补 | 角色蓝图里没有 Authority 分支（搜不到 `Authority` / `SwitchHasAuthority`）。单人无影响，**阶段 C 联网前必须补**：只有服务器能 `GiveAbility` |
| 0.3 ASC 复制模式 | ⚠️ 待定 | 仍是引擎默认（Full）；教程建议玩家角色用 `Mixed`，到 **阶段 C1** 再定 |
| 4.4 练习：GA 激活阻挡（`Stunned` / `Silenced`） | ⬜ 未做 | 需要新建标签 + 在 GA 上配 Activation Blocked Tags |
| Ability Task 实操 | ⬜ 0 个 | 到第 4.4 节为止所有技能图都是同步图；第一个真正的 Ability Task 在第 5.2 节的 `WaitTargetData` |

## 三、下一步：第 5 步（目标技能）

1. **5.1 输入链路**：`Config/DefaultInput.ini` 加 `ConfirmTargeting` / `CancelTargeting` 与三个技能槽动作（Q/E/R）；C++ 侧加 `EMyAbilitySlotsEnum`（`UENUM(BlueprintType)`）并在 `SetupPlayerInputComponent` 里调 `BindAbilityActivationToInputComponent`；角色蓝图 `GiveAbility` 时带上 `InputID`。
2. **5.2** `GA_TargetingBase`：`WaitTargetData`（`GameplayAbilityTargetActor_LineTrace`）→ 结束后 `EndAbility`。
3. **5.3** `GAR_Area_200` 瞄准圈（Plane + 遮罩圆环材质）。
4. **5.4** `GA_TargetedBase`：球形 Overlap 取 Pawn + 可覆写的 `ApplyToTarget` 事件。
5. **5.5** `GA_TargetedDamage_Fixed15`：检查 `HasAuthority` 后施加 `GE_Health_Minus15`。
6. **5.6 / 5.7** 蓄力技能（`WaitInputReleased`）与反应式技能（`WaitGameplayEvent` + `Abilities.Reactions.Spooked` 标签）。
7. 之后进入**第 6 步 Gameplay Cue**：`GCN_DamageNumber`（`GameplayCue.DamageNumber`）→ `WBP_ScrollingText` / `BP_ScrollingText` 滚动伤害数字。

## 四、实验项目自备配置（非教程必需）

- **可玩角色**：传统输入（WASD + 鼠标）第三人称角色，占位方块视觉（无第三人称模板内容）；
- **镜头**：SpringArm + FollowCamera 过肩视角；
- **默认地图**：引擎自带 `/Engine/Maps/Templates/OpenWorld`（打开编辑器即有可落地世界）。项目目前**没有自己的关卡资产**（`Content/` 下没有 `.umap`）——将来用 `File > Save Current Level As` 存到 `/Game/Maps/...` 后，把 `Config/DefaultEngine.ini` 的 `EditorStartupMap` / `GameDefaultMap` 指过去即可；
- **默认 GameMode**：`AAbilitiesLabGameMode`（`GlobalDefaultGameMode=/Script/AbilitiesLab.AAbilitiesLabGameMode`），出生 Pawn 即 GAS 角色。

## 五、如何打开

方式一（推荐）：双击 `AbilitiesLab.uproject`（`EngineAssociation` 写的是 `5.8`；若提示找不到引擎，用方式二）。

方式二：直接指定引擎启动：

```
"D:\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe" "D:\GAS-Notes\AbilitiesLab\AbilitiesLab.uproject"
```

只编译不开编辑器：双击仓库内的 `Build-Editor.bat`（内部调用 `D:\UE_5.8\Engine\Build\BatchFiles\Build.bat`，目标 `AbilitiesLabEditor Win64 Development`）。

首次打开编辑器会提示编译缺失模块——点 Yes 让它编译（或先跑一次 `Build-Editor.bat`）。

## 六、下次继续时的验证清单

- PIE 里按 X 激活 `GA_DamageSelf`：Health 每次 -15、Energy -20、进入 3 秒冷却；
- 连续触发到 Energy 为 0：第 6 次无法激活（`AbilitySystem.Effect.ListActive` 可看冷却 GE 与剩余时间）；
- 控制台 `AbilitySystem.Effect.Apply GE_DamageImmune` 后再按 X：血不掉，Gameplay Debugger（Shift + 反引号键）里能看到 `Damage.Conditions.Immune`；
- 初始值应为 `Health = 100 / MaxHealth = 100 / Energy = 100`。
