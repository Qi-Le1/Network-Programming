import java.util.Vector;

// Example of the Producer-Consumer Problem using Java threads
//
// Producer thread(s) add integers 1, 2, 3, etc. without bound.
//
// Consumer thread(s) read from buffer.

public class SharedBuffer
{
  // shared buffer of 8 integers
  public int maxSize = 8;
  public Vector buffer = new Vector( maxSize );
  public int i = 1;


  class Producer extends Thread
  {
    public void run()
    {
      while ( true )
      {
        produceMessage( this.getId() );
        try { Thread.sleep( 1000 ); } catch( InterruptedException ex ) {  }
      }
    }
  }


//    P( mutex )
//      P( empty_slot )
//        produceMessage( ... )

//  public void produceMessage( long threadId )
  public synchronized void produceMessage( long threadId )
  {
    if ( buffer.size() < maxSize )
    {
      buffer.addElement( i );
      System.out.println( "PRODUCER " + threadId + ": added " + i );
      i++;
    }
  }


  class Consumer extends Thread
  {
    public void run()
    {
      while ( true )
      {
        consumeMessage( this.getId() );
      }
    }
  }


//  public void consumeMessage( long threadId )
  public synchronized void consumeMessage( long threadId )
  {
    if ( buffer.size() > 0 )
    {
      int n = (Integer)buffer.firstElement();
      buffer.removeElement( n );
      System.out.println( "CONSUMER " + threadId + ": removed " + n );
    }
  }


  public void doit()
  {
    Producer p1 = new Producer();
    Producer p2 = new Producer();
    Consumer c1 = new Consumer();
    Consumer c2 = new Consumer();
    Consumer c3 = new Consumer();
    Consumer c4 = new Consumer();
    p1.start();
    p2.start();
    c1.start();
    c2.start();
    c3.start();
    c4.start();
  }


  public static void main( String[] args )
  {
    SharedBuffer program = new SharedBuffer();
    program.doit();
  }
}
