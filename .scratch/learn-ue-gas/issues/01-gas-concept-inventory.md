# 盘点 GAS 的完整概念清单与依赖关系

Type: research
Status: resolved

## Question

UE 的 Gameplay Ability System（GAS）到底包含哪些核心概念/子系统？它们各自的职责是什么，彼此之间的学习依赖关系（哪个概念必须先懂、才能懂下一个）是什么？哪些只属于单人，哪些涉及联网（复制/预测）？

产出：一份完整、按依赖分层的 GAS 概念清单，作为「理论系统梳理」的骨架，也是后续「确定 GAS 概念的学习顺序」的输入。

## Answer

已产出 24 个概念的完整清单（每个含定义/职责/依赖/单机-联网归属，逐条标注出处）；完整清单见 [research/01-gas-concept-inventory.md](../research/01-gas-concept-inventory.md)。

依赖分层（基础 → 高级，前 4 层单机可用，第 5 层联网专用）：
1. **命名与数值**：Gameplay Tag → Attribute → Attribute Set
2. **中枢与修改**：ASC、Modifier、MMC、Execution Calculation、Gameplay Effect
3. **效果实例化**：Effect Context/Spec、Active Effect/Handle、Tag Query
4. **能力与表现**：Gameplay Ability/Spec/Graph、Cooldown/Cost、Ability Task、Gameplay Event、Gameplay Cue（+ Cue Manager、Ability System Globals）
5. **联网层**：Replication → Prediction Key → Prediction

建议阅读顺序：Tag → Attribute → AttributeSet → ASC → Modifier → Execution → Effect → Spec/Context → Ability → Spec/Handle → Cooldown/Cost → Task → Event → Cue →（联网）Replication → PredictionKey → Prediction。
