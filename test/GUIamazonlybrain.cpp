#include<iostream>
#include<iomanip>
#include<ctime>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<sstream>
#include<fstream>
#include<string>
#include<Windows.h>
#include<windows.h>
#include<vector>
#include<cmath>
#include<algorithm>

using namespace std;

// 常量定义
const int BOARD_SIZE = 8;
const int MAX_SIMULATIONS = 5000;  // 最大模拟次数，可根据时间调整
const int SIMULATION_DEPTH = 30;    // 模拟深度
const double UCT_C = 1.414;        // UCT探索常数
const int INITIALSTEPS=12;
// 数据结构定义
struct Move {
    int queen_index;
    int move_x, move_y;
    int arrow_x, arrow_y;
    double value;  // 用于UCT的值
    int visits;    // 访问次数
    int wins;      // 获胜次数
};

struct TreeNode {
    vector<Move> moves;      // 可能的走法
    TreeNode* parent;        // 父节点
    int total_visits;        // 总访问次数
    bool is_white_turn;      // 当前轮到谁
    double territory_score;  // 领土得分
};

// 全局变量
int ArrowAndQueenInBoard[BOARD_SIZE][BOARD_SIZE] = {};
int role = 1;  // 0:黑方, 1:白方

struct queen {
    int x, y;
};

queen queens[2][4] = {};

// 函数声明
void ReadTheFile();
void playgame();
void DrawTheRoad(int x, int y);
int AmazonlyBrain_Multilevel(int search_depth);

bool AbleToMove(int x, int y);
int AmazonlyBrain();
void setarrow(int x, int y);
void temporarycheck();
void initializeBoard();
vector<Move> generateAllMoves(int player);

double evaluatePosition(int player);
int calculateFreedom(int queen_x, int queen_y, int player);
bool isEnclosed(int queen_x, int queen_y, int player);
int calculateQueenDistanceWithObstacles(int x1, int y1, int x2, int y2);
int simulateRandomGame(int player);

Move minimaxSearch(int player, int depth, double alpha, double beta);
double quickEvaluateMove(int player, const Move& move);
Move selectBestMoveUCT();
void updateNode(TreeNode* node, bool win);
TreeNode* selectNode(TreeNode* node);
TreeNode* expandNode(TreeNode* node);
int simulate(TreeNode* node);
void backpropagate(TreeNode* node, bool win);

// 主函数
int main() {
    DWORD start,end;//临时
    start=GetTickCount();
    srand(time(0));
    playgame();
    temporarycheck();
    end = GetTickCount();//临时
    cout<<endl<<"RUNNTIME: "<<end - start<<endl;//临时
    return 0;
}

// 读取游戏文件
void ReadTheFile() {
    // 初始化棋盘
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            ArrowAndQueenInBoard[i][j] = 0;
        }
    }

    // 初始化皇后位置（标准开局）
    // 黑方
    queens[0][0].x = 0; queens[0][0].y = 2;
    queens[0][1].x = 2; queens[0][1].y = 0;
    queens[0][2].x = 5; queens[0][2].y = 0;
    queens[0][3].x = 7; queens[0][3].y = 2;

    // 白方
    queens[1][0].x = 7; queens[1][0].y = 5;
    queens[1][1].x = 5; queens[1][1].y = 7;
    queens[1][2].x = 2; queens[1][2].y = 7;
    queens[1][3].x = 0; queens[1][3].y = 5;

    // 在棋盘上标记皇后
    for (int i = 0; i < 4; i++) {
        ArrowAndQueenInBoard[queens[0][i].x][queens[0][i].y] = i * 10 + 100;
        ArrowAndQueenInBoard[queens[1][i].x][queens[1][i].y] = i * 10 + 101;
    }

    // 读取游戏记录
    ifstream file("GAME.txt");
    string line;
    int steps = 0;
    while (getline(file, line)) {
        istringstream iss(line);
        int lineNums[6];
        for (int i = 0; i < 6; i++) {
            iss >> lineNums[i];
        }
        if (lineNums[0] == -1) {
            role = 0;
            steps++;
        } else {
            steps++;
            int queenandrole = ArrowAndQueenInBoard[lineNums[0]][lineNums[1]];
            queens[(steps + role) % 2][queenandrole / 10 - 10].x = lineNums[2];
            queens[(steps + role) % 2][queenandrole / 10 - 10].y = lineNums[3];
            ArrowAndQueenInBoard[lineNums[2]][lineNums[3]] = queenandrole;
            ArrowAndQueenInBoard[lineNums[0]][lineNums[1]] = 0;
            ArrowAndQueenInBoard[lineNums[4]][lineNums[5]] = 1;
        }
    }
    file.close();
}

