#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#define max(a,b)(a>b?a:b)

#define TIME_MAX 2000
#define K_MAX 1000000
#define REG 2

//Since there are no polyfit on C/C++, so have referenced here.
int polyfit(const double* const dependentValues,
            const double* const independentValues,
            unsigned int        countOfElements,
            unsigned int        order,
            double*             coefficients)
{
    // Declarations...
    // ----------------------------------
    enum {maxOrder = 5};
    
    double B[maxOrder+1] = {0.0f};
    double P[((maxOrder+1) * 2)+1] = {0.0f};
    double A[(maxOrder + 1)*2*(maxOrder + 1)] = {0.0f};
    
    double x, y, powx;
    
    unsigned int ii, jj, kk;
    
    //printf( "for-1\n");
    
    // Verify initial conditions....
    // ----------------------------------
    
    // This method requires that the countOfElements >
    // (order+1)
    if (countOfElements <= order)
        return -1;
    
    // This method has imposed an arbitrary bound of
    // order <= maxOrder.  Increase maxOrder if necessary.
    if (order > maxOrder)
        return -1;
    
    // Begin Code...
    // ----------------------------------
    
    // Identify the column vector
    for (ii = 0; ii < countOfElements; ii++)
    {
        x    = dependentValues[ii];
        y    = independentValues[ii];
        powx = 1;
        
        for (jj = 0; jj < (order + 1); jj++)
        {
            B[jj] = B[jj] + (y * powx);
            powx  = powx * x;
        }
    }
    
    // Initialize the PowX array
    P[0] = countOfElements;
    
    // Compute the sum of the Powers of X
    for (ii = 0; ii < countOfElements; ii++)
    {
        //printf( "for0\n");
        x    = dependentValues[ii];
        powx = dependentValues[ii];
        
        for (jj = 1; jj < ((2 * (order + 1)) + 1); jj++)
        {
            P[jj] = P[jj] + powx;
            powx  = powx * x;
        }
    }
    
    // Initialize the reduction matrix
    //
    for (ii = 0; ii < (order + 1); ii++)
    {
        //printf( "for1\n");
        for (jj = 0; jj < (order + 1); jj++)
        {
            A[(ii * (2 * (order + 1))) + jj] = P[ii+jj];
        }
        
        A[(ii*(2 * (order + 1))) + (ii + (order + 1))] = 1;
    }
    
    // Move the Identity matrix portion of the redux matrix
    // to the left side (find the inverse of the left side
    // of the redux matrix
    for (ii = 0; ii < (order + 1); ii++)
    {
        //printf( "for2\n");
        x = A[(ii * (2 * (order + 1))) + ii];
        if (x != 0)
        {
            for (kk = 0; kk < (2 * (order + 1)); kk++)
            {
                A[(ii * (2 * (order + 1))) + kk] =
                A[(ii * (2 * (order + 1))) + kk] / x;
            }
            
            for (jj = 0; jj < (order + 1); jj++)
            {
                if ((jj - ii) != 0)
                {
                    y = A[(jj * (2 * (order + 1))) + ii];
                    for (kk = 0; kk < (2 * (order + 1)); kk++)
                    {
                        A[(jj * (2 * (order + 1))) + kk] =
                        A[(jj * (2 * (order + 1))) + kk] -
                        y * A[(ii * (2 * (order + 1))) + kk];
                    }
                }
            }
        }
        else
        {
            // Cannot work with singular matrices
            return -1;
        }
    }
    
    // Calculate and Identify the coefficients
    for (ii = 0; ii < (order + 1); ii++)
    {
        //printf( "for3\n");
        for (jj = 0; jj < (order + 1); jj++)
        {
            x = 0;
            for (kk = 0; kk < (order + 1); kk++)
            {
                x = x + (A[(ii * (2 * (order + 1))) + (kk + (order + 1))] *
                         B[kk]);
            }
            coefficients[ii] = x;
        }
    }
    
    return 0;
}

double std_ran()
{
    
    int mean = 0;
    int std = 1;
    
    double u = rand() / (double)RAND_MAX;
    double v = rand() / (double)RAND_MAX;
    double x = sqrt(-2 * log(u)) * cos(2 * M_PI * v) * std + mean;
    
    return x;
}

