# Builds the engine and automatically copy it's includes and the generated library to the Game folder

import os

script_dir = os.path.dirname(os.path.abspath(__file__))

print("⚙️  Building Engine...")

build_status = os.system('''
cd {} 
cd SimpleGameEngine 
make all
'''.format(script_dir))

if build_status != 0:
    print("❌ Failed to build engine")
    exit

print("📑 Copying new files to Game...")
os.system('''
cd {}
mkdir -p Game/SGE
mkdir -p Game/SGE/lib
mkdir -p Game/SGE/include
cp SimpleGameEngine/lib/libSimpleGameEngine.a Game/SGE/lib
cp -a SimpleGameEngine/include/. Game/SGE/include/
'''.format(script_dir))

print("Done ✅")