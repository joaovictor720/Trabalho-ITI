#!/bin/bash

lzw_max=0
original_filename=""

# Passando pelos argumentos
while [[ $# -gt 0 ]]; do
    case "$1" in
		--test)
			original_filename="$2"
			shift 2
			;;
        --lzw-max)
            lzw_max="$2"
            shift 2
            ;;
        --)
            shift # end of options
            break
            ;;
        *)
            echo "Opção desconhecida: $1"
            exit 1
            ;;
    esac
done

# Checando se o arquivo de teste existe
if (! test -f $original_filename) || [[ -z $original_filename ]]; then
	echo "File '$original_filename' doesn't exist."
	exit 1
fi

# Obtendo a "entropia" com o LZW
echo "------- LZW -------"

compressed_filename="comp.lzw"
lzw_params="-c $original_filename $compressed_filename"

# Adicionando o máximo tamanho do dicionário
if [[ $lzw_max -ne 0 ]]; then
	lzw_params="$lzw_params -m $lzw_max"
fi

./jvav_lzw $lzw_params

original_size=$(wc -c < "$original_filename")
compressed_size=$(wc -c < "$compressed_filename")
mean_length=$(echo "scale=2; $compressed_size * 8 / $original_size" | bc)

echo "Comprimento médio: $mean_length"
# rm $compressed_filename
