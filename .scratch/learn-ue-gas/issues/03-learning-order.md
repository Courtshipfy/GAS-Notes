# 确定 GAS 概念的学习顺序（依赖图 → 路线）

Type: grilling
Status: resolved
Blocked by: 01

## Question

基于概念清单（01）的依赖关系，GAS 各概念应按什么顺序学？哪个概念是「地基」（必须先吃透），哪些可以并行，哪些适合放到联网阶段之后？

产出：一份确定的学习顺序（依赖图 → 线性/分阶段路线），作为整份规划的主干。

## Answer

**决策：四阶段主干 + 「硬门槛 / 先用后补」两档深度。**

四阶段顺序（前三阶段单人可用，第四阶段联网专用）：
1. **数值与标签（地基）**：Gameplay Tag → Gameplay Attribute → Attribute Set
2. **效果系统**：ASC → Modifier → MMC → Gameplay Effect
3. **能力与表现**：Gameplay Ability → Ability Spec/Handle → Cooldown/Cost → Ability Task → Gameplay Event → Gameplay Cue
4. **联网层**：Replication → Prediction Key → Prediction

**硬门槛（必须吃透才往下）**：Gameplay Tag、Attribute、Attribute Set、Gameplay Effect（含 Modifier）、Gameplay Ability（含 Cooldown/Cost）。

**先用后补（会用即可，细节用到再深挖）**：Execution Calculation（做伤害公式时深挖）、MMC（轻量公式，Modifier 不够用时学）、Effect Spec/Context、Active Effect/Handle、Tag Query（阶段 3 用能力创建效果时补运行时细节）、Cue Manager / Ability System Globals（按需查阅）、Prediction Key（联网阶段再补）。

**并行/按需**：阶段之间严格顺序；阶段内因依赖基本线性。Tag Query、Cue Manager、Ability System Globals 属「按需查阅」，不排固定课时。