// 主游戏函数
void playgame() {
    ReadTheFile();
    int flag = AmazonlyBrain();
    if (flag > 99) {
        ofstream outFile("GAME.txt", std::ios::app);
        outFile << "-2 -2 -2 -2 -2 -2" << endl;
        outFile.close();
    }
}

// 绘制可移动路径
void DrawTheRoad(int x, int y) {
    // 八个方向
    int dx[] = {1, 1, 0, -1, -1, -1, 0, 1};
    int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};
    
    for (int d = 0; d < 8; d++) {
        for (int step = 1; ; step++) {
            int nx = x + dx[d] * step;
            int ny = y + dy[d] * step;
            if (nx < 0 || nx >= BOARD_SIZE || ny < 0 || ny >= BOARD_SIZE) break;
            if (ArrowAndQueenInBoard[nx][ny] != 0) break;
            ArrowAndQueenInBoard[nx][ny] = 2;
        }
    }
}

// 检查是否可以移动
bool AbleToMove(int x, int y) {
    DrawTheRoad(x, y);
    bool flag = false;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (ArrowAndQueenInBoard[i][j] == 2) {
                flag = true;
                ArrowAndQueenInBoard[i][j] = 0;
            }
        }
    }
    return flag;
}

// 基于评估函数的走法选择（用于前INITIALSTEPS回合）
int AmazonlyBrain_MultiLevel(int search_depth) {    // 搜索深度：前INITIALSTEPS回合使用search_depth层搜索（自己走一步，对手走一步）
    // 检查是否有合法走法
    bool has_legal_move = false;
    for (int q = 0; q < 4; q++) {
        if (AbleToMove(queens[role][q].x, queens[role][q].y)) {
            has_legal_move = true;
            break;
        }
    }
    
    if (!has_legal_move) {
        return 100 + role;  // 没有合法走法，输了
    }
    

    
    
    // 使用Minimax算法搜索最佳走法
    Move best_move = minimaxSearch(role, search_depth, -1e9, 1e9);
    
    // 执行最佳走法
    int qx = queens[role][best_move.queen_index].x;
    int qy = queens[role][best_move.queen_index].y;
    
    ofstream outFile("GAME.txt", std::ios::app);
    outFile << qx << " " << qy << " "
            << best_move.move_x << " " << best_move.move_y << " "
            << best_move.arrow_x << " " << best_move.arrow_y << endl;
    outFile.close();
    
    // 更新棋盘状态
    int queen_code = ArrowAndQueenInBoard[qx][qy];
    ArrowAndQueenInBoard[qx][qy] = 0;
    ArrowAndQueenInBoard[best_move.move_x][best_move.move_y] = queen_code;
    queens[role][best_move.queen_index].x = best_move.move_x;
    queens[role][best_move.queen_index].y = best_move.move_y;
    ArrowAndQueenInBoard[best_move.arrow_x][best_move.arrow_y] = 1;
    
    return 0;
}

