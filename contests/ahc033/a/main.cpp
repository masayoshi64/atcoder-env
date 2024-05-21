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

    bool operator!=(const Pos &p) const { return x != p.x || y != p.y; }
};

struct Crane {
    int id;
    int x, y;
    int holding_container = -1;
    bool is_alive = true;
    Pos start, goal;

    Crane(int id, int x, int y) : id(id), x(x), y(y) {}

    bool is_holding() { return holding_container != -1; }

    bool is_at_goal() { return Pos(x, y) == goal; }

    bool is_at_start() { return Pos(x, y) == start; }

    bool is_finished() { return goal.is_null(); }

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

struct Terminal {
    int turn = 0;
    mat<int> grid;
    mat<int> dispatched_containers;
    mat<int> waiting_containers;
    vector<Crane> cranes;
    vector<string> S;

    Terminal() : grid(N, vi(N, -1)), dispatched_containers(N), waiting_containers(N), S(N) {
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
                if (action == 'L') { dx = -1; }
                if (action == 'R') { dx = 1; }
                if (action == 'U') { dy = -1; }
                if (action == 'D') { dy = 1; }
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

    Terminal terminal;
    // とりあえず20このコンテナをターミナルに出す
    rep(i, N - 1) {
        for (auto &crane : terminal.cranes) {
            if (crane.is_finished()) { crane.set_path(Pos(0, crane.y), Pos(N - i - 2, crane.y)); }
        }
        while (!terminal.cranes[0].is_finished()) { terminal.step(); }
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
    Crane &crane0 = terminal.cranes[0];
    while (!terminal.is_clear() && !terminal.is_timeout()) {
        if (crane0.is_finished()) {
            // 目的地が未設定の場合は、最初に見つかったコンテナを目的地に設定
            rep(x, N) rep(y, N) {
                int container_id = terminal.grid[y][x];
                if (container_id == -1) continue;
                if (container_id % N == terminal.dispatched_containers[container_id / N].size()) {
                    crane0.set_path(Pos(x, y), Pos(N - 1, container_id / N));
                    break;
                }
            }
            if (crane0.is_finished()) {
                rep(y, N) {
                    if (terminal.waiting_containers[y].empty()) continue;
                    Pos start = Pos(0, y);
                    vector<Pos> empty_positions;
                    rep(gx, 0, N - 1) {
                        rep(gy, N) {
                            if (terminal.grid[gy][gx] == -1) { empty_positions.pb(Pos(gx, gy)); }
                        }
                    }
                    sort(all(empty_positions), [&](Pos a, Pos b) {
                        ll cost_a = (abs(a.x - start.x) + abs(a.y - start.y)) * N - abs(a.x);
                        ll cost_b = (abs(b.x - start.x) + abs(b.y - start.y)) * N - abs(b.x);
                        return cost_a < cost_b;
                    });
                    Pos goal = empty_positions[0];
                    crane0.set_path(start, goal);
                    break;
                }
            }
            if (crane0.start.is_null() || crane0.goal.is_null()) { break; }
        }

        cerr << "start = " << crane0.start.x << " " << crane0.start.y << endl;
        cerr << "goal = " << crane0.goal.x << " " << crane0.goal.y << endl;

        terminal.step();
    }

    rep(i, N) { cout << terminal.S[i] << endl; }

    cerr << "Score = " << terminal.calc_score() << endl;
}