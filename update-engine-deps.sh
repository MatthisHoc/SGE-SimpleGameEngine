#!/bin/sh
echo "⚙️  Running Makefile..."
cd SimpleGameEngine
make all
cd ..
echo "📑 Copying new lib file to Game/SGE/lib..."
yes | cp SimpleGameEngine/lib/libSimpleGameEngine.a Game/SGE/lib
echo "📑 Copying include files to Game/SGE/include/..."
cp -a SimpleGameEngine/include/. Game/SGE/include/
echo "Done ✅"