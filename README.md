# LinearWang

An implementation of the algorithm discussed in the paper:
* A linear algorithm for Brick Wang tiling, 
* by A. Derouet-Jourdan, S. Kaji, and Y. Mizoguchi, 
* arXiv:1603.04292, https://arxiv.org/abs/1603.04292
* Derouet-Jourdan, A., Kaji, S. & Mizoguchi, Y. Japan J. Indust. Appl. Math. (2019) 36: 749.
 https://doi.org/10.1007/s13160-019-00369-z
	
## How to compile

    cd LinearWang
    cmake .
    make
 
## How to use

* to obtain an SVG file for the tiling pattern specified by the mask image input.png 

    ./LinearWang input.png

* to obtain the tiling pattern without background

    ./LinearWang -ne input.png
    