// Minimax搜索算法（带Alpha-Beta剪枝）
Move minimaxSearch(int player, int depth, double alpha, double beta) {
    // 生成所有可能走法
    vector<Move> allMoves = generateAllMoves(player);
    
    // 如果到达搜索深度或没有合法走法，返回当前局面的评估
    if (depth == 0 || allMoves.empty()) {
        // 创建一个虚拟的Move来返回评估值
        Move eval_move;
        eval_move.value = evaluatePosition(player);
        return eval_move;
    }
    
    Move best_move;
    
    if (player == role) { // 最大化玩家（AI自己）
        best_move.value = -1e9;
        
        // 对走法进行排序，提高剪枝效率
        vector<pair<double, Move>> scored_moves;
        for (auto& move : allMoves) {
            // 快速评估当前走法
            double quick_score = quickEvaluateMove(player, move);
            scored_moves.push_back({quick_score, move});
        }
        
        // 按分数降序排序
        sort(scored_moves.begin(), scored_moves.end(), 
             [](const pair<double, Move>& a, const pair<double, Move>& b) {
                 return a.first > b.first;
             });
        
        // 只考虑前15个最好的走法（加速搜索）
        int max_moves_to_consider = min(15, (int)scored_moves.size());
        
        for (int i = 0; i < max_moves_to_consider; i++) {
            auto& move_pair = scored_moves[i];
            auto move = move_pair.second;
            
            // 备份棋盘状态
            int board_backup[BOARD_SIZE][BOARD_SIZE];
            queen queens_backup[2][4];
            memcpy(board_backup, ArrowAndQueenInBoard, sizeof(board_backup));
            memcpy(queens_backup, queens, sizeof(queens_backup));
            
            // 执行当前走法
            int qx = queens[player][move.queen_index].x;
            int qy = queens[player][move.queen_index].y;
            int old_queen_code = ArrowAndQueenInBoard[qx][qy];
            
            ArrowAndQueenInBoard[qx][qy] = 0;
            ArrowAndQueenInBoard[move.move_x][move.move_y] = old_queen_code;
            queens[player][move.queen_index].x = move.move_x;
            queens[player][move.queen_index].y = move.move_y;
            ArrowAndQueenInBoard[move.arrow_x][move.arrow_y] = 1;
            
            // 递归搜索
            Move result = minimaxSearch(1 - player, depth - 1, alpha, beta);
            double move_value = result.value;
            
            // 恢复棋盘状态
            memcpy(ArrowAndQueenInBoard, board_backup, sizeof(ArrowAndQueenInBoard));
            memcpy(queens, queens_backup, sizeof(queens));
            
            // 更新最佳走法
            if (move_value > best_move.value) {
                best_move = move;
                best_move.value = move_value;
            }
            
            // Alpha-Beta剪枝
            alpha = max(alpha, move_value);
            if (beta <= alpha) {
                break; // 剪枝
            }
        }
    } else { // 最小化玩家（对手）
        best_move.value = 1e9;
        
        // 对走法进行排序（对手会选择对我们最不利的走法）
        vector<pair<double, Move>> scored_moves;
        for (auto& move : allMoves) {
            double quick_score = quickEvaluateMove(player, move);
            scored_moves.push_back({quick_score, move});
        }
        
        // 对手的走法按分数升序排序（假设对手会选择对我们最不利的走法）
        sort(scored_moves.begin(), scored_moves.end(), 
             [](const pair<double, Move>& a, const pair<double, Move>& b) {
                 return a.first < b.first;
             });
        
        // 只考虑前10个最差的走法（从AI的角度看）
        int max_moves_to_consider = min(10, (int)scored_moves.size());
        
        for (int i = 0; i < max_moves_to_consider; i++) {
            auto& move_pair = scored_moves[i];
            auto move = move_pair.second;
            
            // 备份棋盘状态
            int board_backup[BOARD_SIZE][BOARD_SIZE];
            queen queens_backup[2][4];
            memcpy(board_backup, ArrowAndQueenInBoard, sizeof(board_backup));
            memcpy(queens_backup, queens, sizeof(queens_backup));
            
            // 执行当前走法
            int qx = queens[player][move.queen_index].x;
            int qy = queens[player][move.queen_index].y;
            int old_queen_code = ArrowAndQueenInBoard[qx][qy];
            
            ArrowAndQueenInBoard[qx][qy] = 0;
            ArrowAndQueenInBoard[move.move_x][move.move_y] = old_queen_code;
            queens[player][move.queen_index].x = move.move_x;
            queens[player][move.queen_index].y = move.move_y;
            ArrowAndQueenInBoard[move.arrow_x][move.arrow_y] = 1;
            
            // 递归搜索
            Move result = minimaxSearch(1 - player, depth - 1, alpha, beta);
            double move_value = result.value;
            
            // 恢复棋盘状态
            memcpy(ArrowAndQueenInBoard, board_backup, sizeof(ArrowAndQueenInBoard));
            memcpy(queens, queens_backup, sizeof(queens));
            
            // 更新最佳走法（对于对手来说是最小值）
            if (move_value < best_move.value) {
                best_move = move;
                best_move.value = move_value;
            }
            
            // Alpha-Beta剪枝
            beta = min(beta, move_value);
            if (beta <= alpha) {
                break; // 剪枝
            }
        }
    }
    
    return best_move;
}

