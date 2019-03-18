#include <stdio.h>
#include <math.h>
#define max(a,b)(a>b?a:b)

int main( int argc, char *argv[])
{
	//input
	double S; //spot price
	double K; //strike price
	double r; //risk free interest rate 
	double s; //volatility
	double T; //years
	double n; //numbers of period
    printf("please enter S(spot price), K(strike price), r(risk free interest rate), s(volatility), T(years), n(numbers of period) one by one\n");
	scanf("%lf %lf %lf %lf %lf %lf", &S, &K, &r, &s, &T, &n);
	//output
	double put_price;
	double delta;

	//constant

	//variables
	double vu_pow = pow( s, 1 ) * pow( T/n , 0.5 );
	double u = exp( vu_pow ); // u = e ^ ( s^(1/2) * (1/n)^(1/2) )
	double d = 1/u;
	double q_pow = exp ( r * T/n ) ;
	double q = ( q_pow - d ) / ( u - d ); // q = (e^r(1/n) - d) / (u-d)
    //double q = 1 - p; //str1off
    //double mew = q * log(u/d)
    //double q = 1/2 + 1/2 *
	
    //calcaulate:price
	double price[1000000] = {0};
	int choose = 0;
    double option1 , option2;
    double price_for_delta;
    
	for( int time = n ; time >= 0 ; time-- ) //T *n
	{
		//every recursion here means the Price of option of "time"
		for( choose = 0 ; choose <= time ; choose++ )
		{
            price_for_delta = price[ 0 ]; //price[0] of secound round will be rushed, which may be important for calculating delta
			//every recursion here means the "price", from highest to lowest
            option1 = (K - (pow( u , (time - choose ) ) * pow( d , choose ) * S));
            option2 = (exp(-( r * T/n )) * ( price[ choose ] * q + price [ choose + 1 ] * (1 - q)));
            //printf( "%lf %lf \n" , option1 , option2 );
			price[ choose ] = max ( option1 , option2 ); //str2off
            //printf( "%lf\n" , price [choose] );
            if ( choose >= 1000000)
                break; //prevent from bus error
		}
	}
    //printf( "%lf\n" , pow(q_pow , n));
    double final_price = price[0];
    //printf( "%lf %lf %lf %lf " , q_pow , u , d , q);
    printf( "put price = %lf\n" , final_price );
    
    //caculate:delta
    //delta = ( max(0 , (final_price -  final_price * u )) - max(0 , (final_price - final_price * d)) ) / ((S * u) - (S * d));
    //delta = (final_price * u - final_price * d) / ( S * u - S * d);
    delta = ( price_for_delta - price[1] ) / ( S * u - S * d );
    printf( "delta = %lf\n" , delta);
    return 0;
}


