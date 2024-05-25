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
};

struct Crane {
    int id;
    int x, y;
    int holding_container = -1;
    bool is_alive = true;
    Pos start, goal;

    Crane(int id, int x, int y) : id(id), x(x), y(y) {
    }

    bool is_holding() {
        return holding_container != -1;
    }

    bool is_at_goal() {
        return Pos(x, y) == goal;
    }

    bool is_at_start() {
        return Pos(x, y) == start;
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

    char get_move_to_target(Pos target) {
        if (x < target.x) return 'R';
        if (x > target.x) return 'L';
        if (y < target.y) return 'D';
        if (y > target.y) return 'U';
        assert(false);
    }

    char get_next_action() {
        if (!is_alive) return '.';

        if (is_holding()) {
            if (is_at_goal()) {
                return 'Q';
            } else {
                return get_move_to_target(goal);
            }
        } else {
            if (is_at_start()) {
                return 'P';
            } else {
                return get_move_to_target(start);
            }
        }
    }
};

struct Container {
    int id;
    int x, y;

    Container(int id, int x, int y) : id(id), x(x), y(y) {
    }

    Container() : id(-1), x(-1), y(-1) {
    }

    void move(int nx, int ny) {
        x = nx;
        y = ny;
    }

    bool is_loaded() {
        return x != -1;
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
            containers[grid[i][0]].move(0, i);
        }
    }

    void step(map<int, char> actions = {}) {
        turn++;
        for (auto &crane : cranes) {
            char action = actions.count(crane.id) ? actions[crane.id] : crane.get_next_action();

            if (action == 'Q') crane.clear_path();
            S[crane.id] += action;

            if (action == 'P') {
                pick(crane);
            } else if (action == 'Q') {
                release(crane);
            } else if (action == '.') {
                continue;
            } else if (action == 'B') {
                destroy(crane);
            } else {
                int dx = 0, dy = 0;
                if (action == 'L') {
                    dx = -1;
                }
                if (action == 'R') {
                    dx = 1;
                }
                if (action == 'U') {
                    dy = -1;
                }
                if (action == 'D') {
                    dy = 1;
                }
                move(crane, dx, dy);
            }
        }
    }

    void move(Crane &crane, int dx, int dy) {
        assert(crane.is_alive);

        int x = crane.x, y = crane.y;
        int nx = crane.x + dx;
        int ny = crane.y + dy;
        cerr << "move " << x << " " << y << " " << nx << " " << ny << endl;
        crane.x = nx;
        crane.y = ny;
        if (crane.is_holding()) {
            containers[crane.holding_container].move(nx, ny);
            if (grid[y][0] == -1 && !waiting_containers[y].empty()) {
                grid[y][0] = waiting_containers[y].back();
                containers[grid[y][0]].move(0, y);
                waiting_containers[y].pop_back();
            }
        }
    }

    void pick(Crane &crane) {
        assert(crane.is_alive);
        assert(!crane.is_holding());
        assert(grid[crane.y][crane.x] != -1);

        crane.holding_container = grid[crane.y][crane.x];
        grid[crane.y][crane.x] = -1;
    }

    void destroy(Crane &crane) {
        assert(crane.is_alive);
        assert(!crane.is_holding());

        crane.is_alive = false;
    }

