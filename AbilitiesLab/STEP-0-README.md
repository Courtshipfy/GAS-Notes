# AbilitiesLab —— GAS 学习实验项目（第 0 步已完成）

> 对应 [docs/your-first-60-minutes-with-gas-zh.md](../docs/your-first-60-minutes-with-gas-zh.md) 的「第 0 步 · 项目设置」与仓库根 [LEARNING-ROADMAP.md](../LEARNING-ROADMAP.md) 阶段 A。
> 引擎：**UE 5.8.1**（教程原文按 5.5 编写，本节代码在 5.8 同样适用）。

## 已完成的第 0 步内容

| 教程条目 | 落地情况 |
|---|---|
| 0.1 启用 GameplayAbilities 插件 | ✅ 已在 `AbilitiesLab.uproject` 的 Plugins 里启用（无需进编辑器勾选） |
| 0.2 Build.cs 引入 GAS 模块 | ✅ `Source/AbilitiesLab/AbilitiesLab.Build.cs` 已加入 `GameplayAbilities / GameplayTags / GameplayTasks` |
| 0.3 创建 ASC 子类并挂到角色 | ✅ `ULabAbilitySystemComponent`（`LabAbilitySystemComponent.h`）挂到 `AAbilitiesLabCharacter`（变量 `LabAbilitySystemComp`） |
| 0.4 实现 IAbilitySystemInterface | ✅ `AAbilitiesLabCharacter : public IAbilitySystemInterface` + `GetAbilitySystemComponent()` |
| 0.5 设置 Owner 与 Avatar | ✅ `BeginPlay` 里 `InitAbilityActorInfo(this, this)`；另在 `PossessedBy/UnPossessed` 补了 `RefreshAbilityActorInfo()`（修复多人 PC 过期崩溃的常见坑） |

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

## 下一步（教程第 1 步：Attribute Set 入门）

在 `Source/AbilitiesLab/` 下新建 `ULabHealthAttributeSet`（继承 `UAttributeSet`，定义 `Health / MaxHealth`，`ATTRIBUTE_ACCESSORS` 宏，构造函数 `InitHealth(100)/InitMaxHealth(100)`），并把 `HealthSet` 挂到角色构造函数；然后 PIE（Shift+` 打开 Gameplay Debugger）验证 100/100。