// 快速评估单个走法的好坏（用于排序）
double quickEvaluateMove(int player, const Move& move) {
    // 备份棋盘状态
    int board_backup[BOARD_SIZE][BOARD_SIZE];
    queen queens_backup[2][4];
    memcpy(board_backup, ArrowAndQueenInBoard, sizeof(board_backup));
    memcpy(queens_backup, queens, sizeof(queens_backup));
    
    // 执行走法
    int qx = queens[player][move.queen_index].x;
    int qy = queens[player][move.queen_index].y;
    int old_queen_code = ArrowAndQueenInBoard[qx][qy];
    
    ArrowAndQueenInBoard[qx][qy] = 0;
    ArrowAndQueenInBoard[move.move_x][move.move_y] = old_queen_code;
    queens[player][move.queen_index].x = move.move_x;
    queens[player][move.queen_index].y = move.move_y;
    ArrowAndQueenInBoard[move.arrow_x][move.arrow_y] = 1;
    
    // 快速评估（简化版）
    double score = 0;
    
    // 1. 移动后的皇后自由度
    int free_directions = 0;
    int dx[] = {1, 1, 0, -1, -1, -1, 0, 1};
    int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};
    for (int d = 0; d < 8; d++) {
        int nx = move.move_x + dx[d];
        int ny = move.move_y + dy[d];
        if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE) {
            if (ArrowAndQueenInBoard[nx][ny] == 0) free_directions++;
        }
    }
    score += free_directions * 0.1;
    
    // 2. 是否包围对手皇后
    int opponent = 1 - player;
    for (int q = 0; q < 4; q++) {
        if (isEnclosed(queens[opponent][q].x, queens[opponent][q].y, opponent)) {
            score += 1.0;
        }
    }
    
    // 3. 避免移动到被包围的位置
    if (isEnclosed(move.move_x, move.move_y, player)) {
        score -= 0.5;
    }
    
    // 4. 箭的位置是否好（是否阻挡对手）
    // 检查箭是否在对手可能移动的路径上
    for (int q = 0; q < 4; q++) {
        // 简单检查：如果箭在对手皇后附近，可能阻挡其移动
        int dist = max(abs(move.arrow_x - queens[opponent][q].x),
                      abs(move.arrow_y - queens[opponent][q].y));
        if (dist <= 2) {
            score += 0.3;
        }
    }
    
    // 恢复棋盘状态
    memcpy(ArrowAndQueenInBoard, board_backup, sizeof(ArrowAndQueenInBoard));
    memcpy(queens, queens_backup, sizeof(queens));
    
    return score;
}

// 生成所有可能走法
vector<Move> generateAllMoves(int player) {
    vector<Move> allMoves;
    
    for (int q = 0; q < 4; q++) {
        int qx = queens[player][q].x;
        int qy = queens[player][q].y;
        
        if (!AbleToMove(qx, qy)) continue;
        
        // 临时标记可移动位置
        DrawTheRoad(qx, qy);
        
        // 收集所有移动位置
        vector<pair<int, int>> movePositions;
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (ArrowAndQueenInBoard[i][j] == 2) {
                    movePositions.push_back({i, j});
                    ArrowAndQueenInBoard[i][j] = 0;  // 清除标记
                }
            }
        }
        
        // 对每个移动位置，生成所有射箭位置
        for (auto& movePos : movePositions) {
            int mx = movePos.first;
            int my = movePos.second;
            
            // 临时移动皇后
            int old_queen_code = ArrowAndQueenInBoard[qx][qy];
            ArrowAndQueenInBoard[qx][qy] = 0;
            ArrowAndQueenInBoard[mx][my] = old_queen_code;
            
            // 生成射箭位置
            DrawTheRoad(mx, my);
            for (int i = 0; i < BOARD_SIZE; i++) {
                for (int j = 0; j < BOARD_SIZE; j++) {
                    if (ArrowAndQueenInBoard[i][j] == 2) {
                        Move m;
                        m.queen_index = q;
                        m.move_x = mx;
                        m.move_y = my;
                        m.arrow_x = i;
                        m.arrow_y = j;
                        m.visits = 0;
                        m.wins = 0;
                        m.value = 0.5;  // 初始值
                        allMoves.push_back(m);
                        ArrowAndQueenInBoard[i][j] = 0;
                    }
                }
            }
            
            // 恢复棋盘
            ArrowAndQueenInBoard[qx][qy] = old_queen_code;
            ArrowAndQueenInBoard[mx][my] = 0;
        }
    }
    
    return allMoves;
}


//


