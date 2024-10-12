echo -e "\e[1;33m Compilando o LZW...\e[0m"
g++ source/LZW2/*.cpp -o jvav_lzw -Ofast

echo -e "\e[1;32m Compilando o PPMc...\e[0m"
g++ source/PPMc/source/*.cpp -o jvav_ppmc -Ofast