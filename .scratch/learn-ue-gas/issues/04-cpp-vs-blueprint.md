# 确定 C++ 与蓝图在 GAS 学习中的侧重

Type: grilling
Status: resolved

## Question

GAS 学习里 C++ 与蓝图各占多大比重？是 C++ 为主、蓝图作为暴露层/设计师接口，还是大量用蓝图实现能力（很多课程是蓝图为主）？

默认倾向：C++ 为主（学习者 UE C++ 较熟），蓝图仅作为「把 C++ 能力暴露给策划/表现层」的补充。需要你确认或调整。

产出：一条技术栈约定，决定路线图里「用 C++ 写什么、用蓝图写什么」，以及资源盘点（02）里优先选偏 C++ 还是偏蓝图的材料。

## Answer

**决策：C++ 打底 + 蓝图编排**（Lyra 与 Stephen 课程的实际用法）。

- **C++ 写框架层**：`UAbilitySystemComponent` 接入、`UAttributeSet`（含 `PreAttributeChange`/`PostGameplayEffectExecute` 钩子）、`UGameplayAbility` 基类、`UGameplayEffectExecutionCalculation`（纯 C++）、`UGameplayModMagnitudeCalculation`、`FGameplayEffectContext` 扩展、需要时的自定义 `UAbilityTask` 与 Gameplay Cue 的 C++ 部分。
- **蓝图写表现/编排层**：能力蓝图图（从 C++ 基类派生的 Ability Graph）、Ability Task 节点编排、Gameplay Cue 的视听表现实现、GE/Ability 的 Data Asset 配置（蓝图子类）、数据与变量。
- **对路线图的影响**：资源主线里 Stephen 的 Top Down RPG 课正是这种混合风格，Lyra 亦是 C++ 基类 + 蓝图/Data 配置；因此选材以 C++ 为主、保留蓝图编排，不选「纯蓝图」类教程。
