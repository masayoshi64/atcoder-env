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

ll dist(Pos a, Pos b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

struct Crane {
    int id;
    Pos pos;
    int holding_container = -1;
    bool is_alive = true;
    Pos start, goal;

    Crane(int id, int x, int y) : id(id), pos(x, y) {
    }

    bool is_holding() {
        return holding_container != -1;
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
    int crane_id;

    Container(int id, int x, int y) : id(id), pos(x, y), crane_id(-1) {
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

Pos get_next_pos(Pos here, char move) {
    if (move == 'L') return Pos(here.x - 1, here.y);
    if (move == 'R') return Pos(here.x + 1, here.y);
    if (move == 'U') return Pos(here.x, here.y - 1);
    if (move == 'D') return Pos(here.x, here.y + 1);
    assert(false);
}

char get_next_action(Crane &crane) {
    if (!crane.is_alive || crane.is_finished()) return '.';

    if (crane.is_holding()) {
        if (crane.is_at_goal()) {
            return 'Q';
        } else {
            return get_move_to_target(crane.pos, crane.goal);
        }
    } else {
        if (crane.is_at_start()) {
            return 'P';
        } else {
            return get_move_to_target(crane.pos, crane.start);
        }
    }
}

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

    void step(map<int, char> actions = {}) {
        turn++;
        for (auto &crane : cranes) {
            char action = actions.count(crane.id) ? actions[crane.id] : get_next_action(crane);
            S[crane.id] += action;
            if (action == 'Q') {
                crane.clear_path();
                containers[crane.holding_container].crane_id = -1;
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
        assert(crane.is_alive);
        int y = crane.pos.y;
        cerr << "move " << crane.pos << " to " << next_pos << endl;
        crane.pos = next_pos;
        if (crane.is_holding()) {
            containers[crane.holding_container].move(next_pos);
            Pos entrance = Pos(0, y);
            if (get_cell(entrance) == -1 && !waiting_containers[y].empty()) {
                grid[y][0] = waiting_containers[y].back();
                containers[grid[y][0]].move(Pos(0, y));
                waiting_containers[y].pop_back();
            }
        }
    }

    void pick(Crane &crane) {
        assert(crane.is_alive);
        assert(!crane.is_holding());
        assert(get_cell(crane.pos) != -1);

        crane.holding_container = get_cell(crane.pos);
        set_cell(crane.pos, -1);
    }

    void destroy(Crane &crane) {
        assert(crane.is_alive);
        assert(!crane.is_holding());

        crane.is_alive = false;
    }

    void release(Crane &crane) {
        assert(crane.is_alive);
        assert(crane.is_holding());
        assert(is_empty(crane.pos));

        set_cell(crane.pos, crane.holding_container);
        crane.holding_container = -1;
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

    bool is_clear() {
        rep(i, N) {
            if (dispatched_containers[i].size() != N) return false;
        }
        return true;
    }

    bool is_timeout() {
        return turn >= max_turn;
    }

    void print_S() {
        rep(i, N) {
            cout << S[i] << endl;
        }
    }
};

bool is_full(int state, Terminal &terminal) {
    rep(y, N) {
        int num_dispatched_containers = (state / mypow<int>(N + 1, y)) % (N + 1);
        rep(container_id, N * y, N * y + num_dispatched_containers) {
            if (terminal.containers[container_id].pos.x != 0) return false;
        }
    }
    return true;
}

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

void set_next_target(Crane &crane, Terminal &terminal, vi &que) {
    int next_container_id = que.back();
    Container &next_container = terminal.containers[next_container_id];

    vector<Pos> empty_positions;
    rep(gx, 0, N - 1) {
        rep(gy, N) {
            if (terminal.grid[gy][gx] == -1) {
                empty_positions.pb(Pos(gx, gy));
            }
        }
    }

    if (crane.id == 0) {
        if (next_container.crane_id == -1) {
            if (!next_container.is_loaded()) {
                Pos start = Pos(0, next_container.pos.y);
                sort(all(empty_positions), [&](Pos a, Pos b) {
                    ll cost_a = (abs(a.x - start.x) + abs(a.y - start.y)) * N - abs(a.x);
                    ll cost_b = (abs(b.x - start.x) + abs(b.y - start.y)) * N - abs(b.x);
                    return cost_a < cost_b;
                });
                Pos goal = empty_positions[0];
                crane.set_path(start, goal);
            } else {
                // 次のコンテナを運び出す
                Pos start = next_container.pos;
                Pos goal = Pos(N - 1, next_container_id / N);
                crane.set_path(start, goal);
                que.pop_back();
            }
        }
    } else if (empty_positions.size() >= 2) {
        vector<Container> movable_containers;
        for (Container &container : terminal.containers) {
            if (!container.is_loaded() || container.is_dispatched() || container.crane_id != -1) continue;
            Pos pos = container.pos;
            pos.x++;
            if (pos.x < N - 1 && terminal.grid[pos.y][pos.x] == -1) {
                movable_containers.pb(container);
            }
        }

        if (!movable_containers.empty()) {
            sort(all(movable_containers),
                 [&](Container a, Container b) { return dist(a.pos, crane.pos) < dist(b.pos, crane.pos); });
            Pos start = movable_containers[0].pos;
            Pos goal = Pos(start.x + 1, start.y);
            crane.set_path(start, goal);
        }
    }

    if (!crane.start.is_null()) {
        terminal.containers[terminal.get_cell(crane.start)].crane_id = crane.id;
        terminal.set_cell(crane.goal, -2);
    }

    cerr << "crane_id = " << crane.id << endl;
    cerr << "container_id = " << next_container_id << endl;
    cerr << "start = " << crane.start << endl;
    cerr << "goal = " << crane.goal << endl;
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
    // とりあえず20このコンテナをターミナルに出す
    rep(i, N - 2) {
        for (auto &crane : terminal.cranes) {
            if (crane.is_finished()) {
                crane.set_path(Pos(0, crane.pos.y), Pos(N - i - 2, crane.pos.y));
            }
        }
        while (!terminal.cranes[0].is_finished()) {
            terminal.step();
        }
    }

    // クレーン0以外を破壊
    map<int, char> actions;
    rep(i, N) {
        if (i <= 1)
            actions[i] = '.';
        else
            actions[i] = 'B';
    }
    terminal.step(actions);

    // クレーン0で運び出していく
    vi que = get_best_container_que(terminal);
    cerr << que << endl;

    while (!terminal.is_clear() && !terminal.is_timeout()) {
        Crane &crane0 = terminal.cranes[0];
        Crane &crane1 = terminal.cranes[1];
        if (crane0.is_finished() && !que.empty()) {
            set_next_target(crane0, terminal, que);
        }
        if (crane1.is_finished() && !que.empty()) {
            set_next_target(crane1, terminal, que);
        }
        map<int, char> actions;
        char action0 = get_next_action(crane0);
        char action1 = get_next_action(crane1);
        if (is_move(action0)) {
            Pos next_pos0 = get_next_pos(crane0.pos, action0);
            if (next_pos0 == crane1.pos) {
                if (crane1.is_holding()) {
                    actions[0] = '.';
                    actions[1] = 'Q';
                } else {
                    for (char action : {'L', 'R', 'U', 'D'}) {
                        Pos next_pos1 = get_next_pos(crane1.pos, action);
                        if (in_field(next_pos1) && next_pos1 != crane0.pos) {
                            actions[1] = action;
                            break;
                        }
                    }
                }
            }
            if (is_move(action1)) {
                Pos next_pos1 = get_next_pos(crane1.pos, action1);
                if (next_pos1 == next_pos0) {
                    actions[1] = '.';
                }
            }
        } else if (is_move(action1)) {
            Pos next_pos1 = get_next_pos(crane1.pos, action1);
            if (next_pos1 == crane0.pos) {
                actions[1] = '.';
            }
        }
        terminal.step(actions);
    }

    rep(i, N) {
        cout << terminal.S[i] << endl;
    }

    cerr << "Score = " << terminal.calc_score() << endl;
}