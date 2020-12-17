#include <math.h>
#include <stdio.h>
#include <time.h>
#include "mysql.h"

#define HOSTNAME "localhost"
#define USER "Kenneths_Iphone"
#define PASSWORD "kenneth123sej"
#define DATABASE "trilateration"
#define TABLEAP "PlaceringAP"
#define TABLERSSI "RSSI"
#define TABLE "Placering"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                                                 //
//                                                          Trilateration af client og indsættelse i database                                                      //
//                                                                                                                                                                 //
//                                                                              lavet af                                                                           //
//                                                                                                                                                                 //
//                                          Christian Faurholt, Christoffer Ejsing, David Waltenburg, Kenneth Langballe Flye                                       //
//                                                  Mads Steentoft, Rasmus Louie Jensen & Susanne Miranda Aalestrup                                                //
//                                                                                                                                                                 //
//                                                                                                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Faster værdier/////
int columns = 3; //
int AP = 3;      //
//////////////////////

// Matrix dannelse////////////
int m[3];                  //
int n[3];                  //
float r[1000][3];          //
int RSSI1[1000][3];        //
float Ab[2][3];            //
int V[3][3];               //
int placering[100][100];   //
int RSSI[100][100];        //
int MAC[100];              //
float Enheder[1000][1000]; //
char navnTilMatrix[1000];  //
//////////////////////////////

// Counters //////////////
int antalID = 0;      //
int counter = 0;      //
int matrixRow = 0;    //
int matrixColumn = 0; //
//////////////////////////

// Pointers///////////
int *pointer;    //
float *pointerf; //
//////////////////////

// Funktioner brugt i kode ///////////////////////////////////////
//
int oploeftetI(int i, int j)    //
{                               //
    int res = 1;                //
    for (int a = 0; a < j; a++) //
    {                           //
        res = res * i;          //
    }                           //
    return res;                 //
} //
  //
  //
void delay(int number_of_seconds) //
                                  //
{                                 //
                                  //
    // Converting time into milli_seconds                       //
    //
    int milli_seconds = 1000 * number_of_seconds; //
                                                  //
    // Storing start time                                       //
    //
    clock_t start_time = clock(); //
                                  //
    // looping till required time is not achieved               //
    //
    while (clock() < start_time + milli_seconds)
        ; //
} //
  //
//////////////////////////////////////////////////////////////////

