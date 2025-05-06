#!/bin/bash
export LC_ALL=C # Cambia la codificacion local a ingles

# Establecemos que el código de retorno de un pipeline sea el del último programa con código de retorno
# distinto de cero, o cero si todos devuelven cero.
set -o pipefail

# Put here the program (maybe with path)
GETF0="get_pitch"

# Leer argumentos de línea de comandos
WINDOW="${1:-RECT}"
POTTH="${2:-10.0}"
R1TH="${3:-0.5}"
RMAXTH="${4:-0.5}"



for fwav in pitch_db/train/*.wav; do
    ff0=${fwav/.wav/.f0}
    echo "$GETF0 $fwav $ff0 ----"
	$GETF0 --window=$WINDOW --pot-th=$POTTH --r1-th=$R1TH --rmax-th=$RMAXTH $fwav $ff0 > /dev/null || { echo -e "\nError in $GETF0 $fwav $ff0" && exit 1; }
done

pitch_evaluate pitch_db/train/*.f0ref #| grep TOTAL

echo "Configuracion usada:"
echo "  Ventana: $WINDOW"
echo "  POTTH:   $POTTH"
echo "  R1TH:    $R1TH"
echo "  RMAXTH:  $RMAXTH"
echo ""

exit 0
