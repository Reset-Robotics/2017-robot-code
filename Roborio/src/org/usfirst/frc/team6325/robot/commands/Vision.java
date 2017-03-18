package org.usfirst.frc.team6325.robot.commands;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.InputStreamReader;
import java.net.InetAddress;
import java.net.Socket;

/**
 * The Class Jetson.
 */
public class Vision implements Runnable {

	/** The main socket. */
	private static Socket mainSocket = null;

	/** The latest distance calculation from the Jetson. */
	private static double distance;
	
	/** The latest angle calculation from the Jetson. */
	//private static double angle;
	
	/** The Time class equivalent for when the frame was captured. */
	private static double frameCaptureTime;
	
	/** The robot time when the server sent and received the confirmation byte; */
	private static long serverStartTime;
	
	/**  Has the new data been read?. */
	private static boolean newData = false;
	
	//private static double time; //use this in the future maybe?

	/** The Jetson's supposed STATIC_IP. */ //TODO fix for Raspberry Pi and new Jetson
	private static final String STATIC_IP = "10.63.25.2";
	
	/** 
     * never call this. this is what gets called on Thread.start()
	 * @see java.lang.Runnable#run()
	 */
	public void run() {
		openSocket();
		connectionLoop();
	}

	/**
	 * Is there new data available?
	 *
	 * @return true, if successful
	 */
	public static boolean newData(){
		return newData;
	}
	
	/**
	 * Gets the distance.
	 *
	 * @return the distance
	 */
	public static double getDistance() {
		newData = false;
		return distance;
	}

	/**
	 * Gets the angle.
	 *
	 * @return the angle
	 */
	/*public static double getAngle() {
		newData = false;
		return angle;
	} */
	
	/**
	 * Gets the time.
	 *
	 * @return the time at which the frame was captured
	 */
	public static double getTime() {
		newData = false;
		return frameCaptureTime;
	}

	/**
	 * Checks if the roboRio is connected to the vision processor.
	 *
	 * @return true, if is connected
	 */
	public static boolean isConnected() {
		return (mainSocket != null);
	}
	
	/**
	 * Close connection to the vision processor.
	 *
	 * @return true, if successful
	 */
	public static boolean closeConnection() {
		if (isConnected()) {
			System.out.println("Closing connection to Jetson");
			sendMessage("C");
			try {
				mainSocket.close();
				mainSocket = null;
				return true;
			} catch (Exception e) {
				System.out.println("ERROR closing socket: " + e.getMessage());
				return false;
			}
		} else {
			System.out.println("Warning: Not connected. Cannot close connection.");
			return false;
		}
	}
	
	/**
	 * Start gear vision.
	 */
	public static void startGearVision() { //start gear vision
		sendMessage("G");
		newData = false;
	}

	/**
	 * Start boiler vision.
	 */
	/* public static void startBoilerVision() { //start boiler vision
		sendMessage("B");
		newData = false;
	} */

	/**
	 * Send message to the vision processor.
	 *
	 * @param message the message
	 */
	private static void sendMessage(String message) {
		if (isConnected()) {
			try {
				DataOutputStream toServer = new DataOutputStream(mainSocket.getOutputStream());
				toServer.writeBytes(message + '\n');
			} catch (Exception e) {
				System.out.println("ERROR sending message: " + e.getMessage());
			}
		} else {
			System.out.println("Warning: Not connected. Cannot send message.");
		}
	}

	/**
	 * Open socket.
	 */
	private static void openSocket() {
		// Wait until we're connected to the network
		waitUntilConnected(); // 45 seconds

		// Attempt to connect to vision processor's Static IP
		boolean reachable;
		try {
			reachable = InetAddress.getByName(STATIC_IP).isReachable(10);
		} catch (Exception e) {
			reachable = false;
		}

		if (reachable) {
			System.out.println("Jetson is reachable at " + STATIC_IP);
			mainSocket = connect(STATIC_IP);
		} else {
			System.out.println("Jetson is not reachable at it's static ip.");
			String ip = getJetsonIP(); // 20 seconds
			mainSocket = connect(ip);
		}
		startGearVision();
	}

	/**
	 * Wait until connected.
	 */
	private static void waitUntilConnected() {
		boolean connected = false;
		System.out.println("Waiting until we are connected to gateway...");
		do {
			try {
				String ip = InetAddress.getLocalHost().getHostAddress();
				if (ip != null) {
					connected = true;
				} else {
					Thread.sleep(2000);
				}
			} catch (Exception e) {
			}
		} while (!connected);

		System.out.println("Connected to gateway.");
	}

