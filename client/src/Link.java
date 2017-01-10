import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;



public class Link {
	
	
	
	
	private Socket cSocket;
	private InputStream is;
	private OutputStream os;
	
	public static final byte START=32;
	public static final byte WRONG=33;
	public static final byte WIN=34;
	public static final byte LOSE=35;
	public static final byte ERROR=36;
	
	private class Listen implements Runnable{
			
		
		public void run() {
			byte serverMessage[]=new byte[1];
			while(true){			
				
					try {
						if(is.read(serverMessage)<0)throw new IOException();
						if(serverMessage[0]<0)throw new IOException();
						MyFrame.number(serverMessage[0]);
						if(serverMessage[0]>=34){
							close();
							return;
						}
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
						MyFrame.number(ERROR);
						close();
						return;
					}
					System.out.println(""+serverMessage[0]);
					
				
			}
			
		}
		
	}
	
	public byte send(byte ch){
		try {
			os.write(ch);
			System.out.println("po wysłaniu "+ch);
		} catch (IOException e) {
			MyFrame.number(ERROR);
			close();
			return 1;
		}
		return 0;
	}
	
	public Link(String host, int port) throws IOException{

	
			System.out.println("przed polaczeniem");
			cSocket = new Socket(host, port);
			System.out.println("po polaczeniu");
			is = cSocket.getInputStream();
			os =cSocket.getOutputStream();
			System.out.println("strumienie");
			Runnable game=new Listen();
			System.out.println("watek1");
			Thread th = new Thread(game);
			System.out.println("watek2");
			th.start();
			System.out.println("watek3");


	}
	public void close(){
		try {
			cSocket.close();
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
	}

}
