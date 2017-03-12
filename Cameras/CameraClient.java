package org.usfirst.frc.team6325.robot;

import java.net.*;
import java.io.*;
import java.awt.*;
import java.awt.image.*;
import java.util.Arrays;
import javax.imageio.ImageIO;
import javax.swing.*;

public class CameraClient {
    private String hostIP;
    private int serverPort;
    private Socket client;
    private OutputStream out;
    private InputStream in;
    
    public CameraClient(String hostIP, int serverPort) {
        this.hostIP = hostIP;
        this.serverPort = serverPort;
    }
    public CameraClient() {}
    public void sethostIP(String hostIP) {
        this.hostIP = hostIP;
    }
    public void setServerPort(int port) {
        this.serverPort = port;
    }
    public String getHostIP() {
        return hostIP;
    }
    public int getServerPort() {
        return serverPort;
    }
    public void initSocket() {
        try {
            this.client = new Socket(hostIP, serverPort);
            this.out = this.client.getOutputStream();
            this.in = this.client.getInputStream();
            System.out.println("Client socket successfully initialized.");
        } catch (IOException e) {
            System.out.println("Error Initializing Socket or Input/Output Streams");
        }
        
    }
    public void writeString(String s) {
        try {
            BufferedWriter bw = new BufferedWriter(new OutputStreamWriter(out));
            bw.write(s);
            bw.close();
        } catch (IOException e) {
            System.out.println("Error writing string to socket.");
        }
    }
    public String readSocketString() {
        String s = null;
        try {
            BufferedReader br = new BufferedReader(new InputStreamReader(in));
            s = br.readLine();
            br.close();
        } catch (IOException e) {
            System.out.println("Error reading string from socket.");
        }
        return s;
    }
    private byte[] readSocketBytes() {
        ByteArrayOutputStream bs = null;
        try {   
            bs = new ByteArrayOutputStream();
            byte[] buffer = new byte[10000];
            int s = 0;
            while ((s = in.read(buffer, 0, 10000)) != -1) {
                if ((char)buffer[s-1] == 'e'){
                    bs.write(buffer, 0, s-1);
                    break;
                } else {
                    bs.write(buffer, 0, s);
                }
            }
            if (s == -1) {
                return null;
            }
        } catch (IOException e) {
            System.out.println("Error reading bytes from socket.");
        }
        return bs.toByteArray();
    }
    public BufferedImage readImage() {
        BufferedImage img = null;
        try {
            byte[] bytes = readSocketBytes();
            if (bytes == null) return null;
            ByteArrayInputStream bais = new ByteArrayInputStream(bytes);
            img = ImageIO.read(bais);
        } catch (Exception e) {
            System.out.println("Error creating image from bytes.");
        }
        return img;
    }
    public static void main(String[] args) {
        CameraClient cam1 = new CameraClient("172.17.125.191", 5440);
        CameraClient cam2 = new CameraClient("172.17.125.191", 5441);
        cam1.initSocket();
        cam2.initSocket();
        
        BufferedImage camFrame1 = null;
        BufferedImage camFrame2 = null;
        
        DisplayImage window1 = null;
        DisplayImage window2 = null;
        
        try {
            camFrame1 = cam1.readImage();
            camFrame2 = cam2.readImage();
            window1 = new DisplayImage(camFrame1);
            window2 = new DisplayImage(camFrame2);
        } catch (Exception e) {
            e.printStackTrace();
        }
        
        while (true) {
            try {
                camFrame1 = cam1.readImage();
                camFrame2 = cam2.readImage();
                if (camFrame1 == null || camFrame2 == null) {
                    System.out.println("Dropped Frame");
                    continue;
                }
                window1.updateImage(camFrame1);
                window2.updateImage(camFrame2);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }
    private static class DisplayImage {
        private JLabel lbl;
        public DisplayImage(BufferedImage img) throws IOException {
            ImageIcon icon = new ImageIcon(img);
            JFrame frame = new JFrame();
            lbl = new JLabel();
            lbl.setIcon(icon);
            frame.setSize(200,300);
            frame.add(lbl);
            frame.setVisible(true);
            frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
            frame.pack();
        }
        public void updateImage(BufferedImage img) {
            lbl.setIcon(new ImageIcon(img));
            System.out.println("Updating Image");
        }
    }
}
