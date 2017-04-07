import org.zeromq.ZMQ;
import java.util.StringTokenizer;

public class Test {
	public static void main(String[] args){
		ZMQ.Context context = ZMQ.context(1);
		
		System.out.println("connecting to server");
		
		ZMQ.Socket subscriber = context.socket(ZMQ.SUB);
		
		subscriber.connect("tcp://10.63.25.10:5556");
		
		String filter = "displacement";
		subscriber.subscribe(filter.getBytes());
		
		while(true){
			String data = subscriber.recvStr(0).trim();
			
			String[] strings = data.split(" ");
			
			String str = strings[1].trim().replaceAll("[^\\d|\\.|-]", "");
			//double num = Double.parseDouble(strings[1].trim());
			double num = Double.parseDouble(str);
			
			System.out.println("Data Recieved" + " : " + num);
			
			//System.out.println(str + " : " + str.length());
			
			/*for(int i = 0; i < str.length(); i++){
				char ch = str.charAt(i);
				System.out.println(ch);
			}*/
			
			/*try {
				Thread.sleep(5000);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}*/
		}
	}
}
