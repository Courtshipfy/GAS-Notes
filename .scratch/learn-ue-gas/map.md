# Map: UE GAS 学习规划（Learn UE GAS）

## Destination

产出一份**全面通用、不限玩法类型**的 UE Gameplay Ability System 学习规划：先系统梳理 GAS 的理论知识（概念清单 + 依赖关系 + 权威资源），积累到一定规模后再结合实际游戏 demo 实战；范围覆盖单人核心 → 联网（replication）/预测（prediction）进阶。规划面向 UE C++ 已较熟、可直接上 GAS 的学习者，终点是「能在任意玩法里独立搭建 GAS 技能系统」。

## Notes

- **领域**：Unreal Engine 的 Gameplay Ability System（GAS）。
- **教学路线（standing preference）**：理论先行——先系统学概念，积累到一定规模后再结合实际游戏 demo。
- **范围**：单人核心 → 联网/预测进阶，分阶段。
- **起点**：UE C++ 较熟，可直接上 GAS，不补 C++/UE 基础。
- **技术栈**：C++ 为主，蓝图作为暴露层/设计师接口（默认，待「确定 C++ 与蓝图在 GAS 学习中的侧重」确认）。
- **时间投入**：开放（未指定每周时数），规划按内容阶段排期，每阶段给出可调节节奏。
- **资源语言**：英文权威资源为主（官方文档 / GASDocumentation / Lyra），中文材料辅助。
- **技能**：本 effort 各 session 应参考 grilling、domain-modeling、research（研究类 ticket）、prototype（路线图草案）。
- **tracker**：local-markdown（`.scratch/learn-ue-gas/`）。

## Decisions so far

<!-- 索引：每行一条已关闭的 ticket，gist + 链接 -->

- [盘点 GAS 的完整概念清单与依赖关系](issues/01-gas-concept-inventory.md) — 24 个概念，按 5 层依赖组织（命名数值 → 中枢修改 → 效果实例化 → 能力表现 → 联网），含建议阅读顺序。
- [盘点 GAS 的权威学习资源及其概念覆盖](issues/02-gas-learning-resources.md) — 12 项权威资源；推荐主线：官方 60 分钟入门 → GASDocumentation 精读 → Stephen Top Down RPG 实战课 → Lyra + x157 源码走读；中文只信官方中文 + tranek 社区翻译。
- [确定 C++ 与蓝图在 GAS 学习中的侧重](issues/04-cpp-vs-blueprint.md) — C++ 打底（ASC/AttributeSet/Ability 基类/ExecutionCalc）+ 蓝图编排（能力图/Ability Task/表现）。
- [确定 GAS 概念的学习顺序（依赖图 → 路线）](issues/03-learning-order.md) — 四阶段：数值标签 → 效果系统 → 能力表现 → 联网；硬门槛吃透 + 其余先用后补。

## Not yet specified

<!-- 见 "Fog of war"：尚无法精确定义、随 frontier 推进而毕业的雾区 -->

- 各阶段「掌握程度」的验收方式（如何证明自己真学会了某概念）—— 等学习顺序（03）定了才好定义具体验收点。
- 收尾的 capstone 综合 demo 规格（「任意玩法」的最终综合练习）—— 等 milestone demo（05）定了才好细化。

## Out of scope

<!-- 超出本 effort 目的地的工作，永不毕业 -->

- UE / C++ 基础（起点已熟，不纳入）。
- 某类玩法的深度专项实现（ARPG / 射击 / MOBA 等）—— 目的地是「不限类型通用」。
- 商业化发布、上线运营。
- 学习笔记在 GAS-Notes 仓库里的组织结构 —— 本 effort 只产出「学习规划」本身，笔记组织可作为后续独立 effort。
