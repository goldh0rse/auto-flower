# Import the current working construction
# environment to the `env` variable.
# alias of `env = DefaultEnvironment()`
Import("env")

# Dump construction environment (for debug purpose)
print(env.Dump())

# append extra flags to global build environment
# which later will be used to build:
# - project source code
# - frameworks
# - dependent libraries


def read_env(file_path):
    env_vars = {}
    with open(file_path, 'r') as f:
        for line in f.readlines():
            if line.startswith('#') or not line.strip():
                continue
            key, value = line.strip().split('=', 1)
            env_vars[key] = value
    return env_vars


env_vars = read_env(".env")

env.Append(CPPDEFINES=[(key, '\\\"{}\\\"'.format(value))
           for key, value in env_vars.items()])