int main( int argc, char *argv[])
{
    srand(time(NULL));
	//input
	double S ; //spot price
	double K ; //strike price
	double r ; //risk free interest rate
	double s ; //volatility
	double T ; //years
	double n ; //numbers of period
    double k ;
    printf("please enter S(spot price), K(strike price), T(years), r(risk free interest rate), s(volatility), n(numbers of period), k(paths) one by one\n");
	scanf("%lf %lf %lf %lf %lf %lf %lf", &S, &K, &T, &r, &s, &n, &k);
    if( n > TIME_MAX)
        n = TIME_MAX - 1;
    if( k > K_MAX)
        k = K_MAX - 1;
	//output
    double put_price;
	double standard_deviation;
    
    //final
    double *final_price = malloc(K_MAX * TIME_MAX * sizeof(double));
    double *average_price = malloc(K_MAX * TIME_MAX * sizeof(double));
    double *user_price = malloc(K_MAX * sizeof(double));
    double *X = malloc(K_MAX * sizeof(double));;
    double *Y = malloc(K_MAX * sizeof(double));;
    double final_calculate;
    double ran[REG + 1];
    int pick;
    double random;
    
	//constant
    double t = T / n;
    double f = exp( - r * t);
    double coef1 = t * ( r - 0.5 * pow(s , 2));
    double coef2 = s * pow( t , 0.5);
    double reg;
    for( int methods = 0; methods < k ; methods++)
    {
        final_price[ TIME_MAX * methods ] = S;
        average_price[ TIME_MAX * methods ] = S;
    }
    for( int times = 1; times <= n ; times++)
    {
        for( int methods = 0 ;methods < k ; methods ++ )
        {
            random = std_ran();
            //printf( "%lf\n" , random);
            final_price[ TIME_MAX * methods + times ] = final_price[ TIME_MAX * methods + times - 1 ] * exp( coef1 + random * coef2);
            average_price[ TIME_MAX * methods + times ] = (average_price[ TIME_MAX * methods + times - 1 ] * (times) + final_price[ TIME_MAX * methods + times]) / (times + 1);
            /*if( methods < 20 && times == 1)
                printf( "%lf\n" , random);*/
        }
        //printf( "%d %lf %lf\n", times, average_price[ TIME_MAX * 0 + times ], average_price[ TIME_MAX * 6666 + times ] );
    }
    for( int methods = 0; methods < k; methods++)
        user_price[methods] = max(((K - average_price[(int)(TIME_MAX * methods + n)])) , 0);
    for( int times = n; times > 0; times-- )
    {
        pick = 0;
        for( int methods = 0; methods < k; methods ++)
        {
            if( average_price[ TIME_MAX * methods + times ] > 0)
            {
                X[pick] = average_price[TIME_MAX * methods + times];
                Y[pick] = f * user_price[methods];
                pick++;
            }
        }
        //printf( "%d\n", times);
        polyfit(X, Y, pick, REG, ran);
        //printf( "%d %lf %lf\n", times, user_price[ 0 ], user_price[ 6666 ] );
        for( int methods = 0, picking = 0 ; methods < k; methods ++)
        {
            if( average_price[ TIME_MAX * methods + times ] > 0)
            {
                if( ((ran[2] + ran[1] * X[picking] + ran[0] * pow( X[picking] , 2)) < X[picking]))
                {
                    //printf( "DO!");
                    user_price[methods] = (ran[2] + ran[1] * X[picking] + ran[0] * pow( X[picking] , 2));
                }
                else
                    user_price[methods] = f * user_price[methods];
            }
            else
                user_price[methods] = f * user_price[methods];
        }
    }
    
    double mean;
    
    put_price = 0;
    for( int methods = 0 ; methods < k; methods++)
        put_price += user_price[methods] * f / k;
    mean = put_price;
    put_price = put_price;
    if ( K - S > put_price)
        put_price = K - S;
    
    double std_error = 0;
    for( int methods = 0; methods < k; methods++)
        std_error +=  pow((user_price[methods] - mean) , 2) / k;
    std_error = pow( std_error, 0.5) / pow( k , 0.5);
    
    
    printf( "put price = %lf\n", put_price);
    printf( "standard error = %lf\n", std_error);


	//variables

    free(final_price);
    free(average_price);
    free(user_price);
    free(X);
    free(Y);
    return 0;
}


