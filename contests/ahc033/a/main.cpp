#include "../../../library/library/template/template.cpp"

const int max_turn = 10000;
int N;
mat<int> A;

struct Pos {
    int x, y;

    Pos() : x(-1), y(-1) {
    }

    Pos(int x, int y) : x(x), y(y) {
    }

    bool is_null() {
        return x == -1 && y == -1;
    }

    void set_null() {
        x = -1, y = -1;
    }

    bool operator==(const Pos &p) const {
        return x == p.x && y == p.y;
    }

    bool operator!=(const Pos &p) const {
        return x != p.x || y != p.y;
    }

    friend ostream &operator<<(ostream &os, const Pos p) {
        return os << p.x << " " << p.y;
    }
};

bool is_move(char c) {
    return c == 'L' || c == 'R' || c == 'U' || c == 'D';
}

bool in_field(Pos pos) {
    return 0 <= pos.x && pos.x < N && 0 <= pos.y && pos.y < N;
}

char get_move_to_target(Pos here, Pos target) {
    if (here.x < target.x) return 'R';
    if (here.x > target.x) return 'L';
    if (here.y < target.y) return 'D';
    if (here.y > target.y) return 'U';
    assert(false);
}

Pos get_next_pos(const Pos &here, char move) {
    if (move == 'L') return Pos(here.x - 1, here.y);
    if (move == 'R') return Pos(here.x + 1, here.y);
    if (move == 'U') return Pos(here.x, here.y - 1);
    if (move == 'D') return Pos(here.x, here.y + 1);
    assert(false);
}

char get_avoid_action(const Pos &here, const vector<Pos> &obstacles, char undesirable_move) {
    string moves = "LRUD";
    moves.erase(remove(all(moves), undesirable_move), moves.end());
    moves += undesirable_move;
    for (char action : moves) {
        Pos next_pos = get_next_pos(here, action);

        // 移動可能か判定
        bool is_movable = in_field(next_pos);
        for (const Pos &obstacle : obstacles) {
            if (next_pos == obstacle) {
                is_movable = false;
            }
        }

        if (is_movable) {
            return action;
        }
    }
    return 'B';
}

