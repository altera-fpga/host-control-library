# Continuous Integration Tools

## Static Analysis

This project includes a Statick configuration under `.ci/statick/rsc`.

### Install Statick

```bash
python3 -m venv .venv
. .venv/bin/activate
pip install statick
```

### Run From CMake

```bash
cmake -S . -B build
cmake --build build --target statick
```

The `statick` target passes `--check`, so the build exits nonzero when Statick reports findings.

### Run Directly

```bash
statick . \
	--user-paths .ci/statick \
	--config hcl-config.yaml \
	--profile hcl-profile.yaml \
	--exceptions hcl-exceptions.yaml \
	--output-directory build/statick-output \
	--log INFO
```

Reports are written under `build/statick-output`.