int main()
{
    ///////////////////////////////////////
    // Indsamling af Statiske AP værdier//
    ////////////////////////////////////////////////////////////////////////

    char query[2000];
    MYSQL *conn;
    conn = mysql_init(NULL);
    conn = mysql_real_connect(conn, HOSTNAME, USER, PASSWORD, DATABASE, 3306, NULL, 0);
    if (conn == NULL)
    {
        printf("error during connection\n");
        exit(1);
    }
    sprintf(query, "SELECT * FROM PlaceringAP");
    int res = mysql_query(conn, query);

    if (res != 0)
    {
        printf("error during query %d\n", res);
        exit(1);
    }

    MYSQL_RES *sql_result;
    MYSQL_ROW row;

    sql_result = mysql_store_result(conn);
    if (sql_result)
    {
        int num_fields = mysql_num_fields(sql_result);

        printf("columns %d, rows %llu\n", num_fields, sql_result->row_count);

        row = mysql_fetch_row(sql_result);

        while (row)
        {
            printf("Name: %s x: %s y: %s \n", row[0], row[1], row[2]);
            placering[matrixRow][matrixColumn] = atoi(row[1]);
            matrixRow++;
            placering[matrixRow][matrixColumn] = atoi(row[2]);
            matrixRow--;
            matrixColumn++;
            row = mysql_fetch_row(sql_result);
        }
        matrixColumn = 0;
        matrixRow = 0;
        for (int i = 0; i < AP; i++)
        {
            printf("x_%i er %i og y_%i er %i \n", i + 1, placering[0][i], i + 1, placering[1][i]);
        }

        mysql_free_result(sql_result);
    }

    // Ser om man kan trække værdierne ud af overliggende if statement.

    for (int i = 0; i < AP; i++)
    {
        for (int j = 0; j < columns - 1; j++)
        {
            if (j == 0)
            {
                *(m + i) = placering[j][i];
            }
            if (j == 1)
            {
                *(n + i) = placering[j][i];
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    while (true)
    {
        /////////////////////////
        //  RSSI Indsamling    //
        /////////////////////////////////////////////////////////////////////////////

        char query1[2000];
        //MYSQL *conn;
        conn = mysql_init(NULL);
        conn = mysql_real_connect(conn, HOSTNAME, USER, PASSWORD, DATABASE, 3306, NULL, 0);
        if (conn == NULL)
        {
            printf("error during connection\n");
            //exit(1);
        }
        sprintf(query1, "SELECT * FROM RSSI");
        res = mysql_query(conn, query1);

        if (res != 0)
        {
            printf("error during query %d\n", res);
            //exit(1);
        }

        //MYSQL_RES *sql_result;
        //MYSQL_ROW row;

        sql_result = mysql_store_result(conn);
        if (sql_result)
        {
            int num_fields = mysql_num_fields(sql_result);

            //printf("columns %d, rows %llu\n", num_fields, sql_result->row_count);

            row = mysql_fetch_row(sql_result);
            matrixColumn = 0;
            while (row)
            {
                //printf("Mac: %s RSSI1: %s RSSI2: %s RSSI3: %s Tidspunkt: %s \n", row[0], row[1], row[2], row[3], row[4]); //Hvis der er 3 access points
                //printf("Mac: %s RSSI1: %s RSSI2: %s RSSI3: %s RSSI4: %s Tidspunkt: %s \n", row[0], row[1], row[2], row[3], row[4], row[5]); //Hvis der er 4 access points
                MAC[matrixColumn] = atoi(row[0]);

                RSSI[matrixColumn][matrixRow] = atoi(row[1]);
                matrixRow++;

                RSSI[matrixColumn][matrixRow] = atoi(row[2]);
                matrixRow++;

                RSSI[matrixColumn][matrixRow] = atoi(row[3]);
                matrixRow++;

                //RSSI[matrixColumn][matrixRow] = atoi(row[4]); //Hvis der er 4 access points

                matrixRow = 0;
                matrixColumn++;
                antalID++;
                row = mysql_fetch_row(sql_result);
            }
            matrixColumn = 0;
            matrixRow = 0;
            for (int i = 0; i < sql_result->row_count; i++)
            {
                //printf("MAC er %i og RSSI1 er %i og RSSI2 er %i og RSSI3 er %i /*og RSSI4 er %i*/ \n", MAC[i], RSSI[i][0], RSSI[i][1], RSSI[i][2]/*, RSSI[i][3]*/);       //Hvis der er 4 access points
                //printf("MAC er %i og RSSI1 er %i og RSSI2 er %i og RSSI3 er %i \n", MAC[i], RSSI[i][0], RSSI[i][1], RSSI[i][2]); //Hvis der er 3 access points
            }

            mysql_free_result(sql_result);
        }

        for (int i = 0; i < antalID; i++)
        {
            for (int j = 0; j < AP; j++)
            {
                *(*(RSSI1 + i) + j) = RSSI[i][j];
            }
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        /////////////////////////
        //  Path loss model    //
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // Udregning af r[antalID][AP]
        int dlink_d0 = -36; //
        int asus_d0 = -43;  //
        int d0 = 100;       //
        int PLE = 5;        //
        double ten = 10;    //
        //////////////////////////////

        for (int i = 0; i < antalID; i++)
        {
            double mel1 = RSSI[i][0] - (dlink_d0);
            //printf("mel1 %lf \n", mel1);
            double mel2 = mel1 / (10 * PLE);
            //printf("mel2 %lf \n", mel2);
            double mel3 = pow(10, mel2);
            //printf("mel3 %lf \n", mel3);

            double resul = mel3 * 100;
            *(*(r + i) + 0) = resul;
            //printf("%f\n", r[i][0]);
            //pointer = &r[i][0];
            //*pointer = mel3 * 100;
            //printf("resultat %.10lf \n", r[i][0]);

            mel1 = RSSI[i][1] - (dlink_d0);
            //printf("mel1 %lf \n", mel1);
            mel2 = mel1 / (10 * PLE);
            //printf("mel2 %lf \n", mel2);
            mel3 = pow(10, mel2);
            //printf("mel3 %lf \n", mel3);

            resul = mel3 * 100;
            *(*(r + i) + 1) = resul;
            //printf("%f\n", r[i][1]);
            //pointer = &r[i][1];
            //*pointer = mel3 * 100;
            //printf("resultat %.10lf \n", r[i][1]);

            mel1 = RSSI[i][2] - (asus_d0);
            //printf("mel1 %lf \n", mel1);
            mel2 = mel1 / (10 * PLE);
            //printf("mel2 %lf \n", mel2);
            mel3 = pow(10, mel2);
            //printf("mel3 %lf \n", mel3);
            resul = mel3 * 100;

            *(*(r + i) + 2) = resul;
            //printf("%f\n", r[i][2]);
            //pointer = &r[i][2];
            //*pointer = mel3 * 100;
            //printf("resultat %.10lf \n", r[i][2]);
        }
        // Counter loop
        for (int c = 0; c < antalID; c++)
        {
            ////////////////////
            //  Værdi matrix  //
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //
            //float V[AP][columns];
            for (int i = 0; i < AP; i++)
            {
                for (int j = 0; j < columns; j++)
                {
                    if (j == 0)
                    {
                        //*(*(r + i) + 2) = resul;
                        *(*(V + i) + j) = m[i];
                        //printf("m[1] = %i \n", m[1]); //
                        //printf("V Matrix 1 %i ", V[i][j]);
                        //V[i][j] = m[i];           //
                    }           //
                    if (j == 1) //
                    {
                        *(*(V + i) + j) = n[i]; //
                        //printf("V Matrix 2 %i ", V[i][j]);
                        //V[i][j] = n[i];           //
                    }           //
                    if (j == 2) //
                    {
                        *(*(V + i) + j) = r[c][i]; //
                        //printf("V Matrix 3 %i \n", V[i][j]);
                        //V[i][j] = r[counter][i];  //
                    } //
                }
            }

            ////////////////////
            // Ax = b matrix //
            ////////////////////////////////////////////////

            for (int i = 0; i < AP - 1; i++)      // Kør først række 1 derefter række 2 osv.
            {                                     //
                for (int j = 0; j < columns; j++) // Kør Søjle 1 derefter 2 osv.
                {                                 //
                    if (j < 2)                    // Hvis det er første søjle, udregn x værdien
                    {
                        *(*(Ab + i) + j) = -2 * (V[i][j] - V[i + 1][j]);
                        //printf("A test print = %f", Ab[i][j]);
                        //pointerf = &Ab[i][j];
                        //*pointerf = -2 * (V[i][j] - V[i + 1][j]);
                    }    //
                    else // Hvis det er tredje søjle udregn b værdien
                    {
                        *(*(Ab + i) + j) = pow(V[i][j], 2) - pow(V[i][j - 2], 2) - pow(V[i][j - 1], 2) - pow(V[i + 1][j], 2) + pow(V[i + 1][j - 2], 2) + pow(V[i + 1][j - 1], 2);
                        float testeren = pow(V[i][j], 2) - pow(V[i][j - 2], 2) - pow(V[i][j - 1], 2) - pow(V[i + 1][j], 2) + pow(V[i + 1][j - 2], 2) + pow(V[i + 1][j - 1], 2);
                        //printf("Ab Matrix test = %f \n", testeren);
                        //printf("b test print = %f", Ab[i][j]);
                        //pointerf = &Ab[i][j];
                        //*pointerf = pow(V[i][j], 2) - pow(V[i][j - 2], 2) - pow(V[i][j - 1], 2) - pow(V[i + 1][j], 2) + pow(V[i + 1][j - 2], 2) + pow(V[i + 1][j - 1], 2); //
                    }
                }
            }

            // if (AP < 4)

            float skalering1 = (-1 * Ab[1][0]) / Ab[0][0]; //
                                                           //
            Ab[1][2] = Ab[1][2] + (Ab[0][2] * skalering1); //
            Ab[1][1] = Ab[1][1] + (Ab[0][1] * skalering1); //
                                                           //
            float skalering2 = (-1 * Ab[0][1]) / Ab[1][1]; //
                                                           //
            Ab[0][2] = Ab[0][2] + (Ab[1][2] * skalering2); //
                                                           //
            float x, y;                                    //
                                                           //
            x = Ab[0][2] / Ab[0][0];                       //
                                                           //
            y = Ab[1][2] / Ab[1][1];
            printf("X er %f Y er %f \n", x, y);
            printf("Counter = %i", counter);

            //printf("\nI vores database %s ligger %s\n\n", DATABASE, TABLE);
            conn = mysql_init(NULL);
            conn = mysql_real_connect(conn, HOSTNAME, USER, PASSWORD, DATABASE, 3306, NULL, 0);
            if (conn == NULL)
            {
                printf("error during connection\n");
                //exit(1);
            }

            sprintf(query, "UPDATE trilateration.Placering SET x = %f, y = %f, Tidspunkt = CURRENT_TIME WHERE MAC = %i", x, y, MAC[c]);

            res = mysql_query(conn, query);

            if (res != 0)
            {
                printf("error during query %d\n", res);
                //exit(1);
            }

            sql_result = mysql_store_result(conn);
            mysql_free_result(sql_result);

            sprintf(query, "COMMIT;");

            res = mysql_query(conn, query);

            if (res != 0)
            {
                printf("error during query %d\n", res);
                //exit(1);
            }

            printf("\n done");
            printf("\n result: %d\n", res);
            mysql_close(conn);
            delay(5000);
        }
        // if (AP > 3)    !! Ikke rettet til efter pointers er blevet brugt pga. tidsmangel !!
        /*
        //////////////////////
                //  Ortogonal basis //
                //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                //
                float OrtogonalBasis[AP - 1][columns - 1]; //
                //printf("\n Ortogonal matrix\n");                                                                                                                                                               //
                for (int j = 0; j < columns - 1; j++)                                                                                                                                                          //
                {                                                                                                                                                                                              //
                    for (int i = 0; i < AP - 1; i++)                                                                                                                                                           //
                    {                                                                                                                                                                                          //
                        if (j == 0)                                                                                                                                                                            //
                        {                                                                                                                                                                                      //
                            OrtogonalBasis[i][j] = Ab[i][j];                                                                                                                                                   //
                        }                                                                                                                                                                                      //
                        if (j == 1)                                                                                                                                                                            //
                        {                                                                                                                                                                                      //
                            OrtogonalBasis[i][j] = (Ab[i][j] - (((OrtogonalBasis[j - 1][j - 1] * Ab[j - 1][j]) + (OrtogonalBasis[j][j - 1] * Ab[j][j]) + (OrtogonalBasis[j + 1][j - 1] * Ab[j + 1][j])) /      //
                                                                (pow(OrtogonalBasis[j - 1][j - 1], 2) + pow(OrtogonalBasis[j][j - 1], 2) + pow(OrtogonalBasis[j + 1][j - 1], 2)) * OrtogonalBasis[i][j - 1])); //
                        }                                                                                                                                                                                      //
                    }                                                                                                                                                                                          //
                }                                                                                                                                                                                              //
                //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                // Print Ortogonal basis //

                for (int i = 0; i < AP - 1; i++)
                {
                    for (int j = 0; j < columns - 1; j++)
                    {
                        //printf("%.2f      ", OrtogonalBasis[i][j]);
                    }
                    //printf("\n");
                }

                ///////////////////////////////////
                //  Normalen af OrtognalBasis    //
                //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                //
                float NormalV[columns - 1]; //
                //printf("\n Normalen for V1 og V2\n");                                                                              //
                for (int j = 0; j < columns - 1; j++)                                                                              //
                {                                                                                                                  //
                    NormalV[j] = sqrt(pow(OrtogonalBasis[0][j], 2) + pow(OrtogonalBasis[1][j], 2) + pow(OrtogonalBasis[2][j], 2)); //
                }                                                                                                                  //
                                                                                                                                   //
                //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                // Print Normalen //

                for (int i = 0; i < columns - 1; i++)
                {
                    //printf("%.2f      ", NormalV[i]);
                }
                //printf("\n");

                /////////////////////////////////////
                // Enhedsvektor af OrtogonalBasis  //
                //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                //
                float Q[AP - 1][columns - 1]; //
                //printf("\n Q matrix\n");                             //
                for (int i = 0; i < AP - 1; i++)                     //
                {                                                    //
                    for (int j = 0; j < columns - 1; j++)            //
                    {                                                //
                        Q[i][j] = OrtogonalBasis[i][j] / NormalV[j]; //
                    }                                                //
                }                                                    //
                //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                // Print Q //

                for (int i = 0; i < AP - 1; i++)
                {
                    for (int j = 0; j < columns - 1; j++)
                    {
                        //printf("%.2f      ", Q[i][j]);
                    }
                    //printf("\n");
                }

                //////////////////////
                //  Q transponeret  //
                //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                //
                float QT[columns - 1][AP - 1]; //
                //printf("\n Q transponeret matrix\n"); //
                for (int i = 0; i < columns - 1; i++) //
                {                                     //
                    for (int j = 0; j < AP - 1; j++)  //
                    {                                 //
                        QT[i][j] = Q[j][i];           //
                    }                                 //
                }                                     //
                                                      //
                //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                // Print Q transponeret //

                for (int i = 0; i < columns - 1; i++)
                {
                    for (int j = 0; j < AP - 1; j++)
                    {
                        //printf("%.2f      ", QT[i][j]);
                    }
                    //printf("\n");
                }

                ////////////////
                //  R matrix  //
                //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                //
                float R[columns - 1][AP - 2]; //
                //printf("\n R matrix\n");                                                                                               //
                for (int i = 0; i < columns - 1; i++)                                                                                  //
                {                                                                                                                      //
                    for (int j = 0; j < AP - 2; j++)                                                                                   //
                    {                                                                                                                  //
                        if (i == 0)                                                                                                    //
                        {                                                                                                              //
                            if (j == 0)                                                                                                //
                            {                                                                                                          //
                                R[i][j] = QT[i][j] * Ab[j][i] + QT[i][j + 1] * Ab[j + 1][i] + QT[i][j + 2] * Ab[j + 2][i];             //
                            }                                                                                                          //
                            else if (j == 1)                                                                                           //
                            {                                                                                                          //
                                R[i][j] = QT[i][j - 1] * Ab[j - 1][i + 1] + QT[i][j] * Ab[j][i + 1] + QT[i][j + 1] * Ab[j + 1][i + 1]; //
                            }                                                                                                          //
                        }                                                                                                              //
                        else if (i == 1)                                                                                               //
                        {                                                                                                              //
                            if (j == 0)                                                                                                //
                            {                                                                                                          //
                                R[i][j] = QT[i][j] * Ab[j][i - 1] + QT[i][j + 1] * Ab[j + 1][i - 1] + QT[i][j + 2] * Ab[j + 2][i - 1]; //
                            }                                                                                                          //
                            else if (j == 1)                                                                                           //
                            {                                                                                                          //
                                R[i][j] = QT[i][j - 1] * Ab[j - 1][i] + QT[i][j] * Ab[j][i] + QT[i][j + 1] * Ab[j + 1][i];             //
                            }                                                                                                          //
                        }                                                                                                              //
                    }                                                                                                                  //
                }                                                                                                                      //
                                                                                                                                       //
                //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                // Print R //

                for (int i = 0; i < columns - 1; i++)
                {
                    for (int j = 0; j < AP - 2; j++)
                    {
                        //printf("%.2f      ", R[i][j]);
                    }
                    //printf("\n");
                }

                //////////////////
                // QTb matrix   //
                //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                //
                float QTb[columns - 1]; //
                //printf("\n QTb matrix\n");                                                                                          //
                for (int j = 0; j < columns - 1; j++)                                                                               //
                {                                                                                                                   //
                    if (j == 0)                                                                                                     //
                    {                                                                                                               //
                        QTb[j] = (QT[j][j] * Ab[j][j + 2]) + (QT[j][j + 1] * Ab[j + 1][j + 2]) + (QT[j][j + 2] * Ab[j + 2][j + 2]); //
                    }                                                                                                               //
                    if (j == 1)                                                                                                     //
                    {                                                                                                               //
                        QTb[j] = QT[j][j - 1] * Ab[j - 1][j + 1] + QT[j][j] * Ab[j][j + 1] + QT[j][j + 1] * Ab[j + 1][j + 1];       //
                    }                                                                                                               //
                }                                                                                                                   //
                                                                                                                                    //
                //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                // Print QTb //

                for (int i = 0; i < columns - 1; i++)
                {
                    //printf("%.2f \n", QTb[i]);
                }
                //printf("\n");

                //////////////////////
                //  x og y værdi    //
                //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                //
                float x, y;                             //
                                                        //
                y = QTb[1] / R[1][1];                   //
                                                        //
                x = (QTb[0] - (R[0][1] * y)) / R[0][0]; //

                //printf("\nI vores database %s ligger %s\n\n", DATABASE, TABLE);
                conn = mysql_init(NULL);
                conn = mysql_real_connect(conn, HOSTNAME, USER, PASSWORD, DATABASE, 3306, NULL, 0);
                if (conn == NULL)
                {
                    printf("error during connection\n");
                    //exit(1);
                }

                sprintf(query, "UPDATE trilateration.Placering SET x = %f, y = %f WHERE MAC = %i", x, y, MAC[counter]);
                res = mysql_query(conn, query);

                if (res != 0)
                {
                    printf("error during query %d\n", res);
                    //exit(1);
                }

                sql_result = mysql_store_result(conn);
                mysql_free_result(sql_result);

                sprintf(query, "COMMIT;");

                res = mysql_query(conn, query);

                if (res != 0)
                {
                    printf("error during query %d\n", res);
                    //exit(1);
                }

                printf("\n done");
                printf("\n result: %d\n", res);
                mysql_close(conn);
        */

    }
}