ll dist(Pos a, Pos b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

struct Crane {
    int id;
    Pos pos;
    int holding_container_id = -1;
    bool alive = true;
    bool succeed = false;
    Pos start, goal;

    Crane(int id, int x, int y) : id(id), pos(x, y) {
    }

    bool is_holding() {
        return holding_container_id != -1;
    }

    void holds(int container_id) {
        holding_container_id = container_id;
    }

    void release() {
        holding_container_id = -1;
    }

    bool is_at_goal() {
        return pos == goal;
    }

    bool is_at_start() {
        return pos == start;
    }

    bool is_finished() {
        return goal.is_null();
    }

    bool is_alive() {
        return alive;
    }

    void destroy() {
        alive = false;
    }

    void set_path(Pos start, Pos goal) {
        this->start = start;
        this->goal = goal;
    }

    void clear_path() {
        start.set_null();
        goal.set_null();
    }
};

struct Container {
    int id;
    Pos pos;
    int assigned_crane_id;

    Container(int id, int x, int y) : id(id), pos(x, y), assigned_crane_id(-1) {
    }

    Container() : id(-1) {
    }

    void move(const Pos &next_pos) {
        pos = next_pos;
    }

    bool is_loaded() {
        return pos.x != -1;
    }

    bool is_dispatched() {
        return pos.x == N;
    }

    bool is_assigned() {
        return assigned_crane_id != -1;
    }

    void assign_to(int crane_id) {
        assigned_crane_id = crane_id;
    }

    void unassign() {
        assigned_crane_id = -1;
    }
};

struct Terminal {
    int turn = 0;
    mat<int> grid;
    mat<int> dispatched_containers;
    mat<int> waiting_containers;
    vector<Crane> cranes;
    vector<Container> containers;
    vector<string> S;

    Terminal() : grid(N, vi(N, -1)), dispatched_containers(N), waiting_containers(N), S(N) {
        // 積み込み前のコンテナ（列ごとに出る順番の降順）
        containers.resize(N * N);
        rep(y, N) rrep(x, N) {
            waiting_containers[y].pb(A[y][x]);
            containers[A[y][x]] = Container(A[y][x], -1, y);
        }

        // クレーンの初期位置
        rep(i, N) {
            cranes.pb(Crane(i, 0, i));
        }

        // グリッドの左端にコンテナを積む
        rep(i, N) {
            grid[i][0] = waiting_containers[i].back();
            waiting_containers[i].pop_back();
            containers[grid[i][0]].move(Pos(0, i));
        }
    }

    int get_cell(const Pos &pos) {
        return grid[pos.y][pos.x];
    }

    void set_cell(const Pos &pos, int value) {
        grid[pos.y][pos.x] = value;
    }

    void step(string actions) {
        turn++;
        for (auto &crane : cranes) {
            char action = actions[crane.id];
            S[crane.id] += action;
            debug_S();

            // コンテナを下ろす場合パスをクリアして割り当てを解除
            if (action == 'Q') {
                crane.clear_path();
                containers[crane.holding_container_id].unassign();
            }

            if (action == 'P') {
                pick(crane);
            } else if (action == 'Q') {
                release(crane);
            } else if (action == '.') {
                continue;
            } else if (action == 'B') {
                destroy(crane);
            } else {
                Pos next_pos = get_next_pos(crane.pos, action);
                move(crane, next_pos);
            }
        }
    }

    void move(Crane &crane, Pos &next_pos) {
        assert(crane.is_alive());

        int y = crane.pos.y;
        crane.pos = next_pos;

        // コンテナを掴んでいる場合コンテナも移動
        if (crane.is_holding()) {
            assert(crane.id == 0 || is_empty(next_pos));

            Container &container = containers[crane.holding_container_id];
            container.move(next_pos);

            // 入り口が空いたら待っているコンテナを入れる
            Pos entrance = Pos(0, y);
            if (is_empty(entrance) && !waiting_containers[y].empty()) {
                grid[y][0] = waiting_containers[y].back();
                containers[grid[y][0]].move(Pos(0, y));
                waiting_containers[y].pop_back();
            }
        }
    }

    void pick(Crane &crane) {
        assert(crane.is_alive());
        assert(!crane.is_holding());
        assert(get_cell(crane.pos) != -1);

        crane.holds(get_cell(crane.pos));
        set_cell(crane.pos, -1);
    }

    void destroy(Crane &crane) {
        assert(crane.is_alive());
        assert(!crane.is_holding());

        crane.destroy();
    }

    void release(Crane &crane) {
        assert(crane.is_alive());
        assert(crane.is_holding());
        assert(is_empty(crane.pos));

        set_cell(crane.pos, crane.holding_container_id);
        crane.release();

        // 出口なら出す
        if (crane.pos.x == N - 1) {
            dispatched_containers[crane.pos.y].pb(get_cell(crane.pos));
            containers[get_cell(crane.pos)].move(Pos(N, crane.pos.y));
            set_cell(crane.pos, -1);
        }
    }

    int calc_score() {
        int M0 = turn;
        int M1 = 0;
        int M2 = 0;
        int M3 = N * N;
        rep(i, N) {
            int num_containers = dispatched_containers[i].size();
            M3 -= num_containers;
            rep(j, num_containers) {
                if (dispatched_containers[i][j] < N * i || N * (i + 1) <= dispatched_containers[i][j]) {
                    M2++;
                } else {
                    rep(k, j + 1, num_containers) {
                        if (dispatched_containers[i][k] < N * i || N * (i + 1) <= dispatched_containers[i][k]) continue;
                        if (dispatched_containers[i][j] > dispatched_containers[i][k]) {
                            M1++;
                        }
                    }
                }
            }
        }
        return M0 + M1 * 1e2 + M2 * 1e4 + M3 * 1e6;
    }

    bool is_empty(Pos pos) {
        return get_cell(pos) < 0;
    }

    bool is_goal_settable(Pos pos) {
        if (pos.x == N - 1) return true;
        if (pos.x == 0 && !waiting_containers[pos.y].empty()) return false;
        return get_cell(pos) == -1;
    }

    bool is_clear() {
        rep(i, N) {
            if (dispatched_containers[i].size() != N) return false;
        }
        return true;
    }

    bool is_timeout() {
        return turn >= max_turn;
    }

    void debug_S() {
        rep(i, N) {
            cerr << S[i] << endl;
        }
        cerr << string(20, '-') << endl;
    }
};

char get_next_action(Terminal &terminal, Crane &crane) {
    // すでに破壊されたかパスが指定されていなければ待機
    if (!crane.is_alive() || crane.is_finished()) return '.';

    if (crane.is_holding()) {
        if (crane.is_at_goal()) {
            // コンテナを持っている かつ 目的地にいるならコンテナを離す
            return 'Q';
        } else {
            // ゴールに向かって移動
            return get_move_to_target(crane.pos, crane.goal);
        }
    } else {
        if (crane.is_at_start()) {
            // コンテナを持っていない かつ コンテナの場所にいるなら持つ
            return 'P';
        } else {
            // スタートに向かって移動
            return get_move_to_target(crane.pos, crane.start);
        }
    }
}

// 運び出し状況(state)においてターミナルが満杯か
bool is_full(int state, Terminal &terminal) {
    rep(y, N) {
        int num_dispatched_containers = (state / mypow<int>(N + 1, y)) % (N + 1);
        rep(container_id, N * y, N * y + num_dispatched_containers) {
            if (terminal.containers[container_id].pos.x != 0) return false;
        }
    }
    return true;
}

// 最適なコンテナの取り出し順序
vi get_best_container_que(Terminal &terminal) {
    int num_states = mypow<int>(N + 1, N);
    mat<int> dp(num_states, vi(N, inf));
    mat<tuple<int, int, int>> prev(num_states, vector<tuple<int, int, int>>(N));
    dp[0][terminal.cranes[0].pos.y] = 0;
    rep(state, num_states) {
        rep(crane_y, N) {
            rep(next_crane_y, N) {
                int num_dispatched_containers = (state / mypow<int>(N + 1, next_crane_y)) % (N + 1);
                if (num_dispatched_containers == N) continue;
                int next_container_id = N * next_crane_y + num_dispatched_containers;
                Container &container = terminal.containers[next_container_id];
                if (is_full(state, terminal) && !container.is_loaded()) continue;
                int next_state = state + mypow<int>(N + 1, next_crane_y);
                int cost = abs(crane_y - container.pos.y);
                if (chmin(dp[next_state][next_crane_y], dp[state][crane_y] + cost)) {
                    prev[next_state][next_crane_y] = {state, crane_y, next_container_id};
                }
            }
        }
    }

    vi que;
    int state = num_states - 1;
    int crane_y = terminal.cranes[0].pos.y;
    while (state > 0) {
        auto [prev_state, prev_crane_y, container_id] = prev[state][crane_y];
        que.pb(container_id);
        state = prev_state;
        crane_y = prev_crane_y;
    }
    return que;
}

bool is_next(Terminal &terminal, int container_id) {
    return terminal.dispatched_containers[container_id / N].size() == container_id % N;
}

Pos get_exit(int container_id) {
    return Pos(N - 1, container_id / N);
}

string get_approaching_actions(Terminal &terminal, Pos here, Pos exit) {
    string actions = "";
    if (here.x < exit.x) actions += 'R';
    if (here.x > exit.x) actions += 'L';
    if (here.y < exit.y) actions += 'D';
    if (here.y > exit.y) actions += 'U';
    return actions;
}

bool is_occupied(Terminal &terminal, Pos pos) {
    for (Crane &crane : terminal.cranes) {
        if (crane.pos == pos) return true;
    }
}

// 次のパスを設定する
void set_next_target(Crane &crane, Terminal &terminal, vi &que) {
    // 空いているセルを列挙
    vector<Pos> empty_positions;
    rep(gx, 0, N - 1) {
        rep(gy, N) {
            if (terminal.is_goal_settable(Pos(gx, gy))) {
                empty_positions.pb(Pos(gx, gy));
            }
        }
    }

    if (crane.id == 0) {
        // クレーン0の場合
        while (!que.empty() && terminal.containers[que.back()].is_dispatched())
            que.pop_back();
        if (que.empty()) return;
        int next_container_id = que.back();
        Container &next_container = terminal.containers[next_container_id];

        if (!next_container.is_loaded()) {
            // ターミナルに置かれていなければ邪魔なコンテナを移動させる
            Pos start = Pos(0, next_container.pos.y);
            int start_container_id = terminal.get_cell(start);

            // すでに割り当てられていないなら一番近い空きますに移動
            if (start_container_id != -1 && !terminal.containers[start_container_id].is_assigned()) {

                sort(all(empty_positions), [&](Pos a, Pos b) {
                    ll cost_a = (abs(a.x - start.x) + abs(a.y - start.y)) * N - abs(a.x);
                    ll cost_b = (abs(b.x - start.x) + abs(b.y - start.y)) * N - abs(b.x);
                    return cost_a < cost_b;
                });
                Pos goal = empty_positions[0];

                crane.set_path(start, goal);
            }
        } else {
            if (!next_container.is_assigned()) {
                // すでに割り当てられていないなら次のコンテナを運び出す
                Pos start = next_container.pos;
                Pos goal = Pos(N - 1, next_container_id / N);

                que.pop_back();
                crane.set_path(start, goal);
            }
        }
    } else if (empty_positions.size() >= 2) { // クレーン0以外 かつ 空きマスが二つ以上
        // 一つ右にずらせるコンテナを列挙
        vector<pair<Pos, Pos>> path_candidates;
        for (Container &container : terminal.containers) {
            // コンテナがターミナルにない または すでに割り当て済み ならスキップ
            if (!container.is_loaded() || container.is_dispatched() || container.is_assigned()) continue;

            string moves = get_approaching_actions(terminal, container.pos, get_exit(container.id));
            for (char move : moves) {
                Pos next_pos = get_next_pos(container.pos, move);
                if (next_pos.x == N - 1 && !is_next(terminal, container.id)) continue;
                if (terminal.is_goal_settable(next_pos)) { // 右隣が空いている場合
                    path_candidates.emplace_back(container.pos, next_pos);
                }
            }
        }

        if (!path_candidates.empty()) {
            // 移動可能コンテナがあれば移動させる
            sort(all(path_candidates), [&](pair<Pos, Pos> a, pair<Pos, Pos> b) {
                return dist(a.first, crane.pos) < dist(b.first, crane.pos);
            });
            auto [start, goal] = path_candidates[0];
            crane.set_path(start, goal);
        }
    }

    if (!crane.is_finished()) { // クレーンにパスがセットされた場合
        // 割り当てを更新
        terminal.containers[terminal.get_cell(crane.start)].assign_to(crane.id);
        // 目的地にマーク
        terminal.set_cell(crane.goal, -2);
    }
}

void succeed(Crane &crane, Terminal &terminal) {
    string actions = get_approaching_actions(terminal, crane.pos, get_exit(crane.holding_container_id));
    for (char action : actions) {
        Pos next_pos = get_next_pos(crane.pos, action);
        if (next_pos.x == N - 1 && !is_next(terminal, crane.holding_container_id)) continue;
        if (in_field(next_pos) && terminal.is_goal_settable(next_pos)) {
            terminal.set_cell(crane.goal, -1);
            crane.goal = next_pos;
            terminal.set_cell(crane.goal, -2);
        }
    }
}

int main(int argc, char *argv[]) {
    cin.tie(0);
    ios::sync_with_stdio(0);
    cout << setprecision(30) << fixed;
    cerr << setprecision(30) << fixed;

    scan(N);
    A = mat<int>(N, vi(N));
    scan(A);

    Terminal terminal;

    // とりあえず20個のコンテナをターミナルに出す
    rep(i, N - 2) {
        for (auto &crane : terminal.cranes) {
            if (crane.is_finished()) {
                crane.set_path(Pos(0, crane.pos.y), Pos(N - i - 2, crane.pos.y));
            }
        }
        while (!terminal.cranes[0].is_finished()) {
            string actions = string(N, '.');
            for (Crane &crane : terminal.cranes) {
                actions[crane.id] = get_next_action(terminal, crane);
            }
            terminal.step(actions);
        }
    }

    // クレーン0, 1以外を破壊
    string destroy_actions = string(N, '.');
    rep(i, 2, N) {
        destroy_actions[i] = 'B';
    }
    terminal.step(destroy_actions);

    // クレーン0, 1で運び出していく
    terminal.cranes[1].succeed = true;
    vi que = get_best_container_que(terminal);
    cerr << que << endl;

    while (!terminal.is_clear() && !terminal.is_timeout()) {
        for (Crane &crane : terminal.cranes) {
            if (!crane.is_alive()) continue;
            if (crane.is_finished()) {
                set_next_target(crane, terminal, que);
            } else if (crane.is_holding() && crane.is_at_goal() && crane.succeed) {
                succeed(crane, terminal);
            }
        }

        string actions = string(N, '.');
        for (Crane &crane : terminal.cranes) {
            actions[crane.id] = get_next_action(terminal, crane);
        }

        // 衝突回避
        Crane &crane0 = terminal.cranes[0];
        Crane &crane1 = terminal.cranes[1];
        char action0 = actions[0];
        char action1 = actions[1];
        if (is_move(action1)) {
            Pos next_pos1 = get_next_pos(crane1.pos, action1);
            if (next_pos1 == crane0.pos) {
                actions[0] = get_avoid_action(crane0.pos, {crane1.pos}, action1);
            }
            if (is_move(action0)) {
                Pos next_pos0 = get_next_pos(crane0.pos, action0);
                if (next_pos0 == next_pos1) {
                    actions[0] = '.';
                }
            }
        } else if (is_move(action0)) {
            Pos next_pos0 = get_next_pos(crane0.pos, action0);
            if (next_pos0 == crane1.pos) {
                if (actions[1] == '.') {
                    actions[1] = get_avoid_action(crane1.pos, {crane0.pos}, action0);
                } else {
                    actions[0] = '.';
                }
            }
        }
        terminal.step(actions);
    }

    rep(i, N) {
        cout << terminal.S[i] << endl;
    }

    cerr << "Score = " << terminal.calc_score() << endl;
}