// 简化的评估函数：只考虑皇后距离和封闭惩罚
double evaluatePosition(int player) {
    int opponent = 1 - player;
    double total_score = 0.0;
    
    // ============= 1. 皇后距离评估 =============
    // 计算双方到每个空方格的最小皇后距离
    vector<vector<int>> player_dist(BOARD_SIZE, vector<int>(BOARD_SIZE, 1000));
    vector<vector<int>> opponent_dist(BOARD_SIZE, vector<int>(BOARD_SIZE, 1000));
    
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
                
                // 检查边界
                if (nx < 0 || nx >= BOARD_SIZE || ny < 0 || ny >= BOARD_SIZE) break;
                
                // 检查是否有障碍物
                if (ArrowAndQueenInBoard[nx][ny] != 0) break;
                
                // 更新最小距离
                if (step < player_dist[nx][ny]) {
                    player_dist[nx][ny] = step;
                }
            }
        }
    }
    
    // 计算对手皇后的可达距离
    for (int q = 0; q < 4; q++) {
        int qx = queens[opponent][q].x;
        int qy = queens[opponent][q].y;
        
        int dx[] = {1, 1, 0, -1, -1, -1, 0, 1};
        int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};
        
        for (int d = 0; d < 8; d++) {
            for (int step = 1; step < BOARD_SIZE; step++) {
                int nx = qx + dx[d] * step;
                int ny = qy + dy[d] * step;
                
                if (nx < 0 || nx >= BOARD_SIZE || ny < 0 || ny >= BOARD_SIZE) break;
                if (ArrowAndQueenInBoard[nx][ny] != 0) break;
                
                if (step < opponent_dist[nx][ny]) {
                    opponent_dist[nx][ny] = step;
                }
            }
        }
    }
    
    // 统计游戏阶段（箭的数量）
    int arrow_count = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (ArrowAndQueenInBoard[i][j] == 1) arrow_count++;
        }
    }
    double game_phase = arrow_count / 64.0; // 0: 刚开始, 1: 结束
    
    // 根据皇后距离计算领土分数
    double territory_score = 0.0;
    int empty_squares_count = 0;
    
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (ArrowAndQueenInBoard[x][y] == 0) { // 空方格
                empty_squares_count++;
                int p_dist = player_dist[x][y];
                int o_dist = opponent_dist[x][y];
                
                // 双方都不可达，跳过
                if (p_dist == 1000 && o_dist == 1000) continue;
                
                // 根据文献的Δ函数计算
                if (p_dist < o_dist) {
                    // 玩家更接近
                    // 距离越近，优势越大（非线性）
                    double advantage = 1.0;
                    if (p_dist > 0) {
                        advantage = 1.0 / (p_dist * p_dist); // 距离平方的倒数
                    }
                    territory_score += advantage;
                } else if (p_dist > o_dist) {
                    // 对手更接近
                    double disadvantage = -1.0;
                    if (o_dist > 0) {
                        disadvantage = -1.0 / (o_dist * o_dist);
                    }
                    territory_score += disadvantage;
                } else if (p_dist == o_dist && p_dist < 1000) {
                    // 距离相等，给予小的优势（文献中的κ）
                    double kappa = 0.1;
                    if (player == role) { // 轮到玩家走
                        territory_score += kappa;
                    } else {
                        territory_score -= kappa;
                    }
                }
            }
        }
    }
    
    // 归一化领土分数
    if (empty_squares_count > 0) {
        territory_score = territory_score / empty_squares_count * 64.0;
    }
    
    // 根据游戏阶段调整领土分数权重
    // 早期游戏：领土评估权重较低；后期游戏：领土评估权重较高
    double territory_weight = 0.5 + game_phase * 0.5; // 0.5到1.0之间
    total_score += territory_score * territory_weight;
    
    // ============= 2. 封闭惩罚评估 =============
    // 使用优化后的isEnclosed函数
    double enclosure_score = 0.0;
    
    // 检查玩家的皇后封闭情况
    for (int q = 0; q < 4; q++) {
        int qx = queens[player][q].x;
        int qy = queens[player][q].y;
        
        if (isEnclosed(qx, qy, player)) {
            // 皇后被封闭，给予惩罚
            // 根据游戏阶段调整惩罚强度
            double penalty = -3.0;
            if (game_phase < 0.3) {
                penalty = -5.0; // 早期游戏被封闭惩罚更重
            }
            enclosure_score += penalty;
            
            // 额外：如果皇后自由度特别低，额外惩罚
            int freedom = calculateFreedom(qx, qy, player);
            if (freedom <= 1) {
                enclosure_score -= 2.0; // 额外惩罚
            }
        } else {
            // 皇后未被封闭，给予奖励（奖励较小）
            enclosure_score += 0.5;
        }
    }
    
    // 检查对手的皇后封闭情况（对我们是好事）
    for (int q = 0; q < 4; q++) {
        int qx = queens[opponent][q].x;
        int qy = queens[opponent][q].y;
        
        if (isEnclosed(qx, qy, opponent)) {
            // 对手皇后被封闭，给予奖励
            double reward = 3.0;
            if (game_phase < 0.3) {
                reward = 5.0; // 早期游戏对手被封闭奖励更高
            }
            enclosure_score += reward;
            
            // 额外：如果对手皇后自由度特别低，额外奖励
            int freedom = calculateFreedom(qx, qy, opponent);
            if (freedom <= 1) {
                enclosure_score += 2.0; // 额外奖励
            }
        } else {
            // 对手皇后未被封闭，轻微惩罚
            enclosure_score -= 0.5;
        }
    }
    
    // 根据游戏阶段调整封闭评估权重
    // 早期游戏：封闭评估权重较高；后期游戏：封闭评估权重较低
    double enclosure_weight = 1.0 - game_phase * 0.5; // 1.0到0.5之间
    total_score += enclosure_score * enclosure_weight;
    
    return total_score;
}

