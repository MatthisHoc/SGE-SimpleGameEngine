# Build the game binary and compiles an executable

import os

script_dir = os.path.dirname(os.path.abspath(__file__))
game_bin_name = "libGame.dylib"
game_exec_dir_name = "GameOut"
resources_dir_name = "resources"

# Build game binary
print("⚙️  Building Game ...")
game_build = os.system("cd {}; cd ../../Game;make all BIN_NAME={}".format(script_dir, game_bin_name))

if (game_build != 0):
    print("❌ Error when building the game binary. Aborting")
    exit

print("✅ Game binary built successfuly")

print("🗂️ Preparing game executable directory")

os.system('''
cd {script}
cd ../ 
mkdir -p {exec_dir} 
mkdir -p {exec_dir}/lib
cp ../Game/lib/{game_bin} {exec_dir}/lib
cp -a ../Game/{resources_dir} {exec_dir}
'''.format(script = script_dir, exec_dir = game_exec_dir_name, game_bin = game_bin_name, resources_dir = resources_dir_name))

print("⚙️ Building game executable")

os.system("cd {script};cd ../; make all OUT_DIR={exec_dir} BIN_NAME={game_bin} EXE_NAME={exec_name};".format(script=script_dir, exec_dir=game_exec_dir_name, game_bin=game_bin_name, exec_name="Game"))

print("✅ Done")