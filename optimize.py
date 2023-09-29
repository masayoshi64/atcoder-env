import optuna
from subprocess import PIPE, Popen


n_trials = 1000
n_files = 100
prefix = "Avg Score:"


def objective(trial):
    args = ""
    p = Popen(f"psytester r -t 0-{n_files-1} --tester_arguments \"{args}\"", shell=True, stdout=PIPE, stderr=PIPE)
    stdout, stderr = p.communicate()
    for line in stderr.decode("utf-8").split("\n"):
        if line.startswith(prefix):
            return float(line[len(prefix):])


if __name__ == "__main__":
    study = optuna.create_study(
        direction="maximize",
        study_name="test",
        load_if_exists=True,
    )
    study.optimize(objective, n_trials=1000)