// 辅助函数：计算皇后的自由度（相邻8个方向有多少个可移动）
int calculateFreedom(int queen_x, int queen_y, int player) {
    int freedom = 0;
    int dx[] = {1, 1, 0, -1, -1, -1, 0, 1};
    int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};
    
    for (int d = 0; d < 8; d++) {
        int nx = queen_x + dx[d];
        int ny = queen_y + dy[d];
        
        // 检查相邻格子是否为空（可以移动一步）
        if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE) {
            if (ArrowAndQueenInBoard[nx][ny] == 0) {
                freedom++;
            }
        }
    }
    
    return freedom;
}

// 更准确的皇后封闭性判定函数
bool isEnclosed(int queen_x, int queen_y, int player) {
    // 皇后的移动是8个方向，我们需要检查每个方向是否被阻挡
    
    // 八个方向：水平、垂直、对角线
    int dx[] = {1, 1, 0, -1, -1, -1, 0, 1};
    int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};
    
    int free_directions = 0; // 可以移动的方向数
    vector<pair<int, int>> accessible_squares; // 可达的空格
    
    // 检查每个方向
    for (int d = 0; d < 8; d++) {
        bool direction_blocked = false;
        
        // 沿着这个方向检查
        for (int step = 1; step < BOARD_SIZE; step++) {
            int nx = queen_x + dx[d] * step;
            int ny = queen_y + dy[d] * step;
            
            // 检查边界
            if (nx < 0 || nx >= BOARD_SIZE || ny < 0 || ny >= BOARD_SIZE) {
                direction_blocked = true;
                break;
            }
            
            // 检查是否有障碍物（箭或其他皇后）
            if (ArrowAndQueenInBoard[nx][ny] != 0) {
                direction_blocked = true;
                break;
            }
            
            // 如果没有障碍物，这个方向是通的
            // 记录这个可达的空格
            accessible_squares.push_back({nx, ny});
            
            // 皇后至少可以移动到这个位置，所以这个方向是自由的
            // 但我们还需要检查继续前进是否可能
            if (step == 1) {
                free_directions++; // 至少可以移动一步
            }
        }
    }
    
    // 情况1：完全封闭 - 无法向任何方向移动
    if (free_directions == 0) {
        return true;
    }
    
    // 情况2：严重封闭 - 可移动方向很少，且移动范围有限
    // 计算可达空格的数量
    int reachable_count = accessible_squares.size();
    
    // 检查可达空格是否都被对手控制
    int opponent = 1 - player;
    int opponent_controlled_squares = 0;
    
    // 对于每个可达空格，检查对手是否能更快到达
    for (auto& square : accessible_squares) {
        int x = square.first;
        int y = square.second;
        
        // 计算玩家皇后到这个位置的距离
        int player_distance = max(abs(x - queen_x), abs(y - queen_y));
        
        // 计算对手最近皇后到这个位置的距离
        int opponent_min_distance = 1000;
        for (int q = 0; q < 4; q++) {
            int ox = queens[opponent][q].x;
            int oy = queens[opponent][q].y;
            
            // 检查对手皇后到这个位置的路径是否畅通
            int opponent_distance = calculateQueenDistanceWithObstacles(ox, oy, x, y);
            if (opponent_distance < opponent_min_distance) {
                opponent_min_distance = opponent_distance;
            }
        }
        
        // 如果对手距离更近或相等（且轮到对手走），则这个位置被对手控制
        if (opponent_min_distance < player_distance || 
            (opponent_min_distance == player_distance && opponent == role)) {
            opponent_controlled_squares++;
        }
    }
    
    // 如果大多数可达空格都被对手控制，那么这个皇后是被封闭的
    if (reachable_count > 0 && (double)opponent_controlled_squares / reachable_count > 0.7) {
        return true;
    }
    
    // 情况3：检查皇后是否被困在小区域内
    // 我们可以通过检查皇后的移动范围来判断
    int max_move_distance = 0;
    for (auto& square : accessible_squares) {
        int distance = max(abs(square.first - queen_x), abs(square.second - queen_y));
        if (distance > max_move_distance) {
            max_move_distance = distance;
        }
    }
    
    // 如果皇后最多只能移动很短的距离，认为是被封闭的
    if (max_move_distance <= 2) {
        return true;
    }
    
    // 情况4：检查皇后周围是否被对手皇后包围
    int nearby_opponent_queens = 0;
    for (int q = 0; q < 4; q++) {
        int ox = queens[opponent][q].x;
        int oy = queens[opponent][q].y;
        
        // 计算与对手皇后的距离
        int distance = max(abs(ox - queen_x), abs(oy - queen_y));
        if (distance <= 3) {
            nearby_opponent_queens++;
        }
    }
    
    // 如果被多个对手皇后近距离包围，认为是被封闭的
    if (nearby_opponent_queens >= 2 && free_directions <= 3) {
        return true;
    }
    
    return false;
}

