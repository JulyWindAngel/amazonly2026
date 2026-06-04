# 亚马逊棋游戏程序实验报告

**计算机程序设计实验**

- 姓名：李豫
- 日期：2026年6月3日

北京大学信息科学技术学院

---

## 目录

1. [项目概述](#项目概述)
2. [AI工具使用声明](#ai工具使用声明)
3. [系统架构](#系统架构)
4. [功能实现](#功能实现)
5. [设计思路与算法原理](#设计思路与算法原理)
6. [遇到的问题与解决方案](#遇到的问题与解决方案)
7. [测试与结果](#测试与结果)
8. [总结与展望](#总结与展望)
9. [附录：关键代码片段](#附录关键代码片段)

---

## 项目概述

本项目实现了一个完整的亚马逊棋（Game of Amazons）游戏程序，包括图形用户界面（GUI）和人工智能（AI）对战功能。亚马逊棋是一种两人策略棋盘游戏，结合了国际象棋（皇后移动）和围棋（领土控制）的元素。玩家通过移动自己的亚马逊（皇后）并射箭来阻挡对手的移动路径，最终目标是控制更多领土并迫使对手无法移动。

程序采用C++编写，前端基于EasyX图形库实现交互界面，后端AI采用混合搜索策略：开局阶段使用深度限制为2的Alpha-Beta剪枝搜索，中后局采用基于UCT的扁平蒙特卡洛模拟（Flat Monte Carlo with UCB）实现智能决策。

---

## AI工具使用声明

本项目的开发过程中，合理使用AI辅助工具（如ChatGPT、GitHub Copilot等），具体使用情况如下：

- **前端代码（amazonly.cpp）**：完全由本人手工编写，AI工具仅提供知识性解答（如EasyX库函数用法），未直接生成任何代码。
- **后端AI代码（GUIamazonlybrain.cpp）**：算法整体思路由本人独立设计（包括混合搜索策略、评估函数结构、开局/中后局切换等）。AI工具根据本人逐条指令生成具体函数实现，本人对每个函数进行逐行审核、测试，并反复要求AI修改直至符合预期。接口设计、数据结构定义、文件交互逻辑等均由本人手工完成。
- **实验报告撰写**：报告中的文字描述、图表、算法原理说明由本人指导AI完成，本人已逐字逐句审核。

本人确认：项目的核心逻辑与系统架构体现了自主思考，AI仅作为编码辅助工具，所有生成代码均经过本人严格审查。

---

## 系统架构

### 模块划分

- **前端模块（amazonly.cpp）**：负责图形界面、用户交互、游戏状态管理。
- **后端AI模块（GUIamazonlybrain.cpp）**：负责游戏逻辑、AI决策算法。
- **文件存储模块**：负责游戏记录的保存与读取（GAME.txt, LASTGAME.txt）。

### 技术栈

- 编程语言：C++
- 图形库：EasyX Graphics Library
- 算法：Minimax搜索、Alpha-Beta剪枝、扁平蒙特卡洛模拟（Flat Monte Carlo）、UCB公式
- 数据结构：棋盘状态数组、皇后位置结构体、移动动作结构体

---

## 功能实现

### 基础功能

#### 菜单系统
- 新游戏：初始化棋盘，选择玩家角色（黑方/白方）
- 继续游戏：从上次保存的状态继续
- 历史对局：查看最近一场完整对局的回放（步进式）
- 退出游戏：保存当前对局并退出

#### 图形界面
- 8×8棋盘绘制（采用标准亚马逊棋开局布局）
- 黑白皇后图形化显示
- 蓝色箭头标记（阻挡位置）
- 游戏状态信息显示（当前回合、胜负状态）
- 坐标标注（0-7）

#### 游戏逻辑
- 皇后移动：像国际象棋皇后一样沿八个方向直线移动
- 射箭规则：移动后必须从新位置射箭，箭落点永久阻挡
- 胜负判定：无合法移动的玩家输掉游戏
- 回合制：黑白双方交替进行
- 移动合法性检查：防止移动到已有棋子或箭的位置

#### 存档功能
- 实时保存：每步移动自动记录到GAME.txt
- 对局保存：游戏结束时自动保存完整对局到LASTGAME.txt
- 对局回放：支持历史对局逐步回放

### 高级功能（扩展实现）

#### 智能AI系统

本程序AI采用混合策略：

- **前12回合**：使用2层Alpha-Beta剪枝搜索（带启发式评估与走法排序）
- **后续回合**：使用**扁平蒙特卡洛模拟（Flat Monte Carlo）**，对每个可行走法独立进行最多5000次深度限制为30的随机对局，统计胜率；结合**UCB公式**（Upper Confidence Bound）平衡探索与利用，选择综合得分最高的走法。
- **评估函数**：综合考虑领土控制、皇后自由度、包围战术

#### AI算法细节

**评估函数设计**：

1. **领土评估**：计算每个空方格到双方皇后的最小皇后距离（Queen Distance）
2. **控制权判断**：若玩家到某方格的距离小于对手，则计为潜在领土
3. **封闭惩罚**：对移动受限的皇后给予惩罚
4. **包围奖励**：对限制对手皇后移动的走法给予奖励

**搜索算法**：

1. **Alpha-Beta剪枝**：早期游戏使用（前INITIALSTEPS步），搜索深度2层
2. **基于UCB的扁平蒙特卡洛走法评估**：
   - 对每个候选走法独立进行随机模拟：MAX_SIMULATIONS = 5000，SIMULATION_DEPTH = 30
   - 采用UCB公式计算每个走法的评分：
     \[
     \text{score} = \frac{\text{wins}}{\text{visits}} + C \cdot \sqrt{\frac{\ln(\text{total\_sims})}{\text{visits}}}
     \]
     其中 \( C = 1.414 \)
   - 该方法的本质是将走法选择视为多臂老虎机（Bandit）问题，无需构建多级搜索树
3. **启发式排序**：对候选走法快速评分，优先搜索有潜力的走法

#### 性能优化

1. **走法生成优化**：缓存可达位置，避免重复计算
2. **评估缓存**：重复状态评估结果缓存
3. **提前终止**：深度限制的随机模拟，减少计算时间

---

## 设计思路与算法原理

### 基于文献的算法设计

参考三篇学术文献，本程序AI设计融合了以下先进思想：

#### 领土评估（参考Lieberum论文）

在本程序中，评估函数在Lieberum论文的基础上进行了一定的简化，仅考虑queenmove和封闭性惩罚。这是因为，前INITIALSTEPS无需太精细的计算，使用queenmove即可生成不错的走法。而使用简化的评估函数，有利于提高后续MC模拟的效率。

#### 蒙特卡洛方法（参考Lorentz和Kloetzer论文）

- 纯蒙特卡洛在亚马逊棋中效果有限
- 混合方法：MC模拟 + 静态评估函数
- 采用UCB公式平衡探索与利用，属于多臂老虎机问题的经典解法

### 创新点

1. **自适应策略切换**：前INITIALSTEPS回合使用精确搜索，后期使用扁平蒙特卡洛模拟
2. **混合评估函数**：结合距离评估、自由度评估、包围态势评估
3. **实时性优化**：在有限时间内完成决策，满足实时对局需求

---

## 遇到的问题与解决方案

### 技术问题

1. **图形刷新问题**：
   - 问题：连续点击导致皇后被拖动
   - 解决：改用WM_LBUTTONUP事件代替持续检测

2. **AI决策时间**：
   - 问题：蒙特卡洛模拟耗时过长
   - 解决：设置模拟次数上限，深度限制

### 算法问题

1. **搜索深度不足**：
   - 问题：前期局面复杂，搜索树爆炸
   - 解决：混合策略，前期用α-β剪枝。

2. **优化剪枝算法**：
   - 问题：前12回合效率较低
   - 解决：先用简化的评估函数选出前10种（15种）较优的算法，再用真正的评估函数计算。

---

## 测试与结果

### 功能测试

- 所有基础功能通过测试
- 图形界面响应正常
- 存档加载功能正确

### AI强度测试

- 自对弈测试：AI不同配置间对局
- 与随机AI对比：胜率显著提高
- 时间控制：每步决策时间约1秒，可以通过调整参数来实现速度提升或准确率提升

由于实现的是扁平蒙特卡洛模拟（非完整MCTS），AI的中后局决策依赖于模拟次数的统计稳定性。通过设置5000次模拟，每步决策时间约1秒，达到合理的棋力水平。

---

## 总结与展望

### 项目总结

本项目成功实现了一个功能完整的亚马逊棋游戏系统，具有以下特点：

1. 友好的图形用户界面，操作直观
2. 智能AI对手，采用混合搜索算法（Alpha-Beta + 扁平UCB）
3. 完整的游戏管理功能（存档、回放、继续）
4. 遵循亚马逊棋标准规则，逻辑正确

### 创新贡献

1. **算法融合**：将深度限制的Alpha-Beta剪枝（用于开局）与扁平蒙特卡洛评估（用于中后局）相结合
2. **评估函数设计**：综合多篇文献思想，设计适合亚马逊棋的评估体系
3. **工程实现**：在有限时间内完成复杂游戏系统的开发
4. **图形交互界面**：设计了一个简洁的图形交互界面
5. **逐步复盘**：可以通过单击左键，对之前的棋局进行逐步复盘

### 未来改进方向

1. **算法优化**：
   - 实现更高效的蒙特卡洛模拟（重播技术）
   - 引入模式识别和开局库
   - 优化终局搜索算法

2. **功能扩展**：
   - 多种AI难度级别（可以通过调整参数来完成，这是容易实现的）
   - 对局分析与建议功能
   - 可以保存更多的棋局（目前可以保存当前棋局和上一局，而保存多局是容易实现的）

3. **性能提升**：
   - 并行化蒙特卡洛模拟
   - 更高效的状态表示和哈希
   - 机器学习增强评估函数

---

## 附录：关键代码片段

### 1. 评估函数实现（简化核心部分）

```cpp
double evaluatePosition(int player) {
    int opponent = 1 - player;
    double total_score = 0.0;
    
    // 1. 皇后距离评估
    vector<vector<int>> player_dist(BOARD_SIZE, 
                                   vector<int>(BOARD_SIZE, 1000));
    vector<vector<int>> opponent_dist(BOARD_SIZE, 
                                     vector<int>(BOARD_SIZE, 1000));
    
    // 计算玩家皇后的可达距离
    for (int q = 0; q < 4; q++) {
        int qx = queens[player][q].x;
        int qy = queens[player][q].y;
        
        // 8个方向
        int dx[] = {1, 1, 0, -1, -1, -1, 0, 1};
        int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};
        
        for (int d = 0; d < 8; d++) {
            for (int step = 1; step < BOARD_SIZE; step++) {
                int nx = qx + dx[d] * step;
                int ny = qy + dy[d] * step;
                
                // 检查边界和障碍物
                if (nx < 0 || nx >= BOARD_SIZE || 
                    ny < 0 || ny >= BOARD_SIZE) break;
                if (ArrowAndQueenInBoard[nx][ny] != 0) break;
                
                // 更新最小距离
                if (step < player_dist[nx][ny]) {
                    player_dist[nx][ny] = step;
                }
            }
        }
    }
    
    // 类似计算 opponent_dist...
    
    // 2. 封闭惩罚评估
    double enclosure_score = 0.0;
    for (int q = 0; q < 4; q++) {
        int qx = queens[player][q].x;
        int qy = queens[player][q].y;
        
        if (isEnclosed(qx, qy, player)) {
            enclosure_score -= 3.0; // 封闭惩罚
        }
    }
    
    return total_score + enclosure_score;
}

```

### 2. 扁平UCB走法选择实现

```cpp
Move selectBestMoveUCT() {
    vector<Move> allMoves = generateAllMoves(role);
    if (allMoves.empty()) {
        return Move();
    }
    
    if (allMoves.size() == 1) {
        return allMoves[0];
    }
    
    // 对每个候选走法进行独立蒙特卡洛模拟
    for (int i = 0; i < MAX_SIMULATIONS; i++) {
        // 备份棋盘状态
        int board_backup[BOARD_SIZE][BOARD_SIZE];
        queen queens_backup[2][4];
        memcpy(board_backup, ArrowAndQueenInBoard, sizeof(board_backup));
        memcpy(queens_backup, queens, sizeof(queens_backup));
        
        // 选择一个走法（循环选择，保证每个走法被模拟相同次数）
        int move_index = i % allMoves.size();
        auto& move = allMoves[move_index];
        
        // 执行移动
        int qx = queens[role][move.queen_index].x;
        int qy = queens[role][move.queen_index].y;
        int old_queen_code = ArrowAndQueenInBoard[qx][qy];
        ArrowAndQueenInBoard[qx][qy] = 0;
        ArrowAndQueenInBoard[move.move_x][move.move_y] = old_queen_code;
        queens[role][move.queen_index].x = move.move_x;
        queens[role][move.queen_index].y = move.move_y;
        ArrowAndQueenInBoard[move.arrow_x][move.arrow_y] = 1;
        
        // 随机模拟对局
        int result = simulateRandomGame(role);
        
        // 更新统计
        move.visits++;
        move.wins += result;
        
        // 恢复棋盘
        memcpy(ArrowAndQueenInBoard, board_backup, sizeof(ArrowAndQueenInBoard));
        memcpy(queens, queens_backup, sizeof(queens));
    }
    
    // 使用UCB公式选择最佳走法
    Move best_move;
    double best_score = -1e9;
    for (auto& move : allMoves) {
        if (move.visits > 0) {
            double win_rate = (double)move.wins / move.visits;
            double exploration = UCT_C * sqrt(log(MAX_SIMULATIONS) / move.visits);
            double score = win_rate + exploration;
            // 可加入启发式修正
            if (score > best_score) {
                best_score = score;
                best_move = move;
            }
        }
    }
    return best_move;
}

```