	/**
	 * Gets the vision processor's IP.
	 *
	 * @return the vision processor's IP
	 */
	private static String getJetsonIP() {
		System.out.println("Finding Vision Processor on network...");
		String visionIP = null;
		int n = 0;
		do {
			n++;
			try {
				String localIP = InetAddress.getLocalHost().getHostAddress();
				// System.out.println("RoboRio is on IP address " + localIP);
				String subnet = localIP.substring(0, localIP.length() - 3);
				// System.out.println("We need to search subnet " + subnet);
				int timeout = 5;
				// System.out.println("Reachable IPs:");
				for (int i = 1; i < 255; i++) {
					String ip = subnet + "." + i;
					if (InetAddress.getByName(ip).isReachable(timeout)) {
						// System.out.print(" " + ip + " is reachable ");
						if (i == 1) {
							// System.out.print("(Gateway)");
						} else if (ip.equals(localIP)) {
							// System.out.print("(RoboRio)");
						} else if (visionIP == null) {
							// System.out.print("(Jetson)");
							visionIP = ip;
						} else {
							// System.out.print("(Unknown)");
						}
						// System.out.println();
					}
				}
			} catch (Exception e) {
				// e.printStackTrace();
				System.out.println("ERROR on getting Vision Processor: " + e.getMessage());
				try {
					Thread.sleep(3000);
				} catch (Exception e2) {
				}
			}
		} while (visionIP == null);
		System.out.println("Found Vision Processor at " + visionIP + " in " + n + " tries!");
		return visionIP;
	}

	/**
	 * Connect.
	 *
	 * @param ip the ip
	 * @return the socket
	 */
	private static Socket connect(String ip) {
		boolean connected = false;
		do {
			try {
				int port = 9720;
				String confirmationToken = "972";
				System.out.println(
						"Connecting to " + ip + ":" + port + " with confirmation token:'" + confirmationToken + "'");

				Socket clientSocket = new Socket(ip, port);
				DataOutputStream toServer = new DataOutputStream(clientSocket.getOutputStream());
				BufferedReader fromServer = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
				connected = true;

				String received;
				toServer.writeBytes(confirmationToken + '\n');
				received = fromServer.readLine();
				serverStartTime = System.currentTimeMillis();
				System.out.println("We received " + received + " back from the server.");
				if (confirmationToken.equals(received)) {
					connected = true;
					System.out.println("Connected to the Vision Processor!");
					// clientSocket.close();
					return clientSocket;
				}
			} catch (Exception e) {
				// e.printStackTrace();
				System.out.println("ERROR on connection: " + e.getMessage());
				try {
					Thread.sleep(3000);
				} catch (Exception e2) {
				}
			}
		} while (!connected);
		return null;
	}

	/**
	 * Connection loop for reading data coming in from the vision processor.
	 *
	 * @return true, if successful
	 */
	
	private static void connectionLoop() {
		System.out.println("Starting connection loop...");
		try {
			BufferedReader fromServer = new BufferedReader(new InputStreamReader(mainSocket.getInputStream()));
			String received;	
			while (mainSocket != null) {
				received = fromServer.readLine();
				if(received != null) {
					System.out.println("We received \"" + received+ "\" from the Vision Processor.");
					int comma = received.indexOf(',');
					int colon = received.indexOf(':');
					if (comma != -1 && colon != -1) {
						distance = Double.parseDouble(received.substring(0, comma));
						//angle = Double.parseDouble(received.substring(comma + 1, colon));
					//	long frameProcessingTime = (System.currentTimeMillis() - serverStartTime) - Long.parseLong(received.substring(colon + 1, received.length()));
					//	frameCaptureTime = Time.get() - (frameProcessingTime*1000);
						newData = true;
						System.out.println("d: " + distance);
					} else {
						System.out.println("Info we received isn't a valid format: " + received);
					}
				} else {
					//null: the socket was closed/lost connection
					
					mainSocket.close();
					mainSocket = null;
					
					System.out.println("Socket Connection to Vision Processor Lost");
					break;
				}
			}	
		} catch (Exception e) {
			System.out.println("ERROR read loop: " + e.getMessage());
		}
		System.out.println("Connection loop ended.");
	}

}

