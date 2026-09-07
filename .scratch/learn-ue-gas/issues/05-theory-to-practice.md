# 确定理论→实战的切换时机与 milestone demo

Type: grilling
Status: resolved
Blocked by: 03

## Question

用户要求「先做理论梳理，积累到一定规模后再结合实际游戏 demo」。这个「一定规模」具体指学完哪些概念？（即在学习顺序 03 的哪个节点切换）切换后要做什么 demo？每个 demo 验收哪些概念？

产出：一批渐进式 milestone demo（不限玩法类型），以及理论阶段与实战阶段的清晰分界点。

## Answer

**决策：理论到 ③ 收口 + 一串独立小 demo + 理论阶段内配极小验证实验。**

**分界点（理论 → 实战）**：
- **理论阶段** = 学习顺序的 ①②③（数值标签 / 效果系统 / 能力表现，全部单人核心）。系统梳理为主，但每学完一块概念配一个 **10 分钟极小验证实验**（跟官方 60 分钟教程、GASDocumentation 示例走），确保概念落地——不算游戏 demo。
- **收口标志**：单人部分概念（①-③ 涉及的 21 个概念）能不看文档讲清依赖关系（等价完成标志：GASDocumentation 单人章节精读完毕）。
- **实战阶段** = ③ 收口后切入，做一串**独立小 demo**（不限定玩法类型，通用角色/靶子即可），每个小而完整、做完即停。
- **联网 ④ 不纯学**：直接做联网小 demo，边做边学 Replication → PredictionKey → Prediction。

**渐进式 milestone demo 序列**：
1. **数值沙盒 demo** — 角色 + ASC + AttributeSet（C++），输入触发 Instant GE 伤害/治疗，HUD 显示数值。验收：Tag、Attribute、AttributeSet、ASC、GameplayEffect、Modifier。
2. **状态与效果 demo** — 持续效果、叠加、Debuff、Tag 阻断/移除、冷却标签。验收：GameplayEffect 深度、Active Effect/Handle、Tag Query、标签状态机。
3. **技能施放 demo** — 输入映射技能、Cooldown/Cost、Montage、Ability Task 等待。验收：GameplayAbility、Ability Spec/Handle、Cost/Cooldown、Ability Task。
4. **事件与表现 demo** — Gameplay Event 广播（受击/命中）、Gameplay Cue 视听反馈、做一次真实伤害公式。验收：GameplayEvent、GameplayCue、Effect Context/Spec 运行时细节、MMC / Execution Calculation（「先用后补」在此兑现）。
5. **联网小 demo** — 复用效果/技能场景开监听服务器 + 两个客户端：先跑通复制（属性/效果/技能 Spec 同步），再给技能加 LocalPredicted 预测（Prediction Key、预测 Cue 的追平/拒绝）。验收：Replication、PredictionKey、Prediction。

> 每个 demo 做完即对照「硬门槛 / 先用后补」清单验收——这也同时是各阶段的「掌握程度」检查点；无单一 capstone，序列以联网 demo 收尾。