// 辅助函数：计算两点间的皇后距离（考虑障碍物）
int calculateQueenDistanceWithObstacles(int x1, int y1, int x2, int y2) {
    // 如果两点在同一条直线或对角线上
    if (x1 == x2 || y1 == y2 || abs(x1 - x2) == abs(y1 - y2)) {
        int dx = 0, dy = 0;
        if (x2 > x1) dx = 1;
        else if (x2 < x1) dx = -1;
        
        if (y2 > y1) dy = 1;
        else if (y2 < y1) dy = -1;
        
        // 检查路径上是否有障碍物
        int current_x = x1 + dx;
        int current_y = y1 + dy;
        
        while (current_x != x2 || current_y != y2) {
            if (ArrowAndQueenInBoard[current_x][current_y] != 0) {
                return 1000; // 有障碍物，不可达
            }
            current_x += dx;
            current_y += dy;
        }
        
        // 计算切比雪夫距离
        return max(abs(x2 - x1), abs(y2 - y1));
    }
    
    return 1000; // 不在同一直线上，不可达
}






//

// 随机模拟游戏
int simulateRandomGame(int player) {
    // 深度限制的随机模拟
    int current_player = player;
    int depth = 0;
    
    while (depth < SIMULATION_DEPTH) {
        // 生成当前玩家的所有可能走法
        auto moves = generateAllMoves(current_player);
        if (moves.empty()) {
            // 当前玩家无法移动，对手获胜
            return (current_player == player) ? 0 : 1;
        }
        
        // 随机选择一个走法
        int random_index = rand() % moves.size();
        auto move = moves[random_index];
        
        // 执行移动（临时）
        int qx = queens[current_player][move.queen_index].x;
        int qy = queens[current_player][move.queen_index].y;
        int old_queen_code = ArrowAndQueenInBoard[qx][qy];
        
        ArrowAndQueenInBoard[qx][qy] = 0;
        ArrowAndQueenInBoard[move.move_x][move.move_y] = old_queen_code;
        queens[current_player][move.queen_index].x = move.move_x;
        queens[current_player][move.queen_index].y = move.move_y;
        ArrowAndQueenInBoard[move.arrow_x][move.arrow_y] = 1;
        
        // 切换玩家
        current_player = 1 - current_player;
        depth++;
        
        // 检查游戏是否结束
        bool can_move = false;
        for (int q = 0; q < 4; q++) {
            if (AbleToMove(queens[current_player][q].x, queens[current_player][q].y)) {
                can_move = true;
                break;
            }
        }
        if (!can_move) {
            // 当前玩家无法移动，之前的玩家获胜
            int winner = 1 - current_player;
            // 恢复棋盘（因为这只是模拟）
            // 注意：这里我们需要恢复棋盘状态，但比较复杂
            // 由于这是深度限制的模拟，我们直接返回评估结果
            return (winner == player) ? 1 : 0;
        }
    }
    
    // 达到深度限制，使用评估函数决定胜负
    double eval = evaluatePosition(player);
    return (eval > 0) ? 1 : 0;
}

