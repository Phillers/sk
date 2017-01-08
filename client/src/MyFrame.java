import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import java.io.File;
import java.io.IOException;

import javax.imageio.ImageIO;
import javax.swing.BoxLayout;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTextField;


 
public class MyFrame extends JFrame implements ActionListener{
	
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	public int gameState=0;
	private JButton plansza[];
	private JButton pionki[];
	private ImageIcon ikonki[];
	private byte klik=-1;
	private JLabel labels[];
	private JTextField text[];
	private JButton send;
	private Link game;
	private byte pole;
	private byte pion;
	
	public MyFrame() {
		super( "Quarto" );
		plansza=new JButton[16];
		pionki=new JButton[16];
		ikonki=new ImageIcon[16];
		labels =new JLabel[3];
		text=new JTextField[2];
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setSize(1300, 400);
		setLocation(50,50);
		setLayout(new GridLayout(1, 3,50,50));
		JPanel pan1=new JPanel();
		pan1.setLayout(new GridLayout(4,4));
		for(int i=0; i<16; i++){
			plansza[i]=new JButton(""+i);
			plansza[i].addActionListener(this);
			pan1.add(plansza[i]);
		}
		add(pan1);
		
		JPanel pan2=new JPanel();
		pan2.setLayout(new GridLayout(4,4));
		for(int i=0; i<16; i++){

				ikonki[i] = new ImageIcon(getClass().getResource(""+i+".png"));
				pionki[i] = new JButton(ikonki[i]);

			pionki[i].addActionListener(this);
		
			pan2.add(pionki[i]);
	}
		add(pan2);
		
		
		JPanel pan3=new JPanel();
		pan3.setLayout(new BoxLayout(pan3, BoxLayout.Y_AXIS));
		for(int i=0;i<3;i++){
			labels[i]=new JLabel();
			pan3.add(labels[i]);
		}
		labels[2].setText("Podaj ip i port");
		pan3.add(text[0]=new JTextField("localhost"));
		pan3.add(text[1]=new JTextField("1234"));
		pan3.add(send=new JButton("Wyślij"));
		send.addActionListener(this);
		add(pan3);
		
		setVisible(true);
	}

	public void actionPerformed(ActionEvent s) {

		Object src = s.getSource();
		
		if(src==send){
			if(gameState==0){
				String host=text[0].getText();
				int port = Integer.parseInt(text[1].getText());
				try {
					game = new Link(host,port);
					gameState=1;
					labels[0].setText("Połączono");
					labels[2].setText("Czekaj na przeciwnika");
					labels[2].setText("");
					text[0].setVisible(false);
					text[1].setVisible(false);
				} catch (IOException e) {
					labels[0].setText("Nie udało się połączyć");
				}
				
			} else if(gameState==3){
				if(klik<16&&klik>=0){
					if(plansza[klik].getText()!=""){
						plansza[klik].setIcon(ikonki[pion]);
						plansza[klik].setText("");
						pionki[pion].setVisible(false);
						game.send(klik);
						gameState=2;
						pole=klik;
						labels[1].setText("Wysłałeś pole numer "+pole);
						
						labels[2].setText("Wyślij pionek");
					} else {
						labels[2].setText("To pole jest zajęte, wyślij inne");
					}
				}
			} else if(gameState==2){
				if(klik>=16){
					game.send(klik);
					gameState=1;
					pion=(byte) (klik-16);
					labels[2].setText("Czekaj na przeciwnika");
					labels[0].setText("");
					labels[1].setText("Wysłałeś pion:");
					labels[1].setIcon(null);
		
				}
			}
		} else{
		byte x=16;
		for(byte i=0;i<x;i++){
			if (src==plansza[i]&&gameState==3 ){
				klik = i;
				labels[1].setText("Wybrałeś pole numer " + i);
				labels[2].setIcon(null);
				
			}
			if (src==pionki[i]&&gameState==2){
				
				klik=(byte)(i+x);
				labels[1].setText("Wybrany pionek:");
				labels[2].setIcon(ikonki[i]);
			}
		}
		}
		
	}
	public void num(byte n){
		if(n<16){
			pole = n;
			labels[0].setText("Przeciwnik wybrał pole numer "+pole);
			
			plansza[n].setText("");
			plansza[n].setIcon(ikonki[pion]);
			pionki[pion].setVisible(false);
		} else if(n<32){
			pion=(byte) (n-16);
			labels[0].setText("Przeciwnik wybrał pion:");
			labels[1].setIcon(ikonki[pion]);
			labels[1].setText("");
			labels[2].setText("Wyślij pole");
			labels[2].setIcon(null);
			gameState=3;
		} else switch(n){
		case Link.START: gameState=2;
		labels[2].setText("Wyślij pionek");
		break;
		
		case Link.WIN:
			gameState=0;
			JOptionPane.showMessageDialog(this,
				    "Gratulacje, wygrałeś",
				    "You win",
				    JOptionPane.PLAIN_MESSAGE);
			labels[0].setText("Gratulacje, wygrałeś");
			reset();
			break;
			
		case Link.LOSE:
			gameState=0;			
			JOptionPane.showMessageDialog(this,
				    "Niestety przegrałeś",
				    "You lose",
				    JOptionPane.PLAIN_MESSAGE);
			labels[0].setText("Niestety przegrałeś");
			reset();
			break;
			
		case Link.ERROR:
			gameState=0;
			
			JOptionPane.showMessageDialog(this,
				    "Utracono połączenie",
				    "Connection Lost",
				    JOptionPane.ERROR_MESSAGE);
			
			labels[0].setText("Utracono połączenie");
			
			reset();
			break;
			
		}
	}
	private void reset(){
		for(int i=0; i<16; i++){
			plansza[i].setIcon(null);
			plansza[i].setText(""+i);
		}
		for(int i=0; i<16; i++){
			pionki[i].setVisible(true);
		}
		labels[1].setIcon(null);
		labels[2].setIcon(null);
		labels[1].setText("aby zacząć od nowa połącz się ponownie");
		labels[2].setText("Podaj ip i port");
		text[0].setVisible(true);
		text[1].setVisible(true);
		klik=-1;
	}
	
	public static void number(byte num){
			Main.frame.num(num);
	}

}