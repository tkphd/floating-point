/**
   \file shuffle-sum.cpp
   \brief map histogram of values which, ideally, would all be 10000

   In principal, \$ 10\left\frac{1}{1000}\right)
                   + 9\left\frac{1}{ 100}\right)
                   + 9\left\frac{1}{  10}\right)
                   + 9\left(           1\right)
                   + 9\left(          10\right)
                   + 9\left(         100\right)
                   + 9\left(        1000\right)
                   = 10000 \$.
   This program tests the reality by generating these numbers,
   shuffling them at random, then taking their sum: the order
   of addition seems to matter.
 */

#include <algorithm>
#include <iostream>
#include <map>
#include <random>

int main()
{
    std::map<float,int> imprecision;
    const int N = 1e6;

    std::random_device Maxwell;
    std::default_random_engine prng;
    prng.seed(Maxwell());

    std::vector<float> x;
    for (unsigned int i = 0; i < 9; i++) {
        x.push_back(   0.001);
        x.push_back(   0.01);
        x.push_back(   0.1);
        x.push_back(   1.);
        x.push_back(  10.);
        x.push_back( 100.);
        x.push_back(1000.);
    }
    x.push_back(0.001); // there's an extra one of these guys

    for (unsigned int i = 0; i < N; i++) {
        std::random_shuffle(x.begin(), x.end());
        float z = 0;
        for (unsigned int i = 0; i < x.size(); i++)
            z += x[i];

        imprecision[z]++;
    }

    for (std::map<float,int>::const_iterator it = imprecision.begin(); it != imprecision.end(); it++) {
        printf("%32.26f: %12.9f %%\n", it->first, float(100*it->second)/N);
    }

    return 0;
}
