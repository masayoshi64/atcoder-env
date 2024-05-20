#include "../../../library/library/template/template.cpp"

const int max_turn = 10000;
int N;
mat<int> A;

struct Pos {
    int x, y;
    Pos(int x, int y) : x(x), y(y) {}

    bool is_null() { return x == -1 && y == -1; }

    void set_null() { x = -1, y = -1; }

    bool operator==(const Pos &p) const { return x == p.x && y == p.y; }
};

struct Crane {
    int x, y;
    int holding_container = -1;
    bool is_alive = true;
    Crane(int x, int y) : x(x), y(y) {}

    bool is_holding() { return holding_container != -1; }
};

struct Terminal {
    mat<int> grid;
    mat<int> dispatched_containers;
    mat<int> waiting_containers;
    vector<Crane> cranes;

    Terminal() : grid(N, vi(N, -1)), dispatched_containers(N), waiting_containers(N) {
        // 積み込み前のコンテナ（列ごとに出る順番の降順）
        rep(i, N) rrep(j, N) { waiting_containers[i].pb(A[i][j]); }

        // クレーンの初期位置
        rep(i, N) { cranes.pb(Crane(0, i)); }

        // グリッドの左端にコンテナを積む
        rep(i, N) {
            grid[i][0] = waiting_containers[i].back();
            waiting_containers[i].pop_back();
        }
    }

    void step(string actions) {
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
        crane.holding_container = grid[crane.y][crane.x];
        grid[crane.y][crane.x] = -1;
    }

    void destroy(Crane &crane) { crane.is_alive = false; }

    void release(Crane &crane) {
        grid[crane.y][crane.x] = crane.holding_container;
        crane.holding_container = -1;
        if (crane.x == N - 1) {
            dispatched_containers[crane.y].pb(grid[crane.y][crane.x]);
            grid[crane.y][crane.x] = -1;
        }
    }

    int calc_score(int num_turn) {
        int M0 = num_turn;
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
    int num_turn = 0;
    Pos target_pos(-1, -1);
    Crane &crane0 = terminal.cranes[0];
    rep(turn, max_turn) {
        num_turn++;
        string actions;
        if (turn == 0) {
            // 初回はクレーン0以外を爆破
            actions = string(N, 'B');
            actions[0] = '.';
        } else {
            // 2回目以降はクレーン0を操作
            actions = string(N, '.');

            if (target_pos.is_null()) {
                // 目的地が未設定の場合は、最初に見つかったコンテナを目的地に設定
                rep(x, N) rep(y, N) {
                    if (terminal.grid[y][x] != -1) {
                        target_pos = Pos(x, y);
                        break;
                    }
                }
            }

            if (target_pos == Pos(crane0.x, crane0.y)) {
                // 目的地に到達したら積み下ろし又は積み込み
                if (crane0.is_holding()) {
                    // 積み下ろし
                    actions[0] = 'Q';
                    target_pos.set_null();
                } else {
                    // 積み込み
                    actions[0] = 'P';
                    int container = terminal.grid[crane0.y][crane0.x];
                    // 積み下ろし位置を指定
                    target_pos = Pos(N - 1, container / N);
                }
            } else {
                // 目的地に向かう
                if (crane0.x < target_pos.x) {
                    actions[0] = 'R';
                } else if (crane0.x > target_pos.x) {
                    actions[0] = 'L';
                } else if (crane0.y < target_pos.y) {
                    actions[0] = 'D';
                } else if (crane0.y > target_pos.y) {
                    actions[0] = 'U';
                }
            }
        }
        
        terminal.step(actions);
        actions_list.pb(actions);
        if (terminal.is_clear()) break;
    }

    vector<string> S(N);
    rep(i, N) {
        rep(turn, actions_list.size()) { S[i] += actions_list[turn][i]; }
    }

    rep(i, N) { cout << S[i] << endl; }

    cerr << "Score = " << terminal.calc_score(num_turn) << endl;
}