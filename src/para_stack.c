/*------------------------------------------------------------------
----   Intially coded by Xuping Feng @NJU on Sep., 12th, 2018   ----
----   Nth_root, weighted and slant stacking SAC format files.  ----
-------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "sacio.h"

/*-------------------------------------------------- some CONST values -----------------------------------*/
#define SAC_MAX 500           // Maximum of SAC format files.
#define LINE_LENGTH_MAX 200   // Maximum of characters of every line in "para.file".

/*------------------------------------- get sign of float type number "x" --------------------------------*/
float sign ( float x ) {
    if ( x >= 0. ) return 1.;
    else return -1.;
}

/*---------------------------------------------- MAIN PROGRAM --------------------------------------------*/
int main ( int argc, char *argv[] ) {
    int i, j, k, shift_index, count = 1, npts, line = 0, tmp_index;
    float *data, *sum, delta, Nth_root, shift_time[SAC_MAX], weight[SAC_MAX], tmp1, tmp2, peak;
    char buff[1024], sac_out[200], s1[200], s2[200], sac_name[SAC_MAX][LINE_LENGTH_MAX]={{""}}, norm[16];
    FILE *fin;
    SACHEAD hd;

    if ( argc != 2 ) {
        fprintf(stderr, "\033[0;31m Usage: para_stack para.file\n \033[0:39m");
        exit(1);
    }

/*------------------------------ read in parameters from file "para.file" --------------------------------*/
    fin = fopen(argv[1],"r");
    while( fgets(buff, 1024, fin) ) {
        switch( count ) {
            case 2: sscanf(buff, "%s %s %s", s1, s2, sac_out); break;
            case 5: sscanf(buff, "%s %s %s", s1, s2, norm); break;
            case 8: sscanf(buff, "%s %s %f", s1, s2, &Nth_root); break;
            default: break;
        }

        if ( count >= 16 ) {
            sscanf(buff, "%s %f %f", sac_name[count-16], &shift_time[count-16], &weight[count-16]);
            line ++;
        }
        count ++;
    }
    fclose(fin);

    for ( j = 0; j < line; j ++ ) {
        peak = 0.;
        data = read_sac(sac_name[j], &hd);
        delta = hd.delta; npts = hd.npts;
        if ( j == 0 ) {
            sum = (float*) malloc(sizeof(npts) * npts);
            for ( i = 0; i < npts; i ++ )
                sum[i] = 0.;
        }
        if ( isnan(hd.depmax) != 1 ) {
            for ( k = 0; k < npts; k ++ ) {
                if ( fabs(data[k]) > peak )
                    peak = fabs(data[k]);
            }
            shift_index = (int)(shift_time[j]/delta);
            for ( i = 0; i < npts; i ++ ) {
                tmp_index = i + shift_index;
                if ( tmp_index >= 0 && tmp_index < npts ) {
                    if ( strcmp(norm, "true") == 0 ) {
                        data[tmp_index] /= peak;
                        tmp1 = sign(data[tmp_index]) * pow(fabs(data[tmp_index]), 1./Nth_root) * weight[j];
                    } else {
                        tmp1 = sign(data[tmp_index]) * pow(fabs(data[tmp_index]), 1./Nth_root) * weight[j];
                    }
                    sum[i] += tmp1/line;
                }
                else sum[i] += 0.;
            }
        }
        else continue;
    }


    for ( i = 0; i < npts; i ++ ) {
        tmp2 = sign(sum[i]) * pow(fabs(sum[i]), Nth_root);
        sum[i] = tmp2;
    }


    write_sac(sac_out, hd, sum);
    free(sum); free(data);
    return 0;
}
