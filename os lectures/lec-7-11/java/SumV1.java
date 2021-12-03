/**
 * SumV1 attempts to take a divide-and-conquer
 *  approach to summing integers 1..1,000,000
 */
public class SumV1
{
  // all threads add to this global sum variable
  // to accumulate the final sum
  private long sum;

  public void doit()
  {
    sum = 0L;

    int n = 1000000;  // this must be evenly divisible by 100000
    int initial_thread_count = Thread.activeCount(); // Added this because some IDEs have a debugger thread already!

    SumThread[] t = new SumThread[ n / 100000 ];

    // divide the summation problem into many subtasks (threads)
    for ( int j = 0 ; j < t.length ; j++ )
    {
      t[j] = new SumThread( j * 100000 + 1, (j+1) * 100000 );
    }

            // t[0]         1 - 100,000
            // t[1]   100,001 - 200,000
            // t[2]   200,001 - 300,000
            //   etc.


    // start all threads
    for ( int j = 0 ; j < t.length ; j++ )
    {
      t[j].start();
    }


    // Join in child threads; and once all threads are done,
    // we should have the sum fully calculated
    try
    {
      // while (Thread.activeCount() > 1 ) // If not concerned about IDE features with extra threads
      while ( Thread.activeCount() > initial_thread_count )
      {
        for ( int j = 0 ; j < t.length ; j++ )
        {
          if ( t[j] != null && !t[j].isAlive() )
          {
            // join the child thread back in to the main thread of execution
            t[j].join();  // this is similar to wait()
            // the above join() call BLOCKS

            System.out.println( "PARENT: joined " + t[j].getName() );
            t[j] = null;
          }
        }
      }
    }
    catch ( InterruptedException ex )
    {
      System.err.println( "ERROR: thread interrupted" );
      System.exit( 1 );
    }

    System.out.println( "SUM 1.." + n + " IS " + sum );


    // calculate and show expected result -- CSCI 1100 style
    long calcSum = 0;
    for ( int i = 1 ; i <= n ; i++ )
    {
      calcSum += i;
    }
    System.out.println( "       (should be " + calcSum + ")" );
  }


  class SumThread extends Thread
  {
    private int m;    // sum values m..n
    private int n;

    public SumThread( int m, int n )
    {
      System.out.println( "ADDING " + m + ".." + n + " TO SUM" );
      this.m = m;
      this.n = n;
    }

    public void run()    // the start() method creates the thread,
    {                    //  then calls our run() method here.
      for ( int i = m ; i <= n ; i++ )
      {
        sum += i;     // compiler generates byte code:
                      //
                      //   load the sum variable
                      // --<thread-context-switch>--
                      //   add i to it
                      //   store the result back in the sum variable
      }
    }
  }


  public static void main( String[] args )
  {
    SumV1 a = new SumV1();
    a.doit();
    System.exit(0);
  }
}
