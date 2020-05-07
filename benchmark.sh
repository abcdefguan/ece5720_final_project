#!/bin/bash
make

#rm results/kruskal
#for n in 10000 50000 100000 500000 1000000
#do
#	./kruskal $n 50 >> results/kruskal
#done

#rm results/prim
#for n in 10000 50000 100000 500000 1000000
#do
#	./prim $n 50 >> results/prim
#done

#rm results/boruvka
#for n in 10000 50000 100000 500000 1000000
#do
#	./boruvka2 $n 50 >> results/boruvka
#done

#rm results/filter_kruskal2
#for n in 10000 50000 100000 500000 1000000
#do
#for p in 1 2 3 4 5 6 8 10 12 16 24
#do
#	./filter_kruskal $n 50 $p >> results/filter_kruskal2
#done
#done

rm results/parallel_boruvka2
for n in 10000 50000 100000 500000 1000000
do
for p in 1 2 3 4 5 6 8 10 12 16 24
do
	./parallel_boruvka $n 50 $p >> results/parallel_boruvka2
done
done