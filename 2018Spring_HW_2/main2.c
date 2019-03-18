#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define max(a,b)(a>b?a:b)

#define TIME 10000
#define BACKET 10000
#define MODE 1    //0 for Am = (all_node[n].node[node_number].Amin + (leveling_number * all_node[n].node[node_number].difference);, 1 for Am = (k - m)/k * all_node[n].node[node_number].Amin + m/k * all_node[n].node[node_number].Amax

typedef struct little_node{
    double Amin;
    double Amax;
    double difference;
    double Cprice[BACKET];
}L_NODE;

typedef struct big_node{
    L_NODE node[TIME];
}B_NODE;
    
    
int main( int argc, char *argv[])
{
    //input and output
    //input
    double S ; //spot price
    double K ; //strike price
    double H ; //barrier price
    double T ; //years
    double r ; //risk free interest rate
    double s ; //volatility
    int n ; //numbers of period
    double k ; //number of buckets
    printf("please enter S(spot price), K(strike price), H(barrier price), T(years), r(risk free interest rate), s(volatility), n(numbers of period), k(number of buckets) one by one\n");
    scanf("%lf %lf %lf %lf %lf %lf %d %lf", &S, &K, &H, &T, &r, &s, &n, &k);
    //output
    double final_price;
    double delta;
    
    //main structure
    B_NODE *all_node = malloc( TIME * sizeof(B_NODE) );
    B_NODE *nonh_node = malloc( TIME * sizeof(B_NODE) );
    
    //useful variables
    double vu_pow = pow( s, 1 ) * pow( T/n , 0.5 );
    double u = exp( vu_pow ); // u = e ^ ( s^(1/2) * (1/n)^(1/2) )
    double d = 1/u; // the value of downgoing
    double q_pow = exp ( r * T/n ) ;
    double q = ( q_pow - d ) / ( u - d ); // q = (e^r(1/n) - d) / (u-d)
    double R = exp( pow(r , 1) * T/n);
    
    //printf( "%lf %lf %lf %lf " , u , d , q , R)
    /* EXAMPLE TRYING */
    /*
    S = 64;
    K = 64;
    H = 11000000000;
    u = 2;
    d = 0.5;
    R = 1;
    q = 0.5;
    n = 2;
    k = 1;
    */
    
    //helper variables
    double lower_sum , higher_sum;
    double Amji,Nmji;
    
    //adder variables
    int node_number;
    int average_number;
    int leveling_number;
    
    //last period value( time = n ) from highest to lowest
    for( node_number = 0; node_number <= n ; node_number++ ) //n+1 nodes
    {
        //sum
        higher_sum = 0;
        lower_sum = 0;
        //printf("node number = %d\n", node_number);
        for ( average_number = 0 ; average_number <= ( n - node_number ) ; average_number++ )
            higher_sum += S * (pow ( u , average_number ));
        for ( ;average_number <= n ; average_number++ )
            higher_sum += S * (pow ( u , ( n - node_number ) ) * pow( d , (average_number - n + node_number)));
        for ( average_number = 0 ; average_number <= node_number ; average_number++ )
            lower_sum += S * (pow ( d , average_number ));
        for ( ; average_number <= n ; average_number++ )
            lower_sum += S * (pow ( d , node_number) * pow( u , (average_number - node_number)));
        
        /*DEBUG USE*/
        /*for ( average_number = 0 ; average_number <= ( n - node_number ) ; average_number++ )
            printf( "S*U^%d +", average_number);
        for ( ;average_number <= n ; average_number++ )
            printf( "S*U^%d*D^%d +", (n - node_number),(average_number - n + node_number));
        printf("\n");
        for ( average_number = 0 ; average_number <= node_number ; average_number++ )
            printf( "S*D^%d +", average_number);
        for ( ; average_number <= n ; average_number++ )
            printf( "S*D^%d*U^%d +", node_number , (average_number - node_number));
        printf("\n");*/
        all_node[n].node[node_number].Amin = lower_sum/(n + 1);
        all_node[n].node[node_number].Amax = higher_sum/(n + 1);
        all_node[n].node[node_number].difference = ( all_node[n].node[node_number].Amax - all_node[n].node[node_number].Amin ) / k ;
        //nonh_node
        nonh_node[n].node[node_number].Amin = lower_sum/(n + 1);
        nonh_node[n].node[node_number].Amax = higher_sum/(n + 1);
        nonh_node[n].node[node_number].difference = ( nonh_node[n].node[node_number].Amax - nonh_node[n].node[node_number].Amin ) / k ;
        
        for ( leveling_number = k ; leveling_number >= 0 ; leveling_number-- )
        {
            switch ( MODE )
            {
                case 0:
                    Amji = (all_node[n].node[node_number].Amin) + (leveling_number * all_node[n].node[node_number].difference);
                    Nmji = (nonh_node[n].node[node_number].Amin) + (leveling_number * nonh_node[n].node[node_number].difference);
                            break;
                case 1:
                    Amji = (leveling_number/k) * all_node[n].node[node_number].Amax + (k - leveling_number)/k * all_node[n].node[node_number].Amin;
                    Nmji = (leveling_number/k) * nonh_node[n].node[node_number].Amax + (k - leveling_number)/k * nonh_node[n].node[node_number].Amin;

                            break;
            }
            //if( all_node[n].node[node_number].Amax < H)
            if( Amji > H ) //CHANGE
            {
                //if( all_node[n].node[node_number].Amax < H)
                    all_node[n].node[node_number].Cprice[leveling_number] = 0;
                //else
                    //all_node[n].node[node_number].Cprice[leveling_number] = H - K;
            }
            else
                all_node[n].node[node_number].Cprice[leveling_number] = max( Amji - K , 0 );
            nonh_node[n].node[node_number].Cprice[leveling_number] = max( Amji - K , 0 );
            /*DEBUG USE*/
            //printf( "node_number = %d, leveling_number = %d, Cprice = %lf\n" , node_number, leveling_number, node[node_number].Cprice[leveling_number]);
            //printf( "time = %d, node_number = %d, leveling_number = %d, Cprice = %lf\n" , n, node_number, leveling_number, all_node[n].node[node_number].Cprice[leveling_number] );
        }
    }
    
    //secondary structure
    //Node new_node;
    
    //helper_valuables
    double x_up, x_down, x_up_com, x_down_com; //page 445
    double newaverage; //page 445
    double newaverage_up, newaverage_down; //page 445
    double price_up, price_down; //page 445
    int bucket_level; //upper level
    double C_up, C_down; //page 448
    double price_for_delta;
    double C_total; //C_up * q + C_down * (1 - q)
    double option2; //may become useful in American option
    
    double n_x_up, n_x_down, n_x_up_com, n_x_down_com; //page 445
    double n_newaverage; //page 445
    double n_newaverage_up, n_newaverage_down; //page 445
    double n_price_up, n_price_down; //page 445
    int n_bucket_level; //upper level
    double n_C_up, n_C_down; //page 448
    double n_price_for_delta;
    double n_C_total; //C_up * q + C_down * (1 - q)
    double n_option2; //may become useful in American option
    
    //adder_valuables
    int time;
    
    //calculate
    for ( time = (n - 1) ; time >= 0 ; time-- )
    {
        //price_for_delta = node[0].Cprice[0];
        for( node_number = 0 ; node_number <= time ; node_number++ )
        {
            //node Amin and difference
            higher_sum = 0;
            lower_sum = 0;
            for ( average_number = 0 ; average_number <= ( time - node_number ) ; average_number++ )
                higher_sum += S * pow ( u , average_number );
            for ( ;average_number <= time ; average_number++ )
                higher_sum += S * pow ( u , ( time - node_number ) ) * pow( d , (average_number - time + node_number));
            for ( average_number = 0 ; average_number <= node_number ; average_number++ )
                lower_sum += S * pow ( d , average_number );
            for ( ; average_number <= time ; average_number++ )
                lower_sum += S * pow ( d , node_number) * pow( u , (average_number - node_number) );
            
            /*DEBUG USE*/
            /*printf("time = %d node number = %d\n", time, node_number);
             for ( average_number = 0 ; average_number <= ( time - node_number ) ; average_number++ )
             printf( "S*U^%d +", average_number);
             for ( ;average_number <= time ; average_number++ )
             printf( "S*U^%d*D^%d +", (time - node_number),(average_number - time + node_number));
             printf("\n");
             for ( average_number = 0 ; average_number <= node_number ; average_number++ )
             printf( "S*D^%d +", average_number);
             for ( ; average_number <= time ; average_number++ )
             printf( "S*D^%d*U^%d +", node_number , (average_number - node_number));
             printf("\n");*/
            /*DEBUG END*/
            all_node[time].node[node_number].Amin = lower_sum/(time + 1);
            all_node[time].node[node_number].Amax = higher_sum/(time + 1);
            nonh_node[time].node[node_number].Amin = lower_sum/(time + 1);
            nonh_node[time].node[node_number].Amax = higher_sum/(time + 1);
            /*DEBUG USE*/
            //if( new_node.Amin > H)
                //new_node.Amin = 0;
            all_node[time].node[node_number].difference = ( higher_sum - lower_sum ) / (k * (time + 1));
            nonh_node[time].node[node_number].difference = ( higher_sum - lower_sum ) / (k * (time + 1));
            //printf( "%lf %lf\n", new_node.Amax , (new_node.Amin + k * new_node.difference));
            
            //calculating the Cprice
            for ( leveling_number = k ; leveling_number >= 0 ; leveling_number-- )
            {
                newaverage = all_node[time].node[node_number].Amin + leveling_number * all_node[time].node[node_number].difference;
                //up
                price_up = S * pow( u, (time - node_number + 1)) * pow( d , (node_number));
                newaverage_up = ( (all_node[time].node[node_number].Amin + leveling_number * all_node[time].node[node_number].difference) * ( time + 1 ) + price_up ) / (time + 2);
                if( all_node[time+1].node[node_number].difference != 0 )
                {
                    bucket_level = ceil( (newaverage_up - all_node[time+1].node[node_number].Amin) / all_node[time+1].node[node_number].difference );
                    n_bucket_level = ceil( (newaverage_up - nonh_node[time+1].node[node_number].Amin) / nonh_node[time+1].node[node_number].difference );
                    //if( (newaverage_down - node[node_number+1].Amin) < -node[node_number].difference )
                        //printf( "WRONG Time = %d, node_number = %d, leveling_number = %d, bucket = %d\n" , time, node_number, leveling_number, bucket_level);
                    if( bucket_level <= 0)
                        bucket_level = 1;
                    if( bucket_level > k)
                        bucket_level = k;
                    if( n_bucket_level <= 0)
                        n_bucket_level = 1;
                    if( n_bucket_level > k)
                        n_bucket_level = k;
                    x_up = ( bucket_level * all_node[time+1].node[node_number].difference + all_node[time+1].node[node_number].Amin - newaverage_up ) / all_node[time+1].node[node_number].difference;
                    n_x_up = ( n_bucket_level * nonh_node[time+1].node[node_number].difference + nonh_node[time+1].node[node_number].Amin - newaverage_up ) / nonh_node[time+1].node[node_number].difference;
                }
                else
                {
                    bucket_level = 1;
                    x_up = 1;
                    n_bucket_level = 1;
                    n_x_up = 1;
                }
                /*
                 if( x_up > 1 )
                 x_up = 1;
                 if( x_up < 0 )
                 x_up = 0;*/
                C_up = x_up * all_node[time+1].node[node_number].Cprice[bucket_level - 1] + ( 1 - x_up ) * all_node[time+1].node[node_number].Cprice[bucket_level];
                n_C_up = n_x_up * nonh_node[time+1].node[node_number].Cprice[bucket_level - 1] + ( 1 - n_x_up ) * nonh_node[time+1].node[node_number].Cprice[bucket_level];
                
                /*x_up = bucket_level * node[node_number].difference + node[node_number].Amin - newaverage_up;
                x_up_com = newaverage_up - node[node_number].difference * (bucket_level - 1) - node[node_number].Amin;
                if( node[ node_number].difference != 0)
                    C_up = (node[node_number].Cprice[bucket_level - 1] * (x_up) + node[node_number].Cprice[bucket_level] * (x_up_com)) / node[node_number].difference;
                else
                    C_up = node[node_number].Cprice[bucket_level - 1];*/
                
                //down(may become buggy
                price_down = S * pow( u, (time - node_number)) * pow( d , (node_number + 1));
                newaverage_down = ( (all_node[time].node[node_number].Amin + leveling_number * all_node[time].node[node_number].difference) * ( time + 1 ) + price_down ) / (time + 2);
                if( all_node[time+1].node[node_number + 1].difference != 0 )
                {
                    bucket_level = ceil( (newaverage_down - all_node[time+1].node[node_number + 1].Amin) / (all_node[time+1].node[node_number + 1].difference) );
                    n_bucket_level = ceil( (newaverage_down - nonh_node[time+1].node[node_number + 1].Amin) / (nonh_node[time+1].node[node_number + 1].difference) );
                    //if( (newaverage_down - node[node_number+1].Amin) < -node[node_number].difference )
                        //printf( "WRONG Time = %d, node_number = %d, leveling_number = %d, bucket = %d\n" , time, node_number, leveling_number, bucket_level);
                    if( bucket_level <= 0)
                        bucket_level = 1;
                    if( bucket_level > k)
                        bucket_level = k;
                    if( n_bucket_level <= 0)
                        n_bucket_level = 1;
                    if( n_bucket_level > k)
                        n_bucket_level = k;
                    x_down = ( bucket_level * all_node[time+1].node[node_number + 1].difference + all_node[time+1].node[node_number + 1].Amin - newaverage_down ) / all_node[time+1].node[node_number+1].difference;
                    n_x_down = ( n_bucket_level * nonh_node[time+1].node[node_number + 1].difference + nonh_node[time+1].node[node_number + 1].Amin - newaverage_down ) / nonh_node[time+1].node[node_number+1].difference;
                    
                }
                else
                {
                    bucket_level = 1;
                    x_down = 1;
                    n_bucket_level = 1;
                    n_x_down = 1;
                }
                /*
                if( x_down > 1 )
                    x_down = 1;
                if( x_down < 0 )
                    x_down = 0;*/
                C_down = x_down * all_node[time+1].node[node_number + 1].Cprice[bucket_level - 1] + ( 1 - x_down ) * all_node[time+1].node[node_number + 1].Cprice[bucket_level];
                n_C_down = n_x_down * nonh_node[time+1].node[node_number + 1].Cprice[bucket_level - 1] + ( 1 - n_x_down ) * nonh_node[time+1].node[node_number + 1].Cprice[bucket_level];
                
                /*x_down = bucket_level * node[node_number + 1].difference + node[node_number + 1].Amin - newaverage_down;
                x_down_com = newaverage_down - node[node_number + 1].difference * (bucket_level - 1) - node[node_number + 1].Amin;
                if( node[node_number + 1].difference != 0)
                    C_down = (node[node_number + 1].Cprice[bucket_level - 1] * (x_down) + node[node_number + 1].Cprice[bucket_level] * (x_down_com)) / node[node_number + 1].difference;
                else
                    C_down = node[node_number + 1].Cprice[bucket_level - 1];*/
                
                //total
                C_total = C_up * q + C_down * (1 - q);
                n_C_total = n_C_up * q + n_C_down * (1 - q);
                //option2 = exp(-( r * T/n )) * ( node[node_number].Cprice[0] * q + node[node_number - 1].Cprice[0] * (1 - q)); //American
                all_node[time].node[node_number].Cprice[leveling_number] = C_total/R; //buggy //European
                nonh_node[time].node[node_number].Cprice[leveling_number] = n_C_total/R;
                if( newaverage > H)
                    all_node[time].node[node_number].Cprice[leveling_number] = 0;
                //printf( "time = %d, node_number = %d, leveling_number = %d, Cprice = %lf\n" , time, node_number, leveling_number, new_node.Cprice[leveling_number]);
                //node[node_number].Cprice[leveling_number] = new_node.Cprice[leveling_number];
            }
            //replace
            //all_node[time].node[node_number].Amin = new_node.Amin;
            //all_node[time].node[node_number].difference = new_node.difference;
            
            
            

        }
    }
    final_price = - all_node[0].node[0].Cprice[0] +  nonh_node[0].node[0].Cprice[0];
    delta = ( -all_node[1].node[0].Cprice[0] + all_node[1].node[1].Cprice[0] + nonh_node[1].node[0].Cprice[0] - nonh_node[1].node[1].Cprice[0] ) / ( S * u - S * d ) ;
    printf( "put price = %lf\n" , final_price );
    printf( "delta = %lf\n" , delta );
    
    free(all_node);
    free(nonh_node);
        
    return 0;
}




































