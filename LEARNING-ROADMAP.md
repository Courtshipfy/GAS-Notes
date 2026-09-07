# UE GAS 学习路线图（定稿 v1.0）

> **状态**：定稿 v1.0（wayfinder 用户批准 v0.1，无修改）。由 wayfinder effort `.scratch/learn-ue-gas`（ticket 01–07）产出，合成全部决策；原型初稿存档于 `.scratch/learn-ue-gas/roadmap-draft.md`。
>
> 面向读者：UE C++ 较熟、可直接上 GAS。
> 教学路线：**理论先行 → 积累到一定规模后实战**；范围：单人核心 → 联网/预测；不限玩法类型。

## 一图流（总体结构）

```
阶段 A  理论学习（单人核心 ①②③）── 系统梳理 + 块级 10 分钟验证实验
         A1 数值与标签 → A2 效果系统 → A3 能力与表现
                │ 收口：GASDocumentation 单人章节精读完毕
阶段 B  实战小 demo（demo 1–4）── 独立小 demo，逐个验收单人核心
阶段 C  联网（④ + demo 5）── UE 网络基础速览 → 复制 + 完整预测 → 联网 demo
阶段 D  进阶延伸（可选）── Lyra + x157 源码走读
```

**贯穿约定**
- 技术栈：**C++ 打底**（ASC 接入、AttributeSet、Ability 基类、Execution Calculation、Effect Context 扩展），**蓝图编排**（能力图、Ability Task 节点、Gameplay Cue 表现、数据/变量）。
- 深度政策：**硬门槛吃透**才往下走；其余**先用后补**、用到再深挖。硬门槛 = Gameplay Tag、Attribute/AttributeSet、Gameplay Effect（含 Modifier）、Gameplay Ability（含 Cost/Cooldown）。
- 时间：**开放排期**——每块给参考体量，你定了每周时数后可按比例铺成日历；未定前按内容推进，不绑固定节奏。
- 语言：英文权威为主（GASDocumentation），中文对照（官方中文页 `lang=zh-CN`、`jockchou/GASDocumentation_Chinese`）；CSDN/知乎/Bilibili 个人笔记不作依据。

---

## 阶段 A：理论学习（单人核心 ①②③）

推进方式：以「概念块」为单位——**读 → 10 分钟验证实验 → 自查**。验证实验只求概念落地，不算游戏 demo。

### A1 数值与标签（地基 · 硬门槛）
- **概念**：Gameplay Tag → Gameplay Attribute → Attribute Set
- **读**：官方《Your First 60 Minutes with GAS》（有中文版）→ GASDocumentation 的 *Gameplay Tags / Attributes / Attribute Set* 章节 → 官方对应文档页
- **10 分钟实验**：角色挂 ASC + 含 `MaxHealth/Health` 的 AttributeSet，C++ 读写、HUD 显示
- **收口自查**：Tag 与枚举的差别；Attribute「存值不存逻辑」；AttributeSet 的 `PreAttributeChange` / `PostGameplayEffectExecute` 钩子用途

### A2 效果系统（硬门槛：Gameplay Effect 含 Modifier）
- **概念**：ASC → Modifier → MMC → Gameplay Effect（Execution Calculation、MMC 此处先用后补）
- **读**：GASDocumentation 的 *Gameplay Effects* 章节 → 官方 *Attributes & Effects*
- **10 分钟实验**：Instant GE 做伤害/治疗；Duration GE 做 buff/debuff；观察标签授予/移除
- **收口自查**：GE 是唯一合法改属性的手段；Modifier 幅值来源（曲线 / AttributeBased / MMC / SetByCaller）各自适用场景

### A3 能力与表现（硬门槛：Gameplay Ability 含 Cost/Cooldown）
- **概念**：Gameplay Ability → Ability Spec/Handle → Cost/Cooldown → Ability Task → Gameplay Event → Gameplay Cue（运行时再补 Effect Spec/Context、Active Effect、Tag Query 细节）
- **读**：GASDocumentation 的 *Gameplay Abilities / Ability Tasks / Cooldowns / Gameplay Events / Gameplay Cues* 章节 → 官方 *Using Gameplay Abilities / Ability Tasks*
- **10 分钟实验**：一个输入施放的技能（C++ 基类 + 蓝图编排，带冷却/消耗 + montage）；受击发 Gameplay Event，命中触发 Gameplay Cue
- **收口自查**：Ability 生命周期 `CanActivate → Activate → Commit → End`；AbilityTags / Block / Cancel 过滤；Task 异步编排怎么用

