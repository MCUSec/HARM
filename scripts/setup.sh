#!/bin/bash

set -euo pipefail

WORKDIR=`pwd`

if [[ -e "./harm" ]]; then
	echo "virtualenv already setup."
	exit 1
fi
	
python3 -m venv harm

set +u
source harm/bin/activate
set -u

pip3 install --upgrade wheel
pip3 install --upgrade pip
pip3 install -r librwtool/requirements.txt

echo "source $(pwd)/harm/bin/postactivate" >> harm/bin/activate
echo "export PYTHONPATH=$(pwd)/librwtool" > harm/bin/postactivate
echo "export PATH=$(pwd):$PATH" >> harm/bin/postactivate
echo "export HARM_HOME=$(dirname $(pwd))" >> harm/bin/postactivate
echo "#!/usr/bin/env python" > rwtool
cat librwtool/main.py >> rwtool
chmod +x rwtool

set +u
deactivate
set -u

cd $WORKDIR

echo "[+] All done and ready to go"
echo "[ ] You can start run: source ./harm/bin/activate"
