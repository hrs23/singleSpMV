#!/bin/bash 
MATRIX_DIR=../../../matrix/artificial/
LOG_DIR=.
BINARY_DIR=.

matrices=`ls $MATRIX_DIR/*.mtx | xargs -i basename {}`
logfile=$LOG_DIR/cpu-`date +%y-%m-%d-%H-%M`.tsv && echo "" > $logfile

export OMP_NUM_THREADS=24
for matrix in $matrices
do
    echo "CPU $matrix"
    srun -p NOEL $BINARY_DIR/spmv $MATRIX_DIR/$matrix >> $logfile
done