**阶段 A 总收口标志**：单人部分 21 个概念能不看文档讲清依赖关系（等价：GASDocumentation 单人章节精读完毕）。→ 未达标志不进入阶段 B。

---

## 阶段 B：实战小 demo 序列（独立 · 不限玩法 · 做完即停）

每个 demo：**目标 → 验收概念 → 做完对照硬门槛/先用后补清单自查**。通用角色/靶子即可，不套某类玩法。
可选对照：Stephen Ulibarri 课程对应章节（另一种实现视角，非主线依赖）。

| # | demo | 目标 | 验收概念 |
|---|---|---|---|
| B1 | **数值沙盒** | 角色 + ASC + AttributeSet，输入触发 Instant GE 伤害/治疗，HUD 数值变化 | Tag、Attribute、AttributeSet、ASC、GameplayEffect、Modifier |
| B2 | **状态与效果** | Duration 效果、叠加、Debuff、Tag 阻断/移除、冷却标签 | GameplayEffect 深度、Active Effect/Handle、Tag Query、标签状态机 |
| B3 | **技能施放** | 输入映射技能，Cost/Cooldown、Montage、Ability Task 等待 | GameplayAbility、Ability Spec/Handle、Cost/Cooldown、Ability Task |
| B4 | **事件与表现** | Gameplay Event 广播（受击/命中）、Gameplay Cue 视听反馈、做一次真实伤害公式 | GameplayEvent、GameplayCue、Effect Context/Spec 运行时细节、MMC/Execution（兑现「先用后补」） |

---

## 阶段 C：联网（demo 5 · 标准深度：复制 + 完整预测）

**前置检查（全过才进，任一不过先补）**：
1. 硬门槛全部吃透；
2. B1–B4 完成且是自己的代码（亲手写过 Ability 激活/Commit/End、创建并施加 Effect Spec、发 Event、触发 Cue）；
3. **UE 网络基础速览**（起点「会用但不深」）：Listen Server + PIE 多开、Actor/ActorComponent 复制、Role（Authority / AutonomousProxy / SimulatedProxy）、Server/Client RPC。

- **C0 UE 网络基础速览**：官方 Networking 文档；用 PIE 双开联调小场景验证 Role 与 RPC 走向
- **C1 复制**：ASC 复制的四类对象（属性 / Active Effect / Ability Spec / 标签）；复制模式 Minimal / Mixed / Full；`NetExecutionPolicy`（LocalOnly / LocalPredicted / ServerOnly / ServerInitiated）
- **C2 预测**：LocalPredicted 技能；Prediction Key 与 `FScopedPredictionWindow`；客户端本地先行 → 服务器追平（caught up）/ 拒绝（rejected）；预测 Gameplay Cue 对账；瞬时伤害不做预测的原则
- **C3 demo 5 联网小 demo**：复用效果/技能场景，监听服务器 + 两个客户端——先跑通复制，再给技能加完整预测

**读**：GASDocumentation 的 *Prediction / Replication / RPC* 章节（单机→联网的桥梁）

**demo 5 验收**：两端属性/效果/技能状态一致；本地预测技能无可见延迟；预测错误能正确回滚。

---

## 阶段 D：进阶延伸（可选 · 抵达终点后）

- **Lyra + x157 源码走读**：GameFeatures、装备系统、Enhanced Input + GameplayTags 联动、联网最佳实践——把「会用」升级到「工程化」。读：`EpicGames/Lyra` + [x157 源码走读](https://x157.github.io/UE5/LyraStarterGame/)
- 如需引导式完整工程兜底：Stephen 的 Top Down RPG 付费课（与主线「独立小 demo」路线不同，作为备选路径）

---

## 检查点小结（掌握程度验收 = 硬门槛清单 + 每 demo 验收点）

| 位置 | 检查点 |
|---|---|
| A1/A2/A3 末 | 各块收口自查（见上） |
| 阶段 A 末 | 不看文档讲清单人 21 概念依赖 |
| B1–B4 每个 demo 末 | demo 验收概念清单对照 |
| C0/C1/C2 末 | 网络基础 / 复制 / 预测自查 |
| C3（demo 5）末 | 两端一致 + 无延迟 + 可回滚 |

**怎么用**：从 A1 往下走；硬门槛不跳过；demo 独立做完即停；到 C3 即达「能在任意玩法独立搭 GAS 技能系统（含联网）」的终点；D 按需。
