
#include "utils/Statistics.h"
#include "utils/Timer.h"
#include "math/MathUtils.h"

using namespace realisim;
using namespace math;

void print(const realisim::utils::Statistics& iSt)
{
  printf("number of sample %d\n"
         "min %.4f\n"
         "max %.4f\n"
         "mean %.4f\n"
         "std dev %.4f\n\n",
         iSt.getNumberOfSamples(),
         iSt.getMin(),
         iSt.getMax(),
         iSt.getMean(),
         iSt.getStandardDeviation() );
}

bool test()
{
  const int NUM_SAMPLES = 10;
  const double samples[] = {
    6.1061334, 9.6783204, 1.2747090, 8.2395131, 0.3333483,
    6.9755066, 1.0626275, 7.6587523, 4.9382973, 9.5788115
  };

  const double expect_min = 0.3333;
  const double expect_max = 9.6783;
  const double expect_mean = 5.584602;
  const double expect_stddev = 3.547868;
  
  realisim::utils::Statistics st;
  st.add(samples, NUM_SAMPLES);
  print(st);
  
  const double epsilon = 0.0001;
  return st.getNumberOfSamples() == NUM_SAMPLES &&
    isEqual( st.getMin(), expect_min, epsilon) &&
    isEqual( st.getMax(), expect_max, epsilon) &&
    isEqual( st.getMean(), expect_mean, epsilon) &&
  isEqual( st.getStandardDeviation(), expect_stddev, epsilon);
  
}

int main(int argc, char** argv)
{
  realisim::utils::Statistics st;
  for(int i = 0; i <= 200; ++i)
  {
    st.add(i);
  }
  
  print(st);
  
  if( test() ) { printf("success"); }
  else{ printf("failure"); }
  
  
  realisim::utils::Timer t;
  for(int i = 0; i < 1000; ++i)
  { printf("*"); }
  printf("\n time to print 1000 stars %.6f\n", t.getElapsed());
  
  t.start();
  for(int i = 0; i < 1000; ++i)
  { printf("*"); }
  printf("\n time to print 1000 stars again %.6f\n", t.getElapsed());
  
  for(int i = 0; i < 1000; ++i)
  { printf("*"); }
  printf("\n time to print 1000 stars 2 times %.6f\n", t.getElapsed());
  return 0;
}
