import java.awt.EventQueue;

public class Main {

	/**
	 * @param args
	 * @throws IOException 
	 * @throws UnknownHostException 
	 */
	public static MyFrame frame;
	public static void main(String[] args)  {

		
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				frame=new MyFrame();
			}
		});

		
		
		
	}

}
