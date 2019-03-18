#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int DAY = 200;
int DIVERGE = 10000;
int NODE_VARIANCE = 30;
int DAY_VARIANCE = 30;

#define MINDAY 40
#define MINNODE 2000
#define YEAR 365

long double min_( long double a, long double b)
{
    if( a > b)
        return b;
    return a;
}

long double max_( long double a, long double b)
{
    if( a > b)
        return a;
    return b;
}

int flag = 0;

int array(int i, int j, int k)
{
    /*if ( i == 13 && j == 76)
    {
        flag = 100;
        printf( "CHECK on %d %d %d \n", i , j , k );
    }
    else if ( flag > 0)
    {
        printf( "CHECK on %d %d %d \n", i , j , k );
        flag--;
    }*/
    if( j > DIVERGE/2)
    {
        //printf( "Too big!" );
        j = DIVERGE/2 - 1;
    }
    if( j < -DIVERGE/2)
    {
        //printf( "Too small!" );
        j = -DIVERGE/2 + 1;
    }
    return i * DIVERGE * (NODE_VARIANCE + 10) + j * (NODE_VARIANCE + 10) + k ;
}

int array2(int i, int j, int k, int l)
{
    if( j > DIVERGE/2)
    {
        //printf( "Too big!" );
        j = DIVERGE/2 - 1;
    }
    if( j < -DIVERGE/2)
    {
        //printf( "Too small!" );
        j = -DIVERGE/2 + 1;
    }
    return i * DIVERGE * NODE_VARIANCE * DAY_VARIANCE + j * NODE_VARIANCE * DAY_VARIANCE + k * DAY_VARIANCE + l + 50;
}

int level(int a)
{
    //!
    int a_;
    for( a_ = 1 ; a > 0 ; a--)
        a_ = a_ * a;
    return a_;
}

int jump_calculater( long double variance , long double daily_rate,long double gamma, int n1)
{
    //Page 925
    //printf( "%lf %lf %lf %d\n" , variance, daily_rate, gamma, n1);
    int jump;
    long double low, mid, high;
    jump = (int)(ceil(pow(variance , 0.5) / gamma));
    while(1)
    {
        low = fabsl( daily_rate - (variance/2) ) / (2 * jump * gamma * pow( n1 , 0.5 ));
        mid = variance / (2 * pow(jump , 2) * pow(gamma , 2));
        high = min_( 0.5 , (1 - low));
        //printf( "%d\n" , jump);
        //printf( "%lf %lf %lf\n" , low, mid, high);
        if (low <= mid && mid <= high)
            break;
//        if (mid < low)
//            printf( "wrong" );
        jump++;
    }
    return jump;
}

long double prob_calculater(long double pu,long double pm,long double pd,int n1,int l)
{
    long double prob = 0;
    int jm, jd, ju;
    for(ju = 0; ju <= n1 ; ju++ )
    {
        jd = ju - l;
        jm = n1 - ju - jd;
        if( jm >= 0 && jd >= 0)
            prob += (level(n1) / (level(ju) * level(jm) * level(jd))) * pow( pu , ju) * pow( pm , jm) * pow( pd , jd);
    }
    return prob;
}

