import org.zeromq.ZMQ;
import java.util.StringTokenizer;

public class Test {
	public static void main(String[] args){
		ZMQ.Context context = ZMQ.context(1);
		
		System.out.println("connecting to server");
		
		ZMQ.Socket subscriber = context.socket(ZMQ.SUB);
		
		subscriber.connect("tcp://raspberrypi.local:5556");
		
		String filter = "displacement";
		subscriber.subscribe(filter.getBytes());
		
		while(true){
			String data = subscriber.recvStr(0).trim();
			
			StringTokenizer scan = new StringTokenizer(data, " ");
			
			String str = scan.nextToken();
			String num = scan.nextToken();
			
			System.out.println("Data Recieved" + " : " + num);
		}
	}
}
