# AbilitiesLab —— GAS 学习实验项目（C++ 已完成至第 4.3 节）

> 对应 [docs/your-first-60-minutes-with-gas-zh.md](../docs/your-first-60-minutes-with-gas-zh.md) 的第 0 步至第 4.3 节，以及仓库根 [LEARNING-ROADMAP.md](../LEARNING-ROADMAP.md) 阶段 A。
> 引擎：**UE 5.8.1**（教程原文按 5.5 编写，本节代码在 5.8 同样适用）。

## 已完成的 C++ 内容

| 教程条目 | 落地情况 |
|---|---|
| 0.1 启用 GameplayAbilities 插件 | ✅ 已在 `AbilitiesLab.uproject` 的 Plugins 里启用（无需进编辑器勾选） |
| 0.2 Build.cs 引入 GAS 模块 | ✅ `Source/AbilitiesLab/AbilitiesLab.Build.cs` 已加入 `GameplayAbilities / GameplayTags / GameplayTasks` |
| 0.3 创建 ASC 子类并挂到角色 | ✅ `ULabAbilitySystemComponent`（`LabAbilitySystemComponent.h`）挂到 `AAbilitiesLabCharacter`（变量 `LabAbilitySystemComp`） |
| 0.4 实现 IAbilitySystemInterface | ✅ `AAbilitiesLabCharacter : public IAbilitySystemInterface` + `GetAbilitySystemComponent()` |
| 0.5 设置 Owner 与 Avatar | ✅ `BeginPlay` 里 `InitAbilityActorInfo(this, this)`；另在 `PossessedBy/UnPossessed` 补了 `RefreshAbilityActorInfo()`（修复多人 PC 过期崩溃的常见坑） |
| 1.1–1.3 生命属性集 | ✅ 新增 `ULabHealthAttributeSet`，包含 `Health / MaxHealth`、访问器、100/100 初始值，并挂到角色 |
| 3.1 属性钳制 | ✅ `PreAttributeChange` 将 `Health` 钳制到 `[0, MaxHealth]` |
| 3.2 属性变化通知 | ✅ 本地变化和网络复制都会广播蓝图可绑定的 `OnHealthChanged` |
| 3.4 Damage 元属性 | ✅ GE 写入 `Damage` 后由 `PostGameplayEffectExecute` 扣减 `Health`，并将 `Damage` 归零；`Health` 已从 Modifier 列表隐藏 |
| 3.5 Execution 概念 | ✅ 当前代码已具备接收 GE Modifier 或后续自定义 Execution 输出到 `Damage` 的结算入口（本节教程不要求实现具体 Execution 类） |
| 4.1–4.2 Gameplay Ability | ℹ️ 本节是 GA 蓝图创建、授予、激活及网络策略配置，没有新增必需 C++ 类型 |
| 4.3 Energy 属性 | ✅ 新增 `Energy / MaxEnergy`、100/100 初始值、访问器、复制回调、范围钳制和 `OnEnergyChanged` |

## 附带的额外配置（实验项目自备，非教程必需）

- **可玩角色**：传统输入（WASD + 鼠标）第三人称角色，占位方块视觉（无第三人称模板内容）；
- **镜头**：SpringArm + FollowCamera 过肩视角；
- **默认地图**：直接用引擎自带 `OpenWorld`（打开编辑器即有可落地世界）；
- **默认 GameMode**：`AAbilitiesLabGameMode`，出生 Pawn 即 GAS 角色。

## 如何打开

方式一（推荐）：双击 `AbilitiesLab.uproject`（引擎关联写的是 `5.8`；若提示找不到引擎，用方式二）。

方式二：直接指定引擎启动：
```
"D:\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe" "C:\GAS-Notes\AbilitiesLab\AbilitiesLab.uproject"
```

首次打开编辑器会提示编译缺失模块——点 Yes 让它编译（或先运行过仓库内的编译命令）。

## 仍需在编辑器中完成的蓝图配置

- 2.2–2.3 / 3.4：将 `GE_Health_Minus15` 的 Modifier 改为 `Damage +15`，并用调试输入把它施加给自己；
- 3.3：创建 `WBP_HealthBarWorld`，绑定 `ULabHealthAttributeSet.OnHealthChanged`，再给角色蓝图添加 Widget Component；
- 4.1–4.3：在 `GA_DamageSelf` 中调用 `CommitAbility`，配置 `GE_Cost_Energy20` 和 `GE_Cooldown_DamageSelf`，并在服务器授予技能；
- PIE 后可用 Gameplay Debugger 验证初始值为 `Health = 100 / MaxHealth = 100`。
