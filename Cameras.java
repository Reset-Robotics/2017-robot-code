public void robotInit() {
    	
    	Thread t = new Thread(() -> {
    		
    		boolean allowCam1 = false;
    		
    		UsbCamera camera1 = CameraServer.getInstance().startAutomaticCapture(0);
            camera1.setResolution(320, 240);
            camera1.setFPS(30);
            UsbCamera camera2 = CameraServer.getInstance().startAutomaticCapture(1);
            camera2.setResolution(320, 240);
            camera2.setFPS(30);
            
            CvSink cvSink1 = CameraServer.getInstance().getVideo(camera1);
            CvSink cvSink2 = CameraServer.getInstance().getVideo(camera2);
            CvSource outputStream = CameraServer.getInstance().putVideo("Switcher", 320, 240);
            
            Mat image = new Mat();
            
            while(!Thread.interrupted()) {
            	
            	if(oi.getGamepad().getRawButton(9)) {
            		allowCam1 = !allowCam1;
            	}
            	
                if(allowCam1){
                  cvSink2.setEnabled(false);
                  cvSink1.setEnabled(true);
                  cvSink1.grabFrame(image);
                } else{
                  cvSink1.setEnabled(false);
                  cvSink2.setEnabled(true);
                  cvSink2.grabFrame(image);     
                }
                
                outputStream.putFrame(image);
            }
            
        });
        t.start();
