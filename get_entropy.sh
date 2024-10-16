#!/bin/bash

lzw_max=0
original_filename=""
is_using_model=false
is_saving_model=false
is_restarting_map=false
is_benchmarking=false
subsample_factor=200
plot_id=""
model_name=""

# Processando os argumentos
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
        -sm)
            is_saving_model=true
            model_name=$2
            shift 2
            ;;
        -um)
            is_using_model=true
            model_name=$2
            shift 2
            ;;
        -r)
            is_restarting_map=true
            shift 1
            ;;
        -b)
            is_benchmarking=true
            plot_id=$2
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
	echo "Arquivo '$original_filename' não existe."
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
if [ $is_using_model = true ]; then
    lzw_params="$lzw_params -um $model_name"
fi
if [ $is_saving_model = true ]; then
    lzw_params="$lzw_params -sm $model_name"
fi
if [ $is_restarting_map = true ]; then
    lzw_params="$lzw_params -r"
fi
if [ $is_benchmarking = true ]; then
    lzw_params="$lzw_params -b"
fi

echo "Parâmetros do LZW: '$lzw_params'\n"
./jvav_lzw $lzw_params

# Calculando o comprimento médio
original_size=$(wc -c < "$original_filename")
compressed_size=$(wc -c < "$compressed_filename")
mean_length=$(echo "scale=3; $compressed_size * 8 / $original_size" | bc)

echo "Tamanho original: $original_size bytes"
echo "Tamanho comprimido: $compressed_size bytes"
echo "Comprimento médio: $mean_length bits (codificados) / bytes (originais)"
rm $compressed_filename

# Plotando o gráfico do benchmark com assíntota exponencial
if [ $is_benchmarking = true ]; then
    echo "Plotando gráfico de benchmark..."
    python plotter.py benchmark_data_file $subsample_factor $plot_id
fi
