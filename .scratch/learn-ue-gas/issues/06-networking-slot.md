# 确定联网/预测在路线中的插入位置

Type: grilling
Status: resolved
Blocked by: 03

## Question

范围已定「单人 → 联网/预测进阶」。那么 replication（复制）与 prediction（预测）在学习顺序（03）的哪里插入？单人阶段需要先吃透哪些概念，联网阶段才不卡壳？

产出：联网/预测阶段的前置条件 + 插入位置 + 该阶段要覆盖的内容。

## Answer

**决策：联网/预测 = 第四阶段（demo 5 落地），标准深度（复制 + 完整预测），进门前先补一段「UE 网络基础」速览。**

**插入位置**：学习顺序第四阶段，位于单人核心（③）与单人 demo 1-4 全部完成后；以 demo 5（联网小 demo）为落地载体边做边学（05 已定）。

**前置条件（进联网前必须满足）**：
1. 硬门槛全部吃透：GameplayTag、Attribute/AttributeSet、GameplayEffect（含 Modifier）、GameplayAbility（含 Cost/Cooldown）。
2. 单人 demo 1-4 完成且是自己的代码——亲手写过 Ability 激活/Commit/End、创建 Effect Spec 并施加、发 GameplayEvent、触发 GameplayCue。因为复制/预测包裹的正是这些操作。
3. **UE 网络基础速览模块**（起点为「会用但不深」）：Listen Server + PIE 多开、Actor/ActorComponent 复制、Role（Authority/AutonomousProxy/SimulatedProxy）、Server/Client RPC——先补这块，否则会在最底层概念上卡壳。

**该阶段覆盖内容（标准深度）**：
- **复制**：ASC 复制的四类对象（属性、Active Effect、Ability Spec、标签）；复制模式 Minimal / Mixed / Full；NetExecutionPolicy（LocalOnly / LocalPredicted / ServerOnly / ServerInitiated）。
- **预测**：LocalPredicted 技能；Prediction Key 与 FScopedPredictionWindow；客户端本地先行 → 服务器权威追平（caught up）/拒绝（rejected）；预测 GameplayCue 对账；瞬时伤害不做预测的原则。
- **落地与验收（demo 5）**：监听服务器 + 两个客户端，先跑通复制再上完整预测；验收 = 两端属性/效果/技能状态一致、本地预测技能无可见延迟、预测错误能正确回滚。