    void release(Crane &crane) {
        assert(crane.is_alive);
        assert(crane.is_holding());
        assert(grid[crane.y][crane.x] == -1);

        grid[crane.y][crane.x] = crane.holding_container;
        crane.holding_container = -1;
        if (crane.x == N - 1) {
            dispatched_containers[crane.y].pb(grid[crane.y][crane.x]);
            containers[grid[crane.y][crane.x]].move(N, crane.y);
            grid[crane.y][crane.x] = -1;
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

    bool is_clear() {
        rep(i, N) {
            if (dispatched_containers[i].size() != N) return false;
        }
        return true;
    }

    bool is_timeout() {
        return turn >= max_turn;
    }
};

bool is_full(int state, Terminal &terminal) {
    rep(y, N) {
        int num_dispatched_containers = (state / mypow<int>(N + 1, y)) % (N + 1);
        rep(container_id, N * y, N * y + num_dispatched_containers) {
            if (terminal.containers[container_id].x != 0) return false;
        }
    }
    return true;
}

vi get_best_container_que(Terminal &terminal) {
    int num_states = mypow<int>(N + 1, N);
    mat<int> dp(num_states, vi(N, inf));
    mat<tuple<int, int, int>> prev(num_states, vector<tuple<int, int, int>>(N));
    dp[0][terminal.cranes[0].y] = 0;
    rep(state, num_states) {
        rep(crane_y, N) {
            rep(next_crane_y, N) {
                int num_dispatched_containers = (state / mypow<int>(N + 1, next_crane_y)) % (N + 1);
                if (num_dispatched_containers == N) continue;
                int next_container_id = N * next_crane_y + num_dispatched_containers;
                Container &container = terminal.containers[next_container_id];
                if (is_full(state, terminal) && !container.is_loaded()) continue;
                int next_state = state + mypow<int>(N + 1, next_crane_y);
                int cost = abs(crane_y - container.y);
                if (chmin(dp[next_state][next_crane_y], dp[state][crane_y] + cost)) {
                    prev[next_state][next_crane_y] = {state, crane_y, next_container_id};
                }
            }
        }
    }

    vi que;
    int state = num_states - 1;
    int crane_y = terminal.cranes[0].y;
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

    // 次のコンテナが積まれていない場合は邪魔なコンテナを移動させる
    if (!next_container.is_loaded()) {
        Pos start = Pos(0, next_container.y);
        vector<Pos> empty_positions;
        rep(gx, 0, N - 1) {
            rep(gy, N) {
                if (terminal.grid[gy][gx] == -1) {
                    empty_positions.pb(Pos(gx, gy));
                }
            }
        }
        sort(all(empty_positions), [&](Pos a, Pos b) {
            ll cost_a = (abs(a.x - start.x) + abs(a.y - start.y)) * N - abs(a.x);
            ll cost_b = (abs(b.x - start.x) + abs(b.y - start.y)) * N - abs(b.x);
            return cost_a < cost_b;
        });
        Pos goal = empty_positions[0];
        crane.set_path(start, goal);
    } else {
        // 次のコンテナを運び出す
        Pos start = Pos(next_container.x, next_container.y);
        Pos goal = Pos(N - 1, next_container_id / N);
        crane.set_path(start, goal);
        que.pop_back();
    }

    cerr << "crane_id = " << crane.id << endl;
    cerr << "container_id = " << next_container_id << endl;
    cerr << "start = " << crane.start.x << " " << crane.start.y << endl;
    cerr << "goal = " << crane.goal.x << " " << crane.goal.y << endl;
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
    rep(i, N - 1) {
        for (auto &crane : terminal.cranes) {
            if (crane.is_finished()) {
                crane.set_path(Pos(0, crane.y), Pos(N - i - 2, crane.y));
            }
        }
        while (!terminal.cranes[0].is_finished()) {
            terminal.step();
        }
    }

    // クレーン0以外を破壊
    map<int, char> actions;
    rep(i, N) {
        if (i == 0)
            actions[i] = '.';
        else
            actions[i] = 'B';
    }
    terminal.step(actions);

    // クレーン0で運び出していく
    vi que = get_best_container_que(terminal);
    cerr << que << endl;

    while (!terminal.is_clear() && !terminal.is_timeout()) {
        rep(crane_id, N) {
            Crane &crane = terminal.cranes[crane_id];
            if (crane.is_alive && crane.is_finished()) {
                set_next_target(crane, terminal, que);
            }
        }
        terminal.step();
    }

    rep(i, N) {
        cout << terminal.S[i] << endl;
    }

    cerr << "Score = " << terminal.calc_score() << endl;
}