// UCT算法选择最佳走法
Move selectBestMoveUCT() {
    vector<Move> allMoves = generateAllMoves(role);
    if (allMoves.empty()) {
        return Move();  // 返回空走法
    }
    
    // 如果走法很少，直接返回第一个
    if (allMoves.size() == 1) {
        return allMoves[0];
    }
    
    // 进行蒙特卡洛模拟
    for (int i = 0; i < MAX_SIMULATIONS; i++) {
        // 复制当前棋盘状态
        int board_backup[BOARD_SIZE][BOARD_SIZE];
        queen queens_backup[2][4];
        
        memcpy(board_backup, ArrowAndQueenInBoard, sizeof(board_backup));
        memcpy(queens_backup, queens, sizeof(queens_backup));
        
        // 选择一个走法进行测试
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
        
        // 模拟随机游戏
        int result = simulateRandomGame(role);
        
        // 更新统计数据
        move.visits++;
        move.wins += result;
        
        // 恢复棋盘状态
        memcpy(ArrowAndQueenInBoard, board_backup, sizeof(ArrowAndQueenInBoard));
        memcpy(queens, queens_backup, sizeof(queens));
    }
    
    // 选择最佳走法（UCT公式）
    Move best_move;
    double best_score = -1e9;
    
    for (auto& move : allMoves) {
        if (move.visits > 0) {
            // UCT公式：win_rate + C * sqrt(log(total_visits) / visits)
            double win_rate = (double)move.wins / move.visits;
            double exploration = UCT_C * sqrt(log(MAX_SIMULATIONS) / move.visits);
            double score = win_rate + exploration;
            
            // 加上评估函数的启发值
            double heuristic_value = 0;
            
            // 1. 检查这个走法是否包围对手皇后
            int opponent = 1 - role;
            for (int q = 0; q < 4; q++) {
                if (isEnclosed(queens[opponent][q].x, queens[opponent][q].y, opponent)) {
                    heuristic_value += 0.1;
                }
            }
            
            // 2. 检查移动后皇后的自由度
            int new_x = move.move_x;
            int new_y = move.move_y;
            int free_directions = 0;
            int dx[] = {1, 1, 0, -1, -1, -1, 0, 1};
            int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};
            for (int d = 0; d < 8; d++) {
                int nx = new_x + dx[d];
                int ny = new_y + dy[d];
                if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE) {
                    if (ArrowAndQueenInBoard[nx][ny] == 0) free_directions++;
                }
            }
            heuristic_value += free_directions * 0.01;
            
            score += heuristic_value;
            
            if (score > best_score) {
                best_score = score;
                best_move = move;
            }
        }
    }
    
    return best_move;
}

// 改进的AI大脑
int AmazonlyBrain() {
    // 计算当前游戏步数
    ifstream file("GAME.txt");
    string line;
    int move_count = 0;
    while (getline(file, line)) {
        istringstream iss(line);
        int lineNums[6];
        for (int i = 0; i < 6; i++) {
            iss >> lineNums[i];
        }
        // 只计算有效的移动（跳过-1开头的行）
        if (lineNums[0] != -1) {
            move_count++;
        }
    }
    file.close();
    
    // 如果是前INITIALSTEPS回合，使用alpha-beta
    if (move_count < INITIALSTEPS) {
        return AmazonlyBrain_MultiLevel(2);  // 调用alpha-beta函数
    }
    
    // 检查是否有合法走法
    bool has_legal_move = false;
    for (int q = 0; q < 4; q++) {
        if (AbleToMove(queens[role][q].x, queens[role][q].y)) {
            has_legal_move = true;
            break;
        }
    }
    
    if (!has_legal_move) {
        return 100 + role;  // 没有合法走法，输了
    }
    
    // 使用UCT算法选择最佳走法
    Move best_move = selectBestMoveUCT();
    
    // 执行最佳走法
    int qx = queens[role][best_move.queen_index].x;
    int qy = queens[role][best_move.queen_index].y;
    
    ofstream outFile("GAME.txt", std::ios::app);
    outFile << qx << " " << qy << " "
            << best_move.move_x << " " << best_move.move_y << " "
            << best_move.arrow_x << " " << best_move.arrow_y << endl;
    outFile.close();
    
    // 更新棋盘状态
    int queen_code = ArrowAndQueenInBoard[qx][qy];
    ArrowAndQueenInBoard[qx][qy] = 0;
    ArrowAndQueenInBoard[best_move.move_x][best_move.move_y] = queen_code;
    queens[role][best_move.queen_index].x = best_move.move_x;
    queens[role][best_move.queen_index].y = best_move.move_y;
    ArrowAndQueenInBoard[best_move.arrow_x][best_move.arrow_y] = 1;
    
    return 0;
}

// 设置箭的位置，废弃案
void setarrow(int xarrow, int yarrow) {
    DrawTheRoad(xarrow, yarrow);
    int flag = 1;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (ArrowAndQueenInBoard[i][j] == 2) {
                if (flag) {
                    flag = 0;
                    ArrowAndQueenInBoard[i][j] = 1;
                    ofstream outfile("GAME.txt", std::ios::app);
                    outfile << i << " " << j << endl;
                    outfile.close();
                } else {
                    ArrowAndQueenInBoard[i][j] = 0;
                }
            }
        }
    }
}

// 临时检查函数，在控制台打印棋盘，废弃案
void temporarycheck() {
    cout << endl;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            cout << setw(4) << ArrowAndQueenInBoard[j][i] << " ";
        }
        cout << endl;
    }
    cout << endl;
}