int main( int argc , char* argv[])
{
    
    /*     input     */
    
    int E ; //days before expiration
    long double r ; //risk free interest rate
    long double S ; //spot price
    long double h0, b0, b1, b2, c ;
    long double K ; //strike price
    int n1, n2;
    printf("please enter E(days before expiration), r(risk free interest rate,percentage), S(stock price), h0, b0, b1, b2, c, K(strike price), n1(numbers of partition per day), n2(numbers of variance per node) one by one\n");
    scanf("%d %LF %LF %LF %LF %LF %LF %LF %LF %d %d", &E, &r, &S, &h0, &b0, &b1, &b2, &c, &K, &n1, &n2);
    r = r / 100;
//    E = 40;
//    r = 0.06;
//    S = 100;
//    h0 = 0.010469;
//    b0 = 0.000006575;
//    b1 = 0.9;
//    b2 = 0.04;
//    c = 0;
//    K = 100;
//    n1 = 3;
//    n2 = 3;
    
    DAY = max_(E + 2 , MINDAY);
    DIVERGE = max_(MINNODE , r *100);
    NODE_VARIANCE = 2 * n2 + 10;
    DAY_VARIANCE = 2 * n1 + 10;
    
    //prevent from bus error
    if( E >= DAY)
        E = DAY - 1;
    if( n2 >= NODE_VARIANCE)
        n2 = NODE_VARIANCE - 1;
    if( 2 * n1 + 1 >= DAY_VARIANCE)
        n1 = DAY_VARIANCE / 2 - 2;
    if( n2 < 2 )
        n2 = 2;
    
    /*     tree     */
    
    long double *price_tree = calloc((DAY + 1) * DIVERGE * (NODE_VARIANCE + 10)  , sizeof(long double) );
    int *jump_tree = calloc((DAY + 1) * DIVERGE * (NODE_VARIANCE + 10) , sizeof(int) );
    long double *variance_tree = calloc((DAY + 1) * DIVERGE * (NODE_VARIANCE + 10) , sizeof(long double) );
    long double *probabily_tree = calloc(DAY * DIVERGE * NODE_VARIANCE * DAY_VARIANCE + 100 , sizeof(long double) );
    
    /*     variable     */
    
    long double daily_rate = r / YEAR ;
    long double gamma = h0;
    long double gamma_n = h0 / pow( n1 , 0.5 ); //Page 914
    long double variable_buffer;
    
    /*     initialize     */
    for( int k = 0; k < n2 ; k++ )
        variance_tree[array(0,0,k)] = pow( h0, 2 );
    variance_tree[array(0,0,NODE_VARIANCE)] = 1; //means there has data;
    variance_tree[array(0,0,NODE_VARIANCE+1)] = pow( h0 , 2 ); //means min
    variance_tree[array(0,0,NODE_VARIANCE+2)] = pow( h0 , 2 ); //means max
    int *max = calloc(( DAY + 1) , sizeof(int));
    int *min = calloc(( DAY + 1) , sizeof(int));
    
    /*     useless variable     */
    int jump;
    int j2;
    int m;
    long double variance, variance2;
    long double DRV;
    long double pu, pm, pd;
    long double min_var, max_var;
    
    /*     forward     */
    for( int i = 0 ; i < E ; i++ )
    {
        for( int j = min[i] ; j <= max[i] ; j++ )
        {
            if( variance_tree[array(i,j,NODE_VARIANCE)] == 0 ) //means empty
                continue;
            for( int k = 0 ; k < n2 ; k++)
            {
                //jump_tree
                variance = variance_tree[array(i,j,k)];
                //( " %lf\n" , variance);
                jump = jump_calculater( variance, daily_rate, gamma, n1);
                jump_tree[array( i, j ,k)] = jump;
                //Page 917
                pu = variance / (2 * pow(jump , 2) * pow(gamma , 2)) + ( daily_rate - (variance / 2)) / ( 2 * jump * gamma * pow(n1 , 0.5));
                pm = 1 - variance / (pow(jump , 2) * pow(gamma , 2));
                pd = variance / (2 * pow(jump , 2) * pow(gamma , 2)) - ( daily_rate - (variance / 2)) / ( 2 * jump * gamma * pow(n1 , 0.5));
                for( int l = -n1 ; l <= n1 ; l++)
                {
                    //printf( "%d %d %d %d " , i, j, k,l );
                    probabily_tree[array2(i , j, k, l)] = prob_calculater( pu, pm, pd, n1, l);
                    j2 = j + jump * l;
                    //printf( "jump = %d " , jump);
                    max[i+1] = max_( max[i+1] , j2);
                    min[i+1] = min_( min[i+1] , j2);
                    //printf( "%d \n" , jump);
                    //Page 923
                    DRV = (l * jump * gamma_n - daily_rate + variance/2) / pow(variance , 0.5);
                    variance2 = b0 + b1 * variance + b2 * variance * pow((DRV - c) , 2);
                    //printf( "%Lf\n" , variance2);
                    //if( i <= 2)
                    //printf( "%d %d %d %d %LF\n" , i , j , k, l , probabily_tree[array2(i , j, k, l)]);
                    /*if( variance > 0.001)
                    {
                        printf( "WARNING on %d %d %d %d %LF\n", i , j , k , l ,variance);
                        printf( " %d\n" , array(i,j,k) );
                    }*/
                    if( variance_tree[array(i+1,j2,NODE_VARIANCE)] == 0 )
                    {
                        variance_tree[array(i+1,j2,NODE_VARIANCE+1)] = variance2;
                        variance_tree[array(i+1,j2,NODE_VARIANCE+2)] = variance2;
                    }
                    else
                    {
                        variance_tree[array(i+1,j2,NODE_VARIANCE+1)] = min_(variance2 , variance_tree[array(i+1,j2,NODE_VARIANCE+1)]);
                        variance_tree[array(i+1,j2,NODE_VARIANCE+2)] = max_(variance2 , variance_tree[array(i+1,j2,NODE_VARIANCE+2)]);
                    }
                    variance_tree[array(i+1,j2,NODE_VARIANCE)] = 1;
                }
            }
        }
        for( int j = min[i+1] ; j <= max[i+1] ; j++ )
        {
//            if( variance_tree[array(i+1,j,NODE_VARIANCE)] == 0 ) //means empty
//                break;
            if( variance_tree[array(i+1 , j , NODE_VARIANCE+1)] != 0)
            {
                for( int k = 0 ; k < n2 ; k++)
                {
                    variable_buffer = variance_tree[array(i+1 , j , NODE_VARIANCE+1)] + (variance_tree[array(i+1 , j , NODE_VARIANCE+2)] -variance_tree[array(i+1 , j , NODE_VARIANCE+1)]) * k / (n2 - 1);
                    variance_tree[array(i+1 , j , k)] = variable_buffer;
//                    if( variable_buffer > 0.001)
//                    {
//                        printf( "WARNING on %d %d %d %d %LF\n", i , j , k  ,variable_buffer);
//                        printf( "%d" , array)
//                    }
                }//Page 955
            }
        }
        //printf( "%lf\n" , jump_tree[array(i,0,0)]);
    }
    
    //for( int i = 0 ; i <= E ; i++)
        //printf( "%d %d %d \n" , i ,max[i] , min[i]);
    
    /*     strike day     */
    for( int j = min[E] ; j <= max[E] ; j++ )
        for( int k = 0 ; k < n2 ; k++ )
            price_tree[ array(E, j, k ) ] = max_( K - S * exp(gamma_n * j) , 0);
    //printf( "%LF\n" , price_tree[ array(E, -20, 0 ) ]);
    
    /*     backward     */
    long double put_price = 0, l_price;
    long double low, high;
    long double x;
    for( int i = E - 1 ; i >= 0 ; i-- )
    {
        for( int j = min[i] ; j <= max[i] ; j++ )
        {
            if( variance_tree[array(i,j,NODE_VARIANCE)] == 0 ) //means empty
                continue;
            for( int k = 0 ; k < n2 ; k++ )
            {
                put_price = 0;
                for( int l = -n1 ; l <= n1; l++)
                {
                    jump = jump_tree[ array(i , j , k)];
                    variance = variance_tree[ array( i , j , k)];
                    j2 = j + jump * l;
                    DRV = (l * jump * gamma_n - daily_rate + variance/2) / pow(variance , 0.5);
                    variance2 = b0 + b1 * variance + b2 * variance * pow((DRV - c) , 2);
                    low = variance_tree[array(i+1 , j2 , NODE_VARIANCE+1)];
                    high = variance_tree[array( i+1 , j2 , NODE_VARIANCE+2)];
                    if( low != high)
                    {
                        for( m = 1 ; m < n2 ; m++ )
                        {
                            low = variance_tree[array(i+1 , j2 , m-1)];
                            high = variance_tree[array( i+1 , j2 , m)];
                            x = (variance2 - high) / (low - high);
                            if( x <= 1 && x >= 0 )
                                break;
                        }
                        
                    }
                    if( low == high)
                        x = 0.5;
                    
                    //printf( " %LF \n" , x);
                        
                    l_price = x * price_tree[ array(i+1, j2, m-1)] + (1 - x) * price_tree[ array(i+1, j2, m)];
                    put_price += probabily_tree[array2(i, j ,k , l)] * l_price;
                }
                price_tree[array(i,j,k)] = put_price / exp(daily_rate);
                /*if( i == 29 && j == 15)
                    printf( "%LF\n" , put_price );*/
            }
        }
    }
            
    printf( "\noption price = %LF\n" , price_tree[array(0,0,0)]);
    
    /*     free     */
    free ( jump_tree );
    free ( variance_tree );
    free ( probabily_tree );
    free ( price_tree );
    return 0;
}
