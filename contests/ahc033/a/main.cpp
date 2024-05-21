#include "../../../library/library/template/template.cpp"

const int max_turn = 10000;
int N;
mat<int> A;

struct Pos {
    int x, y;

    Pos() : x(-1), y(-1) {}

    Pos(int x, int y) : x(x), y(y) {}

    bool is_null() { return x == -1 && y == -1; }

    void set_null() { x = -1, y = -1; }

    bool operator==(const Pos &p) const { return x == p.x && y == p.y; }
};

struct Crane {
    int id;
    int x, y;
    int holding_container = -1;
    bool is_alive = true;
    Pos goal;

    Crane(int id, int x, int y) : id(id), x(x), y(y) {}

    bool is_holding() { return holding_container != -1; }

    bool is_arrived() { return Pos(x, y) == goal; }

    void set_goal(Pos goal) { this->goal = goal; }

    void clear_goal() { goal.set_null(); }

    char get_action_to_target() {
        if (x < goal.x) return 'R';
        if (x > goal.x) return 'L';
        if (y < goal.y) return 'D';
        if (y > goal.y) return 'U';
        return '.';
    }
};

struct Terminal {
    int turn = 0;
    mat<int> grid;
    mat<int> dispatched_containers;
    mat<int> waiting_containers;
    vector<Crane> cranes;

    Terminal() : grid(N, vi(N, -1)), dispatched_containers(N), waiting_containers(N) {
        // 積み込み前のコンテナ（列ごとに出る順番の降順）
        rep(i, N) rrep(j, N) { waiting_containers[i].pb(A[i][j]); }

        // クレーンの初期位置
        rep(i, N) { cranes.pb(Crane(i, 0, i)); }

        // グリッドの左端にコンテナを積む
        rep(i, N) {
            grid[i][0] = waiting_containers[i].back();
            waiting_containers[i].pop_back();
        }
    }

    void step(string actions) {
        turn++;
        rep(crane_id, N) {
            int action = actions[crane_id];
            if (action == 'P') {
                pick(cranes[crane_id]);
            } else if (action == 'Q') {
                release(cranes[crane_id]);
            } else if (action == '.') {
                continue;
            } else if (action == 'B') {
                destroy(cranes[crane_id]);
            } else {
                int dx = 0, dy = 0;
                if (action == 'L') { dx = -1; }
                if (action == 'R') { dx = 1; }
                if (action == 'U') { dy = -1; }
                if (action == 'D') { dy = 1; }
                move(cranes[crane_id], dx, dy);
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
            if (x == 0 && !waiting_containers[y].empty()) {
                grid[y][x] = waiting_containers[y].back();
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
                        if (dispatched_containers[i][j] > dispatched_containers[i][k]) { M1++; }
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

    bool is_timeout() { return turn >= max_turn; }
};

int main(int argc, char *argv[]) {
    cin.tie(0);
    ios::sync_with_stdio(0);
    cout << setprecision(30) << fixed;
    cerr << setprecision(30) << fixed;

    scan(N);
    A = mat<int>(N, vi(N));
    scan(A);

    vector<string> actions_list;
    Terminal terminal;

    // とりあえず20このコンテナをターミナルに出す
    actions_list.pb(string(N, 'P'));
    rep(i, 3) { actions_list.pb(string(N, 'R')); }
    actions_list.pb(string(N, 'Q'));
    rep(i, 3) { actions_list.pb(string(N, 'L')); }
    actions_list.pb(string(N, 'P'));
    rep(i, 2) { actions_list.pb(string(N, 'R')); }
    actions_list.pb(string(N, 'Q'));
    rep(i, 2) { actions_list.pb(string(N, 'L')); }
    actions_list.pb(string(N, 'P'));
    rep(i, 1) { actions_list.pb(string(N, 'R')); }
    actions_list.pb(string(N, 'Q'));
    rep(i, 1) {
        string actions = string(N, 'B');
        actions[0] = '.';
        actions_list.pb(actions);
    }
    for (auto &actions : actions_list) { terminal.step(actions); }

    // クレーン0で運び出していく
    Crane &crane0 = terminal.cranes[0];
    while (!terminal.is_clear() && !terminal.is_timeout()) {
        string actions = string(N, '.');

        if (crane0.goal.is_null()) {
            // 目的地が未設定の場合は、最初に見つかったコンテナを目的地に設定
            rep(x, N) rep(y, N) {
                if (terminal.grid[y][x] != -1) {
                    crane0.set_goal(Pos(x, y));
                    break;
                }
            }
        }

        if (crane0.is_arrived()) {
            // 目的地に到達したら積み下ろし又は積み込み
            if (crane0.is_holding()) {
                // 積み下ろし
                actions[0] = 'Q';
                crane0.clear_goal();
            } else {
                // 積み込み
                actions[0] = 'P';
                int container = terminal.grid[crane0.y][crane0.x];
                // 積み下ろし位置を指定
                crane0.set_goal(Pos(N - 1, container / N));
            }
        } else {
            // 目的地に向かう
            actions[0] = crane0.get_action_to_target();
        }

        terminal.step(actions);
        actions_list.pb(actions);
    }

    vector<string> S(N);
    rep(i, N) {
        rep(turn, actions_list.size()) { S[i] += actions_list[turn][i]; }
    }

    rep(i, N) { cout << S[i] << endl; }

    cerr << "Score = " << terminal.calc_score() << endl;
}