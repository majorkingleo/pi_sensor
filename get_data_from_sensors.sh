#!/usr/bin/env bash

# Script: run_sensor_data.sh
# Beschreibung: Wechselt ins Script-Verzeichnis, aktiviert die Python-Venv und führt get_data_from_sensors.sh aus

# Ermittle das Verzeichnis, in dem dieses Script liegt
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

# Wechsle in das Script-Verzeichnis
cd "$SCRIPT_DIR" || {
    echo "Fehler: Konnte nicht in das Verzeichnis '$SCRIPT_DIR' wechseln"
    exit 1
}

echo "Arbeitsverzeichnis: $(pwd)"

# Prüfe ob das .venv Verzeichnis existiert
if [ ! -d ".venv" ]; then
    echo "Fehler: .venv Verzeichnis nicht gefunden in '$SCRIPT_DIR'"
    exit 1
fi

# Lade die Python Virtual Environment
echo "Aktiviere virtuelle Umgebung..."
source .venv/bin/activate || {
    echo "Fehler: Konnte .venv/bin/activate nicht ausführen"
    exit 1
}

# Prüfe ob die auszuführende Datei existiert
if [ ! -f "get_data_from_sensors.py" ]; then
    echo "Fehler: get_data_from_sensors.py nicht gefunden in '$SCRIPT_DIR'"
    exit 1
fi

# Führe get_data_from_sensors.sh aus
echo "Führe get_data_from_sensors.sh aus..."
python3 ./get_data_from_sensors.py

# Speichere den Exit-Code
EXIT_CODE=$?

# Deaktiviere die virtuelle Umgebung (optional)
deactivate 2>/dev/null

# Beende mit dem Exit-Code des ausgeführten Scripts
exit $EXIT_CODE


